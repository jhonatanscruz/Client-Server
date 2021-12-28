#include "common.h"
#include "lista.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define BUFSZ 1024

struct client_data{
    int csock;
    struct sockaddr_storage storage;
};

/* %%%%%% RETORNA MENSAGEM DE UTILIZAÇÃO DO PROGRAMA %%%%%% */
void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server Port>\n",argv[0]);
    printf("EXAMPLE: %s v4 51511\n",argv[0]);
    exit(EXIT_FAILURE);
}

/* %%%%%% PERMITE A CONEXÃO DE MÚLTIPLOS CLIENTES %%%%%% */
void* client_thread(void *data){

    /* %%%%%% INICIALIZA O ARQUIVO %%%%%% */
    FILE *arq;
    //abrindo o arquivo
    arq = fopen("connections.log", "a");
    if (arq == NULL)
    {
        arq = fopen("connections.log", "w");
        if (arq == NULL){
            logexit("fopen");
        }
    }

    /* %%%%%% INICIALIZA A HORA %%%%%% */
    //Ponteiro para struct que armazena data e hora  
    struct tm *datetime;
    //Variável do tipo time_t para armazenar o tempo em segundos  
    time_t sec;
    //Obtendo o tempo em segundos  
    time(&sec);
    //para converter de segundos para o tempo local
    datetime = localtime(&sec);

    struct client_data *cdata = (struct client_data*)data;
    struct sockaddr *caddr = (struct sockaddr*)(&cdata->storage);

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[%d/%d/%d as %d:%d:%d] bound to %s, waiting connections...\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec, caddrstr);
    //fprintf(arq, "[%d/%d/%d às %d:%d:%d] bound to %s, waiting connections...\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec, caddrstr);

    unsigned int bitdecode[12];
    unsigned char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    size_t count = recv(cdata->csock, buf, BUFSZ - 1, 0);
    time(&sec);
    datetime = localtime(&sec);
    printf("[%d/%d/%d as %d:%d:%d] Message from %s, %d bytes: %s\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec, caddrstr, (int)count, buf);
    decode(buf, bitdecode);

    float speed = (float)bitdecode[8] + ((float)bitdecode[9]/100);

    fprintf(arq, "[%d/%d/%d as %d:%d:%d] Message from %s, %d bytes:", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec, caddrstr, (int)count);
    fprintf(arq,"\n{\n'devId':%d,\n'date':%d/%d/%d\n'time':%d:%d:%d\n'volume':%d\n'speed':%.2f\n'offset':%c%d\n}\n",bitdecode[6],bitdecode[0],bitdecode[1],bitdecode[2],bitdecode[3],bitdecode[4],bitdecode[5],bitdecode[7],speed,bitdecode[10],bitdecode[11]);
    fclose(arq); // fechando arquivo

    // ENVIA RESPOSTA AO CLIENTE
    int tam = BUFSZ + sizeof(caddrstr) + sizeof(count);
    char cbuf[tam + 30];
    memset(cbuf, 0, tam + 30);
    sprintf(cbuf, "Message from %s, %d bytes: %s\n", caddrstr, (int)count, buf);
    count = send(cdata->csock, cbuf, strlen(cbuf)+1, 0);
    
    if(count != strlen(cbuf)+1){
        logexit("send");
    }

    // Encerra a conexão com o cliente atual e passa a tratar o próximo
    close(cdata->csock);

    // Encerra a thread
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv){
    if(argc < 3){
        usage(argc, argv);
    }

    /* %%%%%% INICIALIZA O SERVIDOR %%%%%% */

    struct sockaddr_storage storage;
    if(server_sockaddr_init(argv[1], argv[2], &storage) != 0){
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); // CHAMANDO A FUNÇÃO SOCKET
    if(s == -1){ // Verifica se a função socket retorna erro
        logexit("socket");
    }

    int enable = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0){
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr*)(&storage);
    // BIND
    if(bind(s, addr, sizeof(storage)) != 0){
        logexit("bind");
    }
    // LISTEN
    if(listen(s, 10) != 0){
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Server is up, waiting for connections...\n");

    /* %%%%%% AGUARDA CONEXÃO DO CLIENTE %%%%%% */
    while(1){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr*)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if(csock == -1){ // Verifica se a função accept retorna erro
            logexit("accept");
        }

        struct client_data* cdata = malloc(sizeof(*cdata));
        if(!cdata){
            logexit("malloc");
        }
        cdata->csock = csock;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        /* %%%%%% CRIA CONEXÕES PARALELAS DOS MÚLTIPLOS CLIENTES %%%%%% */
        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);

    return 0;
}
