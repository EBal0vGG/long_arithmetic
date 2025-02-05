#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Function to convert an integer to binary
string intToBinary(int num) {
    if (num == 0) return "0";
    string binary = "";
    while (num > 0) {
        binary = to_string(num % 2) + binary;
        num /= 2;
    }
    return binary;
}

// Function to multiply a string number by 2
string multiplyByTwo(const string& numStr) {
    string result = "";
    int carry = 0;

    for (int i = numStr.size() - 1; i >= 0; i--) {
        int value = (numStr[i] - '0') * 2 + carry;
        carry = value / 10;
        result = to_string(value % 10) + result;
    }

    if (carry > 0) {
        result = to_string(carry) + result;
    }

    // // Remove leading zeros
    // size_t start = 0;
    // while (start < result.size() && result[start] == '0') {
    //     start++;
    // }

    return result;
}

// Function to convert the fractional part to binary
string fracToBinary(const string& fracStr, int precision = 32) {
    string binary = "";
    string fractionalPart = fracStr;
    int frac_part_size = fractionalPart.size();

    for (int i = 0; i < precision && !fractionalPart.empty(); ++i) {
        // Multiply the fractional part by 2
        string multiplied = multiplyByTwo(fractionalPart);

        if (multiplied.size() == frac_part_size) {
            binary += '0';
            fractionalPart = multiplied;

        } else {
            binary += '1';
            fractionalPart = multiplied.substr(1);
        }

         cout << multiplied << " " << binary << endl;


        if (multiplied.size() == 0) {
            fractionalPart = "0"; // If no more fractional part remains
        }
    }

    return binary;
}

// Main function to convert a fractional number (as a string) to binary
string decimalToBinary(const string& numStr, int precision = 32) {
    // Find the position of the decimal point
    size_t dotPos = numStr.find('.');

    // If no decimal point exists, treat it as an integer
    if (dotPos == string::npos) {
        int integerPart = stoi(numStr);
        return intToBinary(integerPart);
    }

    // Extract the integer and fractional parts
    string integerPartStr = numStr.substr(0, dotPos);
    string fractionalPartStr = numStr.substr(dotPos + 1);

    // Convert the integer part to binary
    int integerPart = stoi(integerPartStr);
    string binaryInteger = intToBinary(integerPart);

    // Convert the fractional part to binary
    string binaryFraction = fracToBinary(fractionalPartStr, precision);

    // Combine the results
    return binaryInteger + "." + binaryFraction;
}

int main() {
    string input;
    cout << "Enter a fractional number: ";
    cin >> input;

    int precision = 32; // Define the precision for the fractional part
    string binaryResult = decimalToBinary(input, precision);

    cout << "Binary representation: " << binaryResult << endl;

    return 0;
}