#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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

    //printf("t%d ", *thisThreadNumber);

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
    int i = 0;
    for (i = 0; i < THREADS_N; i++){
        array[i].threadNumber = i;
        //printf("%d ", array[i].threadNumber);
        pthread_create(&array[i].threadsId, NULL, increase, &array[i].threadNumber);
    }
}

int main(void){
    //printf("a\n");
    thread_t threadsArray[THREADS_N];
    //printf("b\n");
    init(threadsArray);
    //printf("c\n");

    /*for(int i = 0; i < THREADS_N; i++){
        //printf("%d ", threadsArray[i].threadNumber);
        pthread_create(&threadsArray[i].threadsId, NULL, increase, &threadsArray[i].threadNumber);
    }*/
    //printf("d\n");
    for (int i = 0; i < INCREASE_N * THREADS_N; i++){
        while(request == 0);
        respond = request;
        while(respond != 0);
        request = 0;

        printf("SOMA: %d\n\n", SOMA);
    }

    for (int i = 0; i < THREADS_N; i++) pthread_join(threadsArray[i].threadsId, NULL);

    printf("SOMA: %d\n\n", SOMA);

    return 0;
}
