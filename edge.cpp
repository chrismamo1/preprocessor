#include <fstream>
#include <tgmath.h>

#include "edge.hpp"
#include "point.hpp"

LineSegment::LineSegment(double l1, double t1, double l2, double t2) {
  if (l1 < l2) {
    start = Point<double>(l1, t1);
    stop = Point<double>(l2, t2);
  } else {
    start = Point<double>(l2, t2);
    stop = Point<double>(l1, t1);
  }
}

double LineSegment::getSlope() {
  return double(stop.top - start.top) / double(stop.left - start.left);
}

double LineSegment::length() {
  double dh = double(fabs(start.left - stop.left));
  double dv = double(fabs(start.top - stop.top));
  return sqrt(dh * dh + dv * dv);
}

bool LineSegment::sharesEndpoint(LineSegment ls) {
  return (start == ls.start || start == ls.stop) || (stop == ls.start || stop == ls.stop);
}

bool LineSegment::operator==(LineSegment x) {
  if (x.start == start) {
    if (x.stop == stop) {
      return true;
    }
  }
  if (x.start == stop) {
    if (x.stop == start) {
      return true;
    }
  }
  return false;
}

/**
 * combines two overlapping line segments, or throws an exception
 */
LineSegment LineSegment::operator+(LineSegment ls) {
  /// figure out if the lines have the same slope
  if (abs(getSlope() - ls.getSlope()) < 0.00001) {
    Point<double> beg;
    Point<double> end;
    if (start.left < ls.start.left) {
      beg = start;
      if (stop.left < ls.start.left) {
        throw string("Cannot combine line segments that do not intersect");
      }
    } else {
      beg = ls.start;
      if (ls.stop.left < start.left) {
        throw string("Cannot combine line segments that do not intersect");
      }
    }
    if (stop.left > ls.stop.left) {
      end = stop;
    } else {
      end = ls.stop;
    }
    LineSegment rv(beg.left, beg.top, end.left, end.top);
    // need to make sure that the segments line up
    if (fabs(getSlope() - rv.getSlope()) > 0.00001) {
      throw string("Cannot combine line segments that do not intersect");
    }
    return rv;
  }
  throw string("Cannot combine line segments with different slopes");
}

Edge::Edge() {}

Edge::Edge(LineSegment ls) {
  segments.push_back(ls);
}

bool Edge::intersects(LineSegment ls) {
  bool rv = false;
  for (auto it = segments.begin(); it != segments.end(); it++) {
    if (it->sharesEndpoint(ls)) {
      rv = true;
      break;
    }
    try {
      *it + ls;
      rv = true;
    } catch(string exn) {
      ;
    }
  }
  return rv;
}

bool Edge::addSegment(LineSegment ls) {
  bool belongs = (segments.size() == 0 || intersects(ls));
  if (hasSegment(ls)) {
    return false;
  } else if (belongs) {
    /*bool matched = false;
    for (auto it = segments.begin(); it != segments.end(); it++) {
      try {
        *it = *it + ls;
        matched = true;
        break;
      } catch (string exn) {
        ;
      }
    }
    if (!matched) {*/
      segments.push_back(ls);
      return true;
    //}
  }
  return false;
}

void Edge::compress() {
  size_t initial = segments.size();
  for (auto it = segments.begin(); it != segments.end(); it++) {
    for (auto jt = it + 1; jt != segments.end(); jt++) {
      try {
        LineSegment nieuw = *it + *jt;
        if (jt <= it) {
          continue;
        }
        segments.erase(jt);
        jt--;
        *it = nieuw;
      } catch (string exn) {
        ;
      }
    }
  }
  if (initial != segments.size()) {
    cerr << "Done compressing an edge (" << initial << " to " << segments.size() << ")\n";
  }
}

bool Edge::hasSegment(LineSegment ls) {
  for (size_t i = 0; i < segments.size(); i++) {
    if (ls == segments[i]) {
      return true;
    }
  }
  return false;
}

string Edge::toSvg() {
  char buffer[256];
  string rv = "";
  for (auto it = segments.begin(); it != segments.end(); it++) {
    sprintf(
      buffer,
      "<line stroke=\"black\" x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" />",
      it->start.left, it->start.top,
      it->stop.left, it->stop.top);
    rv.append(string(buffer));
  }
  return rv;
}

int roundFl(double x) {
  double y = abs(x);
  int fl = int(y);
  double y2 = y - double(fl);
  int sign = (x < 0) ? -1 : 1;
  if (y2 >= 0.5) {
    return (fl + 1) * sign;
  }
  return fl * sign;
}

EdgeSet::EdgeSet(GradientGrid gg) {
  width = gg.width;
  height = gg.height;
  // so that we can mark nodes that have been traversed
  auto tmp = gg;
  bool hasShown = false;
  for (int l = 0; l < gg.width; l++) {
    if (l % 75 == 0) {
      for (auto it = edges.begin(); it != edges.end(); it++) {
        it->compress();
      }
      //cerr << "Done compressing edges.\n";
    }
    if (l % 100 == 0) {
      cerr << "Doing column " << l << endl;
    }
    for (int t = 0; t < gg.height; t++) {
      if (gg.getAt(l, t).intensity > 0.1) {
        const double PI = 3.14159265358979;
        for (double th = 0.0; th < 2.0 * PI; th += PI / 4.0) {
          int l2, t2;
          l2 = roundFl(l + cos(th));
          t2 = roundFl(t + sin(th));
          if (!hasShown && l > 0 && t > 0) {
            cerr << "l2: " << (l - l2) << ", t2: " << (t - t2) << ", th: " << th << endl;
          }
          if (tmp.isIn(l2, t2)) {
            if (l2 == l && t2 == t) {
              cerr << "FAILURE\n";
            }
            if (tmp.getAt(l2, t2).intensity > 0.1) {
              LineSegment ls(l, t, l2, t2);
              if (edges.size() == 0) {
                Edge e(ls);
                edges.push_back(e);
              } else {
                bool added = false;
                for (auto it = edges.begin(); it != edges.end(); it++) {
                  if (it->intersects(ls)) {
                    if (it->addSegment(ls)) {
                      added = true;
                      break;
                    }
                  }
                }
                if (!added) {
                  Edge e(ls);
                  edges.push_back(e);
                }
              }
            }
          }
        }
        if (!hasShown && l > 0 && t > 0) {
          hasShown = true;
        }
        tmp.setAt(l, t, Gradient(0.0));
      }
    }
  }
  cerr << "Done making EdgeSet\n";
}

void EdgeSet::toSvg(string name) {
  int w = width, h = height;
  long int nLines = 0;
  int upScale = 1;
  ofstream fOut(name);
  char buffer[256];
  cerr << "Writing SVG wireframe.\n";
  cerr << "<svg viewBox=\"" << 0 << " " << 0 << " " << w << " " << h << "\" width=\"" << w << "\" height=\"" << h << "\"" << ">\n";
  fOut << "<svg viewBox=\"" << 0 << " " << 0 << " " << w << " " << h << "\" width=\"" << w << "\" height=\"" << h << "\"" << ">\n";
  for (auto it = edges.begin(); it != edges.end(); it++) {
    fOut << it->toSvg();
  }
  fOut << "</svg>\n";
  fOut.close();
  cout << name << endl;
  return;
}