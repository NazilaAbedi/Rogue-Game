CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinclude
LDLIBS += -lncurses
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
BIN := rogue

all: $(BIN)
$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)
run: $(BIN)
	./$(BIN)
clean:
	rm -f $(OBJ) $(BIN)
debug: CFLAGS := -std=c11 -O0 -g3 -Wall -Wextra -Wpedantic -fsanitize=address,undefined
debug: LDLIBS += -fsanitize=address,undefined
debug: clean $(BIN)
.PHONY: all run clean debug
