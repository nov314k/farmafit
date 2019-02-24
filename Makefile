#!/usr/bin/make -f
SHELL = /bin/sh
CC = gcc
CFLAGS = -Wall -std=c99
EXE = farmafit.exe
OBJECTS = farmafit.o main.o cJSON.o
SOURCES = src/farmafit.c src/main.c 
LIBRARIES = lib/cJSON.c

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE)
	rm $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES) $(LIBRARIES)

.PHONY: indent run clean
indent:
	indent -l79 $(SOURCES)

run: $(EXE)
	./$(EXE)

clean:
	rm $(EXE) $(OBJECTS)
