#ifndef __EDGE_HPP
#define __EDGE_HPP

#include <vector>
#include "gradient.hpp"
#include "point.hpp"

using namespace std;

class LineSegment {
public:
  Point<double> start, stop;
  LineSegment(double l1, double t1, double l2, double t2);

  double getSlope();
  double length();
  bool sharesEndpoint(LineSegment);

  bool operator==(LineSegment);
  /// combine two line segments that share at least one endpoint and have the same incline
  LineSegment operator+(LineSegment);
};

/// represents a contiguous edge
class Edge {
public:
  vector<LineSegment> segments;

  Edge();
  Edge(LineSegment);

  bool intersects(LineSegment);
  bool addSegment(LineSegment);
  void compress();
  bool hasSegment(LineSegment);
  string toSvg();
};

/// represents all the edges in an image
class EdgeSet {
public:
  int width, height;
  vector<Edge> edges;
  EdgeSet(GradientGrid gg);

  void toSvg(string name);
};

#endif