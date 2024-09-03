#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef LOGGING_ENABLED
#define LOG_OUT(fmt, ...) (void)0
#define LOG_ERR(fmt, ...) (void)0
#else
#define LOG_OUT(fmt, ...) \
    fprintf(stdout, fmt"\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) \
    fprintf(stderr, fmt"\n", ##__VA_ARGS__)
#endif /* LOGGING_ENABLED */

#endif /* __LOGGER_H__ */
