#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourist_knight.h"

/*	Trabalho Realizado por Henrique Lourenço 90092 e Rodrigo Aires 90182 
	Este ficheiro contém as função mais intimamente relacionadas com funções variante_A variante_B e variante_c*/

/*
	Aplica o algoritmo de Dikstra para calcular o caminho com o menor custo entre dois pontos. Começa por criar um acervo e inicializar o ponto atual.
	Para cada ponto atual vai verificar se os 8 pontos à sua volta são acessíveis e, se for o caso, investiga-os(explicado em melhor detalhe na função investiga pontos)
	Seguidamente, avança no acervo para atualizar o ponto atual e o ciclo repete-se até chegar ao ponto de destino ou não haver mais jogadas possíveis.
	Termina a libertar memória do acervo.
	Os últimos 2 parâmetros referem-se aos pontos de começo e de chegada, respetivamente
*/
void variante_A(int **cidade, int **pontos_turi, int n_linhas, int n_colunas, int **custoP, path_t **path, int n, int k)
{
	int i = 0, j = 0, ponto_atual[2];
	Heap *acervo = NULL;

	acervo = novo_acervo(n_linhas, n_colunas);

	ponto_atual[0] = pontos_turi[n][0];
	ponto_atual[1] = pontos_turi[n][1];

	while(ponto_atual[0] != pontos_turi[k][0] || ponto_atual[1] != pontos_turi[k][1])
	{
		for(i = -1; i <= 1; i += 2)
		{
			for(j = -1; j <= 1; j+= 2)
			{
				if((ponto_atual[0] + 2*i >= 0 && ponto_atual[0] + 2*i < n_linhas) && (ponto_atual[1] + j >= 0 && ponto_atual[1] + j < n_colunas))
					investiga_ponto(ponto_atual, ponto_atual[0] + 2*i, ponto_atual[1] + j, path, custoP, cidade, acervo);
				
				if((ponto_atual[0] + i >= 0 && ponto_atual[0] + i < n_linhas) && (ponto_atual[1] + 2*j >= 0 && ponto_atual[1] + 2*j < n_colunas))
					investiga_ponto(ponto_atual, ponto_atual[0] + i, ponto_atual[1] + 2*j, path, custoP, cidade, acervo);
			}
		}

		if(acervo->n_elementos != 0)
			avanca_acervo(acervo, ponto_atual, custoP);//ponto atual foi explorado, logo há que o atualizar
		else
			break;
	}

	liberta_acervo(acervo);
}

/*
	Esta função aplica a variante B que vai retornar um vetor de listas ordenado na ordem correta. Começa por alocar memória para este vetor com n_paragem-1,
	 pois para N pontos turísticos, existem N-1 passeios. O for aplica o algoritmo de Dikstra(variante A) e transfere o caminho calculado para um das posiçoes do
	 vetor das listas. Faz o reset das matrizes para poder aplicar o Dikstra outra vez.
*/
fila_t **variante_B(int **cidade, int **pontos_turi, int n_linhas, int n_colunas, int **custoP, path_t **path, int n_paragem)
{
	int i = 0, j = 0;
	fila_t **caminho_h = NULL;

	caminho_h = (fila_t**)calloc(1, sizeof(fila_t*) * (n_paragem - 1));
	if(caminho_h == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

	for(i = 0; i < n_paragem - 1; i++)
	{
		if(pontos_turi[i][0] != pontos_turi[i+1][0] || pontos_turi[i][1] != pontos_turi[i+1][1])//só acontece se os pontos consecutivos não forem iguais
		{
			path[pontos_turi[i][0]][pontos_turi[i][1]].ponto[0] = -2;//ponto inicial fica a -2 para indicar que é o ponto inicial
		
			variante_A(cidade, pontos_turi, n_linhas, n_colunas, custoP, path, i, i+1);

			if(custoP[pontos_turi[i + 1][0]][pontos_turi[i + 1][1]] != MAX_CUSTO)
				caminho_h[j] = transfere_path(path, pontos_turi[i+1][0], pontos_turi[i+1][1]);// uso do j como indice para, no caso de haverem
																							// dois pontos iguais consecutivos, não existirem elementos do vetor a NULL
			else  																			// desta forma todos as linhas ficam adjacentes no vetor
				return caminho_h;

			reset_matrizes(path, custoP, n_linhas, n_colunas);
			j++;
		}
		custoP[pontos_turi[i + 1][0]][pontos_turi[i + 1][1]] = 0;
	}
	return caminho_h;
}

/*
	A partir do path, irá criar uma lista com todas as etapas do passeio, incluindo o ponto de partida mas não o de chegada, para não haverem pontos repetidos.
	Portanto, no final desta função head é igual ao ponto de partida e o último node é igual ao penúltimo ponto do passeio
	//CONVEÇÃO UTILIZADA: cada lista terá o ponto inicial, os pontos intermédios, mas não o ponto final, para evitar repetição
*/
fila_t *transfere_path(path_t **path, int x, int y)
{
	fila_t *new = NULL, *head = NULL;
	int x_aux = 0;

	while(path[x][y].ponto[0] != -2)
	{
		new = (fila_t*)malloc(sizeof(fila_t));
		if(new == NULL) 
		{
    		fprintf(stderr, "Error: Can not allocate memory.\n");
    		exit(0);
  		}

		new->coords[0] = path[x][y].ponto[0];
		new->coords[1] = path[x][y].ponto[1];
		new->next = head;//percorre o caminho do fim para o início, logo há que atualizar a head a cada iteração
		head = new;

		x_aux = x;
		x = path[x][y].ponto[0];
		y = path[x_aux][y].ponto[1];
	}
	return head;
}

/*
	Esta função começa por remover os pontos repetidos dos pontos turísticos para só ter que fazer os algorítmos com todos os pontos diferentes,
	 de modo a evitar redundâncias. Sendo assim, por cada ponto turístico removido o número de paragens será decrementado e é introduzida uma nova variável
	 (n_paragem_removidos) que é utilizada na main como parâmetro de funcões que fazem os fprintf, por ex.
	Desta vez não temos um vetor de listas, mas sim uma espécie de matriz triangular de listas, cuja primeira linha terá as listas do ponto inicial para os outros pontos,
	a segunda linha terá as listas do segundo ponto para todos os pontos seguintes(e não anteriores!!). Ou seja, as listas começam por ser calculadas apenas para uma direção
	Após criar cada lista e igualá-la ao respetivo elemento da "matriz", chama-se a função calcula custos que irá calcular os custos de cada passeio em cada direção,
	ou seja de X0 -> X1 e X1 -> X0, por ex. Os percursos impossíveis(por exemplo: X3 -> X0(pois começa sempre no ponto X0) ou X2 -> X2(não vais para o mesmo ponto), estão a -1)
	Seguidamente, ao chamar a função permutações, é calculado o percuso com o menor custo. A funcão tranfere_C tranfere as listas (que fazem parte do caminho mais curto)
	da matriz das listas para um vetor caminho_final, tal como na variante B. Termina por libertar memória.
*/
fila_t **variante_C(int **cidade, int **pontos_turi, int n_linhas, int n_colunas, int **custoP, path_t **path, int n_paragem, int *ultimo, int *n_paragem_removidos)
{
	int i = 0, j = 0, **matriz_custos = NULL, menor_custo = MAX_CUSTO, *menor_custo_caminho = NULL, custo_atual = 0, *custo_atual_caminho = NULL;
	fila_t ***caminho_h = NULL, **caminho_final = NULL;

	n_paragem = remove_repetidos(n_paragem, pontos_turi);
	*n_paragem_removidos = n_paragem;
	//no main n_paragem significa o número total de paragens. No entanto, nesta função, o n_paragens, indica o número de paragens diferentes

	caminho_h = (fila_t***)malloc(sizeof(fila_t**) * (n_paragem - 1));//para N pontos turisticos existem N-1 passeios
	if(caminho_h == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

  	for(i = 0; i < n_paragem - 1; i++)
  	{
  		caminho_h[i] = (fila_t**)calloc(1, sizeof(fila_t*) * n_paragem -1 - i);//resultará numa matriz triangular superior deste tipo:
  		if(caminho_h[i] == NULL)																			    //(para N paragens = 4) X X X
		{																															  //X X	
    		fprintf(stderr, "Error: Can not allocate memory.\n");																	  //X
    		exit(0);
  		}
  	}

	for(i = 0; i < n_paragem - 1; i++)
	{
		for(j = i + 1; j < n_paragem; j++)
		{
			path[pontos_turi[i][0]][pontos_turi[i][1]].ponto[0] = -2;
			custoP[pontos_turi[i][0]][pontos_turi[i][1]] = 0;
			variante_A(cidade, pontos_turi, n_linhas, n_colunas, custoP, path, i, j);

			if(custoP[pontos_turi[j][0]][pontos_turi[j][1]] != MAX_CUSTO)//basta um dos dikstra não ter solução para que não haja solução para o tabuleiro
				caminho_h[i][j-i-1] = transfere_path(path, pontos_turi[j][0], pontos_turi[j][1]);
			else
			{
				*ultimo = -2;//no caso de não haver solução, esta variável é utilizada para sinalizar esta ocorrência
				liberta_caminho(caminho_h, n_paragem);
				return caminho_final;
			}

			reset_matrizes(path, custoP, n_linhas, n_colunas);
		}
	}

	matriz_custos = calcula_custos(caminho_h, cidade, pontos_turi, n_paragem);

	menor_custo_caminho = (int*)calloc(1, sizeof(int) * n_paragem);
	if(menor_custo_caminho == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

	custo_atual_caminho = (int*)malloc(sizeof(int) * n_paragem);
	if(custo_atual_caminho == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

  	for(i = 0; i < n_paragem; i++)
  	{
  		custo_atual_caminho[i] = i;
  	}

	permutacoes(matriz_custos, &menor_custo, menor_custo_caminho, custo_atual, custo_atual_caminho, n_paragem-1, 1);

	*ultimo = menor_custo_caminho[n_paragem-1];//a variável "ultimo" serve para imprimir o último passo
	caminho_final = transfere_C(menor_custo_caminho, caminho_h, n_paragem, pontos_turi);

	free(menor_custo_caminho);
	free(custo_atual_caminho);
	liberta_caminho(caminho_h, n_paragem);
	liberta_memoria(matriz_custos, n_paragem, 1);
	return caminho_final;
}

/*
	Faz as permutações do vetor custo_atual_caminho, mas fixando o primeiro ponto. Irá calcular o custo para cada permutação e, se for menor que "menor_custo"
	então menor_custo = custo_atual e copia os elementos de custo_atual_caminho para menor_custo_caminho. É utilizada uma aproximação branch and bound.
*/
void permutacoes(int **matriz_custos, int *menor_custo, int *menor_custo_caminho, int custo_atual, int *custo_atual_caminho, int final, int inicio)
{
	int i = 0, k = 0, swap_aux = 0;

	if(inicio == final)//se a permutação estiver completa, passa-se à comparação dos valores
	{
		custo_atual += matriz_custos[custo_atual_caminho[final-1]][custo_atual_caminho[final]];//soma-se o último antes de comparar
		if(custo_atual < *menor_custo)
		{
			*menor_custo = custo_atual;
			for(k = 1; k <= final; k++)
			{
				menor_custo_caminho[k] = custo_atual_caminho[k];
			}
		}
	}
	else
	{
		for(i = inicio; i <= final; i++)
		{
			swap_aux = custo_atual_caminho[inicio];
			custo_atual_caminho[inicio] = custo_atual_caminho[i];
			custo_atual_caminho[i] = swap_aux;

			if(inicio > 0)
			{
				custo_atual = custo_atual + matriz_custos[custo_atual_caminho[inicio-1]][custo_atual_caminho[inicio]];
			}

			if(custo_atual < *menor_custo)//branch and bound approach, se a permutação estiver incompleta e custo_atual já for maior que menor_custo
										//, então da skip da permutação
				permutacoes(matriz_custos, menor_custo, menor_custo_caminho, custo_atual, custo_atual_caminho, final, inicio+1);

			custo_atual = custo_atual - matriz_custos [custo_atual_caminho[inicio-1]][custo_atual_caminho[inicio]];

			swap_aux = custo_atual_caminho[inicio];
			custo_atual_caminho[inicio] = custo_atual_caminho[i];
			custo_atual_caminho[i] = swap_aux;
		}
	}
}

/*
	Irá tranferir as listas que conrespondem ao caminho mais curto da matriz de listas para o vetor de listas utilizado para impressão

*/
fila_t** transfere_C(int* menor_custo_caminho, fila_t*** caminho_h, int n_paragem, int** pontos_turi)
{
	fila_t **caminho_final = NULL, *new = NULL;
	int i = 0;

	caminho_final = (fila_t**)malloc(sizeof(fila_t*) * (n_paragem-1));
	if(caminho_final == NULL) 
	{
    	fprintf(stderr, "Error: Can not allocate memory.\n");
    	exit(0);
  	}

	for(i = 0; i < n_paragem - 1; i++)
	{
		if(menor_custo_caminho[i] < menor_custo_caminho[i+1])//se esta condição se verificar, a ordem da lista criada está correta, ou seja basta tranferir
		{
			caminho_final[i] = caminho_h[menor_custo_caminho[i]][menor_custo_caminho[i+1] - 1 - menor_custo_caminho[i]];
			caminho_h[menor_custo_caminho[i]][menor_custo_caminho[i+1] - 1 - menor_custo_caminho[i]] = NULL;
		}
		else 
		{//neste caso é também necessário inverter a lista criada, após de a tranferir
			caminho_final[i] = caminho_h[menor_custo_caminho[i+1]][menor_custo_caminho[i] - 1 - menor_custo_caminho[i+1]];
			caminho_h[menor_custo_caminho[i+1]][menor_custo_caminho[i] - 1  -menor_custo_caminho[i+1]] = NULL;
			inverte(caminho_final, i);
			new = (fila_t*)malloc(sizeof(fila_t));//como as listas tem o primeiro mas nao o ultimo ponto, coloca-se o primeiro ponto e atualiza head.
			if(new == NULL) 
			{	
    			fprintf(stderr, "Error: Can not allocate memory.\n");
    			exit(0);
  			}

			new->coords[0] = pontos_turi[menor_custo_caminho[i]][0];
			new->coords[1] = pontos_turi[menor_custo_caminho[i]][1];
			new->next = caminho_final[i];
			caminho_final[i] = new;
		}
	}
	return caminho_final;
}

/*
	Inverte a lista criada, mas elimina o primeiro node antes de inverter(equivalente a eliminar o ultimo depois inverter),
	pois, relembrando, na nossa convenção as listas tem todos os pontos do passeia incluindo o primeiro mas excluindo o ultimo para evitir repetição
*/
void inverte (fila_t** caminho_final, int i)
{
	fila_t *aux = caminho_final[i], *prox = NULL, *prev = NULL;
	prev = aux;
	aux = aux->next;
	free(prev);
	prev = NULL;

	while(aux != NULL)
	{
		prox = aux->next;
		aux->next = prev;
		prev = aux;
		aux = prox;
	}
	caminho_final[i] = prev;
}