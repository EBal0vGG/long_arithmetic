CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++17
GTFLAGS=-lgtest -lgtest_main -lpthread
PATH_TO_GTEST=/wsl.localhost/Ubuntu/usr

all: build build/tests

build:
	@mkdir -p build

run: build/tests
	@printf "Running executable\n"
	@./build/tests

build/tests: build/long_ariphmetic.o build/test_long_ariphmetic.o build/pi_calculation.o build/main.o
	@printf "Compilation is successful\n"
	@$(CC) build/long_ariphmetic.o build/test_long_ariphmetic.o build/pi_calculation.o build/main.o -L $(PATH_TO_GTEST)/lib $(GTFLAGS) -o build/tests
	@printf "Linking is successful\n"

build/long_ariphmetic.o: src/long_ariphmetic.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/long_ariphmetic.cpp -o build/long_ariphmetic.o

build/test_long_ariphmetic.o: src/test_long_ariphmetic.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/test_long_ariphmetic.cpp -o build/test_long_ariphmetic.o

build/pi_calculation.o: src/pi_calculation.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/pi_calculation.cpp -o build/pi_calculation.o

build/main.o: src/main.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/main.cpp -o build/main.o

clean:
	@printf "Cleaning successful\n"
	@rm -rf build

.PHONY: run clean