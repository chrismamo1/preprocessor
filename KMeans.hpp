#ifndef _KMEANS_HPP
#define _KMEANS_HPP

#include<iostream>
#include<map>
#include<thread>
#include<utility>
#include<vector>

using namespace std;

#define DISTANCE(VALS, IVAL, CENS, ICEN) ((VALS[IVAL] - CENS[ICEN]).abs())

template<typename T>
static T distance(int centroid, vector<T> centroids, vector<T> values, vector<int> mappings) {
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
pair<vector<T>, vector<int>> cluster_kmeans(int k, vector<T> values) {
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
    guess = guess + ((max - min) / k) / 2;
    centroids.push_back(guess);
  }

  //map<int, vector<size_t>> clusters; // mapping of centroid vector indices and point vector indices
  vector<int> mappings; // keys value indices to centroid indices
  mappings.assign(values.size(), 1);
  map<int, T> distances; // cache distances
  cerr << " ";
  int icount = 0;
  while(true) {
    double mean_distance = 0.0;
    for (int i = 0; i < k; i++) {
      mean_distance += distances[i].normalize() / double(distances.size());
    }
    cerr << "\riteration " << icount++ << ", average distance: " << mean_distance;
    cerr.flush();
    /// put each value in the centroid it matches most closely
    #define NTHREADS 2
    vector<thread*> threads;
    for (int iThread = 0; iThread < NTHREADS; iThread++) {
      size_t beginAt = iThread * (values.size() / NTHREADS);
      size_t endAt = (iThread == NTHREADS - 1) ? values.size() - 1 : beginAt + (values.size() / NTHREADS);
      thread *thr = new thread(
        [centroids, &mappings, &values](size_t beginAt, size_t endAt) {
          for (size_t i = beginAt; i < endAt; i++) {
            T shortest_distance = DISTANCE(values, i, centroids, 0);
            mappings[i] = 0;
            for (size_t j = 1; j < centroids.size(); j++) {
              T dist = DISTANCE(values, i, centroids, j);
              if (dist < shortest_distance) {
                shortest_distance = dist;
                mappings[i] = (int)j;
              }
            }
          }
          return;
        },
        beginAt, endAt);
      threads.push_back(thr);
    };
    for (auto it = threads.begin(); it != threads.end(); it++) {
      (*it)->join();
    }
    /// Check for convergence.
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
    /// Reposition each centroid to better fit its points.
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
  cerr << "\riteration " << icount++ << ", average distance: " << mean_distance;
  cerr.flush();
  cerr << endl;
  return pair<vector<T>, vector<int>>(centroids, mappings);
}

#endif