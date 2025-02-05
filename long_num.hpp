#include <iostream>
#include <vector>
#include <algorithm>

class FixedPoint {
public:
  // Constructor (integer and fractional parts are vectors of unsigned chars (bytes))
  FixedPoint(const std::vector<unsigned char>& integerPart, const std::vector<unsigned char>& fractionalPart, int fractionalBits) :
    integer(integerPart), fractional(fractionalPart), fractional_bits(fractionalBits) {}

  // Helper function to convert a decimal number to FixedPoint representation
  FixedPoint(double num, int fractionalBits) : fractional_bits(fractionalBits) {
    long long integral = static_cast<long long>(num);
    double fractional_part = num - integral;

    // convert integral part to vector of bytes
    integer = toByteArray(integral);

    // convert fractional part to vector of bytes
    long long fractional_val = static_cast<long long>(fractional_part * (1LL << fractionalBits));
    fractional = toByteArray(fractional_val);
  }



  // Addition
  FixedPoint operator+(const FixedPoint& other) const;

  // Subtraction
  FixedPoint operator-(const FixedPoint& other) const;

  // Multiplication
  FixedPoint operator*(const FixedPoint& other) const;

  // Division
  FixedPoint operator/(const FixedPoint& other) const;

  // Comparison operators
  bool operator>(const FixedPoint& other) const;
  bool operator<(const FixedPoint& other) const;
  bool operator>=(const FixedPoint& other) const;
  bool operator<=(const FixedPoint& other) const;
  bool operator==(const FixedPoint& other) const;
  bool operator!=(const FixedPoint& other) const;

  // print the fixed point number
  void print() const;

private:
  std::vector<unsigned char> integer;
  std::vector<unsigned char> fractional;
  int fractional_bits;

  //Helper function to convert long long to vector of bytes
  std::vector<unsigned char> toByteArray(long long num) const;

  //Helper function to convert vector of bytes to long long
  long long fromByteArray(const std::vector<unsigned char>& arr) const;


};

std::vector<unsigned char> FixedPoint::toByteArray(long long num) const {
    std::vector<unsigned char> result;
    if(num == 0){
        result.push_back(0);
        return result;
    }
    while (num > 0) {
        result.push_back(num % 256); // Assuming 8-bit bytes
        num /= 256;
    }
    std::reverse(result.begin(),result.end());
    return result;
}


long long FixedPoint::fromByteArray(const std::vector<unsigned char>& arr) const {
    long long result = 0;
    long long power = 1;
    for(int i = arr.size() - 1; i >= 0; --i){
        result += arr[i] * power;
        power *= 256;
    }
    return result;
}

FixedPoint FixedPoint::operator+(const FixedPoint& other) const {
  //Basic addition, you would need to handle carry bits properly
  long long int_sum = fromByteArray(integer) + fromByteArray(other.integer);
  long long frac_sum = fromByteArray(fractional) + fromByteArray(other.fractional);

  //Handle carry from fractional part
  if(frac_sum >= (1LL << fractional_bits)){
      int_sum++;
      frac_sum -= (1LL << fractional_bits);
  }

  return FixedPoint(toByteArray(int_sum),toByteArray(frac_sum), fractional_bits);
}

//Implement the remaining operators similarly (subtraction, multiplication, division, comparisons)  Remember to handle potential overflows carefully.

FixedPoint FixedPoint::operator-(const FixedPoint& other) const {
    // Implement subtraction similarly to addition
    long long int_diff = fromByteArray(integer) - fromByteArray(other.integer);
    long long frac_diff = fromByteArray(fractional) - fromByteArray(other.fractional);

    //Handle borrow from fractional part (similar to carry)
    if(frac_diff < 0){
        int_diff--;
        frac_diff += (1LL << fractional_bits);
    }

    return FixedPoint(toByteArray(int_diff),toByteArray(frac_diff), fractional_bits);

}

FixedPoint FixedPoint::operator*(const FixedPoint& other) const {
    long long int1 = fromByteArray(integer);
    long long frac1 = fromByteArray(fractional);
    long long int2 = fromByteArray(other.integer);
    long long frac2 = fromByteArray(other.fractional);

    long long product = (int1 * (1LL << fractional_bits) + frac1) * (int2 * (1LL << fractional_bits) + frac2);

    long long int_part = product / (1LL << fractional_bits);
    long long frac_part = product % (1LL << fractional_bits);


    return FixedPoint(toByteArray(int_part), toByteArray(frac_part), fractional_bits);
}


FixedPoint FixedPoint::operator/(const FixedPoint& other) const {
    //Division is more complex and might require a dedicated algorithm
    //This is a placeholder; a proper implementation would need to handle rounding and potential division by zero.
    long long num = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long den = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);

    long long quotient = num / den;
    long long remainder = num % den;

    return FixedPoint(toByteArray(quotient), toByteArray(remainder), fractional_bits);
}

bool FixedPoint::operator>(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum > otherNum;
}

bool FixedPoint::operator<(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum < otherNum;
}


bool FixedPoint::operator>=(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum >= otherNum;
}

bool FixedPoint::operator<=(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum <= otherNum;
}

bool FixedPoint::operator==(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum == otherNum;
}

bool FixedPoint::operator!=(const FixedPoint& other) const {
    long long thisNum = fromByteArray(integer) * (1LL << fractional_bits) + fromByteArray(fractional);
    long long otherNum = fromByteArray(other.integer) * (1LL << fractional_bits) + fromByteArray(other.fractional);
    return thisNum != otherNum;
}

void FixedPoint::print() const {
    std::cout << fromByteArray(integer) << ".";
    std::cout << fromByteArray(fractional) << std::endl;
}

int main() {
  FixedPoint num1(1.5, 8); // 1.5 with 8 fractional bits
  FixedPoint num2(4.2, 8); // 4.2 with 8 fractional bits

  FixedPoint sum = num1 + num2;
  FixedPoint diff = num1 - num2;
  FixedPoint prod = num1 * num2;
  FixedPoint div = num1 / num2;

  std::cout << "Sum: "; sum.print();
  std::cout << "Difference: "; diff.print();
  std::cout << "Product: "; prod.print();
  std::cout << "Division: "; div.print();


  std::cout << "num1 > num2: " << (num1 > num2) << std::endl;
  std::cout << "num1 < num2: " << (num1 < num2) << std::endl;
  std::cout << "num1 == num2: " << (num1 == num2) << std::endl;


  return 0;
}