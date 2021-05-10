#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

typedef struct {int a; int b;} myarg_t;
typedef struct {int x; int y;} myret_t;
// heap is part of shared memory of threads.

void *mythread(void *arg) {
    myarg_t *args = (myarg_t *) arg;
    printf("args %d %d\n", args->a, args->b);
    myret_t *rvals = malloc(sizeof(myret_t));
    assert(rvals != NULL);  // if conditon is True => return error
    rvals->x = 1;
    rvals->y = 2;
    return (void *) rvals;
}
int main(int argc, char *argv[]) {
    pthread_t thread;
    myret_t *rvals;
    myarg_t args = { 10, 20 };
    pthread_create(&thread, NULL, mythread, &args);
    pthread_join(thread, (void **) &rvals);
    printf("returned %d %d\n", rvals->x, rvals->y);
    free(rvals);
    return 0;
}

