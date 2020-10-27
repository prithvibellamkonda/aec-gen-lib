CC = g++
CFLAGS = -lpthread -O2 -std=c++20 -Wall -Wextra -Wno-unused-parameter -Wno-unused-result

all: main xox

xox: xox.cpp my.hpp
	$(CC) $(CFLAGS) -o $@ $<

main: main.cpp gen.hpp my.hpp
	$(CC) $(CFLAGS) -o $@ $<

.PONY: clean

clean: 
	rm -rf main *.plist
