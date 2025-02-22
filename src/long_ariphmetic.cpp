#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "../include/long_ariphmetic.hpp"

// Constructor: Converts a decimal string to binary representation with specified fractional bits
FixedPoint::FixedPoint(const std::string &num_str, int frac_bits) : fractional_bits(frac_bits) {
    auto binaryResult = decimalToBinary(num_str, fractional_bits);

    integer = binaryResult.first;     // Store the integer part in binary
    fractional = binaryResult.second; // Store the fractional part in binary
}

// Default copy constructor and destructor
FixedPoint::FixedPoint(const FixedPoint& other) = default;
FixedPoint::~FixedPoint() = default;

// Default assignment operator
FixedPoint& FixedPoint::operator=(const FixedPoint& other) = default;

// Overload the + operator for adding two FixedPoint numbers
FixedPoint FixedPoint::operator+(const FixedPoint &other) const {
    // Create a result object with the maximum fractional bits between the two operands
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    // Add fractional parts and handle carry
    auto add_res = add_frac(fractional, other.fractional);
    result.fractional = add_res.first;

    // Add integer parts and handle carry from fractional addition
    add_res = add_int(integer, other.integer, add_res.second);
    result.integer = add_res.first;

    // If there's still a carry, append it to the integer part
    if (add_res.second) {
        result.integer.push_back(1);
    }
    return result;
}

// Overload the - operator for subtracting two FixedPoint numbers
FixedPoint FixedPoint::operator-(const FixedPoint &other) const {
    // Create a result object with the maximum fractional bits between the two operands
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    std::vector<uint32_t> result_int;
    std::vector<uint32_t> result_frac;

    size_t this_int_sz = integer.size();
    size_t this_frac_sz = fractional.size();
    size_t other_int_sz = other.integer.size();
    size_t other_frac_sz = other.fractional.size();

    // Determine the maximum size of the combined integer and fractional parts
    size_t max_sz = std::max(this_int_sz + this_frac_sz, other_int_sz + other_frac_sz);
    size_t max_frac_sz = std::max(this_frac_sz, other_frac_sz);

    uint32_t borrow = 0; // Borrow flag for subtraction

    size_t this_i = 0, other_i = 0;

    // Perform subtraction bit by bit for both fractional and integer parts
    while (this_i < max_sz && other_i < max_sz) {
        if (this_i < max_frac_sz && other_i < max_frac_sz) {
            result_frac.push_back(0); // Initialize fractional result
        } else {
            result_int.push_back(0); // Initialize integer result
        }

        // Handle cases where one operand has fewer fractional bits than the other
        if (other_i < max_frac_sz - this_frac_sz) {
            borrow = subtract(result_frac.back(), 0, other.fractional[other_i++], borrow);
            continue;
        }

        if (this_i < max_frac_sz - other_frac_sz) {
            borrow = subtract(result_frac.back(), fractional[this_i++], 0, borrow);
            continue;
        }

        // Subtract corresponding fractional bits
        if (this_i < max_frac_sz && other_i < max_frac_sz) {
            borrow = subtract(result_frac.back(), fractional[this_i++], other.fractional[other_i++], borrow);
            continue;
        }

        // Subtract corresponding integer bits
        uint32_t val_a = (this_i - this_frac_sz < this_int_sz) ? integer[this_i++ - this_frac_sz] : 0;
        uint32_t val_b = (other_i - other_frac_sz < other_int_sz) ? other.integer[other_i++ - other_frac_sz] : 0;
        borrow = subtract(result_int.back(), val_a, val_b, borrow);
    }
    result.integer = result_int;     // Assign the computed integer part to the result
    result.fractional = result_frac; // Assign the computed fractional part to the result

    return result;
}

// Overload the * operator for multiplying two FixedPoint numbers
FixedPoint FixedPoint::operator*(const FixedPoint &other) const {
    // Create a result object with sufficient fractional bits for multiplication
    FixedPoint result("0.0", (fractional_bits / 32 + 1) * 32 + (other.fractional_bits / 32 +  1) * 32);

    // Compute the sizes of the operands
    int this_sz = integer.size() + fractional.size();
    int other_sz = other.integer.size() + other.fractional.size();

    // Allocate space for intermediate multiplication results
    int mid_mult_sz = (this_sz + other_sz) * 32 + 1;
    std::vector<uint32_t> mid_mult(mid_mult_sz);

    // Perform bitwise multiplication
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

    // Convert the intermediate multiplication result into integer and fractional parts
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

    result.integer = mult_int_result;     // Assign the computed integer part to the result
    result.fractional = mult_frac_result; // Assign the computed fractional part to the result

    return result;
}

// Overload the / operator (No implementation in this version)
FixedPoint FixedPoint::operator/(const FixedPoint &other) const {
    // Create a result object with sufficient fractional bits for division
    FixedPoint result("0.0", std::max(fractional_bits, other.fractional_bits));

    result.integer = divide(*this, other, std::max(fractional_bits, other.fractional_bits));

    return result;
}

// Overload comparison operators for two FixedPoint numbers
bool FixedPoint::operator>(const FixedPoint &other) const {
    if (integer.size() > other.integer.size()) return true;
    if (integer.size() < other.integer.size()) return false;

    for (int i = integer.size() - 1; i >= 0; i--) {
        if (integer[i] > other.integer[i]) {
            return true;
        }
        if (integer[i] < other.integer[i]) {
            return false;
        }
    }

    int this_i = fractional.size() - 1, other_i = other.fractional.size() - 1;
    for (; this_i >= 0 && other_i >= 0; this_i--, other_i--) {
        if (fractional[this_i] > other.fractional[other_i]) {
            return true;
        }
        if (fractional[this_i] < other.fractional[other_i]) {
            return false;
        }
    }

    if (this_i == -1 && other_i == -1) return false;
    if (this_i == -1) return false;
    return true;
}

bool FixedPoint::operator<(const FixedPoint &other) const {
    if (integer.size() < other.integer.size()) return true;
    if (integer.size() > other.integer.size()) return false;

    for (int i = integer.size() - 1; i >= 0; i--) {
        if (integer[i] < other.integer[i]) {
            return true;
        }
        if (integer[i] > other.integer[i]) {
            return false;
        }
    }

    int this_i = fractional.size() - 1, other_i = other.fractional.size() - 1;
    for (; this_i >= 0 && other_i >= 0; this_i--, other_i--) {
        if (fractional[this_i] < other.fractional[other_i]) {
            return true;
        }
        if (fractional[this_i] > other.fractional[other_i]) {
            return false;
        }
    }

    if (this_i == -1 && other_i == -1) return false;
    if (other_i == -1) return false;
    return true;
}

bool FixedPoint::operator==(const FixedPoint &other) const {
    if (integer.size() < other.integer.size()) return false;
    if (integer.size() > other.integer.size()) return false;

    for (int i = integer.size() - 1; i >= 0; i--) {
        if (integer[i] != other.integer[i]) {
            return false;
        }
    }

    int this_i = fractional.size() - 1, other_i = other.fractional.size() - 1;
    for (; this_i >= 0 && other_i >= 0; this_i--, other_i--) {

        if (fractional[this_i] != other.fractional[other_i]) {
            return false;
        }
    }

    if (this_i == -1 && other_i == -1) return true;
    if (this_i == -1) {
        for (; other_i >= 0; other_i--) {
            if (other.fractional[other_i] != 0) return false;
        }
    }
    if (other_i == -1) {
        for (; this_i >= 0; this_i--) {
            if (fractional[this_i] != 0) return false;
        }
    }
    return false;
}

bool FixedPoint::operator<=(const FixedPoint &other) const {
    return !(*this > other);
}

bool FixedPoint::operator>=(const FixedPoint &other) const {
    return !(*this < other);
}

bool FixedPoint::operator!=(const FixedPoint &other) const {
    return !(*this == other);
}

FixedPoint& FixedPoint::operator+=(const FixedPoint &other) {
    *this = *this + other;
    return *this;
}

FixedPoint& FixedPoint::operator*=(const FixedPoint &other) {
    *this = *this * other;
    return *this;
}

FixedPoint& FixedPoint::operator-=(const FixedPoint &other) {
    *this = *this - other;
    return *this;
}

FixedPoint& FixedPoint::operator/=(const FixedPoint &other) {
    *this = *this / other;
    return *this;
}

// Reduces the precision of the fractional part by removing bits and updating the fractional representation
void FixedPoint::set_precision(size_t precision) {
    if (precision > fractional_bits) {
        std::cout << "You can just set less value" << std::endl;
        return;
    }
    int need_to_del = fractional_bits - precision;
    int low_order_bits = fractional_bits % 32;
    int q_del = (need_to_del - low_order_bits) / 32 + (low_order_bits < need_to_del);

    fractional.erase(fractional.begin(), fractional.begin() + q_del);
    fractional[0] &= 0xFFFFFFFF << (need_to_del - low_order_bits) % 32;
    fractional_bits = precision;
}

void FixedPoint::print_bin() const {
    std::cout << "Integer bits:    ";
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

// Function to print bits of a uint32_t value
void FixedPoint::printBits(uint32_t value) const {
    for (int i = 31; i >= 0; --i) {
        std::cout << ((value >> i) & 1);
    }
}

// Function to add fractional parts of two numbers
std::pair<std::vector<uint32_t>, bool> FixedPoint::add_frac(const std::vector<uint32_t> &a,
                                                            const std::vector<uint32_t> &b,
                                                            uint32_t carry) const {
    std::vector<uint32_t> result;
    size_t maxSize = std::max(a.size(), b.size());

    size_t a_i = 0, b_i = 0;

    // Perform addition bit by bit
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

// Function to add integer parts of two numbers
std::pair<std::vector<uint32_t>, bool> FixedPoint::add_int(const std::vector<uint32_t> &a,
                                                           const std::vector<uint32_t> &b,
                                                           uint32_t carry) const {
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

// Function to perform subtraction of two 32-bit words with borrow
int FixedPoint::subtract(uint32_t &res, uint32_t val_a, uint32_t val_b, uint32_t borrow) const {
    uint32_t int_res = val_a ^ val_b;
    for (size_t bit_i = 0; bit_i < 32; bit_i++) {
        switch (borrow) {
        case 0:
            res = res | (int_res & (1 << bit_i));
            if ((val_a & (1 << bit_i)) == 0 && (val_b & (1 << bit_i)) != 0) {
                borrow = 1; // Set borrow if a bit needs to be borrowed
            }
            break;
        case 1:
            res = res | ((~int_res) & (1 << bit_i));
            if ((val_a & (1 << bit_i)) != 0 && (val_b & (1 << bit_i)) == 0) {
                borrow = 0; // Clear borrow if the borrow is resolved
            }
            break;
        }
    }
    return borrow;
}

std::vector<uint32_t> FixedPoint::subtract_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const {
    uint32_t borrow = 0; // Borrow flag for subtraction

    uint32_t a_i = 0, b_i = 0;
    uint32_t max_sz = std::max(a.size(), b.size());

    std::vector<uint32_t> result;

    // Perform subtraction bit by bit for both fractional and integer parts
    while (a_i < max_sz && b_i < max_sz) {
        result.push_back(0);

        uint32_t val_a = (a_i < a.size()) ? a[a_i++] : 0;
        uint32_t val_b = (b_i < b.size()) ? b[b_i++] : 0;
        borrow = subtract(result.back(), val_a, val_b, borrow);
    }
    return result;
}

std::vector<uint32_t> FixedPoint::divide(const FixedPoint &a, const FixedPoint &b, uint32_t frac_bits) const {

    std::vector<uint32_t> result;

    size_t a_int_sz = a.integer.size();
    size_t a_frac_sz = a.fractional.size();
    size_t b_int_sz = b.integer.size();
    size_t b_frac_sz = b.fractional.size();

    size_t a_sz = a_int_sz + a_frac_sz;

    std::vector<uint32_t> remainder;
    uint32_t bit_taken = 0;

    std::vector<uint32_t> divider(b.fractional);
    divider.insert(divider.end(), b.integer.begin(), b.integer.end());

    while (!divider.empty() && divider.front() == 0) {
        divider.erase(divider.begin());
    }

    if (divider.empty()) {
        throw std::runtime_error("Attempted division by zero");
    }

    for (uint32_t bit_i = 0; bit_i < a_sz * 32 + frac_bits; bit_i++) {

        uint32_t addition;

        if (bit_i < a_int_sz * 32) {
            addition = ((a.integer[a_int_sz - 1 - (bit_i / 32)] >> (31 - bit_taken)) & 0x00000001);
            add_bit_div(remainder, addition);
        } else if (bit_i < a_frac_sz * 32) {
            addition = ((a.fractional[a_frac_sz - 1 - (bit_i / 32)] >> (31 - bit_taken)) & 0x00000001);
            add_bit_div(remainder, addition);
        } else {
            add_bit_div(remainder, false);
        }

        bit_taken = (bit_taken + 1) % 32;

        if (not_less_vec(remainder, divider)) {
            remainder = subtract_vec(remainder, divider);
            add_bit_div(result, true);
        } else {
            add_bit_div(result, false);
        }
    }

    while (!result.empty() && result.front() == 0) {
        result.erase(result.begin());
    }

    return result;
}

bool FixedPoint::not_less_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const {

    for (int i = std::max(a.size(), b.size()) - 1; i >= 0; i--) {
        uint32_t val_a = (i < a.size() ? a[i] : 0);
        uint32_t val_b = (i < b.size() ? b[i] : 0);
        if (val_a > val_b) {
            return true;
        }
        if (val_a < val_b) {
            return false;
        }
    }

    return true;
}

void FixedPoint::add_bit_div(std::vector<uint32_t> &vec, bool is_one) const {
    if (vec.empty() || vec.back() & 0x80000000) vec.push_back(0);
    for (uint32_t i = vec.size() - 1; i > 0; i--) {
        vec[i] <<= 1;
        vec[i] |= vec[i - 1] >> 31;
    }
    vec[0] <<= 1;
    vec[0] |= (is_one ? 1 : 0);
}


// Function to convert an integer part from decimal to binary
std::vector<uint32_t> FixedPoint::int_part_to_bin(const std::string& numStr) const{
    std::string currentNumStr = numStr; // Copy of the input string
    std::vector<uint32_t> binaryResult; // To store the binary result

    if (currentNumStr == "0") {
        binaryResult.push_back(0);
        return binaryResult;
    }

    int bit_added = 0;

    // Repeatedly divide the number by 2 to extract binary digits
    while (currentNumStr != "0") {
        int remainder = 0;
        std::string result;

        for (char digitChar : currentNumStr) {
            int currentDigit = (digitChar - '0') + remainder * 10;
            int quotient = currentDigit / 2;
            remainder = currentDigit % 2;

            result += std::to_string(quotient);
        }

        // Remove leading zeros from the result
        result.erase(0, result.find_first_not_of('0'));
        if (result.empty()) {
            result = "0";
        }

        if (bit_added == 0) binaryResult.push_back(0);

        binaryResult.back() = binaryResult.back() | (remainder << bit_added);

        currentNumStr = result;
        bit_added = (bit_added + 1) % 32;
    }

    return binaryResult;
}

// Function to multiply a decimal string by 2
std::string FixedPoint::multiplyByTwo(const std::string& numStr) const{
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

// Function to convert a fractional part from decimal to binary
std::vector<uint32_t> FixedPoint::fracToBinary(const std::string& fracStr, int frac_bits) const{
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
            fractionalPart = "0";
        }
    }

    binary[0] <<= 32 - (frac_bits % 32);

    return binary;
}

// Function to convert a decimal string to binary representation
std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
FixedPoint::decimalToBinary(const std::string& numStr, int frac_bits) const{
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


// User-defined literal operator for creating FixedPoint objects
FixedPoint operator""_long(long double number) {
    printf("Create FixedPoint as <%Lf>_long\n", number);
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
    // num1.set_precision(10);
    // num1.print_bin();
    // std::cout << std::endl;

    // FixedPoint num2{"4294967295.23", 100};
    // std::cout << "NUM_2" << std::endl;
    // num2.print_bin();
    // std::cout << std::endl;
    // num2.set_precision(45);
    // num2.print_bin();
    // std::cout << std::endl;

    // FixedPoint num3{"1.2", 65};
    // std::cout << "NUM_3" << std::endl;
    // num3.print_bin();
    // std::cout << std::endl;

    // FixedPoint add_result = num1 + num2;
    // std::cout << "ADDITION_RESULT" << std::endl;
    // add_result.print_bin();
    // std::cout << std::endl;

    // std::cout << std::endl << std::endl << std::endl;

    // FixedPoint num4{"832398293833333333.9298328382", 64};
    // std::cout << "NUM_4" << std::endl;
    // num4.print_bin();
    // std::cout << std::endl;

    // FixedPoint num5{"92929328382364.235273672632", 64};
    // std::cout << "NUM_5" << std::endl;
    // num5.print_bin();
    // std::cout << std::endl;

    // FixedPoint mult_result = num4 * num5;
    // std::cout << "MULTIPLY_RESULT" << std::endl;
    // mult_result.print_bin();
    // std::cout << std::endl;

    // FixedPoint a{"837387287387192891829137827382.98329831891029090909000000000000000000000000000000000000001", 500} ;
    // FixedPoint b{"837387287387192891829137827382.98329831891029090909000000000000000000000000000000000000000", 700} ;

    // a.print_bin();
    // std::cout << std::endl;
    // b.print_bin();

    // if (a > b) {
    //     std::cout << "a > b" << std::endl;
    // } else {
    //     std::cout << "a <= b" << std::endl;
    // }

    // if (a == b) {
    //     std::cout << "a == b" << std::endl;
    // } else {
    //     std::cout << "a != b" << std::endl;
    // }

    // FixedPoint num6{"949793928392839829382989483.203802839294", 100};
    // std::cout << "NUM_6" << std::endl;
    // num6.print_bin();
    // std::cout << std::endl;

    // FixedPoint num7{"38238283782382783728337.3239727318782128718728", 200};
    // std::cout << "NUM_7" << std::endl;
    // num7.print_bin();
    // std::cout << std::endl;

    // FixedPoint sub_result = num6 - num7;
    // std::cout << "SUBTRACTION_RESULT" << std::endl;
    // sub_result.print_bin();
    // std::cout << std::endl;

    FixedPoint num8{"78.0", 32};
    std::cout << "NUM_8" << std::endl;
    num8.print_bin();
    std::cout << std::endl;

    FixedPoint num9{"19.0", 32};
    std::cout << "NUM_9" << std::endl;
    num9.print_bin();
    std::cout << std::endl;

    FixedPoint div_result = num8 / num9;
    std::cout << "DIVISION_RESULT" << std::endl;
    div_result.print_bin();
    std::cout << std::endl;

    return 0;
}