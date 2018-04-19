#ifndef _COLOR_HPP
#define _COLOR_HPP

#include<iostream>
#include<map>
#include<tgmath.h>
#include<vector>

using namespace std;

#define MAKE_STRING(x) #x

enum Color_channel_e {
  RED,
  GREEN,
  BLUE,
  ALPHA
};

template <typename N>
class color {
public:
  map<Color_channel_e, N> value;
  vector<Color_channel_e> channels;

  color(): value() {};
  color(map<Color_channel_e, N> init);
  color(const color<N>& x): value(x.value), channels(x.channels) {
  };
  N get_channel(Color_channel_e chan) {
    return this->value[chan];
  }
  void set_channel(Color_channel_e chan, N x) {
    if (!this->has_channel(chan)) {
      this->channels.push_back(chan);
    }
    this->value[chan] = x;
  };
  bool has_channel(Color_channel_e chan) {
    for (size_t i = 0; i < this->channels.size(); i++)
      if (chan == this->channels[i])
        return true;
    return false;
  };
  N get_brightness();
  Color_channel_e brightest_channel();
  //Color_channel_e darkest_channel();
  N get_average() {
    typename map<Color_channel_e, N>::iterator it;
    N acc = 0;
    for (it = this->value.begin(); it != this->value.end(); it++) {
      acc += it->second;
    }
    return acc / this->value.size();
  };

  N get_average(double x) {
    typename map<Color_channel_e, N>::iterator it;
    N acc = 0;
    for (it = this->value.begin(); it != this->value.end(); it++) {
      acc += pow(it->second, x);
    }
    return acc / this->value.size();
  };

  template<typename Func>
  void map_channels(Func f/*N (*f)(N)*/) {
    this->set_channel(RED, f(this->value[RED]));
    this->set_channel(GREEN, f(this->value[GREEN]));
    this->set_channel(BLUE, f(this->value[BLUE]));
    return;
  };

  void set_all(N);

  color<N> multiply(double x) {
    color<N> cp;
    cp.channels = this->channels;
    for (size_t i = 0; i < cp.channels.size(); i++) {
      cp.value[channels[i]] = value[channels[i]] * x;
    }
    return cp;
  };

  /*color<N> operator*(const color<N> &x) {
    auto ochans = x.channels;
    for (size_t i = 0; i < ochans.size(); i++) {
      if (!has_channel(ochans[i])) {
        set_channel(ochans[i], x.get_channel(ochans[i]));
      } else {
        set_channel(ochans[i], x.get_channel(ochans[i]) * value[ochans[i]]);
      }
    }
  };*/

  color<N> operator+(color<N> x) {
    auto ochans = x.channels;
    color<double> cp;
    cp.channels = channels;
    for (size_t i = 0; i < ochans.size(); i++) {
      if (!has_channel(ochans[i])) {
        cp.set_channel(ochans[i], x.get_channel(ochans[i]));
      } else {
        cp.set_channel(ochans[i], x.get_channel(ochans[i]) + value[ochans[i]]);
      }
    }
    return cp;
  };

  int compare(color<N> y) {
    for (int i = 0; i < int(channels.size()); i++) {
      if (value[channels[i]] != y.get_channel(channels[i])) {
        return -1;
      }
    }
    return 0;
  }
};

template <class N>
color<N>::color(map<Color_channel_e, N> init) {
  this->value = init;
}

template <typename N>
N color<N>::get_brightness() {
  typename map<Color_channel_e, N>::iterator it;
  N acc = 0;
  acc += this->get_channel(RED);
  acc += this->get_channel(GREEN);
  acc += this->get_channel(BLUE);
  return acc;
}

template <typename N>
Color_channel_e color<N>::brightest_channel() {
  int it = 0;
  Color_channel_e rv = channels[it];
  N brightest = value[channels[it]];
  for (size_t i = 1; i < this->channels.size(); i++) {
    if (this->value[channels[i]] > brightest) {
      brightest = value[channels[i]];
      rv = channels[i];
    }
  }
  return rv;
}

/*template <typename N>
Color_channel_e color<N>::darkest_channel() {
  int it = 0;
  Color_channel_e rv = channels[it];
  N darkest = value[channels[it]];
  for (size_t i = 1; i < this->channels.size(); i++) {
    if (this->value[channels[i]] < darkest) {
      darkest = value[channels[i]];
      rv = channels[i];
    }
  }
  return rv;
}*/

template <typename N>
void color<N>::set_all(N val) {
  for (size_t i = 0; i < channels.size(); i++) {
    value[channels[i]] = val;
  }
}
#endif