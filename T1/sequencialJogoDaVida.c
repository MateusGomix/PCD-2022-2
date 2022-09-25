//Falta definir a posição inicial e testar :D
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define BILLION  1000000000.0
#define N 2048
#define geracoes 4

void inverte_matrizes(int* **matriz_A, int* **matriz_B);
void zera_matriz(int** matriz);
void InicializaFormas(int** matriz);
int numero_vizinhos(int** matriz, int x, int y);

void print_matriz(int** matriz)
{
    for(int x = 0; x < N; x ++){
        for(int y = 0; y < N; y++)
            printf("%d ", matriz[x][y]);
        printf("\n");
    }
}


int main()
{
    //Alocação dinâmica das matrizes

    int** matriz_ying;
    int** matriz_yang;

    matriz_ying = (int**) malloc(N * sizeof(int*));
    matriz_yang = (int**) malloc(N * sizeof(int*));


    for(int i = 0; i < N; i++)
    {
        matriz_ying[i] = (int*) malloc(N * sizeof(int));
        matriz_yang[i] = (int*) malloc(N * sizeof(int));
    }
    //Começo da lógica

    matriz_yang[0][0] = 0;


    zera_matriz(matriz_ying);
    InicializaFormas(matriz_ying);
    zera_matriz(matriz_yang);

//    print_matriz(matriz_ying);

    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    //Inicia as gerações
    for(int i = 0; i < geracoes; i++)
    {
        for(int x = 0; x < N; x++)
        {
            for(int y = 0; y < N; y++)
            {
                //Regras do jogo da vida
                int n_viz = numero_vizinhos(matriz_ying, x, y);
                if(n_viz == 3 || (n_viz == 2 && matriz_ying[x][y] == 1)){
                    matriz_yang[x][y] = 1;
                }else
                    matriz_yang[x][y] = 0;
            }
        }

        inverte_matrizes(&matriz_ying, &matriz_yang);

//        printf("%d \n", i);
//        print_matriz(matriz_ying);

    }

    //Faz a soma final
    int soma_final = 0;

    for(int x = 0; x < N; x++)
        for(int y = 0; y < N; y++)
            soma_final += matriz_ying[x][y];

    clock_gettime(CLOCK_REALTIME, &end);

    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;

    
    
    printf("Duração: %f\n", time_spent);

    printf("Somatório: %d\n\n", soma_final);

    //Liberação da memória alocada
    for(int i = 0; i < N; i++)
    {
        free(matriz_ying[i]);
        free(matriz_yang[i]);
    }
    free(matriz_ying);
    free(matriz_yang);

    return 0;
}

void inverte_matrizes(int* **matriz_A, int* **matriz_B)
{
    int **aux = *matriz_A;

    *matriz_A = *matriz_B;
    *matriz_B = aux;

}

void zera_matriz(int** matriz)
{
    for(int x = 0; x < N; x ++)
        for(int y = 0; y < N; y++)
            matriz[x][y] = 0;
}

void InicializaFormas(int** matriz)
{
    //GLIDER
    int lin = 1, col = 1;
    matriz[lin  ][col+1] = 1;
    matriz[lin+1][col+2] = 1;
    matriz[lin+2][col  ] = 1;
    matriz[lin+2][col+1] = 1;
    matriz[lin+2][col+2] = 1;
    if(N > 40){
    //R-entomino
        lin =10; col = 30;
        matriz[lin  ][col+1] = 1;
        matriz[lin  ][col+2] = 1;
        matriz[lin+1][col  ] = 1;
        matriz[lin+1][col+1] = 1;
        matriz[lin+2][col+1] = 1;
    }

}

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