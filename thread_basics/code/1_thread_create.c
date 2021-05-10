#include <assert.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {int a; int b;} myarg_t;

void *my_thread_funtion(void *arg) {
    myarg_t *args = (myarg_t *) arg;
    printf("%d %d\n", args->a, args->b);
    return NULL;
}
int main(int argc, char *argv[]) {
    // pthread_t:   thread ID  => pthread_t is the data type used to uniquely identify a thread
    pthread_t thread_id;
    myarg_t args = { 10, 20 };
    int new_thread_status = pthread_create(&thread_id, NULL, *my_thread_funtion, &args);
    assert(new_thread_status == 0);                        // 0: success      ---     any number: error
    pthread_join(thread_id, NULL);                         // wait for termination of another thread
                                                           // 0: sucess       ---     any number: error
    printf("done\n"); 
    return 0;
}

