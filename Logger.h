#ifndef SIMPLERENDER_LOGGER_H
#define SIMPLERENDER_LOGGER_H

#include <string>
#include <stdio.h>

namespace sr
{
    enum class LogLevel
    {
        Critical = 1,
        Info
    };

    class Logger
    {
    public:
        template<LogLevel level, typename ...Args>
        static void log(Args... args)
        {
            std::string container("Level: ");
            container.append(std::to_string(static_cast<unsigned>(level)));
            log(container, args...);
        }
    private:
        template<typename T, typename ...Args>
        static void log(std::string& container, T val1, Args... args)
        {
            container.append(" ");
            container.append(std::to_string(val1));
            log(container, args...);
        }
        template<typename T>
        static void log(std::string& container, T valN)
        {
            container.append(" ");
            container.append(std::to_string(valN));
            printf("LOG: %s\r\n", container.c_str());
        }
    };
}

#endif