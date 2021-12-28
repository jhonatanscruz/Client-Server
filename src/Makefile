all:
	gcc -Wall -c common.c
	gcc -Wall client-last.c common.o -o client-mt
	gcc -Wall server-mt.c common.o -lpthread -o server-mt
