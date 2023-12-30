/**
 * @file herrlog.hh
 * @author Saphereye
 * @brief Header file only loggin library
 * @version 0.2
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#pragma GCC diagnostic ignored "-Wformat-security"

#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>

#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[1;31m"
#define GREEN_COLOR "\033[1;32m"
#define YELLOW_COLOR "\033[1;33m"
#define BLUE_COLOR "\033[1;34m"

enum class LogType : std::uint8_t {
    Trace = 0b0001,  // For specific details
    Debug = 0b0010,  // Temporary debug printing
    Info = 0b0100,   // General purpose details, e.g. "Program has started"
    Error = 0b1000,  // For errors

    All = 0b1111,
    None = 0b0000,
};

LogType operator|(LogType a, LogType b) {
    return static_cast<LogType>(static_cast<uint8_t>(a) |
                                static_cast<uint8_t>(b));
}

bool operator&(LogType a, LogType b) {
    return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

class Logger {
   private:
    static LogType log_type;
    static std::string output_file_name;
    static std::ofstream output_file;
    static bool is_output_to_console;

    // Recursive function to print formatted arguments
    template <typename T, typename... Rest>
    static void print_to_stream(std::ostream& stream, const char* format, T arg, Rest... rest) {
        for (size_t index = 0; format[index] != '\0'; index++) {
            if (format[index] == '{' && format[index + 1] == '}') {
                stream << arg;
                return print_to_stream(stream, format + index + 2, rest...);
            }
            stream << format[index];
        }
    }

    // Base case for the recursive function
    static void print_to_stream(std::ostream& stream, const char* format) {
        for (size_t index = 0; format[index] != '\0'; index++) {
            stream << format[index];
        }
    }

    template <typename... Args>
    static void log(const char* name, const char* color, const char* format,
                    Args... args) {
        auto current_time_point = std::chrono::system_clock::now();
        std::time_t current_time =
            std::chrono::system_clock::to_time_t(current_time_point);
        char time_string[100];
        std::strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&current_time));

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

   public:
    // BUG destructor is not called
    ~Logger() {
        std::cout << Logger::is_output_to_console << std::endl;
        if (!is_output_to_console) {
            output_file.close();
        }
    }

    static void init(LogType log_type = LogType::All,
                     std::string output_file_name = std::string()) {
        Logger::log_type = log_type;
        Logger::output_file_name = output_file_name;

        if (Logger::output_file_name.size() < 1) {
            is_output_to_console = true;
        } else {
            is_output_to_console = false;
            Logger::output_file = std::ofstream(Logger::output_file_name);
            if (Logger::output_file.is_open()) {
                Logger::output_file.close();
            }
            Logger::output_file.open(Logger::output_file_name);
        }
    }

    template <typename... Args>
    static void trace(const char* format, Args... args) {
        if (log_type & LogType::Trace) {
            log("TRACE", BLUE_COLOR, format, args...);
        }
    }

    template <typename... Args>
    static void debug(const char* format, Args... args) {
        if (log_type & LogType::Debug) {
            log("DEBUG", GREEN_COLOR, format, args...);
        }
    }

    template <typename... Args>
    static void info(const char* format, Args... args) {
        if (log_type & LogType::Info) {
            log(" INFO", YELLOW_COLOR, format, args...);
        }
    }

    template <typename... Args>
    static void error(const char* format, Args... args) {
        if (log_type & LogType::Error) {
            log("ERROR", RED_COLOR, format, args...);
        }
    }
};

LogType Logger::log_type = LogType::All;
std::string Logger::output_file_name = std::string();
std::ofstream Logger::output_file;
bool Logger::is_output_to_console = true;

#pragma GCC diagnostic warning "-Wformat-security"