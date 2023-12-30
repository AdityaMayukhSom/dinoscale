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

#ifdef __linux__
#include <sys/stat.h>
#include <unistd.h>
#endif

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
 *
 * @todo Add a maximum log file size attribute, and whenever that file size is
 * reached, create a new file and start writing logs in the new file. This
 * attribute should be modifiable by the SetPreference function.
 *
 * @todo Create a class or Structure for file preference instead of passing
 * default values. The user will pass the pointer or reference to the struct
 * itself, which can be then parsed and used to set up the preference.
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

    /**
     * Write out the message to target output
     * @param message : String message
     * @return void
     */
    void writeln(const std::string& message) {
        if (logOutput == LogOutput::File) {
            logFile << message;
        } else {
            std::cout << message;
        }
    }

   public:
    /**
     * Get Single Logger Instance or Create new Object if Not Created
     * @return std::shared_ptr<Logger>
     */
    static std::shared_ptr<Logger> GetInstance() {
        if (loggerInstance == nullptr) {
            loggerInstance = std::shared_ptr<Logger>(new Logger());
        }

        return loggerInstance;
    };

    /**
     * Configure Logger Preferences
     * @param level: LogLevel::ERROR by Default
     * @param output: LogOutput::CONSOLE by Default
     * @param logFileName: Path to the output logging file, this parameter does
     * nothing if output type is set to console. If output type is set to file,
     * and no parameter is passed, framework will automatically create a log
     * directory in the root folder and put log files inside that directory.
     */
    void SetPreferences(LogLevel    level = LogLevel::Error,
                        LogOutput   output = LogOutput::Console,
                        std::string logFileName = "") {
        this->logLevel = level;    // sets the level of the log
        this->logOutput = output;  // sets where to output the logs

        if (output == LogOutput::Console) {
            // if output is console, we do not require to setup output file
            return;
        }

        // handle when output is console
        if (logFileName.empty()) {
            // (mkdir("/log", 0777));
        }

        auto logFileOpenMode =
            std::fstream::out | std::fstream::app | std::fstream::ate;
        logFile.open(logFileName, logFileOpenMode);

        if (!logFile.good()) {
            std::cerr << "Cannot open log file." << std::endl;
            std::cerr << "Changing default logging to console." << std::endl;
            this->logOutput = LogOutput::Console;
        }
    }

    /**
     * Log given message with defined parameters and generate message to pass on
     * Console or File, as set by preference. Level set by SetPreference will be
     * used.
     * @param message: Message to be logged.
     */
    void Log(std::string message) { Log(message, this->logLevel); }

    /**
     * Log given message with defined parameters and generate message to pass on
     * Console or File.
     *
     * @param message: Log Message
     * @param messageLevel: Level to be used by the logger. This does not change
     * the level set by SetPreference.
     *
     * @todo In future we can have a proposition of quitting the application if
     * log level error message is displayed. Need to give it a good thought.
     * Maybe have a flag to check if the user wants to quit the application
     * automatically if error level logs are written.
     */
    void Log(std::string message, LogLevel messageLevel) {
        auto time = getLocalTime();
        // we could have checked if the messageLevel
        // existed on the map, but as it is an enum class,
        // int values as argument are not allowed, hence
        // only valid keys are passed, thus not checking.
        auto level = levelName.find(messageLevel)->second;
        auto logstring =
            std::string('[' + time + ']' + '[' + level + ']' + message + '\n');

        threadLock.lock();
        writeln(logstring);
        threadLock.unlock();
    }

    /**
     * @brief Destroy the Logger object and closes any opened logFile before
     * destruction.
     */
    ~Logger() {
        if (this->logFile.is_open()) {
            this->logFile.flush();
            this->logFile.close();
        }
    }
};
#endif