#pragma once

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "Color.hpp"

enum class LogLevel {
    None,
    Error,
    Warn,
    Info,
    Debug,
    Success
};

/**
 * @brief Files can be outputted in two modes. This stores the preference option
 * to where to log the messages. This can be either the console or a file.
 */
enum class LogOutput {
    Console,
    File
};

/**
 * @brief  Logger Class Used to Output Details of Current Application Flow.
 */
class Logger {
   private:
    LogLevel      logLevel;    // the current output level of the logger
    LogOutput     logOutput;   // current output option for the logger
    std::ofstream logFile;     // if output is FILE, then path to the file
    std::mutex    threadLock;  // access file or console in thread safe manner

    /** Stores the level of the log as key and the corresponging name to be
     * printed as the value. */
    std::unordered_map<LogLevel, std::string> levelName = {
        {LogLevel::Info,    Color::FG_CYAN + "INFO : " + Color::FG_RESET    },
        {LogLevel::Debug,   Color::FG_MAGENTA + "DEBUG : " + Color::FG_RESET},
        {LogLevel::Warn,    Color::FG_YELLOW + "WARN : " + Color::FG_RESET  },
        {LogLevel::Error,   Color::FG_RED + "ERROR : " + Color::FG_RESET    },
        {LogLevel::Success, Color::FG_GREEN + "SUCCESS : " + Color::FG_RESET},
        {LogLevel::None,    Color::FG_DEFAULT + "NONE : " + Color::FG_RESET },
    };

    /**
     * @brief Stores the globally available instance of the Logger class.
     * Uses singleton design pattern.
     */
    static std::shared_ptr<Logger> loggerInstance;

    /**
     * @brief A function which generates the current time of the system in a
     * human readable format.
     *
     * @return Current time of the system in a string format.
     */
    const std::string getLocalTime() {
        std::tm localTime;

        // current time of the system
        auto currentTime = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(currentTime);
        localtime_r(&in_time_t, &localTime);

        // it was easier to manipulate, can also use other ways of string
        // formatting.
        std::stringstream date;
        date << localTime.tm_mday << "/" << 1 + localTime.tm_mon << "/"
             << 1900 + localTime.tm_year << " " << 1 + localTime.tm_hour << ":"
             << 1 + localTime.tm_min << ":" << 1 + localTime.tm_sec;

        return date.str();
    }
};
#endif