all:
	clang++ -Wall -O0 -fprofile-arcs -std=c++17 -o build/main main.cpp

# install:
# 	sudo apt-get update
# 	sudo apt-get install libboost-all-dev

clean:
	rm -rf build
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -rf bw-output