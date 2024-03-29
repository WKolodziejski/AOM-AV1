#ifndef __RTL_PROSAC__
#define __RTL_PROSAC__

#include "RANSAC.cpp"
#include <algorithm>
#include <climits>

namespace RTL {

template <class Model, class Datum, class Data>
class PROSAC : virtual public RANSAC<Model, Datum, Data> {
 public:
  explicit PROSAC(Estimator<Model, Datum, Data> *estimator)
      : RANSAC<Model, Datum, Data>(estimator) {
    //fprintf(stderr, "---PROSAC---\n");
  }

 protected:
  virtual void initialize(const Data &data, int N) {
    RANSAC<Model, Datum, Data>::initialize(data, N);

    for (int i = 0; i < MIN_PTS; i++) {
      t_n *= static_cast<double>(n - i) / (N - i);
    }
  }

  virtual bool generateModel(const Data &data, Model &model, int M, int N,
                             double *points1, double *points2) {
    int degenerate = 1;
    int num_degenerate_iter = 0;

    while (degenerate) {
      num_degenerate_iter++;

      Datum subset[MIN_PTS];
      sample(data, subset, N);

      double *pts1 = points1;
      double *pts2 = points2;

      for (auto &d : subset) {
        *(pts1++) = d.x;
        *(pts1++) = d.y;
        *(pts2++) = d.rx;
        *(pts2++) = d.ry;
      }

      degenerate = this->toolEstimator->isDegenerate(points1);

      if (num_degenerate_iter > MAX_DEGENERATE_ITER) return false;
    }

    model = this->toolEstimator->computeModel(points1, points2, M);
    return true;
  }

 private:
  virtual void sample(const Data &data, Data subset, int N) {
    int j = 0;
    t++;

    if (t > t_n_prime && n < N) {
      double t_n_plus1 = (t_n * (n + 1.0)) / (n + 1.0 - MIN_PTS);
      t_n_prime += ceil(t_n_plus1 - t_n);
      t_n = t_n_plus1;
      n++;
    }

    std::vector<int> random_numbers;

    if (t > t_n_prime) {
      for (int i = 0; i < MIN_PTS; i++) {
        int rand_number;
        while (std::find(random_numbers.begin(), random_numbers.end(),
                         (rand_number = random(n))) != random_numbers.end()) {
        }

        random_numbers.push_back(rand_number);
        subset[j++] = data[rand_number];
      }
    } else {
      for (int i = 0; i < MIN_PTS - 1; i++) {
        int rand_number;
        while (std::find(random_numbers.begin(), random_numbers.end(),
                         (rand_number = random(n - 1))) !=
               random_numbers.end()) {
        }

        random_numbers.push_back(rand_number);
        subset[j++] = data[rand_number];
      }

      subset[j++] = data[n];
    }
  }

  int random(int N) {
    int index;
    this->get_rand_indices(N, 1, &index, &this->seed);
    return index;
  }

  double t_n = MIN_TRIALS;
  int n = MIN_PTS;
  int t = 0;
  double t_n_prime = 1.0;
};

}  // namespace RTL

#endif  // End of '__RTL_PROSAC__'
