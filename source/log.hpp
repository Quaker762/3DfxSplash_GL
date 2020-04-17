/**
 * Portable logging class
 */
#pragma once

#include <cstdio>
#include <cstdarg>

enum class LogLevel
{
    INFO,
    WARN,
    ERROR,
    FATAL
};

inline void log(const LogLevel& level, const char* str, ...)
{
    std::FILE* log_file = stderr;
    std::va_list va_args;

    switch(level)
    {
    case LogLevel::INFO:
        fputs("[info] ", log_file);
        break;
    case LogLevel::WARN:
        fputs("[warn] ", log_file);
        break;
    case LogLevel::ERROR:
        fputs("[error] ", log_file);
        break;
    case LogLevel::FATAL:
        fputs("[fatal] ", log_file);
        break;       
    default:
        fputs("[info] ", log_file);
        break;
    }

    va_start(va_args, str);
    vfprintf(log_file, str, va_args);
    va_end(va_args);
}