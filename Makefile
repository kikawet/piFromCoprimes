all:
	mkdir -p build
	g++ -Wall -O0 -pthread -std=c++17 -o build/main main.cpp

clean:
	rm -rf build
	rm -rf release
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -rf bw-output

release:
	mkdir -p release
	g++ -Wall -O3 -pthread -std=c++17 -o release/main main.cpp
