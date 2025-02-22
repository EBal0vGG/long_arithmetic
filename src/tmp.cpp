static std::string divBinaryWithFraction(const std::string &num1, const std::string &num2, int frac_bits) {
    auto [intPart1, fracPart1] = getTwoStrings(num1);
    auto [intPart2, fracPart2] = getTwoStrings(num2);

    intPart1 = decimalToBinary(intPart1);
    intPart2 = decimalToBinary(intPart2);
    fracPart1 = Binary_Faction(fracPart1, frac_bits);
    fracPart2 = Binary_Faction(fracPart2, frac_bits);

    std::string binaryNum1 = intPart1 + fracPart1;
    std::string binaryNum2 = intPart2 + fracPart2;

    binaryNum1.erase(0, binaryNum1.find_first_not_of('0'));
    binaryNum2.erase(0, binaryNum2.find_first_not_of('0'));

    if (binaryNum2.empty()) {
        throw std::invalid_argument("Division by zero");
    }

    std::string result;
    std::string remainder = "0";

    for (size_t i = 0; i < binaryNum1.size() + frac_bits; ++i) {
        remainder.push_back(i < binaryNum1.size() ? binaryNum1[i] : '0');
        remainder.erase(0, remainder.find_first_not_of('0'));

        if (remainder.empty()) {
            remainder = "0";
        }

        if (compareBinary(remainder, binaryNum2) >= 0) {
            result.push_back('1');
            remainder = subtractBinary(remainder, binaryNum2);
        } else {
            result.push_back('0');
        }
    }

    result.insert(result.begin() + binaryNum1.size(), '.');
    size_t dotPos = result.find('.');
    std::string intPartResult = result.substr(0, dotPos);
    intPartResult.erase(0, intPartResult.find_first_not_of('0'));

    if (intPartResult.empty()) {
        intPartResult = "0";
    }

    std::string fracPartResult = result.substr(dotPos + 1);
    fracPartResult.erase(fracPartResult.find_last_not_of('0') + 1);

    std::string finalResult = intPartResult + "." + fracPartResult;

    return finalResult;
}

// Сравнение двоичных чисел
static int compareBinary(const std::string &num1, const std::string &num2) {
    if (num1.size() < num2.size()) {
        return -1;
    } else if (num1.size() > num2.size()) {
        return 1;
    } else {
        return num1.compare(num2);
    }
}
