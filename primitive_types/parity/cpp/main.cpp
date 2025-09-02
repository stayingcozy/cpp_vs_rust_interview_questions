#include <iostream>

// run clang++ main.cpp -o main.exe in "Developer Command Prompt for VS 2019" for Windows with Visual Studio 2019
// docker debian env to come to avoid cursed windows runs
// add -O3 tag for speed tests

short CountBits(unsigned int x) {
    // O(n)
    short num_bits = 0;
    while (x) {
        num_bits += x & 1;
        x >>= 1;
    }
    return num_bits;
}

// brute-force algorithm of parity
short Parity(unsigned long long x) {
    // O(n)
    short result = 0;
    while (x) {
        result ^= (x & 1);
        x >>= 1;
    }
    return result;
}

short ParityBitFiddlin(unsigned long long x) {
    // O(k)
    // k is the number of bits set to 1
    short result = 0;
    while (x) {
        // Removes lowest set bit until 0
        result ^= 1; // simple bit flip
        x &= (x - 1); // Removes lowest set bit 00101100 => 00101000
    }
    return result;
}

int main() {
    unsigned int x = 7;
    short result_x = CountBits(x);
    short result_y = Parity(x);
    short result_z = ParityBitFiddlin(x);

    std::cout << "Bits in x: " << result_x << std::endl;
    std::cout << "Parity Brute Force in x: " << result_y << std::endl;
    std::cout << "Parity Number of 1's in x: " << result_z << std::endl;

    return 0;
}