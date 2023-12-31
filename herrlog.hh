/**
 * @file herrlog.hh
 * @author Saphereye
 * @brief Header file only logging library
 * @version 0.5
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>
#include <mutex>

/**
 * @brief Set of ANSI colors, more can be found here:
 * https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124
 *
 */
#define RESET_COLOR "\033[0m"
#define BOLD_RED_COLOR "\033[1;31m"
#define BOLD_GREEN_COLOR "\033[1;32m"
#define BOLD_YELLOW_COLOR "\033[1;33m"
#define BOLD_BLUE_COLOR "\033[1;34m"
#define BACKGROUND_RED_COLOR "\033[41m"
#define BOLD_WHITE_COLOR "\033[1;37m"

/**
 * @brief Enum defining different log types as bit flags.
 *
 */
enum class LogType : std::uint8_t {
    Trace = 0b000001,  // For specific details
    Debug = 0b000010,  // Temporary debug printing
    Info = 0b000100,   // General purpose details, e.g. "Program has started"
    Error = 0b001000,  // For errors, exits the program
    Warn = 0b010000,   // For reporting warning, the program keeps working
    Fatal = 0b100000,  // Program exits and produces core dump

    All = 0b111111,
    None = 0b000000,
};

/**
 * @brief Overloading the `|` operator for combining log types.
 *
 * @param a
 * @param b
 * @return LogType
 */
LogType operator|(LogType a, LogType b) {
    return static_cast<LogType>(static_cast<uint8_t>(a) |
                                static_cast<uint8_t>(b));
}

/**
 * @brief Overloading the `&` operator for checking if a log type is set.
 *
 * @param a
 * @param b
 * @return true
 * @return false
 */
bool operator&(LogType a, LogType b) {
    return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

/**
 * @brief Logger implementation providing flexible logging capabilities.
 *
 */
class Logger {
   private:
    static LogType log_type;
    static std::string output_file_name;
    static std::ofstream output_file;
    static bool is_output_to_console;
    static const char* datetime_format;
    static std::mutex log_mutex;

    /**
     * @brief A recursive function to print the templated arguments passed
     *
     * @tparam T
     * @tparam Rest
     * @param stream
     * @param format
     * @param arg
     * @param rest
     */
    template <typename T, typename... Rest>
    static void print_to_stream(std::ostream& stream, const char* format, T arg,
                                Rest... rest) {
        for (size_t index = 0; format[index] != '\0'; index++) {
            if (format[index] == '{' && format[index + 1] == '}') {
                stream << arg;
                return print_to_stream(stream, format + index + 2, rest...);
            }
            stream << format[index];
        }
    }

    /**
     * @brief Base case for the recursive function
     *
     * @param stream
     * @param format
     */
    static void print_to_stream(std::ostream& stream, const char* format) {
        for (size_t index = 0; format[index] != '\0'; index++) {
            stream << format[index];
        }
    }

    /**
     * @brief Logs a message with specified details to the console or a file.
     *
     * @tparam Args
     * @param name
     * @param color
     * @param format
     * @param args
     */
    template <typename... Args>
    static void log(const char* name, const char* color, const char* format,
                    Args... args) {
        auto current_time_point = std::chrono::system_clock::now();
        std::time_t current_time =
            std::chrono::system_clock::to_time_t(current_time_point);
        char time_string[100];
        std::strftime(time_string, sizeof(time_string), datetime_format,
                      std::localtime(&current_time));

        std::lock_guard<std::mutex> lock(Logger::log_mutex);
        if (is_output_to_console) {
            std::cout << color << "[" << name << " " << time_string << "]"
                      << RESET_COLOR << " ";
            print_to_stream(std::cout, format, args...);
            std::cout << std::endl;
        } else {
            output_file << "[" << name << " " << time_string << "] ";
            print_to_stream(output_file, format, args...);
            output_file << "\n";
        }
    }

    /**
     * @brief Destroy the Logger object. Furthermore closes the output file if
     * opened. BUG: The destructor is never called.
     */
    ~Logger() {
        std::cout << "Yo" << std::endl;
        if (!is_output_to_console) {
            output_file.close();
        }
    }

    /**
     * @brief Preventing construction of a new logger object.
     *
     */
    Logger() = delete;

   public:
    /**
     * @brief Sets the log type, default is LogType::All.
     *
     * @param log_type
     */
    static void set_type(LogType log_type) { Logger::log_type = log_type; }

    /**
     * @brief Set the output file name object
     *
     * @param output_file_name
     */
    static void set_output_file_name(std::string output_file_name) {
        Logger::output_file_name = output_file_name;
        is_output_to_console = false;
        Logger::output_file = std::ofstream(Logger::output_file_name);
        if (Logger::output_file.is_open()) {
            Logger::output_file.close();
        }
        Logger::output_file.open(Logger::output_file_name);
    }

    /**
     * @brief Set the datetime format object, default is "%Y-%m-%d %H:%M:%S"
     *
     * @param datetime_format
     */
    static void set_datetime_format(const char* datetime_format) {
        Logger::datetime_format = datetime_format;
    }

    /**
     * @brief Logs messages of type trace.
     *
     * @tparam Args
     * @param name
     * @param color
     * @param format
     * @param args
     */
    template <typename... Args>
    static void trace(const char* format, Args... args) {
        if (log_type & LogType::Trace) {
            log("TRACE", BOLD_WHITE_COLOR, format, args...);
        }
    }

    /**
     * @brief Logs messages of type debug
     *
     * @tparam Args
     * @param name
     * @param color
     * @param format
     * @param args
     */
    template <typename... Args>
    static void debug(const char* format, Args... args) {
        if (log_type & LogType::Debug) {
            log("DEBUG", BOLD_BLUE_COLOR, format, args...);
        }
    }

    /**
     * @brief Logs messages of type info
     *
     * @tparam Args
     * @param name
     * @param color
     * @param format
     * @param args
     */
    template <typename... Args>
    static void info(const char* format, Args... args) {
        if (log_type & LogType::Info) {
            log(" INFO", BOLD_GREEN_COLOR, format, args...);
        }
    }

    /**
     * @brief Logs messages of type error. Furthermore closes the output file
     * and exits the program.
     *
     * @tparam Args
     * @param name
     * @param color
     * @param format
     * @param args
     */
    template <typename... Args>
    static void error(const char* format, Args... args) {
        if (log_type & LogType::Error) {
            log("ERROR", BOLD_RED_COLOR, format, args...);
            if (!is_output_to_console) {
                output_file.close();
            }
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Logs messages of type warning. Doesn't exit the program, just
     * notifies the issue.
     *
     * @tparam Args
     * @param format
     * @param args
     */
    template <typename... Args>
    static void warn(const char* format, Args... args) {
        if (log_type & LogType::Warn) {
            log(" WARN", BOLD_YELLOW_COLOR, format, args...);
        }
    }

    /**
     * @brief Logs messages of type fatal. Exits the program and creates a core
     * dump.
     *
     * @tparam Args
     * @param format
     * @param args
     */
    template <typename... Args>
    static void fatal(const char* format, Args... args) {
        if (log_type & LogType::Fatal) {
            log("FATAL", BACKGROUND_RED_COLOR, format, args...);
            if (!is_output_to_console) {
                output_file.close();
            }
            abort();
        }
    }
};

LogType Logger::log_type = LogType::All;
std::string Logger::output_file_name = std::string();
std::ofstream Logger::output_file;
bool Logger::is_output_to_console = true;
const char* Logger::datetime_format = "%Y-%m-%d %H:%M:%S";
std::mutex Logger::log_mutex;