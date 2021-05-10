/* Operating Systems Course - Fall 2020
 * Iran University of Science and Technology
 *
 * The goal of this program is to calculate the summation of values in
 * the data directory in modulos 1e9+7 using multiple threads.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

#define M (1000000000+7)
#define MAX_FILE_PATH_LEN 128
#define COUNT_WORK_QUEUES 10
#define MAX_SIZE_WORK_QUEUES 8192


// ============================================================
// define a thread safe ring buffer
typedef struct {
    int *queue;
    int head;
    int tail;
    volatile int size;
    int capacity;
} ring_buf_t;

ring_buf_t *new_ring_buf(int capacity)
{
    ring_buf_t *ring = calloc(1, sizeof(ring_buf_t));
    assert(ring != NULL);
    ring->queue = calloc(capacity, sizeof(int));
    assert(ring->queue != NULL);
    ring->capacity = capacity;
    return ring;
}

void free_ring_buf(ring_buf_t *ring)
{
    free(ring->queue);
    free(ring);
}

/* Add a new value to the ring buffer (thread safe)
 * Returns:
 * zero (0) if succeed else non-zero value is returned.
 * */
int ring_buf_push(ring_buf_t *ring, int value)
{
    // Check if buffer is full
    if (ring->size == ring->capacity) {
        // printf("failed push, %d/%d\n", ring->size, ring->capacity);
        return 1;
    }

    ring->queue[ring->head] = value;
    ring->head = (ring->head + 1) % ring->capacity;
    ring->size += 1;
    // printf("successful push\n");
    return 0;
}

/*
 * Retrieve value from ring (thread safe)
 * Returns:
 * zero if succeed else non-zero value.
 * */
int ring_buf_pop(ring_buf_t *ring, int *value)
{
    // Check if buffer is empty
    if (ring->size == 0) {
        return 1;
    }

    *value = ring->queue[ring->tail];
    ring->tail = (ring->tail + 1) % ring->capacity;
    ring->size -= 1;
    return 0;
}

// =============================================================

typedef struct {
    ring_buf_t **rings;
} producer_arg_t;

typedef struct {
    // consumer runs until this is set to non-zero
    volatile int run;
    // consumer poll this ring for values
    ring_buf_t *ring;
    // worker stores the result in this field
    int result;
} consumer_arg_t;


int mod(long int num, int m)
{
    int tmp = num % m;
    if (tmp < 0) tmp += m;
    return tmp;
}

void *producer(void *_arg)
{
    producer_arg_t *arg = _arg;
    ring_buf_t **rings = arg->rings;
    int i, j;
    int rc;
    long int sum = 0;

    // iterat over files in data directory
    for (i = 0; i < 100000; i++) {
        do {
            rc = 1;
            for (j = 0; j < COUNT_WORK_QUEUES; j++) {
            // write data to work queues in a round robin fashion
                rc = ring_buf_push(rings[j], i);
                if (rc == 0)
                    break;
            }
        } while(rc != 0);
        sum += i;
        sum = mod(sum, M);
    }
    printf("producer finished its job (ans: %ld)\n", sum);
    pthread_exit(NULL);
}

void *consumer(void *_arg)
{
    consumer_arg_t *arg = _arg;
    int input;
    int rc;
    long int sum = 0;

    while (arg->run) {
        rc = ring_buf_pop(arg->ring, &input);
        if (rc == 0) {
            sum += input;
            sum = mod(sum, M);
        }
    }
    arg->result = sum;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    const int count_threads = 10;
    assert (count_threads > 1);

    pthread_t threads[count_threads];
    producer_arg_t p_arg;
    consumer_arg_t c_args[count_threads];
    int rc; // return code

    int i;
    int child_return;
    long int sum = 0;

    struct timespec before, after;
    long int total_time;

    // set producer thread arguments
    p_arg.rings = malloc(COUNT_WORK_QUEUES * sizeof(ring_buf_t));
    for (i = 0; i < COUNT_WORK_QUEUES; i++) {
        p_arg.rings[i] = new_ring_buf(MAX_SIZE_WORK_QUEUES);
    }

    // get time
    clock_gettime(CLOCK_MONOTONIC, &before);

    for (i = 0; i < count_threads - 1; i++) {
        // what files this child is going to process
        c_args[i].run = 1;
        c_args[i].ring = p_arg.rings[i % COUNT_WORK_QUEUES];

        rc = pthread_create(&threads[i], NULL, consumer, (void *)(c_args+i));
        if (rc != 0) {
            printf("failed to create a thread\n");
        }

        // printf("consumer thread %d\n", i);
    }

    // create producer thread
    rc = pthread_create(&threads[count_threads - 1], NULL,
            producer, (void *)(&p_arg));
    if (rc != 0) {
        printf("failed to create producer thread\n");
        return -1;
    }
    // printf("producer thread created\n");

    // wait until threads do the job
    pthread_join(threads[count_threads - 1], NULL);
    for (i = 0; i < count_threads - 1; i++) {
        while (p_arg.rings[i % COUNT_WORK_QUEUES]->size > 0) {
            // wait
            // busy waiting
        }
        c_args[i].run = 0;
        pthread_join(threads[i], NULL);
    }
    // printf("workers are done\n");

    for (i = 0; i < count_threads - 1; i++) {
        child_return = c_args[i].result;
        // printf("read from pipe: %d\n", child_return);
        sum += child_return;
        sum = mod(sum, M);
    }

    // get after
    clock_gettime(CLOCK_MONOTONIC, &after);

    total_time = (after.tv_sec - before.tv_sec) * 1000;
    total_time += (after.tv_nsec - before.tv_nsec) / 1000000;

    printf("our result is %ld\n", sum);
    printf("duration %ld (ms)\n", total_time);
    return 0;
}