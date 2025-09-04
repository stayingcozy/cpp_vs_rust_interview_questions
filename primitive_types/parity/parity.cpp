#include <iomanip>
#include <iostream>
#include <array>
#include <ctime>

#include <libnotify.h>

#ifdef __clang__
static constexpr auto COMPILER = "clang++";
#else
static constexpr auto COMPILER = "g++";
#endif

using namespace std;

// Original parity functions
short Parity(unsigned long long x) {
    // O(n) brute-force algorithm
    short result = 0;
    while (x) {
        result ^= (x & 1);
        x >>= 1;
    }
    return result;
}

short ParityBitFiddlin(unsigned long long x) {
    // O(k) - k is the number of bits set to 1
    short result = 0;
    while (x) {
        result ^= 1;
        x &= (x - 1); // Removes lowest set bit
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

short ParityPreComputeK(unsigned long long x, std::array<short, 65536>& kPreComputeParity) {
    // O(n/L) with precomputed lookup table
    const int kMaskSize = 16;
    const int kBitMask = 0xFFFF;
    return kPreComputeParity[x >> (3 * kMaskSize)] ^
           kPreComputeParity[(x >> (2 * kMaskSize)) & kBitMask] ^
           kPreComputeParity[(x >> kMaskSize) & kBitMask] ^
           kPreComputeParity[x & kBitMask];
}

short ParityXOR(unsigned long long x) {
    // O(logn) XOR folding
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 0x1;
}

int main() {
    // Test fixtures for correctness validation
    const unsigned long long test_values[] = {7, 15, 255, 0xDEADBEEF, 0xABCDEF123456789ULL};
    const short expected_results[] = {1, 0, 0, 0, 0}; // Expected parity results
    const int num_fixtures = sizeof(test_values) / sizeof(test_values[0]);

    // Pre-generate lookup table for ParityPreComputeK
    std::array<short, 65536> kPreComputeParity;
    generatePreComputeParity(kPreComputeParity);

    // Validate all algorithms produce correct results
    for (auto i = 0; i < num_fixtures; ++i) {
        const auto val = test_values[i];
        const auto expected = expected_results[i];

        if (Parity(val) != expected ||
            ParityBitFiddlin(val) != expected ||
            ParityPreComputeK(val, kPreComputeParity) != expected ||
            ParityXOR(val) != expected) {
            cerr << "Parity mismatch for value: " << val << endl;
            exit(EXIT_FAILURE);
        }
    }

    const auto NUM_VALUES = 131072;
    const auto TRIES = 8192;

    // Generate test data
    unsigned long long test_data[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; i++) {
        test_data[i] = (static_cast<unsigned long long>(rand()) << 32) | rand();
    }

    auto s_brute = 0;
    auto t_brute = 0.0;
    auto s_bitfiddle = 0;
    auto t_bitfiddle = 0.0;
    auto s_precompute = 0;
    auto t_precompute = 0.0;
    auto s_xor = 0;
    auto t_xor = 0.0;

    notifying_invoke(
        [&]() {
            // Benchmark brute force parity
            const auto t1 = clock();
            for (auto i = 0; i < TRIES; i++) {
                for (auto j = 0; j < NUM_VALUES; j++) {
                    s_brute += Parity(test_data[j]);
                }
            }
            t_brute = (float)(clock() - t1) / CLOCKS_PER_SEC;

            // Benchmark bit fiddling parity
            const auto t2 = clock();
            for (auto i = 0; i < TRIES; i++) {
                for (auto j = 0; j < NUM_VALUES; j++) {
                    s_bitfiddle += ParityBitFiddlin(test_data[j]);
                }
            }
            t_bitfiddle = (float)(clock() - t2) / CLOCKS_PER_SEC;

            // Benchmark precomputed parity
            const auto t3 = clock();
            for (auto i = 0; i < TRIES; i++) {
                for (auto j = 0; j < NUM_VALUES; j++) {
                    s_precompute += ParityPreComputeK(test_data[j], kPreComputeParity);
                }
            }
            t_precompute = (float)(clock() - t3) / CLOCKS_PER_SEC;

            // Benchmark XOR parity
            const auto t4 = clock();
            for (auto i = 0; i < TRIES; i++) {
                for (auto j = 0; j < NUM_VALUES; j++) {
                    s_xor += ParityXOR(test_data[j]);
                }
            }
            t_xor = (float)(clock() - t4) / CLOCKS_PER_SEC;
        },
        "C++/{} (parity algorithms)", COMPILER);

    // cout << fixed;
    // cout << "Brute Force Parity O(n): sum=" << s_brute 
    //      << ", time=" << setprecision(4) << t_brute << "s" << endl;
    // cout << "Bit Fiddling Parity O(k): sum=" << s_bitfiddle 
    //      << ", time=" << setprecision(4) << t_bitfiddle << "s" << endl;
    // cout << "Precomputed Parity O(n/L): sum=" << s_precompute 
    //      << ", time=" << setprecision(4) << t_precompute << "s" << endl;
    // cout << "XOR Folding Parity O(log n): sum=" << s_xor 
    //      << ", time=" << setprecision(4) << t_xor << "s" << endl;

    return 0;
}