CFLAGS=-Wall -Wextra -Wswitch-enum -Wconversion -std=c11 -pedantic 
LIBS=

.PHONY: all
all: basm bme debasm

basm: ./src/basm.c ./src/bm.h
	$(CC) $(CFLAGS) -o basm ./src/basm.c $(LIBS)

bme: ./src/bme.c ./src/bm.h
	$(CC) $(CFLAGS) -o bme ./src/bme.c $(LIBS)

debasm: ./src/debasm.c ./src/bm.h
	$(CC) $(CFLAGS) -o debasm ./src/debasm.c $(LIBS)

.PHONY: examples
examples:  ./examples/alloc.bm ./examples/memory.bm

./examples/alloc.bm: basm ./examples/alloc.basm
	./basm ./examples/alloc.basm ./examples/alloc.bm

	
./examples/memory.bm: basm ./examples/memory.basm
	./basm ./examples/memory.basm ./examples/memory.bm