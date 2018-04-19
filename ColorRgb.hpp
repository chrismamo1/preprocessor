#ifndef _COLOR_RGB_HPP
#define _COLOR_RGB_HPP

#include<ctgmath>
#include<tgmath.h>

#include "color.hpp"

struct RgbDoubles_t {
  double red, green, blue;
};

template <>
class color<RgbDoubles_t> {
public:
  RgbDoubles_t value;
  color() {
    value.red = 0.0;
    value.green = 0.0;
    value.blue = 0.0;
  }
  color(map<Color_channel_e, double> init) {
    value.red = init[RED];
    value.green = init[GREEN];
    value.blue = init[BLUE];
  }
  color(color<RgbDoubles_t>* init) {
    value.red = init->value.red;
    value.green = init->value.green;
    value.blue = init->value.blue;
  }
  color(const color<RgbDoubles_t>& init) {
    value.red = init.value.red;
    value.green = init.value.green;
    value.blue = init.value.blue;
  }
  color(double r, double g, double b) {
    value.red = r;
    value.green = g;
    value.blue = b;
  }
  color(double x) {
    value.red = x;
    value.green = x;
    value.blue = x;
  }
  double get_channel(Color_channel_e chan) {
    switch (chan) {
      case RED: {
        return value.red;
      }
      case GREEN: {
        return value.green;
      }
      case BLUE: {
        return value.blue;
      }
      case ALPHA:
      default: {
        return nan("");
      }
    }
  }
  void set_channel(Color_channel_e chan, double x) {
    switch (chan) {
      case RED: {
        value.red = x;
        break;
      }
      case GREEN: {
        value.green = x;
        break;
      }
      case BLUE: {
        value.blue = x;
        break;
      }
      case ALPHA: {
        break;
      }
    }
  }
  bool has_channel(Color_channel_e chan) {
    return (chan == RED || chan == GREEN || chan == BLUE);
  }
  double get_brightness() {
    return value.red + value.green + value.blue;
  }
  Color_channel_e brightest_channel() {
    if (value.red > value.green && value.red > value.blue) {
      return RED;
    }
    if (value.green > value.red && value.green > value.blue) {
      return GREEN;
    }
    if (value.blue > value.green && value.blue > value.red) {
      return BLUE;
    }
  }
  double get_average() {
    return (value.red + value.green + value.blue) / (double)3.0;
  }
  double get_average(double ex) {
    return pow(pow(value.red, ex) + pow(value.green, ex) + pow(value.blue, ex), 1.0 / ex) / (double)3.0;
  }
  template<typename Func>
  void map_channels(Func f) {
    value.red = f(value.red);
    value.green = f(value.green);
    value.blue = f(value.blue);
  }
  void set_all(double x) {
    value.red = value.green = value.blue = x;
  }
  color<RgbDoubles_t> multiply(double x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red *= x;
    rv.value.green *= x;
    rv.value.blue *= x;
    return rv;
  }

  template<typename T>
  color<RgbDoubles_t> operator*(T x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red *= x;
    rv.value.green *= x;
    rv.value.blue *= x;
    return rv;
  }

  color<RgbDoubles_t> raise(double x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red = pow(rv.value.red, x);
    rv.value.green = pow(rv.value.green, x);
    rv.value.blue = pow(rv.value.blue, x);
    return rv;
  }

  color<RgbDoubles_t> operator/(double x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red /= x;
    rv.value.green /= x;
    rv.value.blue /= x;
    return rv;
  }

  template<typename N>
  color<RgbDoubles_t> operator+(color<N> x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red += x.get_channel(RED);
    rv.value.green += x.get_channel(GREEN);
    rv.value.blue += x.get_channel(BLUE);
    return rv;
  }

  template<typename N>
  color<RgbDoubles_t> operator-(color<N> x) {
    color<RgbDoubles_t> rv(this);
    rv.value.red -= x.get_channel(RED);
    rv.value.green -= x.get_channel(GREEN);
    rv.value.blue -= x.get_channel(BLUE);
    return rv;
  }

  template<typename N>
  bool operator>(color<N> x) {
    return get_brightness() > x.get_brightness();
  }

  template<typename N>
  bool operator<(color<N> x) {
    return get_brightness() < x.get_brightness();
  }

  template<typename N>
  bool operator!=(color<N> x) {
    return !(value.red == x.get_channel(RED) && value.green == x.get_channel(GREEN) && value.blue == x.get_channel(BLUE));
  }

  template<typename N>
  bool operator==(color<N> x) {
    return (value.red == x.get_channel(RED) && value.green == x.get_channel(GREEN) && value.blue == x.get_channel(BLUE));
  }

  template<typename N>
  bool operator<=(color<N> x) {
    return (get_brightness() < x.get_brightness()) || (value.red == x.get_channel(RED) && value.green == x.get_channel(GREEN) && value.blue == x.get_channel(BLUE));
  }

  template<typename N>
  void operator=(color<N> x) {
    value.red = x.get_channel(RED);
    value.green = x.get_channel(GREEN);
    value.blue = x.get_channel(BLUE);
    return;
  }

  template<typename N>
  void operator=(N x) {
    value.red = x;
    value.green = x;
    value.blue = x;
    return;
  }

  color<RgbDoubles_t> abs() {
    color<RgbDoubles_t> rv(this);
    rv.value.red = fabs(value.red);
    rv.value.green = fabs(value.green);
    rv.value.blue = fabs(value.blue);
    return rv;
  }

  double normalize() {
    return sqrt(pow(value.red, 2.0) + pow(value.green, 2.0) + pow(value.blue, 2.0));
  }

  /*string to_string() {
    return "(" + value.red + ", " + value.green + ", " + value.blue + ")";
  }*/

  double get_darkest_value() {
    return min(value.red, min(value.green, value.blue));
  }

  string to_string() {
    char* buffer = (char*)malloc(4096);
    sprintf(buffer, "(%lf, %lf, %lf)", value.red, value.green, value.blue);
    string rv(buffer);
    free(buffer);
    return rv;
  }

  template<typename N>
  int compare(color<N> y) {
    if (value.red != y.get_channel(RED)) {
      return 1;
    }
    if (value.green != y.get_channel(GREEN)) {
      return 1;
    }
    if (value.blue != y.get_channel(BLUE)) {
      return 1;
    }
    return 0;
  }
};

#endif