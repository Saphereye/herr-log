/**
 * @file herrlog.hh
 * @author Saphereye
 * @brief Header file logging library
 * @version 1.0
 * @date 2024-01-02
 * @note Requires C++20 or later
 *
 * @copyright Copyright (c) 2023 Adarsh Das
 * 
 * MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <chrono>
#include <fstream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string_view>

static_assert(__cplusplus >= 202002L,
              "This library requires at least C++20 to compile");

/**
 * @brief Set of ANSI colors, more can be found here:
 * https://gist.github.com/JBlond/2fea43a3049b38287e5e9cefc87b2124
 *
 */
namespace ascii_colors {
inline constexpr std::string_view reset_color{"\033[0m"};
inline constexpr std::string_view bold_red_color{"\033[1;31m"};
inline constexpr std::string_view bold_green_color{"\033[1;32m"};
inline constexpr std::string_view bold_yellow_color{"\033[1;33m"};
inline constexpr std::string_view bold_blue_color{"\033[1;34m"};
inline constexpr std::string_view background_red_color{"\033[41m"};
inline constexpr std::string_view bold_white_color{"\033[1;37m"};
}  // namespace ascii_colors

/**
 * @brief The LogType class is used to define different types of logs using bit
 * flags. Each log type is represented by a different bit in a std::uint8_t
 * value. This allows multiple log types to be combined using bitwise
 * operations.
 */
class LogType {
   private:
    std::uint8_t bitflag;

   public:
    /**
     * @brief Enum defining different log types as bit flags.
     *
     */
    enum : std::uint8_t {
        Trace = 0b000001,  // For specific details
        Debug = 0b000010,  // Temporary debug printing
        Info = 0b000100,  // General purpose details, e.g. "Program has started"
        Error = 0b001000,  // For errors, exits the program
        Warn = 0b010000,   // For reporting warning, the program keeps working
        Fatal = 0b100000,  // Program exits and produces core dump

        All = 0b111111,
        None = 0b000000,
    };

    /**
     * @brief Construct a new Log Type object. Allows implicit conversion from
     * uint8_t. Thus allows assignments like LogType Logger::log_type =
     * LogType::All;
     *
     * @param initial_flag
     */
    LogType(std::uint8_t initial_flag) : bitflag(initial_flag) {}

    /**
     * @brief Overloading the `|` operator for combining log types.
     * @param other
     * @return LogType
     */
    LogType operator|(const LogType other) const {
        return LogType((static_cast<std::uint8_t>(bitflag) |
                        static_cast<std::uint8_t>(other.bitflag)));
    }

    /**
     * @brief Overloading the `&` operator for checking if a log type is set.
     *
     * @param other
     * @return true if the bit is set in both operands
     * @return false otherwise
     */
    bool operator&(const LogType other) const {
        return static_cast<bool>(static_cast<std::uint8_t>(bitflag) &
                                 static_cast<std::uint8_t>(other.bitflag));
    }
};

/**
 * @brief Logger implementation providing flexible logging capabilities.
 *
 */
class Logger {
   private:
    static LogType log_type;
    static std::string output_file_name;
    static std::ostream* output_buffer;
    static bool is_color_output;
    static const char* datetime_format;
    static std::mutex log_mutex;
    static std::ofstream output_file;

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
    static void log(const char* name, const std::string_view& color,
                    const char* format, Args... args) {
        std::time_t current_time = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        char time_string[100];
        std::strftime(time_string, sizeof(time_string), datetime_format,
                      std::localtime(&current_time));

        std::stringstream ss;
        ss << (is_color_output ? color : std::string_view()) << "[" << name
           << " " << time_string << "]"
           << (is_color_output ? ascii_colors::reset_color : std::string_view())
           << " ";

        print_to_stream(ss, format, args...);

        std::lock_guard<std::mutex> lock(Logger::log_mutex);
        *output_buffer << ss.str() << '\n';
        /**
         * @brief Ideally buffer shouldn't be flushed every time, but if we
         * don't do it and the program prematurely crashes, all the logs will be
         * lost
         *
         */
        output_buffer->flush();
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
        Logger::output_file = std::ofstream(Logger::output_file_name);
        if (Logger::output_file.is_open()) {
            Logger::output_file.close();
        }
        Logger::output_file.open(Logger::output_file_name);
        if (!Logger::output_file.is_open()) {
            Logger::error(
                "Failed to open {}",
                Logger::output_file_name);  // Peak efficiency ᕦ(ò_óˇ)ᕤ
            return;
        }

        output_buffer = &Logger::output_file;
        is_color_output = false;
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
     * @brief Set the output buffer object
     *
     * @param output_buffer
     */
    static void set_output_buffer(std::ostream& output_buffer) {
        Logger::output_buffer = &output_buffer;
    }

    /**
     * @brief Set the is color output object
     *
     * @param is_color_output
     */
    static void set_is_color_output(bool is_color_output) {
        Logger::is_color_output = is_color_output;
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
            log("TRACE", ascii_colors::bold_white_color, format, args...);
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
            log("DEBUG", ascii_colors::bold_blue_color, format, args...);
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
            std::this_thread::sleep_for(std::chrono::seconds(1));
            log(" INFO", ascii_colors::bold_green_color, format, args...);
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
            log("ERROR", ascii_colors::bold_red_color, format, args...);
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
            log(" WARN", ascii_colors::bold_yellow_color, format, args...);
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
            log("FATAL", ascii_colors::background_red_color, format, args...);
            abort();
        }
    }
};

LogType Logger::log_type = LogType::All;
std::string Logger::output_file_name = std::string();
std::ostream* Logger::output_buffer = &std::cout;
bool Logger::is_color_output = true;
const char* Logger::datetime_format = "%Y-%m-%d %H:%M:%S";
std::mutex Logger::log_mutex;
std::ofstream Logger::output_file = std::ofstream();

/**
 * Special Thanks to:
 *  - Edward for answering at https://codereview.stackexchange.com/questions/288702/header-only-logging-library-in-c/288708#288708
 *  - spdlog for giving me this idea
 * ◝(^⌣^)◜
 */