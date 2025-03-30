CC=gcc

PROGRAM=tetris

SRC_DIR=src
BUILD_DIR=$(SRC_DIR)/build
DEBUG_DIR=$(BUILD_DIR)/debug
LFLAGS=-lraylib -lm

.PHONY: clean

$(BUILD_DIR)/$(PROGRAM): $(SRC_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $< $(LFLAGS) -DLOG_LEVEL=4

$(DEBUG_DIR)/$(PROGRAM): $(SRC_DIR)/main.c
	@mkdir -p $(DEBUG_DIR)
	$(CC) -o $@ $< $(LFLAGS) -DLOG_LEVEL=4

build: $(BUILD_DIR)/$(PROGRAM)

debug: $(DEBUG_DIR)/$(PROGRAM)

run: build
	@./$(BUILD_DIR)/$(PROGRAM)

clean:
	@rm -rf $(BUILD_DIR)