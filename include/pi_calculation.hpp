#ifndef PI_CALC_H
#define PI_CALC_H

#include "../include/long_ariphmetic.hpp"

const std::string pi_right = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";

void CalcPi(FixedPoint &pi, const int k_start, const int k_finish, const FixedPoint &bs);

FixedPoint get_pi();

#endif // PI_CALC_H