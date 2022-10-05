/*
    Projeto 01: Jogo da vida com pthreads
    *--------*-----------------------*
    |   RA   | Aluno                 |
    *--------*-----------------------*
    | 142477 | Luiz Felipe           |
    | 140886 | Mateus Gomes Ferreira |
    | 140729 | Rafael Nascimento     |
    *--------------------------------*
*/
 

/*##############< Libs Include >#############*/
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>

/*##############< Size Parameters >#############*/
<<<<<<< HEAD
#define NUM 50
=======
#define N 2048
>>>>>>> 1f042917e87b3c9c57a3aeac2b00082123635b64
#define THREADS_NUMBER 4
#define PLAY_TIMES 5

/*##############< Mode controller >#############*/
#define HIGH_LIFE 1

/* Grid global pointers */
int **newgrid;
int **grid;

pthread_barrier_t barrier;

/* Stores information that each
 * thread needs when starts running
 */
struct thread_t{
    pthread_t t;
    int threadNumber;
};

typedef struct thread_t thread_t;

/*##############< Custom Methods Headers >#############*/
int numero_vizinhos(int** matriz, int x, int y);
void swap(void);
void *parsing(void *args);
void init(thread_t *parsersInit);
int count(void);
void printGrid(void);
void SaidaFinal(double start,int soma_final);

/*##############< Main Program >#############*/
int main(){
    int **grid1;
    int **grid2;

    grid1 = (int**) malloc(N * sizeof(int*));
    grid2 = (int**) malloc(N * sizeof(int*));

    for(int i = 0; i < N; i++){
        grid1[i] = (int*) malloc(N * sizeof(int));
        grid2[i] = (int*) malloc(N * sizeof(int));
    }

    newgrid = (int **) grid1;
    grid    = (int **) grid2;

    thread_t sweepers[THREADS_NUMBER];

    init(sweepers);
<<<<<<< HEAD

    printGrid();
=======
>>>>>>> 1f042917e87b3c9c57a3aeac2b00082123635b64

    pthread_barrier_init(&barrier, NULL, THREADS_NUMBER);

    double start;
    start = omp_get_wtime();

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_create(&sweepers[j].t, NULL, parsing, &sweepers[j]);
    }

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_join(sweepers[j].t, NULL);
    }

    SaidaFinal(start,count());

    printGrid();

    return 0;
}


void SaidaFinal(double start,int soma_final){
    double end = omp_get_wtime();

    if(HIGH_LIFE) printf("< HIGHLIFE >\n");
    else printf("< JOGO DA VIDA >\n");

    printf("Dimensões: %dx%d \nThreads: %d \nGerações: %d \n------------------------- \n", N, N, THREADS_NUMBER, PLAY_TIMES);

    printf("Tempo paralelo: %.3fs\n", end - start);

    printf("Somatório: %d\n\n", soma_final);
}
/*##############< Custom Methods >#############*/

/* Return the number of alive neighbors
 * from a given cell in the grid
 */
int numero_vizinhos(int** matriz, int x, int y)
{
    int total;
    int yc = y - 1, yb = y + 1;
    int xe = x - 1, xd = x + 1;

    if(xe < 0) xe = N-1;
    else if(xd >= N) xd = 0;

    if(yc < 0) yc = N-1;
    else if(yb >= N) yb = 0;

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
        for(int i = aux->threadNumber; i < N; i += THREADS_NUMBER){
            for(int j = 0; j < N; j++){

                int neighborsNumber = numero_vizinhos(grid, i, j);

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

/* Initiates the threads structure
 * with a identifier number and its starting cell
 * also fills the grids with 0s
 * and creates the initial population
 */
void init(thread_t *parsersInit){

    for(int i = 0; i < THREADS_NUMBER; i++){
        parsersInit[i].threadNumber = i;
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){

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

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            total += grid[i][j];
        }
    }

    return total;
}

/* Prints the current grid
 */
void printGrid(void){

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    printf("------------------------------------------------------------------------------\n");

    //sleep(5);

    return;
}