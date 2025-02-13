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
    }

    void print_bin() const {
        std::cout << "Integer bits: ";
        for (uint32_t value : integer) {
            printBits(value);
            std::cout << " ";
        }
        std::cout << std::endl;

        std::cout << "Fractional bits: ";
        for (uint32_t value : fractional) {
            printBits(value);
            std::cout << " ";
        }
        std::cout << std::endl;
    }

    // Overload the + operator
    FixedPoint operator+(const FixedPoint &other) const {
        FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits)); // Create a result object with the max fractional bits

        // Add fractional parts
        auto add_res = add_frac(fractional, other.fractional);
        result.fractional = add_res.first;

        // Add integer parts
        add_res = add_int(integer, other.integer, add_res.second);
        result.integer = add_res.first;

        if (add_res.second) {
            result.integer.push_back(1);
        }
        return result;
    }

    // Overload the * operator
    FixedPoint operator*(const FixedPoint &other) const {
        FixedPoint result("0.0", 32); // Create a result object

        int this_sz = integer.size() + fractional.size();
        int other_sz = other.integer.size() + other.fractional.size();
        int mid_mult_sz = (this_sz + other_sz) * 32 + 1;
        std::vector<uint32_t> mid_mult(mid_mult_sz);

        for (int this_i = 0; this_i < this_sz; this_i++) {
            for (int bit_this = 0; bit_this < 32; bit_this++) {
                for (int other_i = 0; other_i < other_sz; other_i++) {
                    for (int bit_other = 0; bit_other < 32; bit_other++) {

                        if (this_i < fractional.size() && other_i < other.fractional.size()) {
                            mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                            (fractional[this_i] >> bit_this) & (other.fractional[other_i] >> bit_other) & 0x00000001;
                        }
                        if (this_i < fractional.size() && other_i >= other.fractional.size()) {
                            mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                            (fractional[this_i] >> bit_this) & (other.integer[other_i - other.fractional.size()] >> bit_other) & 0x00000001;
                        }
                        if (this_i >= fractional.size() && other_i < other.fractional.size()) {
                            mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                            (integer[this_i - fractional.size()] >> bit_this) & (other.fractional[other_i] >> bit_other) & 0x00000001;
                        }
                        if (this_i >= fractional.size() && other_i >= other.fractional.size()) {
                            mid_mult[32 * (this_i + other_i) + bit_this + bit_other] +=
                            (integer[this_i - fractional.size()] >> bit_this) & (other.integer[other_i - other.fractional.size()] >> bit_other) & 0x00000001;
                        }

                    }
                }
            }
        }

        std::vector<uint32_t> mult_int_result;
        std::vector<uint32_t> mult_frac_result;
        int bit_added = 0;
        for (size_t i = 0; i < mid_mult_sz - 1; i++) {

            if (i < 32 * (fractional.size() + other.fractional.size())) {

                if (bit_added == 0) mult_frac_result.push_back(0);
                mult_frac_result.back() = mult_frac_result.back() | ((mid_mult[i] % 2) << bit_added);
                mid_mult[i + 1] += mid_mult[i] / 2;
                bit_added = (bit_added + 1) % 32;

            } else {

                if (bit_added == 0) mult_int_result.push_back(0);
                mult_int_result.back() = mult_int_result.back() | ((mid_mult[i] % 2) << bit_added);
                mid_mult[i + 1] += mid_mult[i] / 2;
                bit_added = (bit_added + 1) % 32;

            }

        }

        result.integer = mult_int_result;
        result.fractional = mult_frac_result;

        return result;
    }

private:
    std::vector<uint32_t> integer;
    std::vector<uint32_t> fractional;
    int fractional_bits;
    bool is_negative;

    // Function to print bits of a uint32_t value
    void printBits(uint32_t value) const {
        for (int i = 31; i >= 0; --i) {
            std::cout << ((value >> i) & 1);
        }
    }

    // Function to add frac parts of uint32_t (handling carry)
    std::pair<std::vector<uint32_t>, bool> add_frac(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b, uint32_t carry = 0) const {
        std::vector<uint32_t> result;
        size_t maxSize = std::max(a.size(), b.size());

        size_t a_i = 0, b_i = 0;
        while (a_i < maxSize && b_i < maxSize) {
            if (b_i < maxSize - a.size()) {
                result.push_back(b[b_i++]);
                continue;
            }
            if (a_i < maxSize - b.size()) {
                result.push_back(a[a_i++]);
                continue;
            }

            result.push_back(0);

            for (size_t j = 0; j < 32; ++j) {
                uint32_t addition = (carry == 1 ? 0xFFFFFFFF : 0);
                result.back() = result.back() | ((a[a_i] ^ b[b_i] ^ addition) & (1 << j));
                carry = ((a[a_i] & (1 << j)) && (b[b_i] & (1 << j)) ||
                        ((a[a_i] ^ b[b_i]) & (1 << j)) && addition ? 1 : 0);
            }
            a_i++;
            b_i++;
        }
        return std::make_pair(result, carry);
    }

    // Function to add int parts of uint32_t (handling carry)
    std::pair<std::vector<uint32_t>, bool> add_int(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b, uint32_t carry = 0) const {
        std::vector<uint32_t> result;
        size_t maxSize = std::max(a.size(), b.size());

        for (size_t i = 0; i < maxSize; ++i) {
            if (i >= a.size()) {
                result.push_back(b[i] + carry);
                carry = (b[i] + carry) == 0;
                continue;
            }
            if (i >= b.size()) {
                result.push_back(a[i] + carry);
                carry = (a[i] + carry) == 0;
                continue;
            }

            result.push_back(0);

            for (size_t j = 0; j < 32; ++j) {
                uint32_t addition = (carry == 1 ? 0xFFFFFFFF : 0);
                result.back() = result.back() | ((a[i] ^ b[i] ^ addition) & (1 << j));
                carry = ((a[i] & (1 << j)) && (b[i] & (1 << j)) ||
                        ((a[i] ^ b[i]) & (1 << j)) && addition ? 1 : 0);
            }
        }
        return std::make_pair(result, carry);
    }

    // Function to convert integer part to binary
    std::vector<uint32_t> int_part_to_bin(const std::string& numStr) {
        std::string currentNumStr = numStr; // Copy of the input string
        std::vector<uint32_t> binaryResult; // To store the binary result

        if (currentNumStr == "0") {
            binaryResult.push_back(0);
            return binaryResult;
        }

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

            if (bit_added == 0) binary.insert(binary.begin(), 0);

            if (multiplied.size() == frac_part_size) {
                binary[0] = (binary[0] << 1) | 0;
                fractionalPart = multiplied;
            } else {
                binary[0] = (binary[0] << 1) | 1;
                fractionalPart = multiplied.substr(1);
            }

            if (multiplied.size() == 0) {
                fractionalPart = "0"; // If no more fractional part remains
            }
        }

        binary[0] <<= 32 - (frac_bits % 32);

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

// User-defined literal operator for FixedPoint
FixedPoint operator""_long(long double number) {
    printf("Create FixedPoint as <number>_long\n");
    return FixedPoint(std::to_string(number), 32); // Replace this with actual logic if needed
}

int main() {
    // FixedPoint fixed = 2.0_long;
    // std::cout << "2.0_long" << std::endl;
    // fixed.print_bin();
    // std::cout << std::endl;

    // FixedPoint num1{"4294967295.23", 48};
    // std::cout << "NUM_1" << std::endl;
    // num1.print_bin();
    // std::cout << std::endl;

    // FixedPoint num2{"1.2", 65};
    // std::cout << "NUM_2" << std::endl;
    // num2.print_bin();
    // std::cout << std::endl;

    // FixedPoint add_result = num1 + num2;
    // std::cout << "ADDITION_RESULT" << std::endl;
    // add_result.print_bin();
    // std::cout << std::endl;

    // std::cout << std::endl << std::endl << std::endl;

    FixedPoint num3{"832398293833333333.9298328382", 64};
    std::cout << "NUM_3" << std::endl;
    num3.print_bin();
    std::cout << std::endl;

    FixedPoint num4{"92929328382364.235273672632", 64};
    std::cout << "NUM_4" << std::endl;
    num4.print_bin();
    std::cout << std::endl;

    FixedPoint mult_result = num3 * num4;
    std::cout << "MULTIPLY_RESULT" << std::endl;
    mult_result.print_bin();
    std::cout << std::endl;

    return 0;
}