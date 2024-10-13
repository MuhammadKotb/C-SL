CFLAGS += -I./src/include/
CFLAGS += -std=c11
CFLAGS += -g
CFLAGS += -Wall

SOURCES := src/main.c src/murmur3.c src/hashtable.c

build:
								gcc $(SOURCES) -o main $(CFLAGS)

run: build
								./main