#include <stdio.h>

#define N 4
#define geracoes 50

void inverte_matrizes(int* **matriz_A, int* **matriz_B);
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

    //Começo da lógica

    zera_matriz(matriz_ying, N);
    zera_matriz(matriz_yang, N);

    //Inicia as gerações
    for(int i = 0; i < geracoes; i++)
    {
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

        inverte_matrizes(&matriz_ying, &matriz_yang);
    }

    //Faz a soma final
    int soma_final = 0;

    for(int x = 0; x < N; x++)
        for(int y = 0; y < N; y++)
            soma_final += matriz_ying[x][y];

    printf("%d", soma_final);

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
    int aux = *matriz_A;

    *matriz_A = *matriz_B;
    *matriz_B = aux;
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

    if(x - 1 >= 0)
    {
        total += matriz[x-1][y];

        if(y - 1 >= 0)
            total += matriz[x-1][y-1];
    }

    if(y - 1 >= 0)
    {
        total += matriz[x][y-1];

        if(x + 1 < tam)
            total += matriz[x+1][y-1];
    }

    if(x + 1 < tam)
    {
        total += matriz[x+1][y];

        if(y + 1 < tam)
            total += matriz[x+1][y+1];
    }

    if(y + 1 < tam)
    {
        total += matriz[x][y+1];

        if(x - 1 >= 0)
            total += matriz[x-1][y+1];
    }

    return total;
}
