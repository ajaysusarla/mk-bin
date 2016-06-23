CC=gcc
NULL=
UNAME := $(shell $(CC) -dumpmachine 2>&1 | grep -E -o "linux|darwin")
BIN=mk-noop-bin

ifeq ($(UNAME), linux)
OSFLAGS = -DLINUX
DEBUG = -ggdb
else ifeq ($(UNAME), darwin)
OSFLAGS = -DMACOSX
DEBUG = -g
endif

EXTRA_CFLAGS := \
        -Wall -Wextra -pedantic \
        $(NULL)

CFLAGS := $(DEBUG) $(EXTRA_CFLAGS) $(OSFLAGS)
LIBS := \
        $(NULL)


all: main.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) $(LIBS) main.o -o $(BIN)

clean:
	rm -f *.o $(BIN) core *.bin

.PHONY: check-syntax

check-syntax:
	$(CC) $(CFLAGS) -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
