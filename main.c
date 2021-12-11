#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourist_knight.h"

/*Trabalho Realizado por Henrique Lourenço 90092 e Rodrigo Aires 90182 */

int main(int argc, char *argv[])
{
	int n_linhas = 0, n_colunas = 0, n_paragem = 0, custo = 0, **pontos_turi = NULL, **cidade = NULL, n_passos = 0, **custoP = NULL,
	 i = 0, ultimo = 0, n_paragem_removidos = 0;
	char modo = '\0', buffer[MAX_STRING], *filenameout = NULL;
	FILE *fp_r = NULL, *fp_w = NULL;
	path_t **path = NULL;
	fila_t **caminho_h = NULL;

	verifica_erro(argv[1]);

	filenameout = (char*)calloc(1, sizeof(char)*(strlen(argv[1]) - strlen(".cities") + strlen(".walks") + 1));
	if(filenameout == NULL) 
	{
    	fprintf(stderr, "Error: Cannot allocate memory.\n");
    	exit(0);
  	}

  	//copia o nome do ficheiro (com a excepção da extensão .cities para a string e concatena a extensão .walks)
	memcpy(filenameout, argv[1], sizeof(char)*(strlen(argv[1]) - strlen(".cities")));
	strcat(filenameout, ".walks");
	
	fp_r = fopen(argv[1], "r");
	if (fp_r == NULL) 
	{
		free(filenameout);
    	fprintf(stderr, "Error: Cannot open file.\n");
    	exit(0);
  	}

	fp_w = fopen(filenameout, "w");
	if (fp_w == NULL) 
	{
		free(filenameout);
    	fprintf(stderr, "Error: Cannot open file.\n");
    	exit(0);
  	}

  	while(fgets(buffer, MAX_STRING, fp_r) != NULL)
  	{
  		if(buffer[0] == '\n')//ignora linhas vazias
  			continue;

  		custoP = NULL;
  		path = NULL;
		
  		sscanf(buffer, "%d %d %c %d", &n_linhas, &n_colunas, &modo, &n_paragem);

  		//prevenção de erros
  		if((modo != 'A' && modo != 'B' && modo != 'C') || (modo == 'A' && n_paragem != 2) || (modo == 'B' && n_paragem < 2) || (modo == 'C' && n_paragem < 2))
		{
			n_passos = 0;
			fprintf(fp_w, "%d %d %c %d %d %d\n\n", n_linhas, n_colunas, modo, n_paragem, -1, n_passos);
			skip_tabelas(fp_r);//as matrizes não vao ser lidas e por isso é necessário colocar o ponteiro do file(fp_r) no próximo tabuleiro
			continue;
		}

		pontos_turi = le_tabela(fp_r, n_paragem, 2, pontos_turi);
		cidade = le_tabela(fp_r, n_linhas, n_colunas, cidade);
		if(verifica_coord(pontos_turi, n_linhas, n_colunas, n_paragem, cidade) == -1)
		{
			n_passos = 0;
			fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, -1, n_passos);
		}
		else
		{	
			custoP = cria_custoP(n_linhas, n_colunas, pontos_turi);
			path = cria_path(n_linhas, n_colunas, pontos_turi);
			switch(modo)
			{
				case 'A':
					variante_A(cidade, pontos_turi, n_linhas, n_colunas, custoP, path, 0, 1);

					if(custoP[pontos_turi[1][0]][pontos_turi[1][1]] == MAX_CUSTO)//se não houver solução
						fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, -1, 0);
					else
					{
						n_passos = calcula_npassos(path, pontos_turi);
						fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, custoP[pontos_turi[1][0]][pontos_turi[1][1]], n_passos);
						imprime_passos(fp_w, path, pontos_turi[1][0], pontos_turi[1][1], cidade);
					}
					break;

				case 'B':
					caminho_h = variante_B(cidade, pontos_turi, n_linhas, n_colunas, custoP, path, n_paragem);

					if(custoP[pontos_turi[n_paragem - 1][0]][pontos_turi[n_paragem - 1][1]] == MAX_CUSTO)//se não houver solução
						fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, -1, 0);
					else
					{
						if(caminho_h[0] != NULL)//se todos os pontos forem iguais, não existem listas de caminho
						{
							calcula_valores(caminho_h, n_paragem, cidade, &n_passos, &custo);
							//as próximas duas linhas adicionam os ultimo ponto do passeio, pois este ponto não está nas listas
							n_passos++;
							custo = custo + cidade[pontos_turi[n_paragem - 1][0]][pontos_turi[n_paragem - 1][1]];
							fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, custo, n_passos);
							imprime_passos_BC(caminho_h, n_paragem, fp_w, cidade);
							fprintf(fp_w, "%d %d %d\n", pontos_turi[n_paragem - 1][0], pontos_turi[n_paragem - 1][1],//imprime o último passo
						 	 cidade[pontos_turi[n_paragem - 1][0]][pontos_turi[n_paragem - 1][1]]);
						}
						else
							fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, 0, 0);
						
					}

					for(i = 0; i < n_paragem - 1; i++)
					{
						liberta_fila(caminho_h[i]);
					}
					free(caminho_h);
					break;

				case 'C':
					caminho_h = variante_C(cidade, pontos_turi, n_linhas, n_colunas, custoP, path, n_paragem, &ultimo, &n_paragem_removidos);

					if(ultimo == -2)//ultimo == -2 no caso de não haver solução
						fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, -1, 0);
					else
					{
						calcula_valores(caminho_h, n_paragem_removidos, cidade, &n_passos, &custo);
						//as próximas duas linhas adicionam os ultimo ponto do passeio, pois este ponto não está nas listas
						n_passos++;
						custo = custo + cidade[pontos_turi[ultimo][0]][pontos_turi[ultimo][1]];
						fprintf(fp_w, "%d %d %c %d %d %d\n", n_linhas, n_colunas, modo, n_paragem, custo, n_passos);
						imprime_passos_BC(caminho_h, n_paragem_removidos, fp_w, cidade);
						fprintf(fp_w, "%d %d %d\n", pontos_turi[ultimo][0], pontos_turi[ultimo][1],//imprime o ultimo passo
						 	 cidade[pontos_turi[ultimo][0]][pontos_turi[ultimo][1]]);

						for(i = 0; i < n_paragem_removidos - 1; i++)
						{
							liberta_fila(caminho_h[i]);
						}
						free(caminho_h);
					}
					break;

				default:
					break;
			}
		}

		fprintf(fp_w, "\n");

		liberta_memoria((Item)pontos_turi, n_paragem, 1);
		liberta_memoria((Item)cidade, n_linhas, 1);
		liberta_memoria((Item)custoP, n_linhas, 1);
		liberta_memoria((Item)path, n_linhas, 2);
	}

	fclose(fp_r);
	fclose(fp_w);
	free(filenameout);

	exit(0);
}
