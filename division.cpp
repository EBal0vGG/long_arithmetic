#include <iostream>
#include <string>
#include <vector>

void convertion_to_bin(const std::string& numStr) {

    if (numStr.size() == 1 && (numStr[0] == '1' || numStr[0] == '0')) {
        std::cout << numStr[0];
        return;
    }

    int remainder = 0; // Остаток от деления
    std::string result; // Результат деления

    for (char digitChar : numStr) {
        int currentDigit = (digitChar - '0') + remainder * 10; // Текущая цифра с учетом остатка
        int quotient = currentDigit / 2; // Частное от деления на 2
        remainder = currentDigit % 2; // Остаток от деления на 2

        result += std::to_string(quotient); // Добавляем частное к результату
    }

    // Убираем ведущие нули в результате
    result.erase(0, result.find_first_not_of('0'));
    if (result.empty()) {
        result = "0";
    }

    convertion_to_bin(result);
    std::cout << remainder;
}

int main() {

    convertion_to_bin("10");

    return 0;
}


