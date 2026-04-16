#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "my_swiss_table.h"

using namespace std;

// Basic testing for my SwissTable implementation
// All tests are just AI-slop, but look - they are all passed )
int main() {
  {
    SwissTable<double> table;
    assert(table.capacity() == 32);
    assert(table.find(0.4) == table.end());
    assert(table.begin() == table.end());
  }

  {
    SwissTable<double> table;
    auto it = table.insert(1.5);
    assert(it != table.end());
    assert(table.find(1.5) != table.end());
    assert(*table.find(1.5) == 1.5);
    assert(table.find(2.5) == table.end());
  }

  {
    SwissTable<double> table;
    auto it1 = table.insert(3.14);
    auto it2 = table.insert(3.14);
    assert(it1 == it2);
    assert(table.find(3.14) != table.end());
  }

  {
    SwissTable<double> table;
    for (int i = 0; i < 10; ++i) {
      table.insert(i * 1.0);
    }

    for (int i = 0; i < 10; ++i) {
      assert(table.find(i * 1.0) != table.end());
    }

    int count = 0;
    for (auto it = table.begin(); it != table.end(); ++it) {
      ++count;
    }
    assert(count == 10);
  }

  {
    SwissTable<double> table;
    for (int i = 0; i < 40; ++i) {
      table.insert(i * 0.5);
    }

    for (int i = 0; i < 40; ++i) {
      assert(table.find(i * 0.5) != table.end());
    }

    assert(table.capacity() == 64);
  }
  std::cout << "All basic tests passed\n";

  SwissTable<double> table;

  auto it1 = table.insert(10.0);
  auto it2 = table.insert(20.0);
  auto it3 = table.insert(30.0);

  assert(table.find(20.0) != table.end());

  // erase middle element
  table.erase(table.find(20.0));

  assert(table.find(20.0) == table.end());
  assert(table.find(10.0) != table.end());
  assert(table.find(30.0) != table.end());

  std::cout << "erase basic test passed\n";

  {
    SwissTable<double> table;

    auto it1 = table.insert(10.0);
    auto it2 = table.insert(20.0);
    auto it3 = table.insert(30.0);

    assert(table.find(20.0) != table.end());

    // erase middle element
    table.erase(20.0);

    assert(table.find(20.0) == table.end());
    assert(table.find(10.0) != table.end());
    assert(table.find(30.0) != table.end());

    std::cout << "erase by value test passed\n";
  }

  {
    SwissTable<double> table;

    table.insert(1.0);
    table.insert(2.0);
    table.insert(3.0);

    auto it = table.find(2.0);
    assert(it != table.end());

    table.erase(it);

    assert(table.find(2.0) == table.end());

    // повторная вставка удалённого элемента
    auto it_new = table.insert(2.0);
    assert(it_new != table.end());
    assert(table.find(2.0) != table.end());

    std::cout << "erase + reinsert test passed\n";
  }

  {
    SwissTable<double> table;

    // создаём коллизию искусственно через много элементов
    for (int i = 0; i < 50; ++i) {
      table.insert(i * 1.0);
    }

    table.erase(table.find(10.0));
    table.erase(table.find(20.0));
    table.erase(table.find(30.0));

    // ВСЕ остальные должны находиться
    for (int i = 0; i < 50; ++i) {
      if (i == 10 || i == 20 || i == 30)
        continue;
      assert(table.find(i * 1.0) != table.end());
    }

    std::cout << "probing integrity test passed\n";
  }

  {
    SwissTable<double> table;

    auto a = table.insert(1.0);
    auto b = table.insert(2.0);
    auto c = table.insert(3.0);

    table.erase(b);

    size_t cap_before = table.capacity();

    // вставляем новый элемент — он должен занять deleted слот
    table.insert(4.0);

    assert(table.find(4.0) != table.end());
    assert(table.capacity() == cap_before);

    std::cout << "reuse deleted slot test passed\n";
  }

  {
    SwissTable<int> table;

    table.insert(1);
    table.insert(2);
    table.insert(3);
    table.insert(4);

    std::vector<int> seen;

    for (auto it = table.begin(); it != table.end(); ++it) {
      seen.push_back(*it);
    }

    std::sort(seen.begin(), seen.end());

    assert(seen.size() == 4);
    assert(seen == std::vector<int>({1, 2, 3, 4}));

    std::cout << "test_iteration_basic passed\n";
  }

  {
    SwissTable<int> table;

    table.insert(10);
    table.insert(20);

    std::vector<int> seen;

    for (auto it = table.begin(); it != table.end(); ++it) {
      seen.push_back(*it);
    }

    assert(seen.size() == 2);
    assert((seen[0] == 10 && seen[1] == 20) || (seen[0] == 20 && seen[1] == 10));

    std::cout << "test_iteration_skips_empty passed\n";
  }

  {
    SwissTable<int> table;

    for (int i = 0; i < 10; i++) {
      table.insert(i);
    }

    table.erase(table.find(5));
    table.erase(table.find(7));

    std::vector<int> seen;

    for (auto it = table.begin(); it != table.end(); ++it) {
      seen.push_back(*it);
    }

    assert(std::find(seen.begin(), seen.end(), 5) == seen.end());
    assert(std::find(seen.begin(), seen.end(), 7) == seen.end());
    assert(seen.size() == 8);

    std::cout << "test_iteration_after_erase passed\n";
  }

  {
    SwissTable<int> table;

    for (int i = 0; i < 1000; i++) {
      table.insert(i);
    }

    int count = 0;

    for (auto it = table.begin(); it != table.end(); ++it) {
      count++;
    }

    assert(count == 1000);

    std::cout << "test_iteration_stress passed\n";
  }

  {
    SwissTable<int> table;

    assert(table.begin() == table.end());

    std::cout << "test_iteration_empty passed\n";
  }

  {
    SwissTable<int> table;

    table.insert(42);

    auto it = table.begin();

    assert(*it == 42);

    std::cout << "test_iteration_deref passed\n";
  }

  {
    SwissTable<std::string> table;

    table.insert("hello");
    table.insert("world");
    table.insert("test");

    assert(table.find("hello") != table.end());
    assert(table.find("world") != table.end());
    assert(table.find("test") != table.end());
    assert(table.find("not_found") == table.end());

    std::cout << "test_strings_basic passed\n";
  }

  {
    SwissTable<std::string> table;

    table.insert("a");
    table.insert("b");
    table.insert("c");

    std::vector<std::string> seen;

    for (auto it = table.begin(); it != table.end(); ++it) {
      seen.push_back(*it);
    }

    std::sort(seen.begin(), seen.end());

    assert(seen == std::vector<std::string>({"a", "b", "c"}));

    std::cout << "test_strings_iteration passed\n";
  }

  {
    SwissTable<std::string> table;

    table.insert("one");
    table.insert("two");
    table.insert("three");

    auto it = table.find("two");
    assert(it != table.end());

    table.erase(it);

    assert(table.find("two") == table.end());
    assert(table.find("one") != table.end());
    assert(table.find("three") != table.end());

    std::cout << "test_strings_erase passed\n";
  }

  {
    SwissTable<std::string> table;

    table.insert("abc");
    table.erase(table.find("abc"));

    assert(table.find("abc") == table.end());

    table.insert("abc");

    assert(table.find("abc") != table.end());

    std::cout << "test_strings_reinsert passed\n";
  }

  return 0;
}
