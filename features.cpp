#include "features.hpp"
#include "filter.hpp"

image corners(image img) {
  auto img2(img);
  return img2;
}

image harris(image img) {
  /// do Harris feature detection
  auto diffKerns = filter::prewitt<double>(3);
  auto k1 = diffKerns.first;
  auto k2 = diffKerns.second;
  auto g_x(img), g_y(img);
  g_x.runKernel(k1);
  g_y.runKernel(k2);
  auto img2(img);
  img2.mapPixels([&g_x, &g_y](color<RgbDoubles_t> input, int l, int t, image *img) {
      color<RgbDoubles_t> xi = g_x.getPixel(l, t);
      color<RgbDoubles_t> yi = g_y.getPixel(l, t);
      auto cmp1 = xi.raise(2);
      auto cmp2 = yi.raise(2);
      color<RgbDoubles_t> rv = (cmp1 + cmp2).sqrt();
      return rv;
    },
    10);
  return img2;
}