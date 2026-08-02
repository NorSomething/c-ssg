CC = gcc
CFLAGS = -Wall -Wextra

all: build

c-ssg:
	$(CC) $(CFLAGS) ./src/*.c -o c-ssg

build: c-ssg
	mkdir htmlfiles
	./c-ssg
	cp -r cssfiles htmlfiles/
	cp -r site-imgs htmlfiles/

run: build

clean:
	rm -f c-ssg
	rm -rf htmlfiles

.PHONY: all build run clean
