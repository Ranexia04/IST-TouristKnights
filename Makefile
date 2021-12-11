CC = gcc

CFLAGS = -Wall -std=c99 -O3

SOURCES = main.c variaveis.c auxiliares.c acervo.c memoria.c

OBJECTS = main.o variaveis.o auxiliares.o acervo.o memoria.o

tuktuk: $(OBJECTS)
	gcc -O3 -o $@ $(OBJECTS)

main.o: tourist_knight.h main.c

variaveis.o: tourist_knight.h variaveis.c

auxiliares.o: tourist_knight.h auxiliares.c

acervo.o: tourist_knight.h acervo.c

memoria.o: tourist_knight.h memoria.c

clean::
	rm -f *.o core a.out tuktuk *~

