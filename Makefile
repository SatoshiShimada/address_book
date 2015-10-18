
CC=gcc
CFLAGS=-Wall

OBJ=main.o address.o
OUT=address

.PHONY: all
all: main.o address.o
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)
