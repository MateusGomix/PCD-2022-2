/*
    Projeto 01: Jogo da vida com pthreads
    *--------*-----------------------*
    |   RA   | Aluno                 |
    *--------*-----------------------*
    | 140886 | Mateus Gomes Ferreira |
    *--------------------------------*

    Usage: 
    -> Compiled as: gcc -Wall pthreadJogoDaVida.c -o pthreadJogoDaVida 
    -> Executed as: ./pthreadJogoDaVida -lpthread

    Function:
    -> 
*/


/*##############< Libs Include >#############*/
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

/*##############< Size Parameters >#############*/
#define NUM 2048
#define THREADS_NUMBER 4
#define PLAY_TIMES 2000

/*##############< Mode controller >#############*/
#define HIGH_LIFE 1

/* Grid global pointers */
int **newgrid;
int **grid;

pthread_barrier_t barrier;

/* Stores the position of a cell
 * from the grids
 */
struct pos{
    int row;
    int column;
};

typedef struct pos pos;

/* Stores information that each
 * thread needs when starts running
 */
struct thread_t{
    pthread_t t;
    //pos start;
    int threadNumber;
};

typedef struct thread_t thread_t;

/*##############< Custom Methods Headers >#############*/
int numero_vizinhos(int** matriz, int x, int y);
void swap(void);
void *parsing(void *args);
void split(thread_t *current);
void init(thread_t *parsersInit);
int count(void);
void printGrid(void);

/*##############< Main Program >#############*/
int main(){
    struct timeval inicio, final2;

    int **grid1;
    int **grid2;
    int tmili;

    grid1 = (int**) malloc(NUM * sizeof(int*));
    grid2 = (int**) malloc(NUM * sizeof(int*));

    for(int i = 0; i < NUM; i++){
        grid1[i] = (int*) malloc(NUM * sizeof(int));
        grid2[i] = (int*) malloc(NUM * sizeof(int));
    }

    newgrid = (int **) grid1;
    grid    = (int **) grid2;

    thread_t sweepers[THREADS_NUMBER];
    //gettimeofday(&inicio, NULL);
    init(sweepers);
    //gettimeofday(&final2, NULL);

    //tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

    //printf("tempo decorrido da init: %d milisegundos\n", tmili);

    //printGrid();

    pthread_barrier_init(&barrier, NULL, THREADS_NUMBER);

    gettimeofday(&inicio, NULL);

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_create(&sweepers[j].t, NULL, parsing, &sweepers[j]);
    }

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_join(sweepers[j].t, NULL);
    }

    gettimeofday(&final2, NULL);

    //printGrid();

    tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

    if(HIGH_LIFE) printf("< HIGHLIFE >\n");
    else printf("< JOGO DA VIDA >\n");

    printf("Dimensões: %dx%d \nThreads: %d \nGerações: %d \n------------------------- \n", NUM, NUM, THREADS_NUMBER, PLAY_TIMES);
    
    printf("Tempo paralelo: %.3fs\n", tmili/1000.00);

    printf("Somatório: %d\n\n", count());

    return 0;
}

/*##############< Custom Methods >#############*/

/* Return the number of alive neighbors
 * from a given cell in the grid
 */
int numero_vizinhos(int** matriz, int x, int y)
{
    int total;
    int yc = (y - 1)%NUM, yb = (y + 1)%NUM;
    int xe = (x - 1)%NUM, xd = (x + 1)%NUM;

    if(xe < 0) xe = NUM-1;
    //else if(xd >= NUM) xd = 0;

    if(yc < 0) yc = NUM-1;
    //else if(yb >= NUM) yb = 0;

    total =   matriz[x][yc]
            + matriz[x][yb]
            + matriz[xe][y]
            + matriz[xd][y]
            + matriz[xe][yc]
            + matriz[xe][yb]
            + matriz[xd][yc]
            + matriz[xd][yb];


    return total;
}

/* Swaps the pointers to the grids
 */
void swap(void){

    int **aux = grid;

    grid = newgrid;
    newgrid = aux;

    return;
}

/* Method that implements the thread
 * - it runs the grid from its start position
 * and updates the corresponding cell in the newgrid
 */
void *parsing(void *args){
    thread_t *aux = (thread_t *) args;

    for(int k = 0; k < PLAY_TIMES; k++){
        for(int i = aux->threadNumber; i < NUM; i += THREADS_NUMBER){
            for(int j = 0; j < NUM; j++){

                int neighborsNumber = numero_vizinhos(grid, i, j);

                /*if(grid[i][j] == 1){
                    if(neighborsNumber == 2 || neighborsNumber == 3){
                        newgrid[i][j] = 1;
                    }
                    else{
                        newgrid[i][j] = 0;
                    }
                }
                else{
                    if(neighborsNumber == 3 || (HIGH_LIFE && neighborsNumber == 6)){
                        newgrid[i][j] = 1;
                    }
                    else{
                        newgrid[i][j] = 0;
                    }
                }*/

                if(neighborsNumber == 3 || (neighborsNumber == 2 && grid[i][j] == 1) || (neighborsNumber == 6 && HIGH_LIFE && grid[i][j] == 0)){
                    newgrid[i][j] = 1;
                }else
                    newgrid[i][j] = 0;
            }
        }

        pthread_barrier_wait(&barrier);

        if(aux->threadNumber == 0) swap();

        pthread_barrier_wait(&barrier);

    }

    pthread_exit(0);
}

/* Sets the starting cell for each thread
 */
void split(thread_t *current){

    //current->start.column = current->threadNumber * ((NUM * NUM / THREADS_NUMBER) % NUM);
    //current->start.row = current->threadNumber * (NUM * NUM / THREADS_NUMBER) / NUM;

    return;
}

/* Initiates the threads structure
 * with a identifier number and its starting cell
 * also fills the grids with 0s
 * and creates the initial population
 */
void init(thread_t *parsersInit){

    for(int i = 0; i < THREADS_NUMBER; i++){
        parsersInit[i].threadNumber = i;
        //split(&parsersInit[i]);
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
    lin = 10; col = 30;
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;

    return;
}

/* Returns the number of alive cells in the grid
 */
int count(void){
    int total = 0;

    for(int i = 0; i < NUM; i++){
        for(int j = 0; j < NUM; j++){
            total += grid[i][j];
        }
    }

    return total;
}

/* Prints the current grid
 */
void printGrid(void){

    for(int i = 0; i < NUM; i++){
        for(int j = 0; j < NUM; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    printf("------------------------------------------------------------------------------\n");

    //sleep(5);

    return;
}