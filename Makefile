CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
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
examples:  ./examples/alloc.bm ./examples/pi.bm ./examples/123f.bm

./examples/alloc.bm: basm ./examples/alloc.basm
	./basm ./examples/alloc.basm ./examples/alloc.bm

./examples/pi.bm: basm ./examples/pi.basm
	./basm ./examples/pi.basm ./examples/pi.bm

./examples/123f.bm: basm ./examples/123f.basm
	./basm ./examples/123f.basm ./examples/123f.bm