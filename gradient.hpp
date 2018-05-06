#ifndef __GRADIENT_HPP
#define __GRADIENT_HPP

#include<vector>
#include "color.hpp"
#include "ColorRgb.hpp"
#include "filter.hpp"
#include "image.hpp"

using namespace std;

class Gradient {
public:
  int left, top;
  double intensity;
  double angle;
  Gradient();
  Gradient(double i);
  Gradient(int l, int t, double i, double theta);

  double get_brightness();
  double normalize();

  Gradient abs(double);
  Gradient abs();

  Gradient operator+(Gradient g);
  Gradient operator-(Gradient g);
  Gradient operator/(Gradient g);
  Gradient operator/(double g);

  bool operator==(Gradient g);
  bool operator>(Gradient g);
  bool operator<(Gradient g);
};

class GradientGrid {
public:
  vector<Gradient> repr;
  void nonMaximumSuppression();
  int width, height;
  GradientGrid(int w, int h);
  bool isIn(int l, int t);
  void setAt(int l, int t, Gradient);
  Gradient getAt(int l, int t);
};

GradientGrid makeGradientGrid(image);

image gradientGridToImage(GradientGrid gg);

void gradientGridToSvg(GradientGrid gg, string name);

#endif