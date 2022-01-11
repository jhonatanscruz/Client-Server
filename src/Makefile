all:
	gcc -Wall -c common.c lista.c
	gcc -Wall client-last.c common.o lista.o -o client-mt -lm
	gcc -Wall server-mt.c common.o lista.o -lpthread -o server-mt -lm
