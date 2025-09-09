# C++ vs Rust Interview Questions
Solutions for Elements of Programming Interviews Book written in C++ and Rust. Speed and Memory compared for which is the fastest across algorithms. I'm a big fan of both c++ and rust. This effort is to improve my skills in high performance computing in a fun competitive way. 

clang++ is to be used for C++ compiler to have both rust and c++ use the LLVM compiler. g++ will be added later to challenge speed rankings. 

Benchmarks inspired by kostya/benchmarks. Speed, memory, and energy consumed will be compared. Docker container of debian will be used to standardize the environment. 

run.bat for windows. (moving to debian as soon as I get a clean, working solution for Solidworks on there)

# Results
base64 (non EPI book) baseline test. 
|                Language |                  Time, s |                                     Memory, MiB |
| :---------------------- | -----------------------: | ----------------------------------------------: |
|                    Rust | 1.033<sub>±00.005</sub> | 2.34<sub>±00.00</sub> + 0.00<sub>±00.00</sub> |
| C++/clang++ (libcrypto) | 4.485<sub>±00.040</sub> | 6.48<sub>±00.08</sub> + 0.00<sub>±00.00</sub> |

parity - 4.2; too small input to properly test (fix)
|                        Language |                  Time, s |                                     Memory, MiB |
| :------------------------------ | -----------------------: | ----------------------------------------------: |
| C++/clang++ (parity algorithms) | 0.000<sub>±00.000</sub> | 0.00<sub>±00.00</sub> + 0.00<sub>±00.00</sub> |
|                            Rust | 0.007<sub>±00.001</sub> | 2.81<sub>±00.00</sub> + 0.00<sub>±00.00</sub> |



# Manual Run
Build Analyzer
```
clang++ analyzer.cpp -o analyzer 
```

Build and Run Docker Container
```
run.sh build # run.bat for windows
run.sh shell
```

Build and Run Test (in Container)
```
cd <test_dir>
make build
ATTEMPTS=<#_of_attempts> ../../analyzer make run
```

# TODO
Up Nxt
- [ ] Test swap_bits cpp, push

Benchmark Environment & Testing
- [ ] Scale Parity inputs for proper testing. 

EPI Book Sections
- [ ] Complete Data Structures and Algorithms
- [ ] Complete Domain Specific Problems
- [ ] Complete Honors Class
