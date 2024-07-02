CXX := g++
COMMON_FLAGS := -Wall -Wextra -pedantic -std=c++20 -pthread -lboost_thread -lboost_chrono -lboost_random
SRC := pfc.cpp
TARGET := pfc

MAKEFLAGS += -j2

all: build release

release: $(SRC)
	mkdir -p $@
	$(CXX) $(COMMON_FLAGS) -O3 -o $@/$(TARGET) $(SRC)
	
build: $(SRC)
	mkdir -p $@
	$(CXX) $(COMMON_FLAGS) -O0 -o $@/$(TARGET) $(SRC)

debug: $(SRC)
	mkdir -p $@
	$(CXX) $(COMMON_FLAGS) -ggdb -o $@/$(TARGET) $(SRC)

clean:
	rm -rf build release debug
	rm -f *.gcda *.gcno *.gcov
	rm -rf bw-output
