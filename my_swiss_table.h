#pragma once
#include <emmintrin.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <new>
#include <ostream>
#include <type_traits>

enum class ctrl_t : int8_t {
  kEmpty = -128,   // 0b10000000
  kDeleted = -2,   // 0b11111110
  kSentinel = -1,  // 0b11111111
                   // 0b0... > 0 - Full
};

// SplitMix64 Written in 2015 by Sebastiano Vigna (vigna@acm.org)
// To the extent possible under law, the author has dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide.
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.
uint64_t mix(uint64_t x) {
  uint64_t z = (x += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

struct Group {
  __m128i ctrl;

  explicit Group(const ctrl_t* ptr) {
    ctrl = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
  }

  // returns a bitmask of all slots with that h2 hash
  uint16_t Match(int8_t hash) const {
    __m128i h = _mm_set1_epi8(hash);
    __m128i eq = _mm_cmpeq_epi8(ctrl, h);
    return _mm_movemask_epi8(eq);
  }

  // returns a mask of empty slots
  uint16_t MaskEmpty() {
    __m128i match = _mm_set1_epi8(static_cast<char>(ctrl_t::kEmpty));
    return _mm_movemask_epi8(_mm_cmpeq_epi8(ctrl, match));
  }

  uint16_t MatchNotFullSlots() {
    return _mm_movemask_epi8(ctrl);
  }
};

template <typename T>
class SwissTable {
private:
  size_t size;
  size_t groups;
  void* buffer;  // backing array
  void* slots;   // reference to the slots array

  size_t align_up(size_t n, size_t alignment) {
    // n + (a-1) mod a
    return (n + alignment - 1) & ~(alignment - 1);
  }

  size_t H1(size_t hash) {
    return hash >> 7;
  }
  size_t H2(size_t hash) {
    return hash & 0x7F;
  }

  struct iterator {
    T* ptr;
    T* end_ptr;
    T* slots;
    ctrl_t* ctrl;

    T& operator*() {
      return *ptr;
    }

    iterator& operator++() {
      ++ptr;
      size_t index = ptr - slots;

      while (ptr != end_ptr && (int)ctrl[index] < 0) {
        ++ptr;
        ++index;
      }

      return *this;
    }

    bool operator==(const iterator& other) const {
      return ptr == other.ptr;
    }

    bool operator!=(const iterator& other) const {
      return ptr != other.ptr;
    }
  };

public:
  SwissTable() {
    size = 0;
    groups = 2;  // 2 groups on the start so AND op. in find() will reference either to 0 or 1 group
    // groups is always a power of 2
    size_t total_size = align_up(capacity() * sizeof(ctrl_t), alignof(T)) + capacity() * sizeof(T);
    buffer = operator new(total_size);

    slots = static_cast<char*>(buffer) + align_up(capacity() * sizeof(ctrl_t), alignof(T));

    std::memset(buffer, static_cast<int8_t>(ctrl_t::kEmpty), capacity());
  }

  ~SwissTable() {
    clear();
    operator delete(buffer);
  }

  void clear() {
    auto ctrl_ptr = ctrl_();
    auto slot_ptr = slot_();

    for (int i = 0; i < capacity(); ++i) {
      if ((int8_t)ctrl_ptr[i] >= 0) {
        slot_ptr[i].~T();
      }
    }

    std::memset(ctrl_ptr, static_cast<int8_t>(ctrl_t::kEmpty), capacity());
    size = 0;
  }

  size_t capacity() const {
    return groups * 16;
  }

  ctrl_t* ctrl_() {
    return static_cast<ctrl_t*>(buffer);
  }

  T* slot_() {
    return static_cast<T*>(slots);
  }

  size_t hash_element(const T& value) {
    return mix(std::hash<T>{}(value));
  }

  iterator find(T value) {
    size_t hash = hash_element(value);
    size_t h1 = H1(hash);
    size_t h2 = H2(hash);
    size_t group = h1 & (groups - 1);

    while (true) {
      Group g{ctrl_() + group * 16};
      int i;
      uint16_t mask = g.Match(h2);
      // std::cout << "Got mask : " << mask << std::endl;
      while (mask) {
        i = __builtin_ctz(mask);
        mask &= (mask - 1);

        if (value == slot_()[group * 16 + i])
          return iterator{slot_() + group * 16 + i, slot_() + capacity(), slot_(), ctrl_()};
      }
      if (g.MaskEmpty() != 0) {
        // std::cout << "Not a full group" << std::endl;
        return end();
      }
      group = (group + 1) % groups;
    }
  }

  iterator begin() {
    if ((int8_t)ctrl_()[0] < 0) {
      return ++iterator{slot_(), slot_() + capacity(), slot_(), ctrl_()};
    }

    return iterator{slot_(), slot_() + capacity(), slot_(), ctrl_()};
  }

  iterator end() {
    return iterator{slot_() + capacity(), slot_() + capacity(), slot_(), ctrl_()};
  }

  iterator insert(T value) {
    iterator is_already_in = find(value);
    if (is_already_in != end()) {
      return is_already_in;
    }
    size_t hash = hash_element(value);
    size_t h1 = H1(hash);
    size_t h2 = H2(hash);
    if (size > capacity() * 0.85) {
      grow();
      return insert(value);
    }
    // find the group and try to insert
    size_t group = h1 & (groups - 1);
    uint16_t mask;
    while (true) {
      Group g{ctrl_() + group * 16};
      mask = g.MatchNotFullSlots();
      // if group is not full
      if (mask != 0) {
        break;
      }
      group = (group + 1) % groups;
    }

    int i = __builtin_ctz(mask);
    // slot_()[group * 16 + i] = value;
    new (slot_() + group * 16 + i) T(value);
    static_cast<int8_t*>(buffer)[group * 16 + i] = (int8_t)h2;

    size++;

    return iterator{slot_() + group * 16 + i, slot_() + capacity(), slot_(), ctrl_()};
  }

  void erase(iterator it) {
    --size;

    size_t index = it.ptr - slot_();
    size_t group = index / 16;
    size_t offset = index % 16;

    Group g{ctrl_() + group * 16};
    ctrl_()[group * 16 + offset] = g.MaskEmpty() ? ctrl_t::kEmpty : ctrl_t::kDeleted;
    it.ptr->~T();
  }

  void erase(T value) {
    iterator it = find(value);
    if (it != end()) {
      erase(it);
    }
  }

  void grow() {
    // double the size of the backing array, rehash everything and store again.

    auto old_buff = buffer;
    auto old_ctrl = static_cast<ctrl_t*>(buffer);
    auto old_slots = slots;
    auto old_capacity = capacity();

    auto* old_slots_ptr = reinterpret_cast<T*>(old_slots);
    auto* old_ctrl_ptr = reinterpret_cast<ctrl_t*>(old_ctrl);

    groups *= 2;

    size_t slot_offset = align_up(capacity() * sizeof(ctrl_t), alignof(T));
    size_t new_total_size = slot_offset + capacity() * sizeof(T);
    buffer = operator new(new_total_size);
    slots = static_cast<char*>(buffer) + slot_offset;
    std::memset(buffer, static_cast<int8_t>(ctrl_t::kEmpty), capacity());
    size = 0;

    for (size_t i = 0; i < old_capacity; ++i) {
      if ((int8_t)old_ctrl_ptr[i] >= 0) {
        insert(std::move(old_slots_ptr[i]));
        old_slots_ptr[i].~T();
      }
    }

    operator delete(old_buff);
  }

  void print() {
    std::cout << "Metadata : " << std::endl;
    for (auto i = 0; i < capacity(); ++i) {
      std::cout << (int)*(uint8_t*)((uint8_t*)buffer + i * sizeof(ctrl_t)) << ' ';
    }
    std::cout << std::endl;

    std::cout << "Slots : " << std::endl;
    for (auto i = 0; i < capacity(); ++i) {
      std::cout << *((T*)slots + i) << ' ';
    }
    std::cout << std::endl;
  }
};
