#include <iostream>
#include <random>
#include <libnotify.h>

#ifdef __clang__
static constexpr auto COMPILER = "clang++";
#else 
static constexpr auto COMPILER = "g++";
#endif 

using namespace std;

typedef mt19937 myRNG;
uint32_t seed_val = 29231093;
myRNG rng;

long long SwapBits(long long x, int i, int j) {
    // Extract ith and jth values and see if they differ
    if (((x >> i) & 1) != ((x >> j) & 1)) {
        // bit swap
        unsigned long long bit_mask = (1LL << i) | (1LL << j);
        x ^= bit_mask;
    }
    return x;
}



int main() {
    // Known test values
    // Test fixtures for SwapBits function validation
    const long long test_values[] = {
        0b1010,                    // 10
        0b11110000,               // 240
        0x12345678,               // 305419896
        0xDEADBEEF,              // 3735928559
        0xABCDEF123456789LL,     // 12379813812177893257
        0,                        // 0
        -1,                       // all 1s
        1,                        // single bit
        0x8000000000000000LL     // high bit set
    };

    const int i_positions[] = {0, 2, 4, 0, 8, 0, 0, 0, 63};
    const int j_positions[] = {1, 5, 7, 31, 16, 63, 1, 5, 0};

    const long long expected_results[] = {
        0b1001,                   // 0,1 swap in 1010
        0b11010100,              // 2,5 swap in 11110000  
        0x12345668,              // 4,7 swap in 0x12345678
        0x5EADBEEF,              // 0,31 swap in 0xDEADBEEF
        0xABCDEF123456689LL,     // 8,16 swap in 0xABCDEF123456789LL
        0,                        // 0,63 swap in 0 (no change)
        -1,                       // 0,1 swap in all 1s (no change)
        1,                        // 0,5 swap in 1 (no change - bit 5 is 0)
        0x0000000000000001LL     // 63,0 swap in 0x8000000000000000LL
    };

    const int num_fixtures = sizeof(test_values) / sizeof(test_values[0]);

    for (int k = 0; k < num_fixtures; ++k) {
        const auto input = test_values[k];
        const auto i = i_positions[k];
        const auto j = j_positions[k];
        const auto expected = expected_results[k];
        
        const auto result = SwapBits(input, i, j);
        
        if (result != expected) {
            cerr << "Parity mismatch for value: " << input << endl;
            exit(EXIT_FAILURE);
        }
    }

    const auto NUM_VALUES = 131072;
    const auto TRIES = 8192;

    // Generate test data
    rng.seed(seed_val);
    uniform_int_distribution<int> uint_distByte(0,7);

    unsigned long long test_data[NUM_VALUES];
    int i_test_data[NUM_VALUES];
    int j_test_data[NUM_VALUES];

    for (int i = 0; i < NUM_VALUES; i++) {
        test_data[i] = (static_cast<unsigned long long>(rand()) << 32) | rand();
        int first = uint_distByte(rng);
        int last = uint_distByte(rng);
        if (first > last) {
            j_test_data[i] = last;
            i_test_data[i] = first;
        } else {
            i_test_data[i] = first;
            j_test_data[i] = last;
        }

    }

    auto t = 0.0;
    long long ans;

    // starting gun
    notifying_invoke(
        [&]() {
            long long ans;

            // benchmark
            const auto t1 = clock();
            // test
            for (auto i = 0; i < TRIES; i++) {
                for (auto j = 0; j < NUM_VALUES; j++) {
                    ans = SwapBits(test_data[j], i_test_data[j], j_test_data[j]);
                }
            }
            t = (float)(clock() - t1) / CLOCKS_PER_SEC;
        },
        "C++/{} (bit swap algorithm)", COMPILER);

    return 0;
}