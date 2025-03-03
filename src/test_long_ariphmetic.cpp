#include <gtest/gtest.h>
#include <chrono>
#include <vector>

#include "../include/long_arithmetic.hpp"
#include "../include/pi_calculation.hpp"

// Test class for all operation tests
class FixedPointTest: public ::testing::Test {
protected:
    void SetUp() override {}
};

// Тест для конструктора
TEST_F(FixedPointTest, Constructor) {
    FixedPoint num("123.456");
    EXPECT_EQ(num.to_string(3), "123.456");
}

// Тест для сложения
TEST_F(FixedPointTest, Addition) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    FixedPoint result = num1 + num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

// Тест для вычитания
TEST_F(FixedPointTest, Subtraction) {
    FixedPoint num1("30.75");
    FixedPoint num2("0");
    FixedPoint result = num1 - num2;
    EXPECT_EQ(result.to_string(), "30.75");
}

// Тест для умножения
TEST_F(FixedPointTest, Multiplication) {
    FixedPoint num1("10.5");
    FixedPoint num2("2.0");
    FixedPoint result = num1 * num2;
    EXPECT_EQ(result.to_string(), "21.0");
}

// Тест для деления
TEST_F(FixedPointTest, Division) {
    FixedPoint num1("21.0", 2);
    FixedPoint num2("2.0", 2);
    FixedPoint result = num1 / num2;
    EXPECT_EQ(result.to_string(), "10.5");
}

// Тест для сравнения
TEST_F(FixedPointTest, Comparison) {
    FixedPoint num1("10.5");
    FixedPoint num2("20.25");
    EXPECT_TRUE(num1 < num2);
    EXPECT_FALSE(num1 > num2);
    EXPECT_TRUE(num1 != num2);
}

// Тест для числа Pi
TEST_F(FixedPointTest, PiCalculation) {
    auto start = std::chrono::high_resolution_clock::now();
    FixedPoint pi = get_pi();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                   (std::chrono::high_resolution_clock::now() - start);
    std::string pi_str = pi.to_string();
    pi_str.resize(102);

    EXPECT_EQ(pi_str, pi_right);
    EXPECT_TRUE(duration.count() < 1000);

}