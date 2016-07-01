#pragma once
#include <cstddef>  // for size_t
#include <utility>

template <class Iter>
class IterWrap {
 private:
  Iter s, e;

 public:
  IterWrap(Iter s, Iter e) : s(s), e(e) {}
  Iter begin() const { return s; }
  Iter end() const { return e; }
};
template <class Iter>
inline IterWrap<Iter> as_range(Iter s, Iter e) {
  return {s, e};
}

template <class Iter>
inline IterWrap<Iter> as_range(std::pair<Iter, Iter> pair) {
  return {pair.first, pair.second};
}

template <class T, size_t incr>
class RangeIterator {
 private:
  T t;

 public:
  RangeIterator(T t) : t(t) {}
  T operator*() const { return t; }
  RangeIterator<T, incr> &operator++() {
    t += incr;
    return *this;
  }
  bool operator==(const RangeIterator<T, incr> &other) const {
    return t == other.t;
  }
  bool operator!=(const RangeIterator<T, incr> &other) const {
    return t != other.t;
  }
};

template <class T, size_t incr = 1>
using Range = IterWrap<RangeIterator<T, incr>>;

inline Range<size_t, 1> range(size_t i) { return Range<size_t, 1>(0, i); }
