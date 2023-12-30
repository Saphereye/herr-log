/**
 * @file herr-log.hh
 * @author Saphereye
 * @brief Header file only loggin library
 * @version 0.1
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#pragma GCC diagnostic ignored "-Wformat-security"

#include <chrono>
#include <iostream>
#include <ostream>

#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[1;31m"
#define GREEN_COLOR "\033[1;32m"
#define YELLOW_COLOR "\033[1;33m"
#define BLUE_COLOR "\033[1;34m"

enum class Style {
    Trace = 1,       // For specific details
    Debug = 1 << 1,  // Temporary debug printing
    Info = 1 << 2,   // General purpose details, e.g. "Program has started"
    Error = 1 << 3,  // For errors

    All = (1 << 4) - 1,
    None = 0,
};

Style operator|(Style a, Style b) {
    return static_cast<Style>(static_cast<uint8_t>(a) |
                              static_cast<uint8_t>(b));
}

bool is_set_style(Style target, Style reference) {
    return (static_cast<uint8_t>(target) & static_cast<uint8_t>(reference)) !=
           0;
}

class Logger {
   private:
    Style style;
    std::ostream& output_stream;
    bool color_output = false;

    template <typename... Args>
    void print_line_heading(const char* name, const char* color,
                            const char* format, Args... args) {
        auto current_time_point = std::chrono::system_clock::now();
        std::time_t current_time =
            std::chrono::system_clock::to_time_t(current_time_point);
        char time_string[100];
        std::strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&current_time));

        output_stream << (color_output ? color : "") << "[" << name << " "
                      << time_string << "]" << (color_output ? RESET_COLOR : "") << " ";
        output_stream << format;
        output_stream << "\n";
    }

   public:
    Logger(Style style = Style::All, std::ostream& output_stream = std::cout)
        : style(style), output_stream(output_stream) {
        color_output = (&output_stream == &std::cout);
    }

    template <typename... Args>
    void trace(const char* format, Args... args) {
        if (is_set_style(style, Style::Trace)) {
            print_line_heading("TRACE", BLUE_COLOR, format, args...);
        }
    }

    template <typename... Args>
    void debug(const char* format, Args... args) {
        if (is_set_style(style, Style::Debug)) {
            print_line_heading("DEBUG", GREEN_COLOR, format, args...);
        }
    }

    template <typename... Args>
    void info(const char* format, Args... args) {
        if (is_set_style(style, Style::Info)) {
            print_line_heading(" INFO", YELLOW_COLOR, format, args...);
        }
    }

    template <typename... Args>
    void error(const char* format, Args... args) {
        if (is_set_style(style, Style::Error)) {
            print_line_heading("ERROR", RED_COLOR, format, args...);
        }
    }
};

#pragma GCC diagnostic warning "-Wformat-security"