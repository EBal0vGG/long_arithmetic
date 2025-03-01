CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++17 -O3
GTFLAGS=-lgtest -lgtest_main -lpthread
PATH_TO_GTEST=/wsl.localhost/Ubuntu/usr

# Default length if no argument is provided
DEFAULT_LEN_PI=100

all: build build/tests build/pi

build:
	@mkdir -p build

tests: build/tests
	@printf "Running executable\n"
	@./build/tests

pi:
ifeq ($(words $(MAKECMDGOALS)),2)
	$(eval PI_LEN := $(word 2,$(MAKECMDGOALS)))
else
	$(eval PI_LEN := $(DEFAULT_LEN_PI))
endif
	@printf "Running executable with length: $(PI_LEN)\n"
	@$(MAKE) --no-print-directory silent-pi PI_LEN=$(PI_LEN)

silent-pi:
	@./build/pi $(PI_LEN)

%:
ifeq ($(filter pi,$(MAKECMDGOALS)),pi)
	@:
else
	$(error No rule to make target '$@'. Usage: make pi [length])
endif

build/tests: build/long_ariphmetic.o build/test_long_ariphmetic.o build/pi_calculation.o build/main.o
	@printf "Tests compilation is successful\n"
	@$(CC) build/long_ariphmetic.o build/test_long_ariphmetic.o build/pi_calculation.o build/main.o -L $(PATH_TO_GTEST)/lib $(GTFLAGS) -o build/tests
	@printf "Tests linking is successful\n"

build/pi: build/long_ariphmetic.o build/pi_calculation.o build/calculate_pi.o
	@printf "Pi compilation is successful\n"
	@$(CC) build/long_ariphmetic.o build/pi_calculation.o build/calculate_pi.o -o build/pi
	@printf "Pi linking is successful\n"

build/long_ariphmetic.o: src/long_ariphmetic.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/long_ariphmetic.cpp -o build/long_ariphmetic.o

build/test_long_ariphmetic.o: src/test_long_ariphmetic.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/test_long_ariphmetic.cpp -o build/test_long_ariphmetic.o

build/pi_calculation.o: src/pi_calculation.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/pi_calculation.cpp -o build/pi_calculation.o

build/main.o: src/main.cpp
	@$(CC) $(CFLAGS) -I $(PATH_TO_GTEST)/include -c src/main.cpp -o build/main.o

build/calculate_pi.o: src/calculate_pi.cpp
	@$(CC) $(CFLAGS) -c src/calculate_pi.cpp -o build/calculate_pi.o

clean:
	@printf "Cleaning successful\n"
	@rm -rf build

.PHONY: all build tests pi clean silent-pi