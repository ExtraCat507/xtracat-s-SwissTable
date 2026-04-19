#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>

#include "../my_swiss_table.h"

using namespace std;

vector<int> gen_int(size_t n) {
  vector<int> v(n);
  iota(v.begin(), v.end(), 0);
  shuffle(v.begin(), v.end(), mt19937{random_device{}()});
  return v;
}

int main() {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist3(
      1, 3
  );  // distribution in range [1, 6]

  SwissTable<int> swiss;
  unordered_set<int> un_set;
  int n = 1e4;
  int ind = 0;
  auto all = gen_int(n);

  for (int i = 0; i < n; ++i) {
    switch (dist3(rng)) {
      case 1:  // insert
        swiss.insert(all[ind]);
        un_set.insert(all[ind]);
        ind++;
        break;
      case 2:                  // find
        if (dist3(rng) % 2) {  // smth that might be in
          int q = all[std::uniform_int_distribution<std::mt19937::result_type>(0, ind)(rng)];
          bool res1 = swiss.find(q) != swiss.end() ? 1 : 0;
          bool res2 = un_set.find(q) != un_set.end() ? 1 : 0;
          assert(res1 == res2);
        } else {  // most likely not
          int q = std::uniform_int_distribution<std::mt19937::result_type>(0, INT_MAX)(rng);
          bool res1 = swiss.find(q) != swiss.end() ? 1 : 0;
          bool res2 = un_set.find(q) != un_set.end() ? 1 : 0;
          assert(res1 == res2);
        }
        break;
      case 3:  // erase
        int q = all[std::uniform_int_distribution<std::mt19937::result_type>(0, ind)(rng)];
        swiss.erase(q);
        un_set.erase(q);
        assert(swiss.find(q) == swiss.end());
    }
  }

  return 0;
}
