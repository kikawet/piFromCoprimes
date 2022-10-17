all:
	mkdir -p build
	g++ -O0 -Wall -Wextra -pedantic -std=c++17 -o build/main main.cpp -pthread -lboost_thread -lboost_chrono

clean:
	rm -rf build
	rm -rf release
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -rf bw-output

compile:
	mkdir -p release
	g++ -O3 -Wall -Wextra -pedantic -std=c++17 -o release/main main.cpp  -pthread -lboost_thread -lboost_chrono
