## How to proceed:
Simple tests: `testing/gpt-tests.cpp`  
Difference test against _std::unordered_set_ : `testing/diff-test.cpp`  

### Benchmarks (./benchmarkings)
0. Build google benchmarks and compile googlebench.exe - `./gbenchbuild.sh`
1. Benchmark `googlebench.exe` -> `./results/googlebenchmark.csv`
2. Plot `./plot_benchmarks.py` -> `benchmark_9graphs.png`

### Results
<img width="4378" height="4226" alt="benchmark_9graphs" src="https://github.com/user-attachments/assets/8e8ddb09-8773-4cd9-986b-9e41e6ca96ab" />
