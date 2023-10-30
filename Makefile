CC := gcc
CFLAGS := -Wall -Wextra --pedantic -g
INCLUDE := -Iinclude
BIN := bin
SRC := $(shell find src -type f)

.PHONY: all
all:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BIN)/main $(SRC) 


