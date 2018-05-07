#ifndef _FILTER_HPP
#define _FILTER_HPP

#include<iostream>
#include<tgmath.h>
#include<vector>

using namespace std;

namespace filter {
  template<typename T>
  vector<T> gaussian(int k, T sigma) {
    vector<T> rv;
    rv.reserve(k * k);
    T factor = T(1.0) / (T(2.0) * T(3.14159265358979) * sigma * sigma);
    int k2 = (k - 1) / 2;
    for (int r = 0; r < k; r++) {
      for (int c = 0; c < k; c++) {
        T x;
        int i = r + 1;
        int j = c + 1;
        x = factor * exp(T(-((i - (k2 + 1)) * (i - (k2 + 1)) + (j - (k2 + 1)) * (j - (k2 + 1))) / (2 * sigma * sigma)));
        rv.push_back(x);
      }
    }
    return rv;
  }

  template<typename T>
  vector<T> gaussian(int k) {
    return gaussian(k, T(0.8493));
  }

  /// returns a pair of the (horizontal, vertical) differentials
  template<typename T>
  pair<vector<T>, vector<T>> prewitt(int k) {
    vector<T> hRv, vRv;
    hRv.reserve(k * k);
    vRv.reserve(k * k);
    for (int r = 0; r < k; r++) {
      for (int c = 0; c < k; c++) {
        if (c == k / 2) {
          hRv.push_back(0);
        } else if (c < k / 2) {
          hRv.push_back(-1);
        } else {
          hRv.push_back(1);
        }
        if (r == k / 2) {
          vRv.push_back(0);
        } else if (r < k / 2) {
          vRv.push_back(-1);
        } else {
          vRv.push_back(1);
        }
      }
    }
    return pair<vector<T>, vector<T>>(hRv, vRv);
  }

  template<typename T>
  vector<T> normalize(vector<T> x) {
    vector<T> rv(x);
    int k = sqrt(x.size() + 1);
    T sum(0);
    for (int r = 0; r < k; r++) {
      for (int c = 0; c < k; c++) {
        sum = sum + rv[r * k + c];
      }
    }
    for (int r = 0; r < k; r++) {
      for (int c = 0; c < k; c++) {
        rv[r * k + c] = rv[r * k + c] / sum;
      }
    }
    return rv;
  }

  template<typename T>
  void print(vector<T> x) {
    int k = sqrt(x.size());
    for (int r = 0; r < k; r++) {
      for (int c = 0; c < k; c++) {
        cerr << x[r * k + c] << " ";
        cerr.flush();
      }
      cerr << endl;
    }
  }

  template<typename T>
  T diff(vector<T> x, vector<T> y) {
    int k = sqrt(x.size() + 1);
    T acc(0);
    for (size_t i = 0; i < x.size(); i++) {
      acc = acc + abs(x[i] - y[i]);
    }
    return acc;
  }
};

#endif