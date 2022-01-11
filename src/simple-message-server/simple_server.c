#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server Port>\n",argv[0]);
    printf("EXAMPLE: %s v4 51511\n",argv[0]);
    exit(EXIT_FAILURE);
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
    printf("bound to %s, waiting connections...\n", addrstr);

/* %%%%%% AGUARDA CONEXÃO DO CLIENTE %%%%%% */
    while(1){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr*)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if(csock == -1){ // Verifica se a função accept retorna erro
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("bound to %s, waiting connections...\n", caddrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0);
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        // ENVIA RESPOSTA AO CLIENTE
        int tam = BUFSZ + sizeof(caddrstr) + sizeof(count);
        char cbuf[tam + 30];
        memset(cbuf, 0, tam + 30);
        sprintf(cbuf, "Message from %s, %d bytes: %s\n", caddrstr, (int)count, buf);
        count = send(csock, cbuf, strlen(cbuf)+1, 0);
        
        if(count != strlen(cbuf)+1){
            logexit("send");
        }

        // Encerra a conexão com o cliente atual e passa a tratar o próximo
        close(csock);
    }

    exit(EXIT_SUCCESS);

    return 0;
}