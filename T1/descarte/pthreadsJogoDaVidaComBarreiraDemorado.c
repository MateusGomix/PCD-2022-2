/*##############< Libs Include >#############*/
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*##############< Size Parameters >#############*/
#define NUM 2048
#define THREADS_NUMBER 1
#define PLAY_TIMES 2000

/*##############< Mode controller >#############*/
#define HIGH_LIFE 0

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
    pos start;
    int threadNumber;
};

typedef struct thread_t thread_t;

/* Return the number of alive neighbors
 * from a given cell in the grid
 */
int getNeighbors(int x, int y){
    int total = 0, row, column;

    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){

            if(i != 0 || j != 0){

                row = (x + i) % NUM;
                if (row < 0) row = NUM - 1;
                column = (y + j) % NUM;
                if (column < 0) column = NUM - 1;

                total += grid[row][column];
            }
        }
    }

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
    pos *aux = (pos *) args;

    pos currentCell;

    for(int j = 0; j < PLAY_TIMES; j++){

        currentCell.row = aux->row;
        currentCell.column = aux->column;

        for(int i = 0; i < ((NUM * NUM) / THREADS_NUMBER); i++){
            int neighborsNumber = getNeighbors(currentCell.row, currentCell.column);

            if(grid[currentCell.row][currentCell.column] == 1){
                if(neighborsNumber == 2 || neighborsNumber == 3){
                    newgrid[currentCell.row][currentCell.column] = 1;
                }
                else{
                    newgrid[currentCell.row][currentCell.column] = 0;
                }
            }
            else{
                if(neighborsNumber == 3 || (HIGH_LIFE && neighborsNumber == 6)){
                    newgrid[currentCell.row][currentCell.column] = 1;
                }
                else{
                    newgrid[currentCell.row][currentCell.column] = 0;
                }
            }
            if((currentCell.column + 1) >= NUM){
                currentCell.row = (currentCell.row + 1) % NUM;
            }
            currentCell.column = (currentCell.column + 1) % NUM;
        }
        int ret = pthread_barrier_wait(&barrier);

        if(aux->row == 0 && aux->column == 0) swap();

        ret = pthread_barrier_wait(&barrier);
    }

    pthread_exit(0);
}

/* Sets the starting cell for each thread
 */
void split(thread_t *current){

    current->start.column = current->threadNumber * ((NUM * NUM / THREADS_NUMBER) % NUM);
    current->start.row = current->threadNumber * (NUM * NUM / THREADS_NUMBER) / NUM;

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
        split(&parsersInit[i]);
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


/*##############< Main Program >#############*/
int main(){
    struct timeval inicio, final2;

    int **grid1;
    int **grid2;

    grid1 = (int**) malloc(NUM * sizeof(int*));
    grid2 = (int**) malloc(NUM * sizeof(int*));

    for(int i = 0; i < NUM; i++){
        grid1[i] = (int*) malloc(NUM * sizeof(int));
        grid2[i] = (int*) malloc(NUM * sizeof(int));
    }

    newgrid = (int **) grid1;
    grid    = (int **) grid2;

    thread_t sweepers[THREADS_NUMBER];

    init(sweepers);

    //printGrid();

    int ret = pthread_barrier_init(&barrier, NULL, THREADS_NUMBER);

    gettimeofday(&inicio, NULL);

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_create(&sweepers[j].t, NULL, parsing, &sweepers[j].start);
    }

    for(int j = 0; j < THREADS_NUMBER; j++){
        pthread_join(sweepers[j].t, NULL);
    }

    gettimeofday(&final2, NULL);

    //printGrid();

    printf("%d\n", count());

    int tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

    printf("tempo decorrido: %d milisegundos\n", tmili);

    return 0;
}
