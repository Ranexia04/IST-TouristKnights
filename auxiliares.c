#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourist_knight.h"

/*	Trabalho Realizado por Henrique Lourenço 90092 e Rodrigo Aires 90182
	Este ficheiro contém as funções que permitem o programa correr sem erros e as funções que auxiliam as funções principais*/

/*
	Função chamada se o ponto adjacente ao ponto atual está contido no tabuleiro. Começa por verificar se o ponto não está a 0 ou se é acessível
	Se a soma do CustoP do ponto atual com o custo do passo atual for inferior ao custoP do next step(new_x/new_y)(ou se for a primeira vez que se vizita o ponto)
	 então foi encontrado um passo melhor, ou seja irá atualizar o CustoP, e, no caso de ser um novo ponto, insere-o no acervo. Se não for novo apenas reordena.
	 Atualiza o path.
*/
void investiga_ponto(int* ponto_atual, int new_x, int new_y, path_t **path, int **custoP, int **cidade, Heap *acervo)
{
	if(cidade[new_x][new_y] <= 0 || path[new_x][new_y].ponto[0] == -2)
		return;

	if(custoP[new_x][new_y] > custoP[ponto_atual[0]][ponto_atual[1]] + cidade[new_x][new_y])
	{
		custoP[new_x][new_y] = custoP[ponto_atual[0]][ponto_atual[1]] + cidade[new_x][new_y];
		if(path[new_x][new_y].ponto[0] == -1)
			insere_acervo(new_x, new_y, acervo, custoP);
		else
			reordena_acervo(new_x, new_y, acervo, custoP);

		path[new_x][new_y].ponto[0] = ponto_atual[0];
		path[new_x][new_y].ponto[1] = ponto_atual[1];
	}
}

/*
	Esta função verifica se os pontos turisticos:
	-estão dentro do tabuleiro,
	-não tem o valor 0,
	-tem pelo menos um ponto adjacente com o valor diferente de 0.
	Para esta ultima condição, conta-se o número de pontos adjacentes acessíveis e, destes pontos, aqueles que são 0. Se este número for igual, então não há solução..
*/
int verifica_coord(int **pontos_turi, int n_linhas, int n_colunas, int n_paragem, int **cidade)
{
	int i = 0, a = 0, b = 0, counter1 = 0, counter2 = 0;

	for(i = 0; i < n_paragem; i++)
	{
		if(pontos_turi[i][0] < 0 || pontos_turi[i][0] >= n_linhas || pontos_turi[i][1] < 0 || pontos_turi[i][1] >= n_colunas 
		|| cidade[pontos_turi[i][0]][pontos_turi[i][1]] <= 0)
			return -1;

		for(a = -1; a <= 1; a += 2)
		{
			for(b = -1; b <= 1; b+= 2)
			{
				if((pontos_turi[i][0] + 2*a >= 0 && pontos_turi[i][0] + 2*a < n_linhas) && (pontos_turi[i][1] + b >= 0 && pontos_turi[i][1] + b < n_colunas))
				{
					counter1++;
					if(cidade[pontos_turi[i][0] + 2*a][pontos_turi[i][1] + b] == 0)
						counter2++;
				}
				
				if((pontos_turi[i][0] + a >= 0 && pontos_turi[i][0] + a < n_linhas) && (pontos_turi[i][1] + 2*b >= 0 && pontos_turi[i][1] + 2*b < n_colunas))
				{
					counter1++;
					if(cidade[pontos_turi[i][0] + a][pontos_turi[i][1] + 2*b] == 0)
						counter2++;
				}
			}
		}

		if(counter1 == counter2)
			return -1;

		counter1 = 0;
		counter2 = 0;
	}

	return 0;
}


//	Função que verifica se o ficheiro contém a extensão .cities

void verifica_erro(char *f_name)
{
	char *posicao = NULL;

	posicao = strstr(f_name, ".cities");
	if(posicao == NULL)
	{
		fprintf(stderr, "Error: Incorrect file name: .cities was not found!\n");
		exit(0);
	}
}

/*
	Esta função serve para calcular o número de passos da variante A. Ao contrário das variantes B e C não é necessário criar uma lista com  o caminho e,
	assim poupa se memória. Percorre o path desde ponto final até ao ponto inicial.
*/
int calcula_npassos(path_t **path, int **pontos_turi)
{
	int n_passos = 0, x = pontos_turi[1][0], y = pontos_turi[1][1], x_aux = 0;

	while(path[x][y].ponto[0] != -2)
	{
		n_passos++;
		x_aux = x;
		x = path[x][y].ponto[0];
		y = path[x_aux][y].ponto[1];
	}

	return n_passos;
}
					
/*
	Função que imprime os passos para a variante A, que não há uso da lista para guardar o caminho. Portanto há que ser uma função recursiva pois percorre o path
	desde o ponto final até ao ponto inicial
*/					
void imprime_passos(FILE *fp_w, path_t **path, int x, int y, int **cidade)
{
	int x_aux = x, y_aux = y;

	x = path[x][y].ponto[0];
	y = path[x_aux][y].ponto[1];

	if(path[x_aux][y_aux].ponto[0] != -2)
	{
		imprime_passos(fp_w, path, x, y, cidade);
		fprintf(fp_w, "%d %d %d\n", x_aux, y_aux, cidade[x_aux][y_aux]);
	}
}

/*
	Função que imprime os passos para as variantes B e C, pois estas necessitam de listas. Simplesmento percorre o vetor de listas e a listas, imprimindo cada ponto.
	Como cada lista tem o ponto inicial mas não o final, os pontos não são repetidos. No main é feito um fprintf adicional para imprimir o último ponto
*/
void imprime_passos_BC(fila_t** caminho_h, int n_paragem, FILE* fp_w, int **cidade)
{
	int i = 0;
	fila_t *aux = NULL;

	for(i = 0; i < n_paragem - 1; i++)
	{
		if(caminho_h[i] == NULL)
			return;

		aux = caminho_h[i];
		if(i == 0)
			aux = aux->next;
		while(aux != NULL)
		{
			fprintf(fp_w, "%d %d %d\n", aux->coords[0], aux->coords[1], cidade[aux->coords[0]][aux->coords[1]]);
			aux = aux->next;
		}
	}
}

/*
	Esta função remove os pontos que se repetem na variante C pois não é necessário ter estes pontos em consideração quando se faz os algoritmos
	Logo, irá fazer um "shift left" dos valores à direita dos pontos removidos, decrementando o numero de pontos turisticos cada vez que isto acontece
*/
int remove_repetidos(int n_paragem, int **pontos_turi)
{
	int i = 0, j = 0, k = 0;

	for(i = 0; i < n_paragem; i++)
	{
		for(j = i+1; j < n_paragem; j++)
		{
			if(pontos_turi[i][0] == pontos_turi[j][0] && pontos_turi[i][1] == pontos_turi[j][1])
			{
				for(k = j; k < n_paragem-1; k++)
				{
					pontos_turi[k][0] = pontos_turi[k+1][0];
					pontos_turi[k][1] = pontos_turi[k+1][1];
				}
				n_paragem--;
			}
		}
	}
	return n_paragem;
}

/*
	Função que calcula o custo do passeio de cada para de pontos, para cada direção(X2 -> X3 e X3 -> X2, por exemplo). Se não houver um caminho para 
	um certo par de pontos, é igualado a -1. No caso de haver solução, a primeira coluna e a diagonal da matriz estarão igualadas a -1.
*/
int **calcula_custos(fila_t ***caminho_h, int **cidade, int **pontos_turi, int n_paragem)
{
	int i = 0, j = 0, **matriz_custos = NULL;
	fila_t *aux = NULL;

	matriz_custos = (int**)malloc(sizeof(int*) * n_paragem);
	if(matriz_custos == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

  	for(i = 0; i < n_paragem; i++)
  	{
  		matriz_custos[i] = (int*)calloc(1, sizeof(int) * n_paragem);
  		if(matriz_custos[i] == NULL) 
		{
    		fprintf(stderr, "Error: Can not allocate memory.\n");
    		exit(0);
  		}
  	}

  	for(i = 0; i < n_paragem; i++)
  	{
  		for(j = 0; j < n_paragem; j++)
  		{
  			if(j == 0 || j == i)
  				matriz_custos[i][j] = -1;
  			else if(j > i)
  			{
  				aux = caminho_h[i][j-i-1];
  				aux = aux->next;
  				while(aux != NULL)
  				{
  					matriz_custos[i][j] = matriz_custos[i][j] + cidade[aux->coords[0]][aux->coords[1]];
  					aux = aux->next;
  				}
  				matriz_custos[i][j] = matriz_custos[i][j] + cidade[pontos_turi[j][0]][pontos_turi[j][1]];
  			}
  			else
  				matriz_custos[i][j] = matriz_custos[j][i] + cidade[pontos_turi[j][0]][pontos_turi[j][1]] - cidade[pontos_turi[i][0]][pontos_turi[i][1]];
  		}
  	}
  	return matriz_custos;
}

//	Para cada iteração do algoritmo de Dikstra, é necessário dar um reset ao path e ao custoP para se obter um novo caminho.
void reset_matrizes(path_t **path, int **custoP, int n_linhas, int n_colunas)
{
	int i = 0, j = 0;

	for(i = 0; i < n_linhas; i++)
		for(j = 0; j < n_colunas; j++)
		{
			path[i][j].ponto[0] = -1;
			path[i][j].ponto[1] = -1;
			custoP[i][j] = MAX_CUSTO;
		}
}

/*
	Função que calcula o custo do passeio completo e o número de passos para as variantes B e C(não é necessário para a variante A).
	Simplesmente percorre a lista somando e incrementado as variáveis atá ao final do passeio.
*/
void calcula_valores(fila_t **caminho_h, int n_paragem, int **cidade, int *n_passos, int *custo)
{
	int i = 0;
	fila_t *aux = caminho_h[i];

	*custo = 0 - cidade[aux->coords[0]][aux->coords[1]];
	*n_passos = -1;

	for(i = 0; i < n_paragem -1; i++)
	{
		aux = caminho_h[i];
		if(caminho_h[i] == NULL)
			return;
		while(aux != NULL)
		{
			*custo = *custo + cidade[aux->coords[0]][aux->coords[1]];
			(*n_passos)++;
			aux = aux->next;
		}
	}
}