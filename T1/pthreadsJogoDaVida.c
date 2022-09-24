#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM 10
#define THREADS_NUMBER 4
#define PLAY_TIMES 4

int **grid1;
int **grid2;

int **newgrid;
int **grid;

struct pos{
    int row;
    int column;
};

typedef struct pos pos;

struct thread_t{
    pthread_t t;
    pos startEnd[2];
    int threadNumber;
};

typedef struct thread_t thread_t;

int numero_vizinhos(int x, int y)
{
    int total = 0;

    if(x - 1 >= 0)
    {
        total += grid[x-1][y];

        if(y - 1 >= 0)
            total += grid[x-1][y-1];
    }

    if(y - 1 >= 0)
    {
        total += grid[x][y-1];

        if(x + 1 < NUM)
            total += grid[x+1][y-1];
    }

    if(x + 1 < NUM)
    {
        total += grid[x+1][y];

        if(y + 1 < NUM)
            total += grid[x+1][y+1];
    }

    if(y + 1 < NUM)
    {
        total += grid[x][y+1];

        if(x - 1 >= 0)
            total += grid[x-1][y+1];
    }

    return total;
}

void *parsing(void *args){

    //printf("AAAA");

    pos *aux = (pos *) args;

    pos currentCell;

    currentCell.row = aux[0].row;
    currentCell.column = aux[0].column;

    for(int i = 0; i < (NUM * NUM) / THREADS_NUMBER; i++){
        int neighborsNumber = numero_vizinhos(currentCell.row, currentCell.column);

        if(grid[currentCell.row][currentCell.column] == 1){
            if(neighborsNumber != 2 || neighborsNumber != 3){
                newgrid[currentCell.row][currentCell.column] = 0;
            }
        }
        else{
            if(neighborsNumber == 3){
                newgrid[currentCell.row][currentCell.column] = 1;
            }
        }
        if((currentCell.column + 1) >= NUM){
            currentCell.row = (currentCell.row + 1) % NUM;
        }
        currentCell.column = (currentCell.column + 1) % NUM;
    }

    pthread_exit(0);

}

void swap(void){

    int **aux = grid;

    grid = newgrid;
    newgrid = aux;

}

void split(thread_t *current, thread_t *previous){
    int newStart[2] = {0,0};

    if(previous != NULL){
        newStart[0] = previous->startEnd[1].row;
        newStart[1] = previous->startEnd[1].column;
    }

    current->startEnd[0].row = newStart[0];
    current->startEnd[0].column = newStart[1];

    current->startEnd[1].row = (newStart[0] + NUM * NUM / THREADS_NUMBER) % NUM;
    current->startEnd[1].column = (newStart[1] + NUM * NUM / THREADS_NUMBER) % NUM;

    return;
}

void init(thread_t *parsersInit){
    thread_t *prev = NULL;

    for(int i = 0; i < THREADS_NUMBER; i++){
        parsersInit[i].threadNumber = i;
        split(&parsersInit[i], prev);
        prev = &parsersInit[i];
    }

    for(int i = 0; i < NUM; i++){
        for(int j = 0; j < NUM; j++){

            grid[i][j] = 0;
            newgrid[i][j] = 0;
        }
    }

    //GLIDER
    int lin = 1, col = 1;
    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;

    //R-pentomino
    /* =10; col = 30;
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;*/

    return;

}

int count(void){
    int total = 0;

    for(int i = 0; i < NUM; i++){
        for(int j = 0; j < NUM; j++){
            total += grid[i][j];
        }
    }

    return total;
}

void printGrid(void){

    for(int i = 0; i < NUM; i++){
        for(int j = 0; j < NUM; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    printf("------------------------------------------------------------------------------\n");

    sleep(5);

}

int main(){

    grid1 = (int**) malloc(NUM * sizeof(int*));
    grid2 = (int**) malloc(NUM * sizeof(int*));

    for(int i = 0; i < NUM; i++){
        grid1[i] = (int*) malloc(NUM * sizeof(int));
        grid2[i] = (int*) malloc(NUM * sizeof(int));
    }

    newgrid = grid1;
    grid    = grid2;

    thread_t sweepers[THREADS_NUMBER];

    init(sweepers);

    printGrid();

    for(int i = 0; i < PLAY_TIMES; i++){

        for(int j = 0; j < THREADS_NUMBER; j++){
            pthread_create(&sweepers[j].t, NULL, parsing, &sweepers[j].startEnd);
        }

        for(int j = 0; j < THREADS_NUMBER; j++){
            pthread_join(sweepers[j].t, NULL);
        }

        swap();

        printGrid();
    }

    printf("%d", count());

    return 0;
}