#include <iostream>
#include <string>
#include <chrono>

#include "../include/long_ariphmetic.hpp"
#include "../include/pi_calculation.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("No arguments provided.\n");
        return 0;
    }
    try {
        int len = std::stoi(argv[1]);

        auto start = std::chrono::high_resolution_clock::now();
        FixedPoint pi = get_pi();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                       (std::chrono::high_resolution_clock::now() - start);
        std::string pi_str = pi.to_string();
        pi_str.resize(len + 2);
        std::cout << pi_str << std::endl;
        std::cout << "Total time (in ms) " << duration.count() << std::endl;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid input." << std::endl;
    }

    return 0;
}