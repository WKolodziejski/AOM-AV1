#ifndef __RTL_LMEDS__
#define __RTL_LMEDS__

#include "RANSAC.cpp"
#include <algorithm>

namespace RTL {

template <class Model, class Datum, class Data>
class LMedS : virtual public RANSAC<Model, Datum, Data> {
 public:
  explicit LMedS(Estimator<Model, Datum, Data> *estimator)
      : RANSAC<Model, Datum, Data>(estimator) {
    //fprintf(stderr, "---LMedS---\n");
  }

 protected:
  virtual double evaluateModel(const Model &model, const Data &data, int N) {
    std::vector<double> errors(N);
    for (int i = 0; i < N; i++)
      errors[i] = fabs(this->toolEstimator->computeError(model, data[i]));
    std::nth_element(errors.begin(), errors.begin() + N / 2, errors.end());
    return errors[N / 2];
  }
};

}  // namespace RTL

#endif  // End of '__RTL_LMEDS__'
