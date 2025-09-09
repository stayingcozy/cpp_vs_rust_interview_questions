// clang++ pow.cpp -o pow.exe && ./pow
#include <iostream>

double compute_pow_brute(double x, int y) {
    // O(2^n)
    double x_const = x;
    for (int i=0; i<y-1; i++) {
        x *= x_const;
    }

    return x;
}

double compute_pow_bit(double x, int y) {
    // takes advantage of the fact if 
    //   y & 1 is true => odd; else even
    //   odd y is x^(y-1)*x; even is x^y
    //   so if odd add extra *x to the base, otherwise for bit of 2 values => x*x
    //   or.. way of cutting it down is... even is x^(y/2)^2; odd is x*x^(y/2)^2
    //   time complexity
    //      dependent on MSB of y worse case 11111111111 for all so 2*bits of 1 in y operations=> O(n)
    double res = 1.0;
    long long power = y;

    // y is negative
    if (y < 0) {
        power *= -1;
        x = 1.0 / x;
    } 


    while (power) {
        if (power & 1) {
            res *= x;
        }
        x *= x;
        power >>= 1;
    }

    return res;
}

int main() {
    // assume y is non-negative
    // assume no overflow, underflow issues

    double x = 3;
    int y = 3;

    std::cout << "x: " << x << ", " << "y: " << y << std::endl;

    double res = compute_pow_bit(x, y);

    std::cout << "Result: " << res << std::endl;
    return 0;
}