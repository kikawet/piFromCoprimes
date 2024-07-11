CXX := g++
COMMON_FLAGS := -Wall -Wextra -pedantic -std=c++20
LIBS := -pthread -lboost_thread -lboost_chrono -lboost_random
SRC := pfc.cpp
TARGET := pfc

MAKEFLAGS += -j2

all: release debug

release: $(SRC)
	mkdir -p $@
	$(CXX) -O3 $(COMMON_FLAGS) -o $@/$(TARGET) $(SRC) $(LIBS)
	
build: $(SRC)
	mkdir -p $@
	$(CXX) -O0 $(COMMON_FLAGS) -o $@/$(TARGET) $(SRC) $(LIBS)

debug: $(SRC)
	mkdir -p $@
	$(CXX) $(COMMON_FLAGS) -ggdb -o $@/$(TARGET) $(SRC)

docker: $(SRC)
	docker build -f ./Docker/Dockerfile -t kikawet/pifromcoprimes:latest .

clean:
	rm -rf build release debug
	rm -f *.gcda *.gcno *.gcov
	rm -rf bw-output
