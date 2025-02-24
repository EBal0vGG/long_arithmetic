#ifndef LONG_NUM_H
#define LONG_NUM_H

#include <vector>
#include <string>
#include <cstdint>
#include <utility>

enum class Op_behavior {
    PLUS_FST,
    PLUS_SND,
    SUB_FST,
    SUB_SND
};

class FixedPoint {
public:
    // Constructor: Converts a decimal string to binary representation with specified fractional bits
    FixedPoint(const std::string &num_str, int frac_bits = 32);

    // Copy constructor and destructor
    FixedPoint(const FixedPoint& other);
    ~FixedPoint();

    // Assignment operator
    FixedPoint& operator=(const FixedPoint& other);

    // Overload the + operator for adding two FixedPoint numbers
    FixedPoint operator+(const FixedPoint &other) const;

    // Overload the - operator for subtracting two FixedPoint numbers
    FixedPoint operator-(const FixedPoint &other) const;

    // Overload the * operator for multiplying two FixedPoint numbers
    FixedPoint operator*(const FixedPoint &other) const;

    // Overload the / operator (No implementation in this version)
    FixedPoint operator/(const FixedPoint &other) const;

    // Overload comparison operators for two FixedPoint numbers
    bool operator>(const FixedPoint &other) const;

    bool operator<(const FixedPoint &other) const;

    bool operator==(const FixedPoint &other) const;

    bool operator<=(const FixedPoint &other) const;

    bool operator>=(const FixedPoint &other) const;

    bool operator!=(const FixedPoint &other) const;

    FixedPoint& operator+=(const FixedPoint &other);

    FixedPoint& operator*=(const FixedPoint &other);

    FixedPoint& operator-=(const FixedPoint &other);

    FixedPoint& operator/=(const FixedPoint &other);

    // Reduces the precision of the fractional part by removing bits and updating the fractional representation
    void set_precision(size_t precision);

    void print_bin() const;

private:
    std::vector<uint32_t> integer;    // Binary representation of the integer part
    std::vector<uint32_t> fractional; // Binary representation of the fractional part
    int fractional_bits;              // Number of fractional bits
    bool is_negative = false;         // Flag for negative numbers

    Op_behavior helper(const FixedPoint &a, const FixedPoint &b, char op) const;

    bool bigger_abs(const FixedPoint &a, const FixedPoint &b) const;

    // Function to print bits of a uint32_t value
    void printBits(uint32_t value) const;

    // Function to add fractional parts of two numbers
    std::pair<std::vector<uint32_t>, bool> add_frac(const std::vector<uint32_t> &a,
                                                    const std::vector<uint32_t> &b,
                                                    uint32_t carry = 0) const;

    // Function to add integer parts of two numbers
    std::pair<std::vector<uint32_t>, bool> add_int(const std::vector<uint32_t> &a,
                                                   const std::vector<uint32_t> &b,
                                                   uint32_t carry = 0) const;

    std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
    subtract_nums(const FixedPoint &a, const FixedPoint &b) const;

    // Function to perform subtraction of two 32-bit words with borrow
    int subtract(uint32_t &res, uint32_t val_a, uint32_t val_b, uint32_t borrow = 0) const;

    std::vector<uint32_t> subtract_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const;

    std::vector<uint32_t> divide(const FixedPoint &a, const FixedPoint &b, uint32_t frac_bits) const;

    bool not_less_vec(const std::vector<uint32_t> &a, const std::vector<uint32_t> &b) const;

    void add_bit_div(std::vector<uint32_t> &vec, bool is_one) const;

    // Function to convert an integer part from decimal to binary
    std::vector<uint32_t> int_part_to_bin(const std::string& numStr) const;

    // Function to multiply a decimal string by 2
    std::string multiplyByTwo(const std::string& numStr) const;

    // Function to convert a fractional part from decimal to binary
    std::vector<uint32_t> fracToBinary(const std::string& fracStr, int frac_bits = 32) const;

    // Function to convert a decimal string to binary representation
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
    decimalToBinary(const std::string& numStr, int frac_bits = 32) const;
};

// User-defined literal operator for creating FixedPoint objects
FixedPoint operator""_long(long double number);

#endif // LONG_NUM_H