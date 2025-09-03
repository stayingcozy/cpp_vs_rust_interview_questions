# C++ vs Rust Interview Questions
Solutions for Elements of Programming Interviews Book written in C++ and Rust. Speed and Memory compared for which is the fastest across algorithms. I'm a big fan of both c++ and rust. This effort is to improve my skills in high performance computing in a fun competitive way. 

clang++ is to be used for C++ compiler to have both rust and c++ use the LLVM compiler. g++ will be added later to challenge speed rankings. 

Benchmarks inspired by kostya/benchmarks. Speed, memory, and energy consumed will be compared. Docker container of debian will be used to standardize the environment. 

run.bat for windows. (moving to debian as soon as I get a clean, working solution for Solidworks on there)


# Manual Run
Build Analyzer
```
clang++ analyzer.cpp -o analyzer 
```

Build and Run Docker Container
```
run.sh build # run.bat for windows
run.bat shell
```

Build and Run Test (in Container)
```
cd <test_dir>
make build
ATTEMPTS=<#_of_attempts> ../../analyzer make run
```

# TODO
Benchmark Environment & Testing
- [ ] Add TCP stream of input data for testing with docker image (great example: [kostya/benchmarks](https://github.com/kostya/benchmarks))

EPI Book Sections
- [ ] Complete Data Structures and Algorithms
- [ ] Complete Domain Specific Problems
- [ ] Complete Honors Class
