# Makefile for Assignment #1
# Christopher Oehler



BIN = ./bin/
SRC = ./src/
LIB = ./lib/
INC = ./include/
OUT = ../

GCC=gcc
CFLAGS= -Wall -g -std=c11 -fpic

all : $(SRC)GEDCOMparser.c $(INC)GEDCOMparser.h $(SRC)GEDCOMutilities.c $(INC)GEDCOMutilities.h $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(GCC) $(CFLAGS) -c $(SRC)GEDCOMparser.c -Iinclude -o $(BIN)GEDCOMparser.o
	$(GCC) $(CFLAGS) -c $(SRC)GEDCOMutilities.c -Iinclude -o $(BIN)GEDCOMutilities.o
	$(GCC) $(CFLAGS) -c $(SRC)LinkedListAPI.c -Iinclude -o $(BIN)LinkedListAPI.o
	$(GCC) -shared -o $(OUT)GEDCOMparser.so $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)LinkedListAPI.o

clean :
	rm $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)LinkedListAPI.o $(BIN)liblist.a $(BIN)libutilities.a $(BIN)libparse.a
