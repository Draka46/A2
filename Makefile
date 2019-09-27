CC = gcc
CFLAGS = -Wall -std=c11 -g -ggdb

all: sim

../src.zip: Makefile *.h *.c
	cd .. && zip src.zip src/*.h src/*.c src/Makefile


sim: *.c *.h
	$(CC) $(CFLAGS) *.c -o sim

clean:
	rm -f *.o sim
