#ifndef _TOURIST_KNIGHT_INCLUDED
#define _TOURIST_KNIGHT_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 100
#define MAX_CUSTO 999999

typedef void* Item;

//estutura utilizada para as listas. Cada node tem as coordenadas de cada investiga_ponto e aponta para o próximo ponto do passeio
//CONVEÇÃO UTILIZADA: cada lista terá o ponto inicial, os pontos intermédios, mas não o ponto final, para evitar repetição
typedef struct fila
{
	struct fila *next;
	int coords[2];
}fila_t;

typedef struct
{
	int ponto[2];
}path_t;

typedef struct
{
	int n_elementos;
	path_t *vetor;
}Heap;

int** le_tabela(FILE*, int, int, int**);

void liberta_memoria(Item, int, int);

void liberta_fila(fila_t*);

void liberta_acervo(Heap*);

void liberta_caminho(fila_t***, int);

void skip_tabelas(FILE*);

void variante_A(int **, int**, int, int, int**, path_t**, int, int);

fila_t **variante_B(int **, int**, int, int, int**, path_t**, int);

fila_t **variante_C(int **, int**, int, int, int**, path_t**, int, int*, int*);

void verifica_erro(char*);

int verifica_coord(int**, int, int, int, int**);

int remove_repetidos(int, int**);

int** cria_custoP(int, int, int**);

path_t **cria_path(int, int, int**);

void insere_acervo(int, int, Heap*, int**);

void reordena_acervo(int, int, Heap*, int**);

void FixUp(Heap*, int, int**);

void FixDown(Heap*, int**);

void investiga_ponto(int*, int, int, path_t**, int**, int**, Heap*);

void avanca_acervo(Heap*, int*, int**);

int calcula_npassos(path_t**, int**);

void imprime_passos(FILE*, path_t**, int, int, int**);

void imprime_passos_BC(fila_t**, int, FILE*, int**);

fila_t* transfere_path(path_t**, int , int );

void reset_matrizes(path_t**, int**, int, int);

void calcula_valores(fila_t**, int, int**, int*, int*);

int **calcula_custos(fila_t ***, int**, int**, int);

fila_t** transfere_C(int*,fila_t***, int, int**);

void inverte(fila_t**, int);

Heap *novo_acervo(int, int);

int calcula_menor_custo(int**, int, int*, int, int*, int, int);

void permutacoes(int**, int*, int*, int, int*, int, int);

#endif