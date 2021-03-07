CC = gcc
CFLAGS  = -g -Wall

default: ite

ite: object/display.o object/ite.o
	$(CC) $(CFLAGS) -o ite  object/display.o object/ite.o -lncurses

object/ite.o: object src/ite.c src/defs.h
	$(CC) $(CFLAGS) -c src/ite.c -o object/ite.o

object/display.o: object src/display.c src/display.h src/defs.h
	$(CC) $(CFLAGS) -c src/display.c -o object/display.o

object:
	mkdir $@

clean:
	$(RM) count object/*.o *~
