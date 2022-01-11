/*
*******************************************************************
*   Autor: Jhonatan da SIlva Cruz
*   Data: 18/12/2021

*******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lista.h"

/*Tipo que define a celula
  Estrutura interna do tipo definida na implementacao do TAD.
*/
typedef struct celula Celula;

struct celula
{
    Celula* prox;
    Celula* ant;
    unsigned char bit;
};

/*Tipo que define a lista (tipo opaco)
  Estrutura interna do tipo deve ser definida na implementacao do TAD.
  Usar lista COM Sentinela
*/
struct tipolista
{
    Celula* Primeiro;
    Celula* Ultimo;
};

/*Inicializa uma lista
* inputs: nenhum
* output: Lista inicializada
* pre-condicao: nenhuma
* pos-condicao: Lista existe e o campo primeiro aponta para NULL
*/
TipoLista* InicializaLista()
{
    TipoLista* lista = (TipoLista*)malloc(sizeof(TipoLista));

    if(!lista){
        printf("Ocorreu erro na alocação de memória!");
        exit(1);
    }

    lista->Primeiro = NULL;

    return lista;
}

/*Insere um bit na lista
* inputs: a matriz a ser inserida na lista (do tipo Matriz) e a lista
* output: nenhum
* pre-condicao: matriz e lista nao sao nulas
* pos-condicao: lista contem a matriz inserida na lista
*/
void Insere (unsigned char bit, TipoLista* lista)
{
    Celula* cel = (Celula*)malloc(sizeof(Celula));
    if(!cel){
        printf("Ocorreu erro na alocação de memória!");
        exit(1);
    }

    if(lista->Primeiro == NULL){
        cel->prox = NULL;
        lista->Ultimo = cel;
    }
    else{
        Celula* aux = lista->Primeiro;

        aux->ant = cel;
        cel->prox = aux;
    }
    cel->ant = NULL;
    cel->bit = bit;
    lista->Primeiro = cel;
}

/*
* Transforma um inteiro n em binário com bitNum bits e insere na lista
* inputs: o inteiro n, o número de bits bitNum e a lista
* output: nenhum
* pre-condicao: lista nao é nula
* pos-condicao: lista contem o número n em binário com bitNum bits
*/
void intToBin(unsigned int n, unsigned char bitNum, TipoLista* lista){

    unsigned int quociente = n;

    while(1){
        unsigned int aux = floor(quociente / 2);
        unsigned char bin = quociente - (aux * 2);
        Insere(bin,lista);
        quociente = aux;
        --bitNum;
        if (aux <= 1) break;
    }
    if(bitNum > 0){
        Insere(quociente, lista);
        --bitNum;
    }

    for(int a = 0; a < bitNum; a++){
        Insere(0, lista);
    }
}

/*
* Codifica uma lista de bits obrigatoriamente com 4*n bits, onde n é um inteiro positivo
* inputs: a lista a ser codificada
* output: nenhum
* pre-condicao: lista nao é nula
* pos-condicao: lista contem uma serie de valores codificados
*/
TipoLista* code(TipoLista* lista){

    TipoLista* novaLista = InicializaLista();

    if(lista){
        Celula* cel = lista->Primeiro;
        unsigned char sum = 0;

        while(cel != NULL){
            sum += 1;
            cel = cel->prox;
        }

        if((sum % 8) == 0){
            cel = lista->Ultimo;

            while(cel != NULL){
                sum = 0;
                for(int i = 0; i < 8; i++){
                    sum += pow(2,i) * cel->bit;
                    cel = cel->ant;
                }
                Insere(sum, novaLista);
            }
        }
    }

    return novaLista;
}

/*
* Armazena cada item da lista no vetor msg
* inputs: a lista e o vetor
* output: nenhum
* pre-condicao: lista e vetor não são nulos
* pos-condicao: vetor preenchido com elementos da lista
*/
void store(TipoLista* lista, unsigned char msg[]){
    // Verifica se a lista não é nula
    if(lista){
        // Imprime o conteúdo da lista
        Celula* cel = lista->Primeiro;
        int i = 0;
        while(cel != NULL){
            msg[i] = cel->bit;
            i++;
            cel = cel->prox;
        }
    }
}

/*
* Decodifica a msg seguindo uma regra de negócio estabeleciada previamente
* inputs: o vetor msg
* output: o vetor com os valores decodificados
* pre-condicao: vetor não nulo
* pos-condicao: novo vetor preenchido
*/
void decode(unsigned char* msg, unsigned int* listaDecod){
    TipoLista* listabit = InicializaLista();

    unsigned char key[12] = {14,1,7,16,24,10,6,6,5,6,4,5};
    unsigned int sum = 0;
    unsigned char l = 11;

    for(int i = 12; i >= 0; i--){
        intToBin(msg[i], 8, listabit);
    }

    Celula* cel = listabit->Ultimo;

    for(unsigned char k = 0; k < 12;k++){
        sum = 0;
        for(unsigned char j = 0; j < key[k]; j++){
            sum += pow(2,j) * cel->bit;
            cel = cel->ant;
        }
        listaDecod[l] = sum;
        l--;
    }

    Libera(listabit);

}

/*
* Imprime a lista de bits
* inputs: a lista
* output: nenhum
* pre-condicao: lista nao e nula
* pos-condicao: lista de bits impressa na tela
*/
void Imprime(TipoLista* lista){
    printf("\n");
    // Verifica se a lista não é nula
    if(lista){
        // Imprime o conteúdo da lista
        Celula* cel = lista->Primeiro;

        while(cel != NULL){
            printf("%d ",cel->bit);
            cel = cel->prox;
        }
    }
    printf("\n");
}

/*Libera toda a memoria alocada para a lista (lembre-se de tambem liberar a memoria para a matriz)
* inputs: a lista encadeada de matrizes
* output: nenhum
* pre-condicao: lista nao nula
* pos-condicao: memoria alocada para a lista e seus componentes liberada
*/
void Libera (TipoLista* lista)
{
    // Verifica se a lista não é nula
    if(lista){
        // Libera o espaço alocado para as células que compõem a lista
        Celula* aux;
        Celula* cel = lista->Primeiro;

        while(cel != NULL){
            aux = cel->prox;
            free(cel);
            cel = aux;
        }
        // Libera o espaço alocado para a estrutura lista
        free(lista);
    }
}