/*
    Projeto 03: Jogo da vida com MPI
    *--------*-----------------------*
    |   RA   | Aluno                 |
    *--------*-----------------------*
    | 142477 | Luiz Felipe           |
    | 140886 | Mateus Gomes Ferreira |
    | 140729 | Rafael Nascimento     |
    *--------------------------------*
*/

// mpicc -o main main.c -fopenmp && time mpiexec -np 8 --oversubscribe ./main

/*##############< Libs Include >#############*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

/*##############< Size Parameters >#############*/
#define N 2048
#define ROUNDS 4

/*##############< Custom Methods >#############*/

int numero_vizinhos(int** matriz, int x, int y, int numberOfRowsPerProc){
    int total;
    int yc = y - 1, yb = y + 1;
    int xe = x - 1, xd = x + 1;

    if(xe < 0) xe = numberOfRowsPerProc + 1;
    else if(xd >= numberOfRowsPerProc + 2) xd = 0; //talvez esse caso nunca ocorra

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

void init(int **grid, int **newgrid, int numberOfRowsPerProc, int *upperNeighbor, int *lowerNeighbor, int processId, int noProcesses){
    for(int i = 0; i < (numberOfRowsPerProc + 2); i++){
        for(int j = 0; j < N; j++){
            grid[i][j] = 0;
            newgrid[i][j] = 0;
        }
    }

    // Initial living cells and neighbors definition 
    if(processId == 0){
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

        // Neighbors
        *lowerNeighbor = noProcesses - 1;
        *upperNeighbor = (processId + 1) % noProcesses;
    }
    else{
        *lowerNeighbor = processId - 1;
        *upperNeighbor = (processId + 1) % noProcesses;
    }
}

void copyRow(int *src, int *dest){
    for(int i = 0; i < N; i++){
        dest[i] = src[i];
    }
}

void exchangeNeighbors(int **grid, int **newgrid, int numberOfRowsPerProc, int upperNeighbor, int lowerNeighbor, int currentRound, int processId){
    MPI_Status status;

    if(processId % 2 == 0){
        // Receive row from upper neighbor
        MPI_Recv(grid[numberOfRowsPerProc + 1], N, MPI_INTEGER, upperNeighbor, currentRound, MPI_COMM_WORLD, &status);
        // Receive row from lower neighbor
        MPI_Recv(grid[numberOfRowsPerProc], N, MPI_INTEGER, lowerNeighbor, currentRound, MPI_COMM_WORLD, &status);
        // Send row to lower neighbor
        MPI_Send(grid[numberOfRowsPerProc - 1], N, MPI_INTEGER, lowerNeighbor, currentRound, MPI_COMM_WORLD);
        // Send row to uper neighbor
        MPI_Send(grid[0], N, MPI_INTEGER, upperNeighbor, currentRound, MPI_COMM_WORLD);
    }
    else{
        // Send row to lower neighbor
        MPI_Send(grid[numberOfRowsPerProc - 1], N, MPI_INTEGER, lowerNeighbor, currentRound, MPI_COMM_WORLD);
        // Send row to uper neighbor
        MPI_Send(grid[0], N, MPI_INTEGER, upperNeighbor, currentRound, MPI_COMM_WORLD);
        // Receive row from upper neighbor
        MPI_Recv(grid[numberOfRowsPerProc + 1], N, MPI_INTEGER, upperNeighbor, currentRound, MPI_COMM_WORLD, &status);
        // Receive row from lower neighbor
        MPI_Recv(grid[numberOfRowsPerProc], N, MPI_INTEGER, lowerNeighbor, currentRound, MPI_COMM_WORLD, &status);
    }
}

void newGridCalculation(int **grid, int **newgrid, int numberOfRowsPerProc){
    for(int j = 0; j < numberOfRowsPerProc; j++){
        for(int k = 0; k < N; k++){

            int neighborsNumber = numero_vizinhos(grid, j, k, numberOfRowsPerProc);

            if(neighborsNumber == 3 || (neighborsNumber == 2 && grid[j][k] == 1))
                newgrid[j][k] = 1;
            else
                newgrid[j][k] = 0;
        }
    }
}

void swap(int ***grid, int ***newgrid){
    int **aux = *grid;

    *grid = *newgrid;
    *newgrid = aux;
}

void finalCalculations(int **grid, int numberOfRowsPerProc, int *sum, int *finalSum){
    for(int j = 0; j < numberOfRowsPerProc; j++){
        for(int k = 0; k < N; k++){
            *sum += grid[j][k];
        }
    }

    MPI_Reduce(sum, finalSum, 1, MPI_INTEGER, MPI_SUM, 0, MPI_COMM_WORLD);
}

void gameRounds(int ***grid, int ***newgrid, int numberOfRowsPerProc, int upperNeighbor, int lowerNeighbor, int processId, int noProcesses){
    for(int i = 0; i < ROUNDS; i++){
        if(noProcesses > 1) exchangeNeighbors(*grid, *newgrid, numberOfRowsPerProc, upperNeighbor, lowerNeighbor, i, processId);

        newGridCalculation(*grid, *newgrid, numberOfRowsPerProc);

        MPI_Barrier(MPI_COMM_WORLD);

        swap(grid, newgrid);

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

/*##############< Main Program >#############*/
int main(int argc, char * argv[]) {

    int processId, noProcesses, finalSum = 0;

    // Initialize parallelism
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int **grid, **newgrid;
    int upperNeighbor, lowerNeighbor, sum = 0;
    int numberOfRowsPerProc = N/noProcesses;

    // Rows allocation, 2 more rows for neighbors
    grid = (int**) malloc((numberOfRowsPerProc + 2) * sizeof(int*));
    newgrid = (int**) malloc((numberOfRowsPerProc + 2) * sizeof(int*));

    // Columns allocation
    for(int i = 0; i < (numberOfRowsPerProc + 2); i++){
        grid[i] = (int*) malloc(N * sizeof(int));
        newgrid[i] = (int*) malloc(N * sizeof(int));
    }

    init(grid, newgrid, numberOfRowsPerProc, &upperNeighbor, &lowerNeighbor, processId, noProcesses);

    double start;
    if(processId == 0) start = omp_get_wtime();

    gameRounds(&grid, &newgrid, numberOfRowsPerProc, upperNeighbor, lowerNeighbor, processId, noProcesses);

    finalCalculations(grid, numberOfRowsPerProc, &sum, &finalSum);

    if(processId == 0) {
        double end = omp_get_wtime();
        printf("Somatório final: %d\n", finalSum);
        printf("Tempo paralelo: %.3fs\n", end - start);
    }

    // Finalize parallelism
    MPI_Finalize();

    return 0;
}