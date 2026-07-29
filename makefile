CC = gcc
CFLAGS = -Wall -Wextra

all: c-ssg

c-ssg:
	$(CC) ./src/*.c -o c-ssg

run: c-ssg
	./c-ssg

clean:
	rm -f c-ssg

.PHONY: all run clean
