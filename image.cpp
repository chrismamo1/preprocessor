#include<fstream>
#include<iostream>
#include<stdlib.h>
#include<string>
#include<tgmath.h>
#include<thread>
#include<unistd.h>
#include<utility>
#include<vector>

#include "color.hpp"
#include "ColorRgb.hpp"
#include "image.hpp"

using namespace std;

image::image(int w, int h): width(w), height(h), data(w * h) {
}

image::image(unsigned char *buf, int w, int h): width(w), height(h), data(w * h) {
  for (int i = 0; i < w * h; i++) {
    color<RgbDoubles_t> x;
    x.set_channel(RED, double(buf[i * 3]));
    x.set_channel(GREEN, double(buf[i * 3 + 1]));
    x.set_channel(BLUE, double(buf[i * 3 + 2]));
    this->data[i] = x;
  }
  this->brightestPixel = this->data[0];
  this->darkestPixel = this->data[0];
  this->forceUpdate();
}

color<RgbDoubles_t>* image::getPixel(int left, int top) {
  /*if (left < 0) {
    if (top < 0) {
      return getPixel(0, 0);
    }
    if (top >= this->height) {
      return getPixel(0, this->height - 1);
    }
    return getPixel(0, top);
  }
  if (left >= this->width) {
    f (top < 0) {
      return getPixel(width - 1, 0);
    }
    if (top >= this->height) {
      return getPixel(width - 1, this->height - 1);
    }
    return getPixel(width - 1, top);
  }
  if (top < 0) {
    if (top >= this->height) {
      return getPixel(0, this->height - 1);
    }
    return getPixel(0, top);
  }
  if (left + top * this->width >= (ssize_t)this->data.size()) {
    return nullptr;
  }*/
  if (left < 0)
    left = 0;
  else if (left >= width)
    left = width - 1;
  if (top < 0)
    top = 0;
  else if (top >= height)
    top = height - 1;
  return &(this->data[left + top * this->width]);
}

void image::setPixel(int left, int top, color<RgbDoubles_t> x) {
  color<RgbDoubles_t> br = this->brightestPixel;
  if (x.get_brightness() > br.get_brightness()) {
    this->brightestPixel = x;
  }
  if (x.get_brightness() < darkestPixel.get_brightness()) {
    this->darkestPixel = x;
  }
  this->data[left + top * this->width] = x;
}

void image::forceUpdate() {
  this->brightestPixel = this->data[0];
  color<RgbDoubles_t> x = this->brightestPixel;
  vector<color<RgbDoubles_t>>::iterator it;
  for (it = this->data.begin(); it != this->data.end(); it++) {
    if (it->get_brightness() > x.get_brightness()) {
      x = *it;
    }
  }
  this->brightestPixel = x;
  this->darkestPixel = data[0];
  x = data[0];
  for (it = this->data.begin(); it != this->data.end(); it++) {
    if (it->get_brightness() < x.get_brightness()) {
      x = *it;
    }
  }
  this->darkestPixel = x;
  return;
}

vector<pair<color<RgbDoubles_t>, int>> image::getNeighbors(int left, int top, int distance) {
  /*  back of the envelope numbers:
        - number of neighbors n(k) = (2k + 1)^2 - 1
        this can be affected by the position, though. For a corner pixel it's
        n(k) = (k + 1)^2 - 1, for an edge pixel it's n(k) = 2((k+1)^2) - 1 - k

        I assume that we'll never have a kernel big enough to hit 2 different
        edges of the image, so we can use the following formula where h is the
        horizontal blockage and v is the vertical blockage (i.e. for a corner
        pixel these will both be equal to k)
        n(k) = ((2k + 1 - h) * (2k + 1 - v)) - 1
   */
  vector<pair<color<RgbDoubles_t>, int>> rv;
  int k = distance;
  int h = min(left, this->width - left);
  h = (h < k) ? h : 0;
  int v = min(top, this->height - top);
  v = (v < k) ? v : 0;
  rv.reserve((2 * k + 1 - h) * (2 * k + 1 - v) - 1);
  for (int k = 1; k <= distance; k++) {
    /// work from the 4 corners in a spiral
    for (int i = 0; i < 2 * k - 1; i++) {
      color<RgbDoubles_t> *tmp;
      tmp = this->getPixel(left - k, top + k - i); /// bottom left going up
      if (tmp) {
        rv.push_back(pair<color<RgbDoubles_t>, int>(*tmp, k));
      }
      tmp = this->getPixel(left - k + i, top - k); /// top left going right
      if (tmp) {
        rv.push_back(pair<color<RgbDoubles_t>, int>(*tmp, k));
      }
      tmp = this->getPixel(left + k, top - k + i); /// top right going down
      if (tmp) {
        rv.push_back(pair<color<RgbDoubles_t>, int>(*tmp, k));
      }
      tmp = this->getPixel(left + k - i, top + k); /// bottom right going left
      if (tmp) {
        rv.push_back(pair<color<RgbDoubles_t>, int>(*tmp, k));
      }
    }
  }
  return rv;
}

vector<color<RgbDoubles_t>> image::getImmediateNeighbors(int left, int top) {
  vector<color<RgbDoubles_t>> rv;
  if (left - 1 > 0) {
    rv.push_back(*this->getPixel(left - 1, top)); // left middle
    if (top - 1 > 0) {
      rv.push_back(*this->getPixel(left - 1, top - 1)); // left top
    }
    if (top + 1 < this->height) {
      rv.push_back(*this->getPixel(left - 1, top + 1)); // left bottom
    }
  }
  if (left + 1 < this->width) {
    rv.push_back(*this->getPixel(left + 1, top)); // right middle
    if (top - 1 > 0) {
      rv.push_back(*this->getPixel(left + 1, top - 1)); // right top
    }
    if (top + 1 < this->height) {
      rv.push_back(*this->getPixel(left + 1, top + 1)); // right bottom
    }
  }
  if (top - 1 > 0) {
    rv.push_back(*this->getPixel(left, top - 1)); // top middle
  }
  if (top + 1 < this->height) {
    rv.push_back(*this->getPixel(left, top + 1)); // bottom middle
  }
  return rv;
}

color<RgbDoubles_t> image::getBrightestPixel() {
  return this->brightestPixel;
}

color<RgbDoubles_t> image::getDarkestPixel() {
  return this->darkestPixel;
}

/*color<RgbDoubles_t> image::getComponentMaximums() {
  color<RgbDoubles_t> x = this->data[0];
  vector<color<RgbDoubles_t>>::iterator it;
  for (it = this->data.begin(); it != this->data.end(); it++) {
    if (it->red > x.red) {
      x.red = it->red;
    }
    if (it->green > x.green) {
      x.green = it->green;
    }
    if (it->blue > x.blue) {
      x.blue = it->blue;
    }
  }
  return x;
}*/

void image::runKernel(double kernel[], size_t k, bool normalize) {
  if (normalize) {
    /// normalize the kernel
    double acc = 0;
    for (size_t r = 0; r < k; r++) {
      for (size_t c = 0; c < k; c++) {
        acc += kernel[r * k + c];
      }
    }
    for (size_t r = 0; r < k; r++) {
      for (size_t c = 0; c < k; c++) {
        kernel[r * k + c] /= acc;
      }
    }
  }
  runKernel(kernel, k);
}

void image::runKernel(double kernel[], size_t k) {
  //int lines = 1 + ((width * height) / (100 * k * k)); // there isn't much to this, just make it a reasonable value
  int lines = 150;
  cerr.flush();
  this->mapPixels(
    [kernel, k](color<RgbDoubles_t> q, int l, int t, image *img) {
      color<RgbDoubles_t> acc(0.0);
      for (ssize_t kr = 0; kr < (ssize_t)k; kr++) {
        for (ssize_t kc = 0; kc < (ssize_t)k; kc++) {
          double kv = kernel[kr * k + kc];
          color<RgbDoubles_t> nv(*img->getPixel(l - (k / 2) + kc, t - (k / 2) + kr));
          acc = acc + nv.multiply(kv);
        }
      }
      return acc;
    },
    lines
  );
}

/*void image::runGaussianFilter(int k) {
  double *kernel = (double*)malloc(k * k * sizeof(double));
  double a = 1, b = double(k), c = double(k) / 2.0;
  int row = 0;
  for (int col = 0; col < k; col++) {
    double x = double(col * 2);
  }
  for (int row = 0; row < k; row++) {
    for (int col = 0; col < k; col++) {
      double x = 
      double val = exp(-pow(x - b, 2.0) / (2 * pow(c, 2.0)));
      kernel[row * k + col] = 
    }
  }
}*/

void image::normalize() {
  vector<color<RgbDoubles_t>>::iterator it;
  this->forceUpdate();
  color<RgbDoubles_t> dark = this->getDarkestPixel();
  for (it = this->data.begin(); it != this->data.end(); it++) {
    *it = *it - color<RgbDoubles_t>(dark.get_darkest_value());
  }
  this->forceUpdate();
  color<RgbDoubles_t> brightest = this->getBrightestPixel();
  float ratio_r = 255.0 / brightest.get_channel(RED);
  float ratio_g = 255.0 / brightest.get_channel(GREEN);
  float ratio_b = 255.0 / brightest.get_channel(BLUE);
  for (it = this->data.begin(); it != this->data.end(); it++) {
    it->set_channel(RED, it->get_channel(RED) * ratio_r);
    it->set_channel(GREEN, it->get_channel(GREEN) * ratio_g);
    it->set_channel(BLUE, it->get_channel(BLUE) * ratio_b);
  }
  return;
}

void image::polarize() {
  this->mapPixels([](color<RgbDoubles_t> q, int l, int t, image *img) {
      color<RgbDoubles_t> p = q;
      color<RgbDoubles_t> br = img->getBrightestPixel();
      double maxComp = br.get_channel(br.brightest_channel());
      double factor = 255.0 / sqrt(maxComp);
      p.map_channels([factor](double x) {
        return factor * sqrt(x);
      });
      double brightness = p.get_brightness();
      if (brightness > 128 * 3) {
        p.set_all(255);
      } else {
        p.set_all(0);
      }
      return p;
    },
    50);
  return;
}

char* image::to_buffer() {
  this->forceUpdate();
  char* buffer = (char*)malloc(3 * this->width * this->height);
  for (size_t i = 0; i < this->data.size(); i++) {
    color<RgbDoubles_t> x = this->data[i];
    double r,g,b;
    r = x.get_channel(RED);
    g = x.get_channel(GREEN);
    b = x.get_channel(BLUE);
    buffer[i * 3] = r;
    buffer[i * 3 + 1] = g;
    buffer[i * 3 + 2] = b;
  }
  return buffer;
}

void image::write(string name) {
  ofstream fOut(name);
  if (!fOut.is_open()) {
    cerr << "ERROR: could not open file << " name << ".\n";
  }
  char *buffer = this->to_buffer();
  fOut << "P6\n" << this->width << " " << this->height << "\n255\n";
  fOut.write(buffer, 3 * this->width * this->height);
  fOut.close();
  free(buffer);
  return;
}