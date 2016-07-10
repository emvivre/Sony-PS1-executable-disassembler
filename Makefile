all:
	g++ -o ps1_reverse main.cpp hexdump.c -Wall -Wno-unused-value -Werror -Wextra -g -std=c++11
