#include<algorithm>
#include<cstdlib>
#include<iostream>
#include<string>
#include<tgmath.h>
#include<fstream>
#include<thread>
#include<utility>
#include<unistd.h>
#include<vector>

#include "color.hpp"
#include "ColorRgb.hpp"
#include "filter.hpp"
#include "image.hpp"
#include "KMeans.hpp"

using namespace std;

float square(float x) {
  return x * x;
}

void writePpm(string fName, vector<char> buffer, int w, int h);

color<RgbDoubles_t> getMedian(vector<color<RgbDoubles_t>> x) {
  vector<color<RgbDoubles_t>> cp(x);
  sort(cp.begin(), cp.end());
  int i;
  if (x.size() % 2 == 0) {
    i = x.size() / 2 - 1;
  } else {
    i = x.size() / 2;
  }
  return cp[i];
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Please do -h for usage advice.\n";
    return 1;
  }
  int c;
  int ck = 2;
  int dist;
  int rowsPerThread = 30;
  float scaleBack;
  float diffPow;
  dist = 20;
  scaleBack = 1.5;
  diffPow = 2.0;
  string fName(argv[argc - 2]);
  string oName(argv[argc - 1]);
  while((c = getopt(argc, argv, "c:d:l:k:i:o:s:h")) != -1) {
    switch(c) {
    case 'c':
      ck = atoi(optarg);
      break;
    case 'd':
      diffPow = atof(optarg);
      break;
    case 'l':
      if(optarg)
        rowsPerThread = atoi(optarg);
      break;
    case 'k':
      if(optarg) {
        dist = atoi(optarg);
        if (dist % 2 == 0) {
          cerr << "ERROR: distance must be odd\n";
          return 1;
        }
      }
      break;
    case 'i':
      fName = string(optarg);
      break;
    case 'o':
      oName = string(optarg);
      break;
    case 's':
      scaleBack = atof(optarg);
      break;
    default:
    case 'h':
    case '?':
      cout << "Usage: " << argv[0] << " [-d difference power] [-s scale back = 1.5] [-l rows/thread=30] [-k range=20] [ppm image] [output image]\n";
      return 0;
    }
  }
  ifstream fIn;
  fIn.open(fName);
  string magic;
  fIn >> magic;
  if (magic.compare("P6") == 0) {
    // ok
  } else {
    cerr << "Invalid magic word in PPM file: \"" << magic << "\"\n";
    return 1;
  }
  cout << "Reading from " << fName << "... ";
  int w, h;
  fIn >> w;
  fIn >> h;
  int max;
  fIn >> max;
  size_t nBytes = 3 * w * h;
  unsigned char *buffer = (unsigned char*)malloc(nBytes);
  fIn.read((char*)buffer, 1);
  fIn.read((char*)buffer, nBytes);
  cout << "Done.\n";
  image img(buffer, w, h);
  vector<thread*> threads;
  /*cout << "Grayscaling image...\n";
  img.mapPixels([dist, diffPow](color<RgbDoubles_t> input, int l, int t, image *img) {
      color<RgbDoubles_t> x(input);
      x.set_all(x.get_average(2.0));
      return x;
    }, rowsPerThread);
  cout << "Done.\n";
  cout << "Processing image...\n";
  double e1[] =
    { 1, 0, -1
    , 0, 0, 0
    , -1, 0, 1
    };
  double e2[] =
    { 0, 1, 0
    , 1, -4, 1
    , 0, 1, 0
    };
  double e3[] =
    { -1, -1, -1
    , -1,  8, -1
    , -1, -1, -1
    };
  img.runKernel(e1, 3, false);
  img.runKernel(e2, 3, false);
  img.runKernel(e3, 3, false);
  */
  //img.runKernel(filter::normalize(filter::gaussian<double>(dist)));
  /*img.mapPixels([dist, diffPow](color<RgbDoubles_t> input, int l, int t, image *img) {
      color<RgbDoubles_t> x(input);
      vector<pair<color<RgbDoubles_t>, int>> neighbors;
      neighbors = img->getNeighbors(l, t, dist);
      color<RgbDoubles_t> da(0.0);
      for (size_t i = 0; i < neighbors.size(); i++) {
        color<RgbDoubles_t> n = neighbors[i].first;
        da = da + (x - n).abs().raise(1.5) / double(neighbors[i].second);
      }
      da = da / float(neighbors.size());
      return da.raise(0.5);
      //x.set_all(da);
      //return x;
    }, rowsPerThread);*/
  //img.runKernel(sharper5, 5, true);
  //cout << "Done.\n";
  cout << "Doing k-means clustering...\n";
  pair<vector<color<RgbDoubles_t>>, map<size_t, int>> clusters = cluster_kmeans(ck, img.data);
  cout << "Done.\n";
  vector<color<RgbDoubles_t>> centroids = clusters.first;
  map<size_t, int> mappings = clusters.second;
  for (int i = 0; i < ck; i++) {
    int count = 0;
    for (size_t j = 0; j < mappings.size(); j++) {
      if (mappings[j] == i)
        count++;
    }
    cout << "centroid " << i << ": value(" << centroids[i].get_brightness() << ") members(" << count << ")\n";
  }
  cout << "Mapping clustered data back into real image.\n";
  color<RgbDoubles_t> median = getMedian(centroids);
  for (size_t i = 0; i < img.data.size(); i++) {
    img.data[i] = (centroids[mappings[i]] <= median) ? 0.0 : 255.0;
  }
  cout << "Done.\n";
  cout << "Normalizing image.\n";
  //img.runKernel(filter::normalize(filter::gaussian<double>(3)));
  img.normalize();
  cout << "Done.\n";
  cout << "Writing to " << oName << "...\n";
  img.write(oName);
  cout << "Done.\n";
  return 0;
}