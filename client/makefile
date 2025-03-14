CC = gcc
FLAGS = -Wall -g

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
LIB_DIR = lib
BUILD_LIB_DIR = $(BUILD_DIR)/lib

SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

OUTPUT_FILENAME = kapsh_traffic_client.bin
BINARY = $(BUILD_DIR)/$(OUTPUT_FILENAME)

all: $(BINARY)

$(BINARY): $(OBJECTS) | $(BUILD_LIB_DIR)
	$(CC) $(FLAGS) $(OBJECTS) -o $@ -lnetworking -L$(LIB_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) # Ensure the directory for the object file exists
	$(CC) $(FLAGS) -I$(SRC_DIR) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_LIB_DIR):
	@mkdir -p $(BUILD_LIB_DIR)
	cp -r $(LIB_DIR)/* $(BUILD_LIB_DIR)

run: $(BINARY)
	LD_LIBRARY_PATH=$(BUILD_LIB_DIR) ./$(BINARY) 4 6 50001 10 32768 50 2 30251

debug: $(BINARY)
	LD_LIBRARY_PATH=$(BUILD_LIB_DIR) valgrind ./$(BINARY) 4 6 50001 10 32768 50 2 30251

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean run debug
