#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <server IP> <server Port>\n",argv[0]);
    printf("EXAMPLE: %s 127.0.0.1 51511\n",argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if(argc < 3){
        usage(argc, argv);
    }

/* %%%%%% INICIA CONEXÃO COM O SERVIDOR %%%%%% */

    struct sockaddr_storage storage;
    if(addrparse(argv[1], argv[2], &storage) != 0){
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); // CHAMANDO A FUNÇÃO SOCKET
    if(s == -1){ // Verifica se a função socket retorna erro
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr*)(&storage);
    if(connect(s, addr, sizeof(storage)) != 0){
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("Connected to %s\n", addrstr);

/* %%%%%% ENVIA REQUISIÇÃO PARA O SERVIDOR %%%%%% */

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    printf("Message: ");
    fgets(buf, BUFSZ-1,stdin);
    size_t count = send(s, buf, strlen(buf)+1, 0);
    if(count != strlen(buf)+1){
        logexit("send");
    }

/* %%%%%% AGUARDA RESPOSTA DO SERVIDOR %%%%%% */

    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while(1){
        count = recv(s, buf + total, BUFSZ - total, 0);
        if(count == 0){
            //connection terminated
            break;
        }
        total += count;
    }

/* %%%%%% ENCERRA A CONEXÃO COM O SERVIDOR %%%%%% */
    
    close(s);
    printf("Received %d bytes\n",total);
    puts(buf);
    exit(EXIT_SUCCESS);

    return 0;
}