# tools
CC = gcc

# flags
CC_FLAGS = -std=c23 -Wall -g

# folders to make
BIN = bin
BUILD = build

# files
BFPPC = src/main.c src/frontend/bfppc.c src/frontend/tokenizer.c
BFPPC_OUTPUT = $(BIN)/bfppc
BFPPC_OUTPUT_GLOBAL = /usr/local/bin/bfppc

RUNTIME = src/runtime/runtime.c
RUNTIME_OBJ = $(BUILD)/libbfpp_runtime.o
RUNTIME_LIB = $(BUILD)/libbfpp_runtime.a
RUNTIME_LIB_GLOBAL = /usr/local/lib/libbfpp_runtime.a

all:
	@mkdir $(BIN) $(BUILD)

	$(CC) $(CC_FLAGS) $(BFPPC) -o $(BFPPC_OUTPUT)
	$(CC) $(CC_FLAGS) -static $(RUNTIME) -c -o $(RUNTIME_OBJ)

	ar rcs $(RUNTIME_LIB) $(RUNTIME_OBJ)

install:
	sudo cp $(BFPPC_OUTPUT) $(BFPPC_OUTPUT_GLOBAL)
	sudo cp $(RUNTIME_LIB) $(RUNTIME_LIB_GLOBAL)

clean:
	rm -fr $(BIN)/ $(BUILD)/
