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

#endif