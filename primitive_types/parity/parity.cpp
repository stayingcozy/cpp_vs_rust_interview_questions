#include <iostream>
#include <array>

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

void generatePreComputeParity(std::array<short, 65536>& kPreComputeParity) {
    for (int i = 0; i < 65536; i++) {
        short parity = 0;
        int x = i;
        while (x) {
            parity ^= 1;
            x &= (x - 1);
        }
        kPreComputeParity[i] = parity;
    }
}

short ParityPreComputeK(unsigned long long x, std::array<short, 65536>& kPreComputeParity) { // 2^16 == 65536
    // L => width of words
    // n => word size
    // O(n/L)
    const int kMaskSize = 16;
    const int kBitMask = 0xFFFF;
    return kPreComputeParity[x >> (3 * kMaskSize)] ^
    kPreComputeParity[(x >> (2 * kMaskSize)) & kBitMask] ^
    kPreComputeParity[(x >> kMaskSize) & kBitMask] ^
    kPreComputeParity[x & kBitMask];
}

short ParityXOR(unsigned long long x) {
    // O(logn)
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 0x1;
}

int main() {
    unsigned int x = 7;
    short result_x = CountBits(x);
    short result_y = Parity(x);
    short result_z = ParityBitFiddlin(x);
    
    std::array<short, 65536> kPreComputeParity;
    generatePreComputeParity(kPreComputeParity);
    short result_0 = ParityPreComputeK(x, kPreComputeParity);
    
    short result_1 = ParityXOR(x);

    std::cout << "Bits in x: " << result_x << std::endl;
    std::cout << "Parity Brute Force in x: " << result_y << std::endl;
    std::cout << "Parity Number of 1's in x: " << result_z << std::endl;
    std::cout << "Parity PreCompute 16 cache in x: " << result_0 << std::endl;
    std::cout << "Parity XOR, associative hack in x: " << result_1 << std::endl;

    return 0;
}