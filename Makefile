CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS=

.PHONY: all
all: basm bme nan

basm: ./src/basm.c ./src/bm.h
	$(CC) $(CFLAGS) -o basm ./src/basm.c $(LIBS)

bme: ./src/bme.c ./src/bm.h
	$(CC) $(CFLAGS) -o bme ./src/bme.c $(LIBS)

nan: ./src/nan.c 
	$(CC) $(CFLAGS) -o nan ./src/nan.c $(LIBS)


.PHONY: examples
examples:  ./examples/123.bm

./examples/123.bm: basm ./examples/123.basm
	./basm ./examples/123.basm ./examples/123.bm