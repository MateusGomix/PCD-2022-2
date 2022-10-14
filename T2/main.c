#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>

#define INCREASE_N 2
#define THREADS_N 4

int request = 0;
int respond = 0;
int SOMA = 0;

/* Stores information that each
 * thread needs when starts running
 */
struct thread_t{
    pthread_t threadsId;
    int threadNumber;
};

typedef struct thread_t thread_t;

void *increase(void *args){
    int *thisThreadNumber = (int *) args;
    int i = *thisThreadNumber;

    printf("%d", i);

    for (int j = 0; j < INCREASE_N; j++){
        while (respond != i) request = i;
        //critical section start
        int local = SOMA;
        //sleep(rand()%2);
        SOMA = local + 1;
        //critical section end
        respond = 0;
    }

    pthread_exit(0);
}

void init(thread_t *array){
    for (int i = 0; i < THREADS_N; i++){
        array[i].threadNumber = i;
        pthread_create(&array[i].threadsId, NULL, increase, &array[i].threadNumber);
    }
}

int main(void){
    printf("c");
    thread_t threadsArray[THREADS_N];
    printf("a");
    init(threadsArray);
    /*for(int i = 0; i < THREADS_N; i++){
        pthread_create(&threadsArray[i].threadsId, NULL, increase, &threadsArray[i].threadNumber);
    }*/
    printf("b");
    for (int i = 0; i < INCREASE_N * THREADS_N; i++){
        while(request == 0){}
        respond = request;
        while(respond != 0){}
        request = 0;
    }

    for (int i = 0; i < INCREASE_N * THREADS_N; i++) pthread_join(threadsArray[i].threadsId, NULL);

    printf("%d\n\n", SOMA);

    return 0;
}
