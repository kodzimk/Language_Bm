CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS=

basm: ./src/debasm.c ./src/bm.h
	$(CC) $(CFLAGS) -o debasm ./src/debasm.c $(LIBS)