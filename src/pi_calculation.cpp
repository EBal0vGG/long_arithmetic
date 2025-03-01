#include "../include/long_ariphmetic.hpp"
#include "../include/pi_calculation.hpp"

void CalcPi(FixedPoint &pi, const int k_start, const int k_finish, const FixedPoint &bs) {
    FixedPoint one = FixedPoint(1.0, 256);
    FixedPoint two = FixedPoint(2.0, 256);
    FixedPoint four = FixedPoint(4.0, 256);
    FixedPoint base = bs;
    FixedPoint res = FixedPoint(0.0, 256);
    for(int i = k_start; i < k_finish; ++i) {
        res = res + ((four / FixedPoint(8 * i + 1, 256)) -
                     (two / FixedPoint(8 * i + 4, 256)) -
                     (one / FixedPoint(8 * i + 5, 256)) -
                     (one / FixedPoint(8 * i + 6, 256))) / base;
        base = base * FixedPoint(16.0, 256);
    }
    pi = pi + res;
}

FixedPoint get_pi() {
    int prec = 86;
    int n = (prec + 15) / 16 * 16;
    int signs = n / 16;
    FixedPoint pi = FixedPoint(0.0, 256);
    FixedPoint curBs = FixedPoint(1.0, 256);

    for (int i = 0; i <= n; i++) {
        if (i % signs == 0)
            CalcPi(pi, i, i + signs, curBs);
        curBs = curBs * FixedPoint(16.0, 256);
    }
    return pi;
}
