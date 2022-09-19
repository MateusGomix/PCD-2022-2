MATEUS GOMES FERREIRA <gomes.mateus@unifesp.br>
	
sex., 16 de set. 22:43 (há 2 dias)
	
para mim
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM 8
#define THREADS_NUMBER 4

int grid1[NUM][NUM];
int grid2[NUM][NUM];

int **newgrid = grid1;
int **grid    = grid2;



int main()
{
    printf("Hello world!\n");
    return 0;
}
