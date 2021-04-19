#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "pthread.h"

void *MyThreadFunc(void *args){
	printf("from thread:: thread is running.....\nfrom thread:: function args: %s\n",(char *)args);
	srand(time(NULL));
	int rnd=rand()%6;
	sleep(rnd);
	printf("from thread:: thread execution lasts %d second(s)\n",rnd);
	pthread_exit("goodbye world");
}
void main(){
	pthread_t thread_id;
	void *thread_result;
	if(pthread_create(&thread_id,NULL,MyThreadFunc,"Hello World")==0){
		printf("from main:: waiting for thread %ld  .....\n\n",thread_id);
		if(pthread_join(thread_id,&thread_result)==0){
			printf("\nfrom main:: thread return value: %s\n",(char *)thread_result);
		}
	}
}