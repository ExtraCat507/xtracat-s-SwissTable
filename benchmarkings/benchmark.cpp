#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "../my_swiss_table.h"

using namespace std;
using namespace std::chrono;

// ------------------ Timer ------------------
template <typename Func>
long long measure(Func f) {
  auto start = high_resolution_clock::now();
  f();
  auto end = high_resolution_clock::now();
  return duration_cast<microseconds>(end - start).count();
}

// ------------------ Data Generators ------------------
vector<int> gen_int(size_t n) {
  vector<int> v(n);
  iota(v.begin(), v.end(), 0);
  shuffle(v.begin(), v.end(), mt19937{random_device{}()});
  return v;
}

vector<double> gen_double(size_t n) {
  vector<double> v(n);
  mt19937 rng(random_device{}());
  uniform_real_distribution<double> dist(0.0, 1e6);
  for (auto& x : v)
    x = dist(rng);
  return v;
}

vector<string> gen_string(size_t n) {
  vector<string> v(n);
  for (size_t i = 0; i < n; ++i)
    v[i] = "str_" + to_string(i);
  shuffle(v.begin(), v.end(), mt19937{random_device{}()});
  return v;
}

// ------------------ Benchmark Function ------------------
template <typename T, typename Generator>
void benchmark_type(ofstream& file, const string& type_name, Generator gen) {
  vector<size_t> sizes = {1000, 5000, 10000, 50000, 100000, 1000000, 10000000};

  for (auto n : sizes) {
    auto data = gen(n);

    // ------------------ std::set ------------------
    {
      std::set<T> s;

      long long insert_t = measure([&]() {
        for (const auto& x : data)
          s.insert(x);
      });

      long long find_t = measure([&]() {
        for (const auto& x : data)
          s.find(x);
      });

      long long erase_t = measure([&]() {
        for (const auto& x : data)
          s.erase(x);
      });

      file << "std::set," << type_name << "," << n << "," << insert_t << "," << find_t << ","
           << erase_t << "\n";
    }

    // ------------------ std::unordered_set ------------------
    {
      std::unordered_set<T> s;

      long long insert_t = measure([&]() {
        for (const auto& x : data)
          s.insert(x);
      });

      long long find_t = measure([&]() {
        for (const auto& x : data)
          s.find(x);
      });

      long long erase_t = measure([&]() {
        for (const auto& x : data)
          s.erase(x);
      });

      file << "std::unordered_set," << type_name << "," << n << "," << insert_t << "," << find_t
           << "," << erase_t << "\n";
    }

    // ------------------ MyStructure ------------------
    {
      SwissTable<T> s;

      long long insert_t = measure([&]() {
        for (const auto& x : data)
          s.insert(x);
      });

      long long find_t = measure([&]() {
        for (const auto& x : data)
          s.find(x);
      });

      long long erase_t = measure([&]() {
        for (const auto& x : data)
          s.erase(x);
      });

      file << "SwissTable," << type_name << "," << n << "," << insert_t << "," << find_t << ","
           << erase_t << "\n";

      // file << "MyStructure," << type_name << "," << n << "," << insert_t << "," << find_t << ","
      //      << "\n";
    }

    cout << "Done: " << type_name << " size=" << n << endl;
  }
}

// ------------------ Main ------------------
int main() {
  ofstream file("benchmark.csv");

  file << "structure,type,size,insert_us,find_us,erase_us\n";

  benchmark_type<int>(file, "int", gen_int);
  benchmark_type<double>(file, "double", gen_double);
  benchmark_type<string>(file, "string", gen_string);

  file.close();

  cout << "Benchmark complete. Results saved to benchmark.csv\n";
  return 0;
}
