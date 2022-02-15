all:
	clang++ -Wall -O0 -fprofile-arcs -std=c++17 -o build/main main.cpp
