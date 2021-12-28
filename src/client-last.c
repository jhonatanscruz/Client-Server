#include "common.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

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

    /* %%%%%% INICIALIZA O ARQUIVO %%%%%% */
    FILE *arq;
    //abrindo o arquivo
    arq = fopen("client-goku.log", "a");
    if (arq == NULL)
    {
        arq = fopen("client-goku.log", "w");
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

    fprintf(arq, "[%d/%d/%d as %d:%d:%d] Conexao solicitada\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);

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
    //objetiva inicializar o gerador de números aleatórios com o valor da função time(NULL).
    srand(time(NULL));

    unsigned char msg[120] = "";

    //Obtendo o tempo em segundos  
    time(&sec);
    //para converter de segundos para o tempo local
    datetime = localtime(&sec);

    //sprintf(msg,"%.2x%.x%.2x%.2x%.2x%.2x0", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year-100, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    
    // criando a lista de bits
	TipoLista* lista = InicializaLista();

    unsigned int signal = 0;
    unsigned int offset = 7896;
    unsigned int real_speed = 60411;
    unsigned int frac_speed = 69;
    unsigned int volume = 7425452;
    unsigned int devId = 565;

    // Transforma em binário e insere na lista
    intToBin(offset, 14, lista);
    intToBin(signal, 1, lista);
    intToBin(frac_speed, 7, lista);
    intToBin(real_speed, 16, lista);
    intToBin(volume, 24, lista);
    intToBin(devId, 10, lista);
    intToBin(datetime->tm_sec, 6, lista);
    intToBin(datetime->tm_min, 6, lista);
    intToBin(datetime->tm_hour, 5, lista);
    intToBin(datetime->tm_year-100, 6, lista);
    intToBin(datetime->tm_mon+1, 4, lista);
    intToBin(datetime->tm_mday, 5, lista);
    // Codifica a lista
    TipoLista* listabit = code(lista);

    store(listabit, msg);

    size_t count = send(s, msg, strlen(msg), 0);

    time(&sec);
    //para converter de segundos para o tempo local
    datetime = localtime(&sec);
    fprintf(arq, "[%d/%d/%d as %d:%d:%d] Mensagem Enviada\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);

    if(count != strlen(msg)){
        logexit("send");
    }

/* %%%%%% AGUARDA RESPOSTA DO SERVIDOR %%%%%% */
    unsigned char buf[BUFSZ];
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

    // Libera lista
    Libera(lista);
    // Libera lista
    Libera(listabit);

    time(&sec);
    //para converter de segundos para o tempo local
    datetime = localtime(&sec);
    fprintf(arq, "[%d/%d/%d as %d:%d:%d] Conexao Encerrada\n\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    
    fclose(arq); // fechando arquivo

    printf("Received %d bytes\n",total);
    puts(buf);
    exit(EXIT_SUCCESS);

    return 0;
}