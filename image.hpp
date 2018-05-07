#include<string>
#include<thread>
#include<utility>
#include<vector>

#ifndef _IMAGE_HPP
#define _IMAGE_HPP

#include "color.hpp"
#include "ColorRgb.hpp"

using namespace std;

class image {
private:
  color<RgbDoubles_t> brightestPixel;
  color<RgbDoubles_t> darkestPixel;
public:
  int width;
  int height;

  image(int w, int h);
  image(unsigned char *buffer, int width, int height);
  vector<color<RgbDoubles_t>> data;

  color<RgbDoubles_t>* getPixel(int left, int top);
  void setPixel(int left, int top, color<RgbDoubles_t> x);

  void forceUpdate();

  vector<pair<int, int>> getCircle(int left, int top, int radius);
  vector<pair<color<RgbDoubles_t>, int>> getNeighbors(int left, int top, int distance);
  vector<color<RgbDoubles_t>> getImmediateNeighbors(int left, int top);
  color<RgbDoubles_t> getBrightestPixel();
  color<RgbDoubles_t> getDarkestPixel();
  //color<RgbDoubles_t> getComponentMaximums();

  template<typename Func>
  void mapPixels(Func f, int rowsPerThread) {
    int w = this->width, h = this->height;
    vector<thread*> threads;
    vector<color<RgbDoubles_t>> tmpB(this->data);
    image *self = this;
    for (int t = 0; t < h; t += rowsPerThread) {
      if (threads.size() >= thread::hardware_concurrency()) {
        vector<thread*>::iterator it;
        for (it = threads.begin(); it != threads.end(); it++) {
          (*it)->join();
        }
        for (it = threads.begin(); it != threads.end(); ) {
          threads.erase(it);
          it = threads.begin();
        }
      }
      thread *thr = new thread([rowsPerThread, t, h, w, f, &tmpB, self]() {
        for (int i = 0; i < rowsPerThread && t + i < h; i++) {
          for (int l = 0; l < w; l++) {
            //cerr << "l: " << l << ", t: " << t << endl;
            color<RgbDoubles_t> in = *self->getPixel(l, t + i);
            color<RgbDoubles_t> p = f(in, l, t + i, self);
            tmpB[l + (t + i) * w] = p;
          }
        }
      });
      threads.push_back(thr);
    }
    vector<thread*>::iterator it;
    for (it = threads.begin(); it != threads.end(); it++) {
      (*it)->join();
    }
    for (size_t i = 0; i < this->data.size(); i++) {
      this->data[i] = tmpB[i];
    }
    this->forceUpdate();
    return;
  };

  template<typename T>
  void runKernel(vector<T> kernel) {
    int k = int(sqrt(kernel.size() + 1.0f));
    T* arr = kernel.data();
    runKernel(arr, k, false);
  }
  void runKernel(double[], size_t);
  void runKernel(double[], size_t, bool);

  void normalize();

  /** scale color channels reverse-quadratically until everything is exactly within the range
   * [0, 255], then do f(x) = if x >= 128 then 255 else 0
   */
  void polarize();

  void harris();

  char* to_buffer();
  void write(string fName);
};

#endif