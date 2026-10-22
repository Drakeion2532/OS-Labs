CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wno-unused-result -O2
SOURCES  := $(wildcard cpu-scheduling/*.c disk-scheduling/*.c page-replacement/*.c deadlock/*.c)
BINARIES := $(patsubst %.c,bin/%,$(SOURCES))   # e.g. bin/cpu-scheduling/fcfs

.PHONY: all test clean

all: $(BINARIES)

bin/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

test: all
	python3 tests/run_tests.py

clean:
	rm -rf bin
