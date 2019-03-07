#!/usr/bin/make -f
SHELL = /bin/sh
CC = gcc
CFLAGS = -pedantic -pedantic-errors -Wall -Wextra -std=c99 -g
APP = farmafit
EXAMPLE = example.json
OBJECTS = farmafit.o main.o cJSON.o
SOURCES = src/farmafit.c src/main.c 
LIBRARIES = lib/cJSON/cJSON.c

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(APP)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES) $(LIBRARIES)

.PHONY: indent example test clean

indent:
	indent -l79 $(SOURCES)

example: $(APP)
	./$(APP) -f $(EXAMPLE)

test: $(APP)
	tests/run_tests.sh

clean:
	rm $(APP) $(OBJECTS)
