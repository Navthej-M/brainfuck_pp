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

BACKEND = src/backend/bfpp_backend.c
BACKEND_OBJ = $(BUILD)/libbfpp_backend.o
BACKEND_LIB = $(BUILD)/libbfpp_backend.a
BACKEND_LIB_GLOBAL = /usr/local/lib/libbfpp_backend.a

all:
	@mkdir $(BIN) $(BUILD)

	$(CC) $(CC_FLAGS) $(BFPPC) -o $(BFPPC_OUTPUT)
	$(CC) $(CC_FLAGS) -static $(BACKEND) -c -o $(BACKEND_OBJ)

	ar rcs $(BACKEND_LIB) $(BACKEND_OBJ)

install:
	sudo cp $(BFPPC_OUTPUT) $(BFPPC_OUTPUT_GLOBAL)
	sudo cp $(BACKEND_LIB) $(BACKEND_LIB_GLOBAL)

clean:
	rm -fr $(BIN)/ $(BUILD)/
