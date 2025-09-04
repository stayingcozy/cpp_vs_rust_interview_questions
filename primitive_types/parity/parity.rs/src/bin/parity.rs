// rustc -O -C debuginfo=0 main.rs

use rand::Rng;
use std::io::Write;
use std::process;
use std::time::Instant;
use utils::notify;

const NUM_VALUES: usize = 131_072;
const TRIES: usize = 8192;

// Original parity functions
fn parity(x: u64) -> i16 {
    // O(n) brute-force algorithm
    let mut result = 0i16;
    let mut x = x;
    while x != 0 {
        result ^= (x & 1) as i16;
        x >>= 1;
    }
    result
}

fn parity_bit_fiddlin(x: u64) -> i16 {
    // O(k) - k is the number of bits set to 1
    let mut result = 0i16;
    let mut x = x;
    while x != 0 {
        result ^= 1; // simple bit flip
        x &= x - 1; // Removes lowest set bit
    }
    result
}

#[allow(clippy::needless_range_loop)]
fn generate_precompute_parity(k_precompute_parity: &mut [i16; 65536]) {
    for i in 0..65536 {
        let mut parity = 0i16;
        let mut x = i;
        while x != 0 {
            parity ^= 1;
            x &= x - 1;
        }
        k_precompute_parity[i] = parity;
    }
}

fn parity_precompute_k(x: u64, k_precompute_parity: &[i16; 65536]) -> i16 {
    // O(n/L) with precomputed lookup table
    const K_MASK_SIZE: u32 = 16;
    const K_BIT_MASK: u64 = 0xFFFF;

    k_precompute_parity[(x >> (3 * K_MASK_SIZE)) as usize]
        ^ k_precompute_parity[((x >> (2 * K_MASK_SIZE)) & K_BIT_MASK) as usize]
        ^ k_precompute_parity[((x >> K_MASK_SIZE) & K_BIT_MASK) as usize]
        ^ k_precompute_parity[(x & K_BIT_MASK) as usize]
}

fn parity_xor(x: u64) -> i16 {
    // O(logn) XOR folding
    let mut x = x;
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    (x & 0x1) as i16
}

fn main() {
    // Test fixtures for correctness validation
    let test_values = [7u64, 15, 255, 0xDEADBEEF, 0xABCDEF123456789];
    let expected_results = [1i16, 0, 0, 0, 0]; // Expected parity results

    // Pre-generate lookup table for parity_precompute_k
    let mut k_precompute_parity = [0i16; 65536];
    generate_precompute_parity(&mut k_precompute_parity);

    // Validate all algorithms produce correct results
    for (val, expected) in test_values.iter().zip(expected_results.iter()) {
        assert_eq!(parity(*val), *expected);
        assert_eq!(parity_bit_fiddlin(*val), *expected);
        assert_eq!(parity_precompute_k(*val, &k_precompute_parity), *expected);
        assert_eq!(parity_xor(*val), *expected);
    }

    // Generate test data
    let mut rng = rand::thread_rng();
    let test_data: Vec<u64> = (0..NUM_VALUES).map(|_| rng.r#gen::<u64>()).collect();

    notify!("Rust\t{pid}", pid = process::id());

    // Benchmark brute force parity
    let time_start = Instant::now();
    let mut _sum_brute = 0i64;
    for _ in 0..TRIES {
        for &val in &test_data {
            _sum_brute += parity(val) as i64;
        }
    }
    let _t_brute = time_start.elapsed().as_secs_f32();

    // Benchmark bit fiddling parity
    let t1_start = Instant::now();
    let mut _sum_bitfiddle = 0i64;
    for _ in 0..TRIES {
        for &val in &test_data {
            _sum_bitfiddle += parity_bit_fiddlin(val) as i64;
        }
    }
    let _t_bitfiddle = t1_start.elapsed().as_secs_f32();

    // Benchmark precomputed parity
    let t2_start = Instant::now();
    let mut _sum_precompute = 0i64;
    for _ in 0..TRIES {
        for &val in &test_data {
            _sum_precompute += parity_precompute_k(val, &k_precompute_parity) as i64;
        }
    }
    let _t_precompute = t2_start.elapsed().as_secs_f32();

    // Benchmark XOR parity
    let t3_start = Instant::now();
    let mut _sum_xor = 0i64;
    for _ in 0..TRIES {
        for &val in &test_data {
            _sum_xor += parity_xor(val) as i64;
        }
    }
    let _t_xor = t3_start.elapsed().as_secs_f32();

    notify!("stop");

    //     println!("Brute Force Parity O(n): sum={}, time={:.4}s", sum_brute, t_brute);
    //     println!("Bit Fiddling Parity O(k): sum={}, time={:.4}s", sum_bitfiddle, t_bitfiddle);
    //     println!("Precomputed Parity O(n/L): sum={}, time={:.4}s", sum_precompute, t_precompute);
    //     println!("XOR Folding Parity O(log n): sum={}, time={:.4}s", sum_xor, t_xor);
}
