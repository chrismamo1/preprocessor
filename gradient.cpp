#include <fstream>

#include "gradient.hpp"
#include "KMeans.hpp"

Gradient::Gradient() {
  return;
}

Gradient::Gradient(double i) {
  intensity = i;
}

Gradient::Gradient(int l, int t, double i, double theta) {
  left = l;
  top = t;
  intensity = i;
  angle = theta;
}

double Gradient::get_brightness() {
  return intensity;
}

double Gradient::normalize() {
  return intensity;
}

Gradient Gradient::abs(double x) {
  return Gradient(left, top, fabs(intensity), angle);
}

Gradient Gradient::abs() {
  return Gradient(left, top, fabs(intensity), angle);
}

Gradient Gradient::operator+(Gradient g) {
  return Gradient(left, top, intensity + g.intensity, angle);
}

Gradient Gradient::operator-(Gradient g) {
  return Gradient(intensity - g.intensity);
}

Gradient Gradient::operator/(Gradient g) {
  return Gradient(left, top, intensity / g.intensity, angle);
}

Gradient Gradient::operator*(Gradient g) {
  return Gradient(left, top, intensity * g.intensity, angle);
}

Gradient Gradient::operator/(double g) {
  return Gradient(left, top, intensity / g, angle);
}

bool Gradient::operator==(Gradient g) {
  /// TODO: the fuck do i do about the angle/intensity here?
  return (left == g.left) && (top == g.top);
}

bool Gradient::operator>(Gradient g) {
  return intensity > g.intensity;
}

bool Gradient::operator<(Gradient g) {
  return intensity < g.intensity;
}

bool GradientGrid::isIn(int l, int t) {
  if (l >= 0) {
    if (l < width) {
      if (t >= 0) {
        if (t < height) {
          return true;
        }
      }
    }
  }
  return false;
}

int roundF(double x) {
  double y = abs(x);
  int fl = int(y);
  double y2 = y - double(fl);
  int sign = (x < 0) ? -1 : 1;
  if (y2 >= 0.5) {
    return (fl + 1) * sign;
  }
  return fl * sign;
}

void GradientGrid::nonMaximumSuppression() {
  for (int l = 0; l < width; l++) {
    for (int t = 0; t < height; t++) {
      auto centre = getAt(l, t);
      double i = centre.intensity;
      double angle2 = centre.angle - 3.14159265358979;
      int l2 = l + roundF(cos(angle2));
      int t2 = t + roundF(sin(angle2));
      if (isIn(l2, t2)) {
        auto n = getAt(l2, t2);
        /*if (centre - n == 0 || centre - n > 2 || n - centre > 2) {
          cerr << "FUCKED UP1\n";
          cerr << "angle2: " << angle2 << endl;
          cerr << "l: " << l << ", t: " << t << endl;
          cerr << "l': " << l2 << ", t': " << t2 << endl;
          cerr << "cos(angle2): " << cos(angle2) << ", sin(angle2): " << sin(angle2) << endl;
        }*/
        if (n.intensity >= i) {
          centre.intensity = 0;
          setAt(l, t, centre);
        }
      }
      l2 = l - roundF(cos(angle2));
      t2 = t - roundF(sin(angle2));
      if (isIn(l2, t2)) {
        auto n = getAt(l2, t2);
        /*if (centre - n == 0 || centre - n > 2 || n - centre > 2) {
          cerr << "FUCKED UP2\n";
        }*/
        if (n.intensity >= i) {
          centre.intensity = 0;
          setAt(l, t, centre);
        }
      }
    }
  }
}

void GradientGrid::setAt(int l, int t, Gradient g) {
  repr[l + t * width] = g;
}

Gradient GradientGrid::getAt(int l, int t) {
  return repr[l + t * width];
}

GradientGrid::GradientGrid(int w, int h): repr(w * h) {
  width = w;
  height = h;
}

GradientGrid makeGradientGrid(image img) {
  img.runKernel(filter::normalize(filter::gaussian<double>(3)));
  auto g_x(img), g_y(img);
  // Roberts Cross
  // based on Wikipedia description of the Roberts Cross
  /*
  double k1[] =
    { 1,  0
    , 0, -1 };
  double k2[] =
    {  0, 1
    , -1, 0 };
  g_x.runKernel(k1, 2, false);
  g_y.runKernel(k2, 2, false);
  */
  // Prewitt Operator (also from Wikipedia)
  double k1[] =
    { -1, 0, 1
    , -1, 0, 1
    , -1, 0, 1 };
  double k2[] =
    { -1, -1, -1
    ,  0,  0,  0
    ,  1,  1,  1 };
  g_x.runKernel(k1, 3, false);
  g_y.runKernel(k2, 3, false);
  auto gradient(img);
  gradient.mapPixels([&g_x, &g_y](color<RgbDoubles_t> input, int l, int t, image *img) {
      color<RgbDoubles_t> xi = g_x.getPixel(l, t);
      color<RgbDoubles_t> yi = g_y.getPixel(l, t);
      auto cmp1 = xi.raise(2);
      auto cmp2 = yi.raise(2);
      color<RgbDoubles_t> rv = (cmp1 + cmp2).sqrt();
      return rv;
    },
    10);
  auto brightest = gradient.getBrightestPixel();
  GradientGrid rv(img.width, img.height);
  for (int l = 0; l < img.width; l++) {
    for (int t = 0; t < img.height; t++) {
      double intensity = gradient.getPixel(l, t)->normalize() / brightest.normalize();
      double angle = atan2(g_y.getPixel(l, t)->normalize(), g_x.getPixel(l, t)->normalize());
      rv.setAt(l, t, Gradient(l, t, intensity, angle));
    }
  }
  rv.nonMaximumSuppression();
  // double-thresholding by k-means clustering
  pair<vector<Gradient>, vector<int>> clusters = cluster_kmeans(7, rv.repr);
  vector<Gradient> centroids = clusters.first;
  for (int i = 0; i < 7; i++) {
    cerr << "centroid[" << i << "] = " << centroids[i].intensity << endl;
  }
  vector<int> mappings = clusters.second;
  for (size_t i = 0; i < rv.repr.size(); i++) {
    if (mappings[i] == 0) {
      rv.repr[i].intensity = 0.0;
    } else if (mappings[i] == 1) {
      rv.repr[i].intensity = 0.5;
    } else {
      rv.repr[i].intensity = 1.0;
    }
  }
  // hysteresis
  auto tmp = rv; // tmp is a copy of rv that will not be mutated
  for (int l = 0; l < rv.width; l++) {
    for (int t = 0; t < rv.height; t++) {
      if (rv.getAt(l, t).intensity == 0.5) {
        Gradient gr = rv.getAt(l, t);
        gr.intensity = 1.0;
        for (double th = 0.0; th < 2.0 * M_PI; th += M_PI / 4.0) {
          int l2, t2;
          l2 = roundF(l + cos(th));
          t2 = roundF(t + sin(th));
          if (tmp.isIn(l2, t2)) {
            if (l2 == l && t2 == t) {
              cerr << "FAILURE\n";
            }
            if (tmp.getAt(l2, t2).intensity == 1.0) {
              rv.setAt(l, t, gr);
              break;
            }
          }
        }
        if (rv.getAt(l, t).intensity == 0.5) {
          gr.intensity = 0;
          rv.setAt(l, t, gr);
        }
      }
    }
  }
  return rv;
}

image gradientGridToImage(GradientGrid gg) {
  int w = gg.width, h = gg.height;
  image rv(w, h);
  for (int l = 0; l < w; l++) {
    for (int t = 0; t < h; t++) {
      rv.setPixel(l, t, color<RgbDoubles_t>(gg.getAt(l, t).intensity * 255.0));
    }
  }
  return rv;
}

void gradientGridToSvg(GradientGrid gg, string name) {
  int w = gg.width, h = gg.height;
  long int nLines = 0;
  int upScale = 10;
  ofstream fOut(name);
  char buffer[256];
  fOut << "<svg viewBox=\"" << 0 << 0 << w * upScale << h * upScale << "\">\n";
  fOut << "\tstroke 'black'\n";
  fOut << "\tfill 'black'\n";
  fOut << "\tstroke-width 1\n";
  for (double l = 0; l < (double)w; l += 1) {
    for (double t = 0; t < (double)h; t += 1) {
      Gradient g = gg.getAt(l, t);
      if (g.intensity > 0.2) {
        double baseL = l * upScale + upScale / 2, baseT = t * upScale + upScale / 2;
        double endL = baseL + double(upScale / 2) * sin(g.angle);
        double endT = baseT + double(upScale / 2) * cos(g.angle);
        double startL = baseL - double(upScale / 2) * sin(g.angle);
        double startT = baseT - double(upScale / 2) * cos(g.angle);
        sprintf(buffer, "<circle stroke=\"black\" cx=\"%lf\" cy=\"%lf\" r=\"%lf\" />", baseL, baseT, double(upScale - 1) / 2.0);
        fOut << "\t" << string(buffer) << endl;
        sprintf(buffer, "<line stroke=\"white\" x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" />\n", startL, startT, endL, endT);
        fOut << "\t" << string(buffer) << endl;
        nLines++;
      }
    }
  }
  fOut << "</svg>\n";
  fOut.close();
  cerr << "Produced SVG with " << nLines << " edges.\n";
  return;
}