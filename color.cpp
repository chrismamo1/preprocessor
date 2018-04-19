#include<map>

#include "color.hpp"

template <typename N>
color<N>::color(): value(0) {
  return;
}

/*template <class N>
color<N>::color(map<Color_channel_e, N> init) {
  this->value = init;
}*/

template <typename N>
N color<N>::get_channel(Color_channel_e chan) {
  return this->value[chan];
}

template <typename N>
void color<N>::set_channel(Color_channel_e chan, N x) {
  this->value[chan] = x;
}

template <typename N>
N color<N>::get_brightness() {
  typename map<Color_channel_e, N>::iterator it;
  N acc = 0;
  for (it = this->value.begin(); it != this->value.end(); it++) {
    pair<Color_channel_e, N> x = *it;
    acc = acc + x.first;
  }
  return acc;
}

template <typename N>
void color<N>::map_channels(N (*f)(N, color<N>*)) {
  typename map<Color_channel_e, N>::iterator it;
  for (it = this->value.begin(); it != this->value.end(); it++) {
    this->set_channel(it->first, f(it->second, &this));
  }
  return;
}

template <typename N>
Color_channel_e color<N>::brightest_channel() {
  Color_channel_e rv = this->value.begin().first;
  N brightest = this->value.begin().second;
  typename map<Color_channel_e, N>::iterator it;
  for (it = this->value.begin(); it != this->value.end(); it++) {
    if (it->second > brightest) {
      brightest = it->second;
      rv = it->first;
    }
  }
  return rv;
}

template <typename N>
void color<N>::set_all(N val) {
  typename map<Color_channel_e, N>::iterator it;
  for (it = this->value.begin(); it != this->value.end(); it++) {
    it->second = val;
  }
}