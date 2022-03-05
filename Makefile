all:
	mkdir build
	g++ -Wall -O0 -pthread -std=c++17 -o build/main main.cpp

clean:
	rm -rf build
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -rf bw-output