# default compiler
CC := gcc

# folders
SOURCE_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# srcs, objs
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
TARGETS := $(patsubst $(SOURCE_DIR)/%.c, $(BIN_DIR)/%, $(SOURCES))

# compile flags
CFLAGS := -Werror

all: $(TARGETS)

$(BIN_DIR)/%: $(BUILD_DIR)/%.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean all