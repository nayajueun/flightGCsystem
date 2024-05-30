CC=gcc
CFLAGS=-I./include

all: main

main: src/main.c src/gc.c src/flight.c
	$(CC) $(CFLAGS) -o main src/main.c src/gc.c src/flight.c

test: tests/test_gc.c src/gc.c
	$(CC) $(CFLAGS) -o test_gc tests/test_gc.c src/gc.c
	./test_gc

clean:
	rm -f main test_gc

.PHONY: all test clean
