# default compiler
CC := gcc

# source codes
SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c, %, $(SOURCES))

# make
all: $(TARGETS)

%: %.c
	$(CC) $^ -o $@

clean:
	@echo rm -rf $(TARGETS)
	@rm -rf $(TARGETS)