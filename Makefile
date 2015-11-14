
CC=gcc
CFLAGS=-Wall

OBJ=main.o address.o
OUT=address

.PHONY: all
all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)

$.o:
	$(CC) $(CFLAGS) %.c
.PHONY: clean
clean:
	rm -f *.o
