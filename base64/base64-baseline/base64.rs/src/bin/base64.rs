use base64::Engine;
use std::io::Write;
use std::time::Instant;
use std::{process, str};
use utils::notify;

const STR_SIZE: usize = 131_072;
const TRIES: usize = 8192;

fn main() {
    let base64_engine = base64::engine::general_purpose::STANDARD;

    for [src, dst] in &[["hello", "aGVsbG8="], ["world", "d29ybGQ="]] {
        let encoded = base64_engine.encode(src);
        assert_eq!(encoded, *dst);

        let decoded = String::from_utf8(base64_engine.decode(dst).unwrap()).unwrap();
        assert_eq!(decoded, *src);
    }

    let input = vec![b'a'; STR_SIZE];
    let output = base64_engine.encode(&input);
    let str3 = base64_engine.decode(&output).unwrap();

    notify!("Rust\t{pid}", pid = process::id());

    let time_start = Instant::now();
    let mut sum_encoded = 0;
    for _ in 0..TRIES {
        sum_encoded += base64_engine.encode(&input).len();
    }
    let t_encoded = time_start.elapsed().as_secs_f32();

    let t1_start = Instant::now();
    let mut sum_decoded = 0;
    for _ in 0..TRIES {
        sum_decoded += base64_engine.decode(&output).unwrap().len();
    }
    let t_decoded = t1_start.elapsed().as_secs_f32();

    notify!("stop");

    println!(
        "encode {input}... to {output}...: {sum_encoded}, {t_encoded}",
        input = str::from_utf8(&input[..4]).unwrap(),
        output = &output[..4]
    );

    println!(
        "decode {output}... to {str3}...: {sum_decoded}, {t_decoded}",
        output = &output[..4],
        str3 = str::from_utf8(&str3[..4]).unwrap()
    );
}
