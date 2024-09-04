#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#define _MSG_MAX_LEN 1024

#define SIG_LOG_DESTROY SIGUSR1
#define SIG_LOG_FATAL SIGUSR2

#define _GET_TIME() \
    time_t time_since_epoch = time(NULL); \
    struct tm* local = localtime(&time_since_epoch)

#ifndef LOGGING_ENABLED
#define LOG_OUT(fmt, ...) (void)0

#define LOG_NONFATAL(fmt, ...) (void)0

#define LOG_FATAL(fmt, ...) (void)0
#else

#define LOG_OUT(fmt, ...) \
    do \
    { \
        struct log log; \
    \
        log.stream = stdout; \
        _GET_TIME(); \
        snprintf(log.msg, _MSG_MAX_LEN, "[%02d:%02d:%02d]\t"fmt"\n", \
            local->tm_hour, local->tm_min, local->tm_sec, \
            ##__VA_ARGS__); \
        log_enqueue(log); \
    } while (0)

#define LOG_NONFATAL(fmt, ...) \
    do \
    { \
        struct log log; \
    \
        log.stream = stderr; \
        _GET_TIME(); \
        snprintf(log.msg, _MSG_MAX_LEN, "[%02d:%02d:%02d]\t"fmt"\n", \
            local->tm_hour, local->tm_min, local->tm_sec, \
            ##__VA_ARGS__); \
        log_enqueue(log); \
    } while (0)

#define LOG_FATAL(fmt, ...) \
    do \
    { \
        LOG_NONFATAL("FATAL ERROR: "fmt, ##__VA_ARGS__); \
        raise(SIG_LOG_FATAL); \
    } while (0)
#endif /* LOGGING_ENABLED */

struct log
{
    FILE* stream;
    char msg[_MSG_MAX_LEN];
};

/**
 * Enqueue a log into the logging queue
 * @param log The log to enqueue
 * @note Throws exception on error
 * @note This function can be called in many contexts, so writing to the state
 *  must be protected
 */
void
log_enqueue
(
    struct log
);

void
init_logger
(
    void
);

void
destroy_logger
(
    void
);

#endif /* __LOGGER_H__ */
