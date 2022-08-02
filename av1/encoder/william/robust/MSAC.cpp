#ifndef __RTL_MSAC__
#define __RTL_MSAC__

#include "RANSAC.cpp"

namespace RTL {

template <class Model, class Datum, class Data>
class MSAC : virtual public RANSAC<Model, Datum, Data> {
 public:
  explicit MSAC(Estimator<Model, Datum, Data> *estimator)
      : RANSAC<Model, Datum, Data>(estimator) {
    //fprintf(stderr, "---MSAC---\n");
  }

 protected:
  virtual double evaluateModel(const Model &model, const Data &data,
                                      int N) {
    double loss = 0;
    for (int i = 0; i < N; i++) {
      double error = this->toolEstimator->computeError(model, data[i]);
      if (error > INLIER_THRESHOLD || error < -INLIER_THRESHOLD)
        loss += INLIER_THRESHOLD * INLIER_THRESHOLD;
      else
        loss += error * error;
    }
    return loss;
  }
};

}  // namespace RTL

#endif  // End of '__RTL_MSAC__'
