#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM 8
#define THREADS_NUMBER 4
#define PLAY_TIMES 50

int grid1[NUM][NUM];
int grid2[NUM][NUM];

int **newgrid = grid1;
int **grid    = grid2;

struct pos
{
    int row;
    int column;
};

typedef struct pos pos;

struct thread_t
{
    pthread_t t;
    pos startEnd[2];
    int threadNumber;
};

typedef struct thread_t thread_t;

void *parsing(void *args)
{

}

void swap(void)
{

}

void split(thread_t current, thread_t previous)
{
    int newStart[2] = {0,0};

    if(previous != NULL)
    {
        newStart[0] = previous.startEnd[1].row;
        newStart[1] = previous.startEnd[1].column;
    }

    current.startEnd[0].row = newStart[0];
    current.startEnd[0].column = newStart[1];

    current.startEnd[1].row = (newStart[0] + NUM * NUM / THREADS_NUMBER) % NUM;
    current.startEnd[1].column = (newStart[0] + NUM * NUM / THREADS_NUMBER) % NUM;;
}

void init(thread_t *parsersInit)
{
    thread_t *prev = NULL;

    for(int i = 0; i < THREADS_NUMBER; i++)
    {
        parsersInit[i].threadNumber = i;
        split(parsersInit[i], prev);
        prev = parsersInit[i];
    }

    for(int i = 0; i < NUM; i++)
    {

        for(int j = 0; j < NUM; j++)
        {

            grid1[i][j] = 0;
            grid2[i][j] = 0;

        }

    }

    /* Chamar aqui o código passado pelos professores para preencher a matriz inicial */
}

int main()
{
    thread_t sweepers[THREADS_NUMBER];

    init();

    for(int i = 0; i < PLAY_TIMES; i++)
    {

        for(int j = 0; j < THREADS_NUMBER; j++)
        {
            pthread_create(&sweepers[j].t, NULL, parsing, &sweepers[j].startEnd);
        }

        for(int j = 0; j < THREADS_NUMBER; j++)
        {
            pthread_join(sweepers[j].t, NULL);
        }

        /* Chamar o swap e o print aqui */
    }

    /* Chamar a contagem aqui */

    return 0;
}