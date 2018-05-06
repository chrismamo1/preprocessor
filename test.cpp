#include<iostream>

#include "color.hpp"

using namespace std;

int main() {
  color<double> blah;
  blah.set_channel(RED, 0);
  cerr << "Red: " << blah.get_channel(RED) << endl;
  return 0;
}