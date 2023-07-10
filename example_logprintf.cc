// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Warn about any use of LogPrintf that does not end with a newline.
#include <string>

enum LogFlags {
    NONE
};

enum Level {
    None
};

template <typename... Args>
static inline void LogPrintf_(const std::string& logging_function, const std::string& source_file, const int source_line, const LogFlags flag, const Level level, const char* fmt, const Args&... args)
{
}

#define LogPrintLevel_(category, level, ...) LogPrintf_(__func__, __FILE__, __LINE__, category, level, __VA_ARGS__)
#define LogPrintf(...) LogPrintLevel_(LogFlags::NONE, Level::None, __VA_ARGS__)

// Use a macro instead of a function for conditional logging to prevent
// evaluating arguments when logging for the category is not enabled.
#define LogPrint(category, ...)              \
    do {                                     \
        LogPrintf(__VA_ARGS__);          \
    } while (0)

void good_func()
{
    LogPrintf("hello world!\n");
}
void good_func2()
{
    LogPrintf("hello world!...");
}
void bad_func()
{
    LogPrintf("hello world!");
}
void bad_func2()
{
    LogPrintf("hello world!..");
}
