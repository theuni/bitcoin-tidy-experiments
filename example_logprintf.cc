// Copyright (c) 2022 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Warn about any use of LogPrintf that does not end with a newline.
#include <string>

template <typename... Args>
static inline void LogPrintf_(const std::string& logging_function, const std::string& source_file, const int source_line, const char* fmt, const Args&... args)
{
}

#define LogPrintf(...) LogPrintf_(__func__, __FILE__, __LINE__, __VA_ARGS__)

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
void bad_func()
{
    LogPrintf("hello world!");
}
