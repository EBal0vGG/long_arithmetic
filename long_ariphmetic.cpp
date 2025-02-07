#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class FixedPoint {
public:
    FixedPoint(const std::string &num_str, int frac_bits) : fractional_bits(frac_bits) {
        std::string binaryResult = decimalToBinary(num_str, fractional_bits);

        size_t dot_pos = binaryResult.find('.');

        std::cout << "Binary Result: " << binaryResult << std::endl;

        // Extract integer part
        for (int i = dot_pos - 1; i >= 0; i--) {
            integer.push_back(binaryResult[i] - '0');
        }

        // Extract fractional part
        for (size_t i = dot_pos + 1; i < binaryResult.size(); i++) {
            fractional.push_back(binaryResult[i] - '0');
        }

        // Print the contents of the vectors
        std::cout << "Integer part: ";
        for (unsigned char ch : integer) {
            std::cout << static_cast<int>(ch);
        }
        std::cout << std::endl;

        std::cout << "Fractional part: ";
        for (unsigned char ch : fractional) {
            std::cout << static_cast<int>(ch);
        }
        std::cout << std::endl;
    }

private:
    std::vector<unsigned char> integer;
    std::vector<unsigned char> fractional;
    int fractional_bits;

    // Function to convert integer part to binary
    std::string int_part_to_bin(const std::string& numStr) {
        std::string currentNumStr = numStr; // Copy of the input string
        std::string binaryResult; // To store the binary result

        // Continue processing until the number is reduced to "0"
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

            // Store the remainder (binary digit)
            binaryResult = std::to_string(remainder) + binaryResult;

            // Update the current number for the next iteration
            currentNumStr = result;
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
    std::string fracToBinary(const std::string& fracStr, int frac_bits = 32) {
        std::string binary = "";
        std::string fractionalPart = fracStr;
        int frac_part_size = fractionalPart.size();

        for (int i = 0; i < frac_bits && !fractionalPart.empty(); ++i) {
            // Multiply the fractional part by 2
            std::string multiplied = multiplyByTwo(fractionalPart);

            if (multiplied.size() == frac_part_size) {
                binary += '0';
                fractionalPart = multiplied;

            } else {
                binary += '1';
                fractionalPart = multiplied.substr(1);
            }

            if (multiplied.size() == 0) {
                fractionalPart = "0"; // If no more fractional part remains
            }
        }

        return binary;
    }

    // Main function to convert a fractional number (as a string) to binary
    std::string decimalToBinary(const std::string& numStr, int frac_bits = 32) {
        // Find the position of the decimal point
        size_t dotPos = numStr.find('.');

        // If no decimal point exists, treat it as an integer
        if (dotPos == std::string::npos) {
            return int_part_to_bin(numStr);
        }

        // Extract the integer and fractional parts
        std::string integerPartStr = numStr.substr(0, dotPos);
        std::string fractionalPartStr = numStr.substr(dotPos + 1);

        // Convert the integer part to binary
        std::string binaryInteger = int_part_to_bin(integerPartStr);

        // Convert the fractional part to binary
        std::string binaryFraction = fracToBinary(fractionalPartStr, frac_bits);

        // Combine the results
        return binaryInteger + "." + binaryFraction;
    }

};

int main() {
    FixedPoint long_num{"10.14", 32};


    return 0;
}
