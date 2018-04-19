#ifndef _KMEANS_HPP
#define _KMEANS_HPP

#include<iostream>
#include<map>
#include<utility>
#include<vector>

using namespace std;

#define DISTANCE(VALS, IVAL, CENS, ICEN) ((VALS[IVAL] - CENS[ICEN]).abs())

template<typename T>
static T distance(int centroid, vector<T> centroids, vector<T> values, map<size_t, int> mappings) {
  T acc = 0.0;
  size_t cnt = 0;
  for (size_t i = 0; i < values.size(); i++) {
    int centroid_index = mappings[i];
    if (centroid_index == centroid) {
      acc = acc + DISTANCE(values, i, centroids, centroid_index);
      cnt++;
    }
  }
  return acc / double(cnt);
};

// return the mapping of values to centroids as well as the centroid vector
template<typename T>
pair<vector<T>, map<size_t, int>> cluster_kmeans(int k, vector<T> values) {
  vector<T> centroids;
  const T threshold = T(0.01 * k);
  vector<int> guesses;
  T min(values[0]);
  T max(values[0]);
  for (size_t i = 0; i < values.size(); i++) {
    if (values[i] < min) {
      min = values[i];
    }
    if (values[i] > max) {
      max = values[i];
    }
  }
  T guess(min);
  for (int i = 0; i < k; i++) {
    /// get the k most disparate values from the input set
    /*size_t best = 0;
    if (i != 0) {
      size_t j;
      T dist = T(0.0);
      for (j = 0; j < values.size(); j++) {
        bool already = false;
        for (size_t l = 0; l < guesses.size(); l++) {
          if (j == guesses[l] || values[j] == values[guesses[l]]) {
            already = true;
            break;
          }
        }
        if (already) {
          continue;
        }
        T dist2 = T(0.0);
        for (size_t c = 0; c < centroids.size(); c++) {
          T diff = values[j] - centroids[c];
          dist2 = dist2 + diff.abs();
        }
        if (dist2 > dist) {
          dist = dist2;
          best = j;
        }
      }
    }
    guesses.push_back(best);*/
    guess = guess + ((max - min) / k);
    cout << "Pushing " << guess.get_brightness() << " as a guess\n";
    centroids.push_back(guess);
  }

  //map<int, vector<size_t>> clusters; // mapping of centroid vector indices and point vector indices
  map<size_t, int> mappings; // keys value indices to centroid indices
  map<int, T> distances; // cache distances
  cout << " ";
  int icount = 0;
  while(true) {
    double mean_distance = 0.0;
    for (int i = 0; i < k; i++) {
      mean_distance += distances[i].normalize() / double(distances.size());
    }
    cout << "\riteration " << icount++ << ", average distance: " << mean_distance;
    cout.flush();
    /// put each value in the centroid it matches most closely
    for (size_t i = 0; i < values.size(); i++) {
      T shortest_distance = DISTANCE(values, i, centroids, 0);
      mappings[i] = 0;
      for (int j = 1; j < k; j++) {
        T dist = DISTANCE(values, i, centroids, j);
        if (dist < shortest_distance) {
          shortest_distance = dist;
          mappings[i] = j;
        }
      }
    }
    bool has_converged = true;
    for (int i = 0; i < k; i++) {
      T dist = distance(i, centroids, values, mappings);
      if (distances.find(i) == distances.end()) {
        if (dist > threshold) {
          has_converged = false;
        }
      } else {
        if (dist > threshold && (dist - distances[i]).abs() > T(0.01 * k)) {
          has_converged = false;
        }
      }
      distances[i] = dist;
    }
    if (has_converged) {
      break;
    }
    vector<int> counts;
    vector<T> accs;
    counts.assign(k, 0);
    accs.assign(k, T(0.0));
    for (size_t i = 0; i < values.size(); i++) {
      int centroid = mappings[i];
      counts[centroid] = counts[centroid] + 1;
      accs[centroid] = accs[centroid] + values[i];
    }
    for (int i = 0; i < k; i++) {
      centroids[i] = accs[i] / double(counts[i]);
    }
  }
  double mean_distance = 0.0;
  for (int i = 0; i < k; i++) {
    mean_distance += distances[i].normalize() / double(distances.size());
  }
  cout << "\riteration " << icount++ << ", average distance: " << mean_distance;
  cout.flush();
  cout << endl;
  return pair<vector<T>, map<size_t, int>>(centroids, mappings);
}

#endif