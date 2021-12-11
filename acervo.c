#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourist_knight.h"

/*	Trabalho Realizado por Henrique Lourenço 90092 e Rodrigo Aires 90182
	Aqui estão todas as variáveis relacionadas com acervo(inserções, reordenaçoes, etc)*/

/*
	Função que cria o acervo a cada iteração do algoritmo de Dikstra. O vetor tem espaço para todos os pontos da tabela
*/
Heap *novo_acervo(int n_linhas, int n_colunas)
{
	Heap *acervo = NULL;

	acervo = (Heap*)malloc(sizeof(Heap));
	if (acervo == NULL){
    	fprintf(stderr, "Error in malloc of heap\n");
    	exit(0);
  	}

  	acervo->vetor = (path_t*)malloc(sizeof(path_t) * n_linhas * n_colunas);
  	if (acervo->vetor == NULL) {
    	fprintf(stderr, "Error in malloc of heap\n");
    	exit(0);
  	}

  	acervo->n_elementos = 0;

  	return acervo;
}


//	Insere um novo elemento no acervo e coloca o na posição correta através do FixUp

void insere_acervo(int new_x, int new_y, Heap *acervo, int **custoP)
{
	acervo->vetor[acervo->n_elementos].ponto[0] = new_x;
	acervo->vetor[acervo->n_elementos].ponto[1] = new_y;

	FixUp(acervo, acervo->n_elementos, custoP);
	acervo->n_elementos++;
}

//	Após um ponto ser explorado no algoritmo de Dikstra é necessário atualizar o ponto atual com a "head" do acervo, que é seguidamente removida do mesmo.
//	Como a head é removida é necessário reorganizar a tabela e decrementar o número de elementos
void avanca_acervo(Heap *acervo, int *ponto_atual, int **custoP)
{
	ponto_atual[0] = acervo->vetor[0].ponto[0];
	ponto_atual[1] = acervo->vetor[0].ponto[1];

	acervo->vetor[0].ponto[0] = acervo->vetor[acervo->n_elementos - 1].ponto[0];
	acervo->vetor[0].ponto[1] = acervo->vetor[acervo->n_elementos - 1].ponto[1];

	acervo->n_elementos--;
	FixDown(acervo, custoP);
}

void FixUp(Heap *acervo, int k, int **custoP)
{
	int aux1, aux2;

	while(k > 0 && custoP[acervo->vetor[(k - 1)/2].ponto[0]][acervo->vetor[(k - 1)/2].ponto[1]] > custoP[acervo->vetor[k].ponto[0]][acervo->vetor[k].ponto[1]])
	{
		aux1 = acervo->vetor[(k - 1)/2].ponto[0];
		aux2 = acervo->vetor[(k - 1)/2].ponto[1];

		acervo->vetor[(k - 1)/2].ponto[0] = acervo->vetor[k].ponto[0];
		acervo->vetor[(k - 1)/2].ponto[1] = acervo->vetor[k].ponto[1];

		acervo->vetor[k].ponto[0] = aux1;
		acervo->vetor[k].ponto[1] = aux2;

		k = (k - 1) / 2;
	}
}

void FixDown(Heap *acervo, int **custoP)
{
	int aux1, aux2;
	int k = 0, j;

	while((k*2 + 1) < acervo->n_elementos)
	{
		j = 2*k + 1;
		if((j + 1) < acervo->n_elementos && 
			custoP[acervo->vetor[j].ponto[0]][acervo->vetor[j].ponto[1]] > custoP[acervo->vetor[j+1].ponto[0]][acervo->vetor[j+1].ponto[1]])
			j++;

		if(custoP[acervo->vetor[k].ponto[0]][acervo->vetor[k].ponto[1]] > custoP[acervo->vetor[j].ponto[0]][acervo->vetor[j].ponto[1]])
		{
			aux1 = acervo->vetor[k].ponto[0];
			aux2 = acervo->vetor[k].ponto[1];

			acervo->vetor[k].ponto[0] = acervo->vetor[j].ponto[0];
			acervo->vetor[k].ponto[1] = acervo->vetor[j].ponto[1];

			acervo->vetor[j].ponto[0] = aux1;
			acervo->vetor[j].ponto[1] = aux2;

			k = j;
		}
		else
			break;
	}
}

//reordena um ponto do acervo após ser investigado durante o Dikstra
void reordena_acervo(int new_x, int new_y, Heap *acervo, int **custoP)
{
	int i = 0;

	for(i = 0; i < acervo->n_elementos; i++)
	{
		if(acervo->vetor[i].ponto[0] == new_x && acervo->vetor[i].ponto[1] == new_y)
		{
			FixUp(acervo, i, custoP);
			return;
		}
	}
}

void liberta_acervo(Heap *acervo)
{
	free(acervo->vetor);
	free(acervo);
}