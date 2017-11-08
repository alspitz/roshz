#include <set>

namespace statistics {

// See https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance

// virtual data origin
double K = 0.0;
double sum = 0.0;
double sq_dev = 0.0;

// For min / max tracking.
std::multiset<double> data;

void add_data_point(double x) {
  if (!data.size()) {
    K = x;
  }

  data.insert(x);
  sum += x - K;
  sq_dev += (x - K) * (x - K);
}

void remove_data_point(double x) {
  // We only want to remove one element.
  data.erase(data.find(x));
  sum -= (x - K);
  sq_dev -= (x - K) * (x - K);
}

double get_min() {
  return *data.begin();
}

double get_max() {
  return *std::prev(data.end());
}

double get_mean() {
  return K + sum / data.size();
}

double get_variance() {
  return (sq_dev - (sum * sum) / data.size()) / data.size();
}

unsigned int get_n() {
  return data.size();
}

}
