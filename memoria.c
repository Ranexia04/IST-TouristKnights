#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourist_knight.h"

/*	Trabalho Realizado por Henrique Lourenço 90092 e Rodrigo Aires 90182
	Ficheiro dedicado a funções estritamente relacionadas com alocação, libertação e poupança de memória*/

/*
	Função que irá ler a tabela(cidade) e os pontos turisticos(pontos_turi)
	-cidade: Matriz(n_linhas por n_colunas) que contém o custo de cada ponto
	-pontos_turi: matriz(n_paragem por 2) que contem as coordenadas de cada ponto turístico
*/
int** le_tabela(FILE *fp, int n_linhas, int n_colunas, int** tabela)
{
	int i = 0, j = 0;

	tabela = (int**)malloc(sizeof(int*)*n_linhas);
	if (tabela == NULL) 
	{
    	fprintf(stderr, "Error: Cannot allocate memory.\n");
    	exit(0);
  	}

	for(i = 0; i < n_linhas; i++)
	{
		tabela[i] = (int*)malloc(sizeof(int)*n_colunas);
		if (tabela[i] == NULL) 
		{
    		fprintf(stderr, "Error: Can not allocate memory.\n");
    		exit(0);
  		}

  		j = 0;
		while(j < n_colunas && (fscanf(fp, "%d ", &tabela[i][j]) == 1))
			j++;
	}

	return(tabela);
}

/*
	Função que que cria uma matriz cujos elementos estão inicializados a MAX_CUSTO(com a exceção do ponto inicial)
	Esta matriz representa o custo de chegar a cada ponto
*/
int **cria_custoP(int n_linhas, int n_colunas, int **pontos_turi)
{
	int i = 0, j = 0, **tabela;

	tabela = (int**)malloc(sizeof(int*)*n_linhas);
	if (tabela == NULL) 
	{
    	fprintf(stderr, "Error: Cannot allocate memory.\n");
    	exit(0);
  	}

  	for(i = 0; i < n_linhas; i++)
	{
		tabela[i] = (int*)malloc(sizeof(int)*n_colunas);
		if (tabela[i] == NULL) 
		{
    		fprintf(stderr, "Error: Can not allocate memory.\n");
    		exit(0);
  		}
  		for(j = 0; j < n_colunas; j++)
  		{
  			tabela[i][j] = MAX_CUSTO;
  		}
	}

	tabela[pontos_turi[0][0]][pontos_turi[0][1]] = 0;

	return(tabela);
}

/*
	Cria uma matriz que guarda duas inteiros que representarão as coordenadas do ponto anterior do passeio(ponto inicial == -2 para indicar que é o inicio do passeio)
	Esta matriz serve para calcular o custo da variante A e para fazer as listas de passeios das variantes B e C que terão muita utilização.
*/
path_t **cria_path(int n_linhas, int n_colunas, int **pontos_turi)
{
	int i = 0, j = 0;
	path_t **tabela = NULL;

	tabela = (path_t**)malloc(sizeof(path_t*)*n_linhas);
	if (tabela == NULL) 
	{
    	fprintf(stderr, "Error: Cannot allocate memory.\n");
    	exit(0);
  	}

  	for(i = 0; i < n_linhas; i++)
	{
		tabela[i] = (path_t*)malloc(sizeof(path_t)*n_colunas);
		if (tabela[i] == NULL) 
		{
    		fprintf(stderr, "Error: Can not allocate memory.\n");
    		exit(0);
  		}
  		for(j = 0; j < n_colunas; j++)
  		{
  			tabela[i][j].ponto[0] = -1;
  			tabela[i][j].ponto[1] = -1;
  		}
	}

	tabela[pontos_turi[0][0]][pontos_turi[0][1]].ponto[0] = -2;

	return(tabela);
}

//	liberta a memória de matrizes, tipo 1 indica que é uma matriz que guarda 1 inteiro, tipo 2 indica que é uma matriz que guarda 2 inteiros(path)

void liberta_memoria(Item tabela, int n, int tipo)
{
	int i = 0, **tabelaint = NULL;
	path_t **tabelapath = NULL;

	if(tabela == NULL) return;

	if(tipo == 1)
		tabelaint = (int**)tabela;
	else if(tipo == 2)
		tabelapath = (path_t**)tabela;

	for(i = 0; i < n; i++)
	{
		if(tipo == 1)
			free(tabelaint[i]);
		else if(tipo == 2)
			free(tabelapath[i]);
	}

	if(tipo == 1)
		free(tabelaint);
	else if(tipo == 2)
		free(tabelapath);
}

//liberta uma lista
void liberta_fila(fila_t *fila_h)
{
	fila_t *aux = fila_h;

	while(fila_h != NULL)
	{
		fila_h = fila_h->next;
		free(aux);
		aux = fila_h;
	}
}

//percorre um vetor de listas, libertando a lista de cada elemento e libertando o próprio vetor no final
void liberta_caminho(fila_t ***caminho, int n_paragem)
{	
	int i = 0, j = 0;

	for(i = 0; i < n_paragem - 1; i++)
	{
		for(j = 0; j < n_paragem-1-i; j++)
		{
			liberta_fila(caminho[i][j]);
		}
		free(caminho[i]);
	}
	free(caminho);
}

/*
	No caso de já sabermos que um certo tabeleiro não tem solução mesmo antes de o ler, passamos à frente a leitura das matrizes
	para poupar tempo e memória
*/
void skip_tabelas(FILE *fp_r)
{
	char buffer[MAX_STRING];
	while(fgets(buffer, MAX_STRING, fp_r) != NULL)
	{
		if(buffer[0] == '\n')
			break;
	}
}