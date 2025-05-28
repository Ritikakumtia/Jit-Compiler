CC = gcc
CFLAGS = -Wall -I./include -I/usr/lib/gcc/x86_64-linux-gnu/14/include
LIBS = -lgccjit

SRC = src/main.c src/lexer.c src/parser.c src/codegen.c src/utils.c src/ast.c

OBJ = $(SRC:.c=.o)
BIN = jit_compiler

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

clean:
	rm -f src/*.o $(BIN)
