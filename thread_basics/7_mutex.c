#include <stdio.h>
#include <pthread.h>
#define count_threads 10

typedef struct {
	int wid;
} t_arg;

pthread_mutex_t lock;
int tail;
int arr[count_threads];

void *worker_func(void *_arg)
{
	t_arg *arg = (t_arg *)_arg;
	pthread_mutex_lock(&lock);
	arr[tail] = arg->wid;
	printf("wid: %d\n", arg->wid);
	tail++;
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}

int main()
{
	pthread_mutex_init(&lock, NULL);
	pthread_t threads[count_threads];
 	t_arg args[count_threads];
	int i;

	tail = 0;
	for (i = 0; i < count_threads; i++) {
		arr[i] = 0;
	}

	for (i = 0; i < count_threads; i++) {
		args[i].wid = i;
		pthread_create(&threads[i], NULL,
				worker_func, (void *)&args[i]);
	}

	for (i = 0; i < count_threads; i++)
		pthread_join(threads[i], NULL);

	// print result
	for (i = 0; i < count_threads; i++)
		printf("%d: wid: %d\n", i, arr[i]);

	pthread_mutex_destroy(&lock);
	return 0;
}