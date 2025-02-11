CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS=

basm: ./src/basm.c ./src/bm.h
	$(CC) $(CFLAGS) -o basm ./src/basm.c $(LIBS)