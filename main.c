#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//------------------------------------- Definicao de dados ----------------------------------------------------------
typedef struct Pubs {
    char ISBN[24];
    char Titulo[80];
    char Autor[256];
    char Categoria[32];
    int Ano;
    int estado;// 0 - disponivel, 1 - indisponivel
} PUBLICACAO;

typedef PUBLICACAO * LISTA_PUBLICACAO;

LISTA_PUBLICACAO lista_publicacao;

int Tamanho_Pubs;

//<<--------------------------------------
typedef struct {
    int dia;
    int mes;
    int ano;
} DATA;

typedef struct requisicao {
    char ISBN[24];
    int nif;
    DATA data_requisicao;//yyyy-mm-dd
    DATA data_devolucao;//yyyy-mm-dd
    int estado; // 1 - com incumprimento, 0 - sem incumprimento
}REQUISICAO;

typedef REQUISICAO * LISTA_REQUISICAO;

LISTA_REQUISICAO lista_requisicao;

int Tamanho_Reqs;

//<<--------------------------------------

typedef struct utente {
    char Nome[80];
    int nif;
    char concelho[80];
    int contacto;
} UTENTE;

typedef UTENTE * LISTA_UTENTE;
LISTA_UTENTE lista_Utente;

int Tamanho_Uts;

//<<--------------------------------------

//-------------------------------------Prototipos ---------------------------------

void carregar_utente();
void inserir_utente(UTENTE utente);
int alterar_utente(int nif, UTENTE utente);
int remover_utente(int nif);
UTENTE pesquisar_utente_por_nif(int nif);
LISTA_UTENTE pesquisar_utente_por_nome(char * nome, int * nr_elementos);
void imprimir_lista_utentes(LISTA_UTENTE lista, int total);
void inserir_utente_front();
void listar_utentes_por_livro(char *ISBN_desejado);
void inserir_requisicao(REQUISICAO requisicao);
DATA somar_n_dias(int n);
void alterar_requisicao(char * ISBN, int nif, REQUISICAO requisicao);
void remover_requisicao(char * ISBN, int nif) ;
float calcular_multa(REQUISICAO req);
void print(char **opcoes, int tamanho);
DATA split_data(char * dt);
DATA data_hoje();
int pub_disponivel(char * ISBN);
void adicionar_publicacao_lista(LISTA_PUBLICACAO lista_resultado, PUBLICACAO publicacao, int *tamanho_lista_resultado);
void carregar_requisicao();
void gravar_dados_requisicao();
void gravar_dados_utentes();
void gravar_dados_publicacoes();
PUBLICACAO calcular_livro_mais_requisitado();
LISTA_PUBLICACAO pesquisar_publicacao_por_autor(char * autor, int * nr_elementos);
LISTA_PUBLICACAO pesquisar_publicacao_por_titulo(char * titulo, int * nr_elementos);
LISTA_PUBLICACAO pesquisar_publicacao_por_categoria(char * categoria, int * nr_elementos);
void imprimir_todos_utentes();
void menu_publicacao();

//------------------------------------- Codigo------------------

int main()
{
    carregar_utente();
    carregar_publicacao();
    carregar_requisicao();
    
	char* opcoes[] = {
		"1 - gerir utentes",
		"2 - gerir publicacoes",
		"3 - gerir requisicoes",
		"0 - sair"
	};
	int escolha;
	do {
		print(opcoes,4);
		printf("Escolha uma das opcoes\n");
		scanf("%d",&escolha);
		switch(escolha){
			case 1: menu_utente(); break;
			case 3: menu_requisicao(); break;
			case 2: menu_publicacao();
		}
	} while(escolha != 0);


    return 0;
}

//---------------------- REQUISICAO------------//
/*Esta fun��o aumenta o tamanho da lista_requisicao e aloca a mem�ria necess�ria usando realloc. 
O novo requisicao � adicionado ao final da lista e o tamanho da lista � atualizado.*/

int menu_requisicao(){
	char * opcoes[] = {
		"1 - Adicionar nova requisicao",
		"2 - alterar requisicao",
		"3 - remover requisicao",
		"9 - gravar dados de requisicoes",
		"0 - voltar para o menu principal"
	};
	printf("Escolha uma das opcoes\n");
	int escolha;
	do{
		print(opcoes,4);
		scanf("%d", &escolha);
		switch(escolha){
			case 1:
				{
					REQUISICAO rq;
					
					scanf("%d%d%d", &(rq.data_devolucao.dia),&(rq.data_devolucao.mes),&(rq.data_devolucao.ano));
					printf("Introduza um ISBN v�lido\n");
					scanf("%s",&(rq.ISBN));
					printf("Introduza um nif v�lido\n");
					scanf("%d",&(rq.nif));
										rq.data_requisicao = data_hoje();
					printf("Digite o n�mero de dias que deseja devolver o livro\n");
					int n;
					if(!verifica_multa_utente_mes_anterior(rq.nif))
						do{
							scanf("%d",&n);
						} while(n <= 0);// � necess�rio que o n�mero de dias seja superior a zero
					else n = 30;
					rq.data_devolucao = somar_n_dias(n);
					
					if(pub_disponivel(rq.ISBN)) 
						{
							inserir_requisicao(rq);
							printf("Sucesso!\n");
							break;
						}
					printf("Sem sucesso\n");
					
					 break;
				}
			case 2:
				{
					REQUISICAO rq;
					int n;
					printf("Introduza a nova data de devolucao dia mes ano -> 12 5 2022\n");
					scanf("%d",&n);
					rq.data_devolucao = somar_n_dias(n);
					rq.data_requisicao = data_hoje();
					printf("Introduza um ISBN v�lido");
					char isbn[40];
					fflush(stdin);
					gets(isbn);
					printf("Introduza o nif\n");
					scanf("%d",&(rq.nif));
					alterar_requisicao(isbn,rq.nif, rq);
					break;
				}
			case 3: {
				int nif;
				char isbn[40];
				scanf("%d",&(nif));
				fflush(stdin);
				gets(isbn);
				remover_requisicao(isbn, nif);
				break;
			}
			case 9: gravar_dados_requisicao();
		}
		
	} while(!escolha);	
}

// Fun��o para somar n dias a uma data
DATA somar_n_dias(int n) {
	DATA data_atual = data_hoje();
	struct tm data_calendario;
	data_calendario.tm_mday = data_atual.dia;
	data_calendario.tm_mon = data_atual.mes - 1;
	data_calendario.tm_year = data_atual.ano - 1900;
	data_calendario.tm_mday += n;
	
	mktime(&data_calendario);
	
	DATA data_resultante;
	data_resultante.dia = data_calendario.tm_mday;
	data_resultante.mes = data_calendario.tm_mon + 1;
	data_resultante.ano = data_calendario.tm_year + 1900;
	return data_resultante;
}



/*
A fun��o itera sobre a lista de publica��es e verifica se o ISBN passado como par�metro � 
igual a algum dos ISBN's da lista. Se for igual, verifica se o estado da publica��o � 0 (dispon�vel). 
Se o estado for 0, a fun��o retorna 1, indicando que a publica��o est� dispon�vel. Se o estado for 
diferente de 0, a fun��o retorna 0, indicando que a publica��o n�o est� dispon�vel. 
Se nenhuma publica��o for encontrada com o ISBN passado como par�metro, a fun��o retorna -1, 
indicando que n�o existe publica��o com esse ISBN.
*/

int pub_disponivel(char * ISBN){
	int i;
	for(i=0;i<Tamanho_Pubs;i++){
		if(strcmp(lista_publicacao[i].ISBN, ISBN) == 0){
			if(lista_publicacao[i].estado == 0){
				return 1;
			}else{
				return 0;
			}
		}
	}
	return -1;
}


int verifica_multa_utente_mes_anterior(int nif) {
  // Recupera a data atual
  DATA data = data_hoje();

  // Decrementa um m�s da data atual
  if (data.mes == 1) {
    data.mes = 12;
    data.ano--;
  } else {
    data.mes--;
  }
	
	int i = 0;
  // Percorre todas as requisi��es do utente procurando por multas no m�s anterior
  for (; i < Tamanho_Reqs; ++i) {
    REQUISICAO requisicao = lista_requisicao[i];
    if (requisicao.nif == nif && requisicao.estado == 1 &&
        requisicao.data_devolucao.ano == data.ano &&
        requisicao.data_devolucao.mes == data.mes) {
      return 1;
    }
  }

  // Se n�o encontrar nenhuma multa, retorna false
  return 0;
}



void inserir_requisicao(REQUISICAO requisicao) {
    Tamanho_Reqs++;
    lista_requisicao = (LISTA_REQUISICAO) realloc(lista_requisicao, Tamanho_Reqs * sizeof(REQUISICAO));
    lista_requisicao[Tamanho_Reqs - 1] = requisicao;
}




// Fun��o para gravar os dados de requisicao em arquivo 
void gravar_dados_requisicao() {
    FILE *fp;
    int i;

    fp = fopen("requisicao.txt", "w");
    if (fp == NULL) {
        printf("N�o foi poss�vel abrir o arquivo");
        exit(1);
    }

    // Escrever cabe�alho
    fprintf(fp, "ISBN\tnif\tdia_req\tmes_req\tano_req\tdia_dev\tmes_dev\tano_dev\testado\n");
	i = 0;
    // Escrever dados das requisi��es
    for (; i < 100; i++) {
        fprintf(fp, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", lista_requisicao[i].ISBN, lista_requisicao[i].nif,
		 lista_requisicao[i].data_requisicao.dia, lista_requisicao[i].data_requisicao.mes, 
		 lista_requisicao[i].data_requisicao.ano, lista_requisicao[i].data_devolucao.dia, lista_requisicao[i].data_devolucao.mes, 
		 lista_requisicao[i].data_devolucao.ano, lista_requisicao[i].estado);
    }

    fclose(fp);
}

/*Esta fun��o itera atrav�s da lista_requisicao e procura pela requisi��o com o ISBN e nif especificados. 
Quando encontrado, a requisi��o � substitu�da pelos novos campos fornecidos na requisicao structure. 
Se n�o encontrar uma requisi��o com o ISBN e nif especificados, a fun��o n�o far� nada.*/

void alterar_requisicao(char * ISBN, int nif, REQUISICAO requisicao) {
    int i;
    for (i = 0; i < Tamanho_Reqs; i++) {
        if (strcmp(lista_requisicao[i].ISBN, ISBN) == 0 && lista_requisicao[i].nif == nif) {
            lista_requisicao[i] = requisicao;
            break;
        }
    }
}


/*Esta fun��o itera atrav�s da lista_requisicao e procura pela requisi��o com o ISBN e nif especificados. 
Quando encontrado, as requisi��es subsequentes na lista s�o deslocadas para ocupar o espa�o da requisi��o removida. 
O tamanho da lista � ent�o atualizado e a mem�ria � realocada para corresponder ao novo tamanho. 
Se n�o encontrar uma requisi��o com o ISBN e nif especificados, a fun��o n�o far� nada.*/

void remover_requisicao(char * ISBN, int nif) {
    int i, j;
    for (i = 0; i < Tamanho_Reqs; i++) {
        if (strcmp(lista_requisicao[i].ISBN, ISBN) == 0 && lista_requisicao[i].nif == nif) {
            for (j = i; j < Tamanho_Reqs - 1; j++) {
                lista_requisicao[j] = lista_requisicao[j + 1];
            }
            Tamanho_Reqs--;
            lista_requisicao = (LISTA_REQUISICAO) realloc(lista_requisicao, Tamanho_Reqs * sizeof(REQUISICAO));
            break;
        }
    }
}

void devolver_requisicao(char * ISBN, int nif){
	int i, j;
    for (i = 0; i < Tamanho_Reqs; i++) {
        if (strcmp(lista_requisicao[i].ISBN, ISBN) == 0 && lista_requisicao[i].nif == nif) {
        	if(!(lista_requisicao[i].estado))
        		{
        			printf("\nEsta publicacao ja foi devolvida\n");
        			break;
				}
            for (j = i; j < Tamanho_Pubs; j++) {
                if(!strcmp(lista_publicacao[j].ISBN, ISBN))
                	{
                		lista_publicacao[j].estado = 0; // a publicacao tornou-se disponivel
                		break;
					}
            }
            
            lista_requisicao[i].estado = 0; // a requisicao tornou-se disponivel
            printf("Multa %d:", calcular_multa(lista_requisicao[i]));
            break;
        }
    }
    
    printf("\nNao existe correspondencia\n");
}


/*fun��o respons�vel por carregar do ficheiro as requisi��es previamente gravadas no ficheiro "requisicao.txt".*/

void carregar_requisicao(){
	FILE* ptr;
	char str[100];
	int size = 50;
	ptr = fopen("requisicao.txt", "a+");

	if (NULL == ptr) {
		printf("O ficheiro nao pode ser aberto\n");
	}
	lista_requisicao = (LISTA_REQUISICAO)malloc(size * sizeof(REQUISICAO));
	char *token;
	int i = 0;
	fgets(str, 200, ptr);
	while (fgets(str, 200, ptr)) {
		token = strtok(str, "\t");
		strcpy(lista_requisicao[i].ISBN, token);
		token = strtok(NULL, "\t");
        lista_requisicao[i].nif = atoi(token);
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_requisicao.dia = atoi(token);
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_requisicao.mes = atoi(token);
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_requisicao.ano = atoi(token);
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_devolucao.dia = atoi(token);   
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_devolucao.mes = atoi(token);   
        token = strtok(NULL, "\t");
        lista_requisicao[i].data_devolucao.ano = atoi(token);         
        token = strtok(NULL, "\t");      
        lista_requisicao[i].estado = atoi(token);
		i++;
		
		if(i == size){
			size+=size;
			lista_requisicao = (LISTA_REQUISICAO)realloc(lista_requisicao, size*sizeof(REQUISICAO));
		}
	}
	Tamanho_Reqs = i;
	lista_requisicao = (LISTA_REQUISICAO)realloc(lista_requisicao, Tamanho_Reqs*sizeof(REQUISICAO));
	fclose(ptr);	
}

//-------------------------------------------UTENTES----------------------------------
void gravar_dados_utentes() {
    FILE *arquivo_utentes = fopen("utentes.txt", "w");
    if (arquivo_utentes == NULL) {
        printf("Erro ao abrir o arquivo para gravar dados dos utentes.\n");
        return;
    }
    
	int i = 0;
    for (; i < Tamanho_Uts; i++) {
        UTENTE utente = lista_Utente[i];
        fprintf(arquivo_utentes, "%s\t%d\t%s\t%d\n", utente.Nome, utente.nif, utente.concelho, utente.contacto);
    }

    fclose(arquivo_utentes);
    printf("Dados dos utentes gravados com sucesso.\n");
}


int compara_nomes(const void *a, const void *b) {
    UTENTE *x = (UTENTE *) a;
    UTENTE *y = (UTENTE *) b;
    return strcmp(x->Nome, y->Nome);
}
/*A fun��o "listar_utentes_por_livro" � uma fun��o que permite listar todos os utentes que j� requisitaram um dado livro, ordenados por ordem alfab�tica do nome. 
Ela faz isso verificando todas as requisi��es presentes na lista de requisi��es "lista_requisicao" e identificando aquelas que correspondem ao livro espec�fico. 
Em seguida, os utentes associados a essas requisi��es s�o buscados na lista de utentes "lista_utentes" e impressos na tela, ordenados pelo nome.*/
void listar_utentes_por_livro(char *ISBN_desejado) {
	int size = 20;
    LISTA_UTENTE utentes_com_livro = (LISTA_UTENTE)malloc(size*sizeof(UTENTE));
    int num_utentes_com_livro = 0;

	int i = 0;
    for (; i < Tamanho_Reqs; i++) {
        if (strcmp(lista_requisicao[i].ISBN, ISBN_desejado) == 0) {
            int nif = lista_requisicao[i].nif;
            int j = 0;
            for (; j < Tamanho_Uts; j++) {
                if (lista_Utente[j].nif == nif) {
                    utentes_com_livro[num_utentes_com_livro++] = lista_Utente[j];
                    break;
                }
            }
        }
    }

    qsort(utentes_com_livro, num_utentes_com_livro, sizeof(UTENTE), compara_nomes);

    for (i = 0; i < num_utentes_com_livro; i++) {
        printf("%s\n", utentes_com_livro[i].Nome);
    }
}



//------------------------------------------------------------------------

void calcular_utente_mais_ativo() {
    int i, j, maxRequisicoes = 0;
    int contador[Tamanho_Uts];

    // Inicializa o contador de requisi��es para cada utente
    for (i = 0; i < Tamanho_Uts; i++) {
        contador[i] = 0;
    }

    // Conta o n�mero de requisi��es para cada utente
    for (i = 0; i < Tamanho_Reqs; i++) {
        for (j = 0; j < Tamanho_Uts; j++) {
            if (lista_requisicao[i].nif == lista_Utente[j].nif) {
                contador[j]++;
                break;
            }
        }
    }

    // Encontra o utente com o maior n�mero de requisi��es
    for (i = 0; i < Tamanho_Uts; i++) {
        if (contador[i] > maxRequisicoes) {
            maxRequisicoes = contador[i];
        }
    }

    // Imprime o nome do(s) utente(s) com o maior n�mero de requisi��es
    printf("Utente(s) mais ativo(s):\n");
    for (i = 0; i < Tamanho_Uts; i++) {
        if (contador[i] == maxRequisicoes) {
            printf("%s\n", lista_Utente[i].Nome);
        }
    }
}



void inserir_utente_front(){
	UTENTE utente;
	printf("Introduza o nome do Utentente\n");
	gets(utente.Nome); 
	printf("Introduza o NIF do Utentente\n");
	scanf("%d",&(utente.nif));
	printf("Introduza o Contacto do Utentente\n");
	scanf("%d",&(utente.contacto));
	printf("Introduza o Concelho do Utentente\n");
	gets(utente.concelho);
	inserir_utente(utente);
	printf("\nSucesso!\n"); 
}

int alterar_utente_front(){
	UTENTE utente;
	printf("Introduza o NIF do Utentente\n");
	scanf("%d",&(utente.nif));
	fflush(stdin);
	printf("Introduza o nome do Utentente\n");
	gets(utente.Nome); 
	printf("Introduza o Contacto do Utentente\n");
	scanf("%d",&(utente.contacto));
	printf("Introduza o Concelho do Utentente\n");
	fflush(stdin);
	gets(utente.concelho);
	return alterar_utente(utente.nif,utente);	 
}
int menu_utente(){
	char * opcoes[] = 
	{
		"1 - inserir Utente",
		"2 - alterar Utente",
		"3 - remover Utente",
		"4 - pesquisar Utente por nif",
		"5 - pesquisar Utente por nome",
		"6 - pesquisar Utente por livro(ISBN)",
		"7 - calcular Utente mais Activo",
		"8 - listar todos Utentes",
		"9 - gravar dados de Utentes",
		"0 - voltar para o menu principal"
	};
	
	print(opcoes, 9);
	int escolha;
	

	do{
		printf("escolha uma das opcoes\n");
		fflush(stdin);
		scanf("%d",&escolha);
		switch(escolha){
			case 1: 
				fflush(stdin);
				inserir_utente_front();
				break;
			case 2: 
				if(alterar_utente_front())
					printf("\nUtente alterado com sucesso!\n");
				else
					printf("O Utente que pretende alterar n�o existe\n"); 
				break;
			case 3: 
				{
					printf("\nIntroduza o nif do Utente\n");
					int nif;
					scanf("%d",&nif);
					if(remover_utente(nif))
						printf("Utente removido com sucesso!\n");
					else 
						printf("O Utente que pretende apagar n�o existe na base de dados\n");
				}
				break;
			case 4: 
				{
					printf("\nIntroduza o nif do Utente\n");
					int nif;
					scanf("%d",&nif);
					UTENTE ut =	pesquisar_utente_por_nif(nif);
					if(ut.nif > 0)
						printf("Nome: %s NIF: %d Contacto: %d Concelho: %s\n",ut.Nome, ut.nif, ut.contacto, ut.concelho);
					else
						printf("Referencia nao encontrada\n");
				}
			case 5:
				{
					fflush(stdin);
					printf("\nIntroduza o nome do Utente\n");
					char nome[80];
					gets(nome);
					int total;
					LISTA_UTENTE lista = pesquisar_utente_por_nome(nome, &total);
					imprimir_lista_utentes(lista, total);
				}	
				
				break;
			case 6: 
			{
				fflush(stdin);
				printf("\nIntroduza o ISBN do livro\n");
				char ISBN[50];
				gets(ISBN);
				listar_utentes_por_livro(ISBN);
			}
			break;
			
			case 7 : {
				calcular_utente_mais_ativo();
			}
			break;
			
			case 8: 
				imprimir_todos_utentes();
				break;
			case 9:
				gravar_dados_utentes();
			case 0: return (0);
				
		}
	} while(escolha != 0);
}



UTENTE pesquisar_utente_por_nif(int nif) {
	int i = 0;
    for (; i < Tamanho_Uts; i++) {
        if (lista_Utente[i].nif == nif) {
            return lista_Utente[i];
        }
    }
    return (UTENTE) { .nif = -1 };
}

LISTA_UTENTE pesquisar_utente_por_nome(char * nome, int * nr_elementos) {
    LISTA_UTENTE resultados = malloc(sizeof(UTENTE) * Tamanho_Uts);
    int numResultados = 0;
	
	int i = 0;
    for (; i < Tamanho_Uts; i++) {
        if (strcmp(lista_Utente[i].Nome, nome) == 0) {
            resultados[numResultados++] = lista_Utente[i];
        }
    }

    resultados = realloc(resultados, sizeof(UTENTE) * numResultados);
    *nr_elementos = numResultados;
    return resultados;
}


void imprimir_lista_utentes(LISTA_UTENTE lista, int total){
	int i = 0;
	if(total < 1)
		printf("Sem nada para imprimir\n");
	for(; i < total; ++i){
		printf("Nome: %s NIF: %d Contacto: %d Concelho: %s\n",lista[i].Nome, lista[i].nif, lista[i].contacto, lista[i].concelho);
	}
}

void imprimir_todos_utentes() {
	int i = 0;
  for (; i < Tamanho_Uts; ++i) {
    printf("NIF: %d\n", lista_Utente[i].nif);
    printf("Nome: %s\n", lista_Utente[i].Nome);
    printf("concelho: %s\n", lista_Utente[i].concelho);
    printf("contacto: %d\n", lista_Utente[i].contacto);
    printf("\n");
  }
}


void carregar_utente(){
	FILE* ptr;
	char str[100];
	int size = 50;
	ptr = fopen("utentes.txt", "a+");

	if (NULL == ptr) {
		printf("O ficheiro nao pode ser aberto\n");
	}
	lista_Utente = (LISTA_UTENTE)malloc(size * sizeof(UTENTE));
	char *token;
	int i = 0;
	fgets(str, 200, ptr);
	while (fgets(str, 200, ptr)) {
		token = strtok(str, "\t");
		strcpy(lista_Utente[i].Nome, token);
		token = strtok(NULL, "\t");
        lista_Utente[i].nif = atoi(token);
        token = strtok(NULL, "\t");
        strcpy(lista_Utente[i].concelho, token);
        token = strtok(NULL, "\t");
        lista_Utente[i].contacto = atoi(token);
		i++;
		
		if(i == size){
			size = 2*i;
			lista_Utente = (LISTA_UTENTE)realloc(lista_Utente,size*sizeof(UTENTE));
		}
	}
	
	Tamanho_Uts = i;
	lista_Utente = (LISTA_UTENTE)realloc(lista_Utente,Tamanho_Uts*sizeof(UTENTE)); // realocamos as posicoes que realmente importam
	fclose(ptr);
	
}

/*Esta fun��o aloca mem�ria para uma nova lista que � uma c�pia da lista anterior, mas com um espa�o a mais para o novo utente. 
Em seguida, adiciona o novo utente ao final da nova lista e atualiza o tamanho da lista. 
Finalmente, libera a mem�ria alocada para a antiga lista e atualiza a refer�ncia da lista para a nova lista.*/

void inserir_utente(UTENTE utente) {
    lista_Utente = (LISTA_UTENTE) realloc(lista_Utente, (Tamanho_Uts + 1) * sizeof(UTENTE));
    lista_Utente[Tamanho_Uts] = utente;
    Tamanho_Uts++;
}


/*Esta fun��o itera atrav�s da lista_Utente e procura pelo utente com o nif especificado. 
Quando encontrado, os campos do utente s�o substitu�dos pelos campos do novo utente passado como argumento. 
Se n�o encontrar um utente com o nif especificado, a fun��o n�o far� nada.*/

int alterar_utente(int nif, UTENTE utente) {
    int i;
    for (i = 0; i < Tamanho_Uts; i++) {
        if (lista_Utente[i].nif == nif) {
            lista_Utente[i] = utente;
            return 1;
        }
    }
    return 0;
}


/*Esta fun��o itera atrav�s da lista_Utente e procura pelo utente com o nif especificado.
Quando encontrado, o utente � removido da lista movendo todos os outros utentes para preencher o buraco.
O tamanho da lista � atualizado e a mem�ria alocada � redimensionada usando realloc.
Se n�o encontrar um utente com o nif especificado, a fun��o n�o far� nada. 
Retorna 1 se for removido o utente, ou zero caso contr�rio*/

int remover_utente(int nif) {
    int i, j;
    for (i = 0; i < Tamanho_Uts; i++) {
        if (lista_Utente[i].nif == nif) {
            for (j = i; j < Tamanho_Uts - 1; j++) {
                lista_Utente[j] = lista_Utente[j + 1];
            }
            Tamanho_Uts--;
            lista_Utente = (LISTA_UTENTE) realloc(lista_Utente, Tamanho_Uts * sizeof(UTENTE));
            return 1;
        }
    }
    return 0;
}


//---------------------------------------PUBLICACAO-----------------------------//
/*Esta fun��o adiciona uma nova publica��o na lista "lista_publicacao". 
Ela aloca mem�ria dinamicamente para armazenar a nova publica��o, aumentando o tamanho da lista.*/

void inserir_publicacao(PUBLICACAO publicacao)
{
    Tamanho_Pubs++;
    lista_publicacao = (LISTA_PUBLICACAO) realloc(lista_publicacao, Tamanho_Pubs * sizeof(PUBLICACAO));
    lista_publicacao[Tamanho_Pubs - 1] = publicacao;
}

/*Esta fun��o altera os campos de uma publica��o espec�fica na lista "lista_publicacao". 
A fun��o usa o ISBN da publica��o como refer�ncia para localizar a publica��o que deseja-se alterar e ent�o atualiza seus campos.*/

void alterar_publicacao(char * ISBN, PUBLICACAO publicacao)
{
	int i = 0;
    for (i = 0; i < Tamanho_Pubs; i++)
    {
        if (strcmp(lista_publicacao[i].ISBN, ISBN) == 0)
        {
            lista_publicacao[i] = publicacao;
            break;
        }
    }
}


/*Esta fun��o remove uma publica��o espec�fica da lista "lista_publicacao". 
A fun��o usa o ISBN da publica��o como refer�ncia para localizar a publica��o que deseja-se remover. 
Em seguida, ela realoca a mem�ria da lista para excluir a publica��o removida, diminuindo o tamanho da lista.*/
void remover_publicacao(char * ISBN)
{
    int pos = -1;
    int i;
    for (i = 0; i < Tamanho_Pubs; i++)
    {
        if (strcmp(lista_publicacao[i].ISBN, ISBN) == 0)
        {
            pos = i;
            break;
        }
    }
    if (pos != -1)
    {
    	int i;
        for (i = pos; i < Tamanho_Pubs - 1; i++)
        {
            lista_publicacao[i] = lista_publicacao[i + 1];
        }
        Tamanho_Pubs--;
        lista_publicacao = (LISTA_PUBLICACAO) realloc(lista_publicacao, Tamanho_Pubs * sizeof(PUBLICACAO));
    }
}

/*
Na fun��o abaixo, estamos abrindo o arquivo "pubs.txt" com permiss�o de escrita ("w"). 
Se o arquivo n�o puder ser aberto, � exibida uma mensagem de erro e a fun��o � encerrada.
Em seguida, usamos um loop para percorrer a lista de publica��es e escrever cada entrada no arquivo, 
usando a fun��o fprintf. 
Finalmente, fechamos o arquivo e exibimos uma mensagem de sucesso.
*/
void gravar_dados_publicacoes() {
    FILE *fp;
    fp = fopen("pubs.txt", "w");
    if (fp == NULL) {
        printf("Erro ao abrir o ficheiro\n");
        return;
    }
    int i = 0;
    for (; i < Tamanho_Pubs; i++) {
        fprintf(fp, "%d\t%s\t%s\t%s\t%d\t%d\n", lista_publicacao[i].ISBN, lista_publicacao[i].Titulo, lista_publicacao[i].Autor,
                lista_publicacao[i].Categoria, lista_publicacao[i].Ano, lista_publicacao[i].estado);
    }
    fclose(fp);
    printf("Publica��es gravadas com sucesso!\n");
}


//----------------------------------------------------------- Publicacao

void menu_publicacao(){
	char * opcoes[] = {
		"1 - inserir nova publicacao",
		"2 - remover publicao",
		"3 - alterar publicaco",
		"4 - calcular livros mais requisitado", 
		"5 - pesquisar publicaco por categoria",
		"6 - pesquisar publicaco por autor",
		"7 - pesquisar publicaco por autor titulo",
		"8 - listar publicacoes por utente",
		"0 - sair"
	};
	
	int escolha;
	print(opcoes, 9);
	

	do {
		scanf("%d", &escolha);
		switch(escolha){
			case 1:{
				PUBLICACAO publicacao;
				inserir_publicacao(publicacao);
				break;
			}
			case 2: 
				{
					char ISBN[100];
					scanf("%s", ISBN);
					remover_publicacao(ISBN);
					break;
				}
			case 3: 
				{
					char ISBN[100];
					PUBLICACAO publicacao;
					scanf("%s", ISBN);
					alterar_publicacao(ISBN, publicacao);
					break;
				}
			case 4:
				{
					PUBLICACAO resultado = calcular_livro_mais_requisitado();
					printf("ISBN %s Titulo %s  Autor %s\n", resultado.ISBN, resultado.Titulo, resultado.Autor);
					break;
				}
			case 5: 
				{
					char categoria[100];
					int nr_elementos;
					printf("Introduza a categoria pela qual pesquisa os elementos");
					scanf("%s", categoria);
					LISTA_PUBLICACAO lista_resultado = pesquisar_publicacao_por_categoria(categoria, &nr_elementos);
					int i = 0;
					PUBLICACAO resultado;
					for(;i < nr_elementos; ++i){
						resultado = lista_resultado[i];
						printf("ISBN %s Titulo %s  Autor %s\n", resultado.ISBN, resultado.Titulo, resultado.Autor);
					}
					break;
				}
			case 6: 
				{
					char autor[100];
					int nr_elementos;
					scanf("%s", autor);
					LISTA_PUBLICACAO lista_resultado = pesquisar_publicacao_por_autor(autor, &nr_elementos);
					break;
				}
			case 7: 
				{
				char titulo[100];
				int nr_elementos;
				scanf("%s", titulo);
				LISTA_PUBLICACAO lista_resultado = pesquisar_publicacao_por_titulo(titulo, &nr_elementos);
				break;
				}
			case 8:
				//ignorado
				break;
			case 0: 
				return;
				break;
				}
		
	} while(!escolha);
}

PUBLICACAO calcular_livro_mais_requisitado() {
    int i, j, contador_livros[Tamanho_Pubs];
    for (i = 0; i < Tamanho_Reqs; i++) {
        for (j = 0; j < Tamanho_Pubs; j++) {
            if (strcmp(lista_requisicao[i].ISBN, lista_publicacao[j].ISBN) == 0) {
                contador_livros[j]++;
                break;
            }
        }
    }

    int livro_mais_requisitado = 0;
    for (i = 1; i < Tamanho_Pubs; i++) {
        if (contador_livros[i] > contador_livros[livro_mais_requisitado]) {
            livro_mais_requisitado = i;
        }
    }

    return lista_publicacao[livro_mais_requisitado];
}




/*LISTA_PUBLICACAO pesquisar_publicacao_por_autor(char * autor, int * nr_elementos): 
Essa fun��o recebe como par�metro o nome do autor da publica��o e um ponteiro para uma vari�vel que ir� 
armazenar o tamanho do resultado da pesquisa. 
A fun��o pesquisa na lista de publica��es todas as publica��es cujo autor � igual ao autor fornecido e 
retorna um ponteiro para o in�cio de um vetor de publica��es que satisfazem esta condi��o. Al�m disso, 
atualiza a vari�vel apontada por nr_elementos com o tamanho do vetor retornado.*/

LISTA_PUBLICACAO pesquisar_publicacao_por_autor(char * autor, int * nr_elementos) {
	int size = 50;
	*nr_elementos = 0;
    LISTA_PUBLICACAO lista_resultado = (LISTA_PUBLICACAO)malloc(size*sizeof(PUBLICACAO));
    int i = 0;
    for (; i < Tamanho_Pubs; i++) {
        if (strcmp(lista_publicacao[i].Autor, autor) == 0) {
            adicionar_publicacao_lista(lista_resultado, lista_publicacao[i], nr_elementos);
            if(size == *nr_elementos){
            	size+=size;
            	lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (size) * sizeof(PUBLICACAO));
			}
        }
    }
    lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (*nr_elementos) * sizeof(PUBLICACAO));
    return lista_resultado;
}
/*LISTA_PUBLICACAO pesquisar_publicacao_por_titulo(char * titulo, int * nr_elementos): 
Essa fun��o funciona de maneira semelhante � anterior, mas pesquisa publica��es pelo t�tulo ao inv�s de pelo autor.*/

LISTA_PUBLICACAO pesquisar_publicacao_por_titulo(char * titulo, int * nr_elementos) {
	int size = 50;
	*nr_elementos = 0;
    LISTA_PUBLICACAO lista_resultado = (LISTA_PUBLICACAO)malloc(size*sizeof(PUBLICACAO));
    int i = 0;
    for (; i < Tamanho_Pubs; i++) {
        if (strcmp(lista_publicacao[i].Titulo, titulo) == 0) {
            adicionar_publicacao_lista(lista_resultado, lista_publicacao[i], nr_elementos);
            if(size == *nr_elementos){
            	size+=size;
            	lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (size) * sizeof(PUBLICACAO));
			}
        }
    }
    lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (*nr_elementos) * sizeof(PUBLICACAO));
    return lista_resultado;
}
/*LISTA_PUBLICACAO pesquisar_publicacao_por_categoria(char * categoria, int * tamanho_resultado, int * nr_elementos): 
Essa fun��o funciona de maneira semelhante �s duas anteriores, mas pesquisa publica��es pela 
categoria ao inv�s de pelo autor ou t�tulo.*/

LISTA_PUBLICACAO pesquisar_publicacao_por_categoria(char * categoria, int * nr_elementos) {
	int size = 50;
	*nr_elementos = 0;
    LISTA_PUBLICACAO lista_resultado = (LISTA_PUBLICACAO)malloc(size*sizeof(PUBLICACAO));
    int i = 0;
    for (; i < Tamanho_Pubs; i++) {
        if (strcmp(lista_publicacao[i].Categoria, categoria) == 0) {
            adicionar_publicacao_lista(lista_resultado, lista_publicacao[i], nr_elementos);
            if(size == *nr_elementos){
            	size+=size;
            	lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (size) * sizeof(PUBLICACAO));
			}
        }
    }
    lista_resultado = (LISTA_PUBLICACAO)realloc(lista_resultado, (*nr_elementos) * sizeof(PUBLICACAO));
    return lista_resultado;
}


/*
A fun��o adicionar_publicacao_lista recebe como argumentos a lista de resultados, 
a publica��o a ser adicionada e o tamanho da lista de resultados.
Em primeiro lugar, o tamanho da lista de resultados � incrementado em 1. 
Em seguida, o tamanho da lista de resultados � aumentado usando a fun��o realloc 
para armazenar a nova publica��o. 
Finalmente, a nova publica��o � adicionada ao final da lista de resultados.
*/
void adicionar_publicacao_lista(LISTA_PUBLICACAO lista_resultado, PUBLICACAO publicacao, int *tamanho_lista_resultado)
{
    (*tamanho_lista_resultado)++;
    lista_resultado[(*tamanho_lista_resultado) - 1] = publicacao;
}




carregar_publicacao(){
	FILE* ptr;
	char str[100];
	int size = 50;
	ptr = fopen("pubs.txt", "a+");

	if (NULL == ptr) {
		printf("file can't be opened \n");
	}
	lista_publicacao = (LISTA_PUBLICACAO)malloc(size * sizeof(PUBLICACAO));
	char *token;
	int i = 0;
	fgets(str, 200, ptr);
	while (fgets(str, 200, ptr)) {
		token = strtok(str, "\t");
		strcpy(lista_publicacao[i].ISBN, token);
		token = strtok(NULL, "\t");
        strcpy(lista_publicacao[i].Titulo, token);
        token = strtok(NULL, "\t");
        strcpy(lista_publicacao[i].Autor, token);
        token = strtok(NULL, "\t");
        strcpy(lista_publicacao[i].Categoria, token);
        token = strtok(NULL, "\t");
        lista_publicacao[i].Ano = atoi(token);
		i++;
		
		if(i == size){
			size = 2*i;
			lista_publicacao = (LISTA_PUBLICACAO)realloc(lista_publicacao,size*sizeof(PUBLICACAO));
		}
	}
	
	Tamanho_Pubs = i;
	lista_publicacao = (LISTA_PUBLICACAO)realloc(lista_publicacao,Tamanho_Pubs*sizeof(PUBLICACAO));
	fclose(ptr);
	
}

//-------------------------------- impl----------------------------
/*A seguinte fun��o "void listar_publicacoes_requisitadas_por_utente(int nif)" lista todas as publica��es requisitadas por um dado utente, 
identificado pelo seu nif. Para isso, percorre-se a lista de requisi��es (lista_requisicao) e para cada requisi��o, verifica-se se o nif da 
requisi��o corresponde ao nif fornecido como argumento. Se corresponderem, procura-se a publica��o correspondente ao ISBN da requisi��o na lista 
de publica��es (lista_publicacao) e imprime-se os seus dados.*/

void listar_publicacoes_requisitadas_por_utente(int nif) {
    int i, j;
    for (i = 0; i < Tamanho_Reqs; i++) {
        if (lista_requisicao[i].nif == nif) {
            for (j = 0; j < Tamanho_Pubs; j++) {
                if (strcmp(lista_requisicao[i].ISBN, lista_publicacao[j].ISBN) == 0) {
                    printf("ISBN: %s\n", lista_publicacao[j].ISBN);
                    printf("T�tulo: %s\n", lista_publicacao[j].Titulo);
                    printf("Autor: %s\n", lista_publicacao[j].Autor);
                    printf("Categoria: %s\n", lista_publicacao[j].Categoria);
                    printf("Ano: %d\n", lista_publicacao[j].Ano);
                    printf("Estado: %s\n", (lista_publicacao[j].estado)? "Nao disponivel" : "Disponivel" );
                }
            }
        }
    }
}

/*A fun��o faz um loop por todas as requisi��es e verifica se o ISBN � igual ao ISBN passado como par�metro. 
Se for igual, o loop segue para o pr�ximo passo, onde verifica o NIF da requisi��o atual com todos os NIFs dos utentes na lista_Utente. 
Se o NIF corresponder, os dados do utente s�o impressos na tela.*/

void listar_utentes_requisitaram_livro(char * ISBN) {
    printf("Utentes que requisitaram o livro %s:\n", ISBN);
    int i = 0;
	for (; i < Tamanho_Reqs; i++) {
        if (strcmp(lista_requisicao[i].ISBN, ISBN) == 0) {
            int j = 0;
			for (; j < Tamanho_Uts; j++) {
                if (lista_requisicao[i].nif == lista_Utente[j].nif) {
                    printf("Nome: %s\n", lista_Utente[j].Nome);
                    printf("NIF: %d\n", lista_Utente[j].nif);
                    printf("Concelho: %s\n", lista_Utente[j].concelho);
                    printf("Contacto: %d\n", lista_Utente[j].contacto);
                }
            }
        }
    }
}



float calcular_multa(REQUISICAO req) {
    DATA hoje = data_hoje();
    
	int diasAtraso = numeroDeDias(req.data_devolucao, hoje);
    
    if (diasAtraso <= 0) {
        return 0.0;
    }

    float multa = 0;
    if (diasAtraso <= 5) {
        multa = 0.5 * diasAtraso;
    } else if (diasAtraso <= 10) {
        multa = 2.5 + (diasAtraso - 5) * 1;
    } else {
        multa = 7 + (diasAtraso - 10) * 2;
    }

    return fmin(multa, 50);
}


//---------------------------------- Utils------------------------------------------//

DATA data_hoje() {
    time_t rawtime;
    struct tm * timeinfo;
    DATA d;
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    d.dia = timeinfo->tm_mday;
    d.mes = timeinfo->tm_mon + 1; // janeiro � representado como 0
    d.ano = timeinfo->tm_year + 1900; // ano � relativo ao ano 1900
    return d;
}

// Fun��o que retorna 1 se a data � v�lida, 0 caso contr�rio
int verificaData(DATA d) {
    if (d.mes < 1 || d.mes > 12)
        return 0;
    if (d.dia < 1 || d.dia > 31)
        return 0;
    if ((d.mes == 4 || d.mes == 6 || d.mes == 9 || d.mes == 11) && d.dia > 30)
        return 0;
    if (d.mes == 2) {
        if (d.dia > 29)
            return 0;
        if (d.dia == 29 && ((d.ano % 4 != 0) || (d.ano % 100 == 0 && d.ano % 400 != 0)))
            return 0;
    }
    return 1;
}

// Fun��o que calcula o n�mero de dias entre duas datas, podendo ser negativo -> ainda n�o passou a data de devolu��o.
int numeroDeDias(DATA d1, DATA d2) {
    int dias1 = d1.ano * 365 + d1.dia;
    int dias2 = d2.ano * 365 + d2.dia;
    int meses[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    dias1 += meses[d1.mes - 1];
    dias2 += meses[d2.mes - 1];
    if (d1.ano % 4 == 0 && (d1.ano % 100 != 0 || d1.ano % 400 == 0) && d1.mes > 2)
        dias1++;
    if (d2.ano % 4 == 0 && (d2.ano % 100 != 0 || d2.ano % 400 == 0) && d2.mes > 2)
        dias2++;
    return dias2 - dias1;
}


//Fun��o que serve para imprimir o menu de cadam op��o selecionada
void print(char **opcoes, int tamanho){
	int i = 0;
	
	for(; i < tamanho; ++i){
		printf("%s\n", opcoes[i]);
	}
}

