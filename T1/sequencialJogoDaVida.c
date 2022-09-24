#include <stdlib.h>
#include <stdio.h>

#define N 2048
#define geracoes 4

void zera_matriz(int** matriz, int tam);
int numero_vizinhos(int** matriz, int x, int y, int tam);

int main()
{
    //Alocação dinâmica das matrizes

    int** matriz_ying;
    int** matriz_yang;

    matriz_ying = malloc(N * sizeof(int*));
    matriz_yang = malloc(N * sizeof(int*));

    for(int i = 0; i < N; i++)
    {
        matriz_ying[i] = malloc(N * sizeof(int));
        matriz_yang[i] = malloc(N * sizeof(int));
    }

    //Iniciando a matriz

    zera_matriz(matriz_ying, N);
    zera_matriz(matriz_yang, N);

    int lin = 1, col = 1;
    matriz_ying[lin  ][col+1] = 1;
    matriz_ying[lin+1][col+2] = 1;
    matriz_ying[lin+2][col  ] = 1;
    matriz_ying[lin+2][col+1] = 1;
    matriz_ying[lin+2][col+2] = 1;

    lin =10; col = 30;
    matriz_ying[lin  ][col+1] = 1;
    matriz_ying[lin  ][col+2] = 1;
    matriz_ying[lin+1][col  ] = 1;
    matriz_ying[lin+1][col+1] = 1;
    matriz_ying[lin+2][col+1] = 1;


    //Começo da lógica

    //Inicia as gerações
    for(int i = 0; i < geracoes; i++)
    {
        //Checa casa por casa
        for(int x = 0; x < N; x++)
        {
            for(int y = 0; y < N; y++)
            {
                //Regras do jogo da vida
                int n_viz = numero_vizinhos(matriz_ying, x, y, N);

                if((n_viz == 2 && matriz_ying[x][y] == 1) || n_viz == 3)
                {
                    matriz_yang[x][y] = 1;
                }
                else
                {
                    matriz_yang[x][y] = 0;
                }
            }
        }

        //Passa o conteúdo das matrizes
        matriz_ying = matriz_yang;
    }

    //Faz a soma final
    int soma_final = 0;

    for(int x = 0; x < N; x++)
        for(int y = 0; y < N; y++)
            soma_final += matriz_ying[x][y];

    printf("%d", soma_final);

    //Liberação da memória alocada
    /*for(int i = 0; i < N; i++)
    {
        free(matriz_ying[i]);
        free(matriz_yang[i]);
    }
    free(matriz_ying);
    free(matriz_yang);*/

    return 0;
}

void zera_matriz(int** matriz, int tam)
{
    for(int x = 0; x < tam; x ++)
        for(int y = 0; y < tam; y++)
            matriz[x][y] = 0;
}

int numero_vizinhos(int** matriz, int x, int y, int tam)
{
    int total = 0;

    //Esquerda
    if(x - 1 >= 0)
        total += matriz[x-1][y];

    //Diagonal Esquerda - Cima
    if(y - 1 >= 0 && x - 1 >= 0)
        total += matriz[x-1][y-1];

    //Cima
    if(y - 1 >= 0)
        total += matriz[x][y-1];

    //Diagonal Direita - Cima
    if(x + 1 < tam && y - 1 >= 0)
        total += matriz[x+1][y-1];

    //Direita
    if(x + 1 < tam)
        total += matriz[x+1][y];

    //Diagonal Direita - Baixo
    if(y + 1 < tam && x + 1 < tam)
        total += matriz[x+1][y+1];

    //Baixo
    if(y + 1 < tam)
        total += matriz[x][y+1];

    //Diagonal Esquerda - Baixo
    if(x - 1 >= 0 && y + 1 < tam)
        total += matriz[x-1][y+1];

    return total;
}


