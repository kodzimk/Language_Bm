CC=gcc
CFLAGS=-Wall -std=c99

# CC=i686-w64-mingw32-gcc
# LN=ld -m i386pe

.PHONY: default build run
default: build run

build: main.c
	$(CC) $(CFLAGS) $(FILES) -o main # add '-lws2_32' if platform 'windows'
run: main
	./main