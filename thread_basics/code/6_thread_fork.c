#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* func()
{
     int i;
     int rc = fork();
     if (rc < 0) {
          fprintf(stderr, "fork failed\n");
          exit(1);
     } else if (rc == 0) {
          // child (new process)
          printf("hello, I am child (pid:%d)     (thread ID: %d)\n", (int) getpid(), pthread_self());
	     sleep(1);
    } else {
        int wc = wait(NULL);
        printf("hello, I am parent of %d (wc:%d) (pid:%d)   (thread ID: %d)\n",
	       rc, wc, (int) getpid(), pthread_self());
    }
}
int main(){
    pthread_t tid;
    printf("%ld",pthread_self());
    pthread_create(&(tid), NULL, &(func),NULL);
    pthread_join(tid, NULL);
    return 0;
}