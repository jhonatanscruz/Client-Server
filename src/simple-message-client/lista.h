/*
************************************************************
*   Autor: Jhonatan da SIlva Cruz
*   Data: 18/12/2021

*************************************************************
*/

#ifndef LISTA_H_
#define LISTA_H_

/*Tipo que define a lista (tipo opaco)
  Estrutura interna do tipo deve ser definida na implementacao do TAD.
  Usar lista COM Sentinela
*/
typedef struct tipolista TipoLista;

/*
* Inicializa o sentinela de uma lista
* inputs: nenhum
* output: Sentinela inicializado
* pre-condicao: nenhuma
* pos-condicao: sentinela da lista de retorno existe e os campos primeiro e ultimo apontam para NULL
*/
TipoLista* InicializaLista();

/*
* Insere um bit na primeira posicao da lista
* inputs: o bit a ser inserido na lista (do tipo unsigned char) e a lista
* output: nenhum
* pre-condicao: lista nao é nula
* pos-condicao: lista contem a matriz inserida na primeira posicao
*/
void Insere(unsigned char bit, TipoLista* lista);

/*
* Transforma um inteiro n em binário com bitNum bits e insere na lista
* inputs: o inteiro n, o número de bits bitNum e a lista
* output: nenhum
* pre-condicao: lista nao é nula
* pos-condicao: lista contem o número n em binário com bitNum bits
*/
void intToBin(unsigned int n, unsigned char bitNum, TipoLista* lista);

/*
* Codifica uma lista de bits obrigatoriamente com 4*n bits, onde n é um inteiro positivo
* inputs: a lista a ser codificada
* output: nenhum
* pre-condicao: lista nao é nula
* pos-condicao: lista contem uma serie de valores codificados
*/
TipoLista* code(TipoLista* lista);

/*
* Armazena cada item da lista no vetor msg
* inputs: a lista e o vetor
* output: nenhum
* pre-condicao: lista e vetor não são nulos
* pos-condicao: vetor preenchido com elementos da lista
*/
void store(TipoLista* lista, unsigned char msg[]);

/*
* Decodifica a msg seguindo uma regra de negócio estabeleciada previamente
* inputs: o vetor msg
* output: o vetor com os valores decodificados
* pre-condicao: vetor não nulo
* pos-condicao: novo vetor preenchido
*/
void decode(unsigned char* msg, unsigned int* listaDecod);

/*
* Imprime a lista de bits
* inputs: a lista
* output: nenhum
* pre-condicao: lista nao e nula
* pos-condicao: lista de bits impressa na tela
*/
void Imprime(TipoLista* lista);

/*Libera toda a memoria alocada para a lista
* inputs: a lista encadeada de matrizes
* output: nenhum
* pre-condicao: lista nao nula
* pos-condicao: memoria alocada para a lista e seus componentes liberada
*/
void Libera (TipoLista* lista);

#endif /* LISTA_H_ */