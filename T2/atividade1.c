/*
    Projeto 02: Manna-Pnueli
    *--------*-----------------------*
    |   RA   | Aluno                 |
    *--------*-----------------------*
    | 142477 | Luiz Felipe           |
    | 140886 | Mateus Gomes Ferreira |
    | 140729 | Rafael Nascimento     |
    *--------------------------------*
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define INCREASE_N 100
#define THREADS_N 2

int request = 0;
int respond = 0;
int SOMA = 0;

pthread_barrier_t barrier;

/* Stores information that each
 * thread needs when starts running
 */
struct thread_t{
    pthread_t threadsId;
    int threadNumber;
};

typedef struct thread_t thread_t;

void *client(void *args){
    int *thisThreadNumber = (int *) args;
    int i = *thisThreadNumber;

    //printf("t%d ", *thisThreadNumber);

    pthread_barrier_wait(&barrier);

    for (int j = 0; j < INCREASE_N; j++){
        //while (respond != i) request = i; //disable
        //critical section start
        int local = SOMA;
        sleep(rand()%2);
        SOMA = local + 1;
        printf("SOMA t%d i%d: %d\n", i, j, SOMA);
        //critical section end
        respond = 0;
    }

    pthread_exit(0);
}

void *server(void *args){
    for (int i = 0; i < INCREASE_N * THREADS_N; i++){
        while(request == 0);
        respond = request;
        while(respond != 0);
        request = 0;

        //printf("SOMA ATUAL: %d\n", SOMA);
    }

    pthread_exit(0);
}

void init(thread_t *array){
    // Sync barrier
    pthread_barrier_init(&barrier, NULL, THREADS_N);
    // Server thread
    pthread_create(&array[0].threadsId, NULL, server, NULL);
    // Client threads
    for (int i = 0; i < THREADS_N; i++){
        array[i + 1].threadNumber = i + 1;
        //printf("%d ", array[i].threadNumber);
        pthread_create(&array[i + 1].threadsId, NULL, client, &array[i + 1].threadNumber);
    }
}

int main(void){
    /* Intializes random number generator */
    time_t t;
    srand((unsigned) time(&t));

    thread_t threadsArray[THREADS_N + 1];

    init(threadsArray);

    /*for(int i = 0; i < THREADS_N; i++){
        //printf("%d ", threadsArray[i].threadNumber);
        pthread_create(&threadsArray[i].threadsId, NULL, increase, &threadsArray[i].threadNumber);
    }*/

    /*for (int i = 0; i < INCREASE_N * THREADS_N; i++){
        while(request == 0);
        respond = request;
        while(respond != 0);
        request = 0;

        //printf("SOMA: %d\n\n", SOMA);
    }*/

    for (int i = 0; i < THREADS_N + 1; i++) pthread_join(threadsArray[i].threadsId, NULL);

    printf("SOMA FINAL: %d\n\n", SOMA);

    return 0;
}
