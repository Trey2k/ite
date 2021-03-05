CC = gcc
CFLAGS  = -g -Wall

default: ite

ite: object/ite.o
	$(CC) $(CFLAGS) -o ite object/ite.o -lncurses

object/ite.o: object src/ite.c src/key_defs.h
	$(CC) $(CFLAGS) -c src/ite.c -o object/ite.o

object:
	mkdir $@

clean:
	$(RM) count object/*.o *~
