#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <utility>

class FixedPoint {
public:
    FixedPoint(const std::string &num_str, int frac_bits) : fractional_bits(frac_bits) {
        auto binaryResult = decimalToBinary(num_str, fractional_bits);

        integer = binaryResult.first;
        fractional = binaryResult.second;

        for (uint32_t value : integer) {
            // Iterate over each bit (from most significant to least significant)
            for (int i = 31; i >= 0; --i) {
                // Use bitwise AND to check if the bit is set
                std::cout << ((value >> i) & 1);
            }
            std::cout << " ";
        }

        std::cout << std::endl;


        for (uint32_t value : fractional) {
            // Iterate over each bit (from most significant to least significant)
            for (int i = 31; i >= 0; --i) {
                // Use bitwise AND to check if the bit is set
                std::cout << ((value >> i) & 1);
            }
        }
    }

private:
    std::vector<uint32_t> integer;
    std::vector<uint32_t> fractional;
    int fractional_bits;

    // Function to convert integer part to binary
    std::vector<uint32_t> int_part_to_bin(const std::string& numStr) {
        std::string currentNumStr = numStr; // Copy of the input string
        std::vector<uint32_t> binaryResult; // To store the binary result

        // Continue processing until the number is reduced to "0"
        int bit_added = 0;
        while (currentNumStr != "0") {
            int remainder = 0; // Remainder from division
            std::string result; // Result of division by 2

            // Perform division by 2 for each digit
            for (char digitChar : currentNumStr) {
                int currentDigit = (digitChar - '0') + remainder * 10; // Current digit with remainder
                int quotient = currentDigit / 2; // Quotient of division by 2
                remainder = currentDigit % 2; // Remainder of division by 2

                result += std::to_string(quotient); // Append quotient to result
            }

            // Remove leading zeros from the result
            result.erase(0, result.find_first_not_of('0'));
            if (result.empty()) {
                result = "0";
            }

            if (bit_added == 0) binaryResult.push_back(0);

            // Store the remainder (binary digit)
            binaryResult.back() = binaryResult.back() | (remainder << bit_added);

            // Update the current number for the next iteration
            currentNumStr = result;

            bit_added = (bit_added + 1) % 32;
        }

        // Return the binary result
        return binaryResult;
    }

    // Function to multiply a string number by 2
    std::string multiplyByTwo(const std::string& numStr) {
        std::string result = "";
        int carry = 0;

        for (int i = numStr.size() - 1; i >= 0; i--) {
            int value = (numStr[i] - '0') * 2 + carry;
            carry = value / 10;
            result = std::to_string(value % 10) + result;
        }

        if (carry > 0) {
            result = std::to_string(carry) + result;
        }

        return result;
    }

    // Function to convert the fractional part to binary
    std::vector<uint32_t> fracToBinary(const std::string& fracStr, int frac_bits = 32) {
        std::vector<uint32_t> binary;
        std::string fractionalPart = fracStr;
        int frac_part_size = fractionalPart.size();

        for (int i = 0, bit_added = 0; i < frac_bits && !fractionalPart.empty(); ++i, bit_added = (bit_added + 1) % 32) {
            // Multiply the fractional part by 2
            std::string multiplied = multiplyByTwo(fractionalPart);

            if (bit_added == 0) binary.push_back(0);

            if (multiplied.size() == frac_part_size) {
                binary.back() = (binary.back() << 1) | 0;
                fractionalPart = multiplied;

            } else {
                binary.back() = (binary.back() << 1) | 1;
                fractionalPart = multiplied.substr(1);
            }

            if (multiplied.size() == 0) {
                fractionalPart = "0"; // If no more fractional part remains
            }
        }

        return binary;
    }

    // Main function to convert a fractional number (as a string) to binary
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>> decimalToBinary(const std::string& numStr, int frac_bits = 32) {
        // Find the position of the decimal point
        size_t dotPos = numStr.find('.');

        // If no decimal point exists, treat it as an integer
        if (dotPos == std::string::npos) {
            return std::make_pair(int_part_to_bin(numStr), std::vector<uint32_t>());
        }

        // Extract the integer and fractional parts
        std::string integerPartStr = numStr.substr(0, dotPos);
        std::string fractionalPartStr = numStr.substr(dotPos + 1);

        // Convert the integer part to binary
        std::vector<uint32_t> binaryInteger = int_part_to_bin(integerPartStr);

        // Convert the fractional part to binary
        std::vector<uint32_t> binaryFraction = fracToBinary(fractionalPartStr, frac_bits);

        // Combine the results
        return std::make_pair(binaryInteger, binaryFraction);
    }
};

int main() {
    FixedPoint long_num{"1848294832849.184829483284921394", 128};


    return 0;
}
