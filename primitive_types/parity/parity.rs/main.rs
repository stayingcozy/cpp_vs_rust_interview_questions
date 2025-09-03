// rustc -O -C debuginfo=0 main.rs

fn count_bits(x: u32) -> i16 {
    // O(n)
    let mut num_bits = 0i16;
    let mut x = x;
    while x != 0 {
        num_bits += (x & 1) as i16;
        x >>= 1;
    }
    num_bits
}

// brute-force algorithm of parity
fn parity(x: u64) -> i16 {
    // O(n)
    let mut result = 0i16;
    let mut x = x;
    while x != 0 {
        result ^= (x & 1) as i16;
        x >>= 1;
    }
    result
}

fn parity_bit_fiddlin(x: u64) -> i16 {
    // O(k)
    // k is the number of bits set to 1
    let mut result = 0i16;
    let mut x = x;
    while x != 0 {
        // Removes lowest set bit until 0
        result ^= 1; // simple bit flip
        x &= x - 1; // Removes lowest set bit 00101100 => 00101000
    }
    result
}

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
    // L => width of words
    // n => word size
    // O(n/L)
    const K_MASK_SIZE: u32 = 16;
    const K_BIT_MASK: u64 = 0xFFFF;
    
    k_precompute_parity[(x >> (3 * K_MASK_SIZE)) as usize] ^
    k_precompute_parity[((x >> (2 * K_MASK_SIZE)) & K_BIT_MASK) as usize] ^
    k_precompute_parity[((x >> K_MASK_SIZE) & K_BIT_MASK) as usize] ^
    k_precompute_parity[(x & K_BIT_MASK) as usize]
}

fn parity_xor(x: u64) -> i16 {
    // O(logn)
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
    let x: u32 = 7;
    let result_x = count_bits(x);
    let result_y = parity(x as u64);
    let result_z = parity_bit_fiddlin(x as u64);
    
    let mut k_precompute_parity = [0i16; 65536];
    generate_precompute_parity(&mut k_precompute_parity);
    let result_0 = parity_precompute_k(x as u64, &k_precompute_parity);
    
    let result_1 = parity_xor(x as u64);

    println!("Bits in x: {}", result_x);
    println!("Parity Brute Force in x: {}", result_y);
    println!("Parity Number of 1's in x: {}", result_z);
    println!("Parity PreCompute 16 cache in x: {}", result_0);
    println!("Parity XOR, associative hack in x: {}", result_1);
}