#include <stdlib.h>
#include <pthread.h>

#include "log.h"

#define CHUNK_SIZE 16
#define NUM_CHUNKS ((state.size / CHUNK_SIZE) + 1)

enum active_state
{
    LOG_STATE_INACTIVE, /* Uninitialized */
    LOG_STATE_ACTIVE, /* Initialized and ready */
    LOG_STATE_DESTROY, /* Finish then cleanup */
    LOG_STATE_FATAL, /* Finish and kill */
    LOG_STATE_MAX,
    LOG_STATE_INVALID = LOG_STATE_MAX
};

struct logging_state
{
    int write_pos; /* Queueing */
    int read_pos; /* Dequeuing */
    int size;
    volatile enum active_state active;
};

/* Threading environment */
static pthread_mutex_t state_sync_mutex;
static pthread_t thread;

/* Logging state */
static struct log* queue;
static struct logging_state state;

/**
 * Retrieve the oldest log in the queue
 * @return Oldest log in queue if non-empty, else NULL
 * @note This function is only called in the logging thread, so it does not
 *  need to be atomically protected
 */
static struct log*
log_dequeue
(
    void
);

/**
 * Worker thread target that handles logging
 */
static void*
log_message
(
    void*
);

/**
 * Intermediate ISR that signals the logging thread to start tearing down
 * the logging framework
 * @param sig SIG_LOG_DESTROY must be passed to initiate destruction
 */
static void
_destroy_logger
(
    int sig
);

static void
handle_log_state_active
(
    void
);

static void
handle_log_state_destroy
(
    void
);

static void
handle_log_state_fatal
(
    void
);

void
log_enqueue
(
    struct log log
)
{
    if (state.active != LOG_STATE_ACTIVE)
        return;

    struct log* queue_copy;

    queue_copy = queue;

    pthread_mutex_lock(&state_sync_mutex);
    if (state.size == NUM_CHUNKS * CHUNK_SIZE)
    {
        if ((queue = realloc(
                queue_copy, sizeof(struct log) * (state.size + CHUNK_SIZE))))
        {
            free(queue_copy);
        }
        else
        {
            LOG_FATAL("Could not realloc queue: current size %d", state.size);
        }
    }
    state.size++;
    queue[state.write_pos] = log;
    state.write_pos = (state.write_pos + 1) % (CHUNK_SIZE * NUM_CHUNKS);
    pthread_mutex_unlock(&state_sync_mutex);
}

static struct log*
log_dequeue
(
    void
)
{
    /* todo: add realloc to shrink queue based on size to prevent memory blow up */
    struct log* log;

    pthread_mutex_lock(&state_sync_mutex);
    state.size--;
    log = &queue[state.read_pos];
    state.read_pos = (state.read_pos + 1) % (CHUNK_SIZE * NUM_CHUNKS);
    pthread_mutex_unlock(&state_sync_mutex);

    return log;
}

static void
handle_log_state_active
(
    void
)
{
    struct log* log;

    while (!state.size && state.active == LOG_STATE_ACTIVE)
        continue;

    log = log_dequeue();
    if (log)
        fprintf(log->stream, "%s", log->msg);
}

static void
handle_log_state_destroy
(
    void
)
{
    struct log* log;

    while (state.size && (log = log_dequeue()))
        fprintf(log->stream, "%s", log->msg);

    pthread_mutex_destroy(&state_sync_mutex);
    free(queue);
    pthread_exit(NULL);
}

static void
handle_log_state_fatal
(
    void
)
{
    struct log* log;

    while(state.size && (log = log_dequeue()))
        fprintf(log->stream, "%s", log->msg);
    pthread_exit(NULL);
}

static void*
log_message
(
    void* arg
)
{
    (void)arg; /* Stub */

    while (true)
    {
        switch (state.active)
        {
            case LOG_STATE_ACTIVE:
                handle_log_state_active();
                break;
            case LOG_STATE_DESTROY:
                handle_log_state_destroy();
                break;
            case LOG_STATE_FATAL:
                handle_log_state_fatal();
                break;
            default:
                LOG_FATAL("Invalid log state %u", state.active);
        }
    }

    return NULL;
}

static void
_destroy_logger
(
    int sig
)
{
    if (state.active == LOG_STATE_INACTIVE)
        return;
    
    if (sig == SIG_LOG_DESTROY)
    {
        state.active = LOG_STATE_DESTROY;
    }
    else if (sig == SIG_LOG_FATAL)
    {
        state.active = LOG_STATE_FATAL;
        pthread_exit(NULL);
    }
}

void
init_logger
(
    void
)
{
    if (!(queue = malloc(sizeof(struct log) * CHUNK_SIZE)))
        LOG_FATAL("Could not create logging queue");
    
    if (pthread_mutex_init(&state_sync_mutex, NULL))
        LOG_FATAL("Error initializing logging mutex");
    
    state.active = LOG_STATE_ACTIVE;
    if (pthread_create(&thread, NULL, log_message, NULL))
        LOG_FATAL("Error creating logging thread");
    if (pthread_detach(thread))
        LOG_FATAL("Error detaching logging thread");
    
    signal(SIG_LOG_DESTROY, _destroy_logger);
    signal(SIG_LOG_FATAL, _destroy_logger);

    LOG_OUT("Logger initialization finished");
}

void
destroy_logger
(
    void
)
{
    LOG_OUT("Logger destruction started");
    raise(SIG_LOG_DESTROY);
    pthread_exit(NULL);
}
