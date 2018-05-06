#include <iostream>
#include <tgmath.h>

#ifndef __POINT_HPP
#define __POINT_HPP

template<class T>
struct Point {
  T left;
  T top;
  Point() {
    left = T(0);
    top = T(0);
  };
  Point(T l, T t) {
    left = l;
    top = t;
  };
  bool operator==(Point p);
};

template<class T>
bool Point<T>::operator==(Point<T> p) {
  std::cerr << "ERROR: shouldn't be using the generic comparison function\n";
  return (T(p.top) == top) && (T(p.left) == left);
}

template<> inline
bool Point<double>::operator==(Point p) {
  double e = 0.000001;
  return fabs(left - p.left) < e && fabs(top - p.top) < e;
};

#endif