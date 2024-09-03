#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define _GET_TIME() \
    time_t time_since_epoch = time(NULL); \
    struct tm* local = localtime(&time_since_epoch)

#ifndef LOGGING_ENABLED
#define LOG_OUT(fmt, ...) (void)0
#define LOG_ERR(fmt, ...) (void)0
#else
#define LOG_OUT(fmt, ...) \
    do \
    { \
        _GET_TIME(); \
        fprintf(stdout, "[%02d:%02d:%02d]\t"fmt"\n", \
            local->tm_hour, local->tm_min, local->tm_sec \
            ##__VA_ARGS__); \
    } while (0)
#define LOG_ERR(fmt, ...) \
    do \
    { \
        _GET_TIME(); \
        fprintf(stderr, "[%02d:%02d:%02d]\t"fmt"\n", \
            local->tm_hour, local->tm_min, local->tm_sec \
            ##__VA_ARGS__); \
    } while (0)
#endif /* LOGGING_ENABLED */

#endif /* __LOGGER_H__ */
