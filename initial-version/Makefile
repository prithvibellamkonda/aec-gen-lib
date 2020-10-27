CC = g++
CFLAGS = -O2 -std=c++20 -Wall -Wextra -Wno-unused-parameter

main: main.cpp gen.hpp my.hpp
	$(CC) $(CFLAGS) -o $@ $<

.PONY: clean

clean: 
	rm main
