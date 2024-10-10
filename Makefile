CFLAGS += -I./src/include/
SOURCES := src/main.c src/murmur3.c

build:
								gcc $(SOURCES) -o main $(CFLAGS)

run: build
								./main