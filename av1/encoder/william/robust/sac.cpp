#include "Base.cpp"
#include "RANSAC.cpp"
#include "LMedS.cpp"
#include "MSAC.cpp"
#include "MLESAC.cpp"
#include "PROSAC.cpp"
#include "AffineEstimator.cpp"
#include "RotZoomEstimator.cpp"
#include "TranslationEstimator.cpp"
#include "av1/encoder/corner_match.h"
#include "av1/encoder/ransac.h"
#include "av1/encoder/william/william.h"
#include <cstdio>

RTL::Estimator<RTL::Affine, Correspondence, Correspondence *> *getEstimator(
    TransformationType type) {
  switch (type) {
    case AFFINE: return new AffineEstimator();
    case ROTZOOM: return new RotZoomEstimator();
    case TRANSLATION: return new TranslationEstimator();
    default: assert(0);
  }
}

void estimate(Correspondence *correspondences, int num_correspondences,
              int *num_inliers_by_motion, MotionModel *params_by_motion,
              TransformationType transformation_type, CV type) {
  RTL::Estimator<RTL::Affine, Correspondence, Correspondence *> *estimator =
      getEstimator(transformation_type);

  if (type == CV::RANSAC) {
    RansacFunc ransac = av1_get_ransac_type(transformation_type);
    ransac(reinterpret_cast<int *>(correspondences), num_correspondences,
           num_inliers_by_motion, params_by_motion, 1);

  } else if (type == CV::MLESAC) {
    RTL::MLESAC<RTL::Affine, Correspondence, Correspondence *> ransac(
        estimator);
    ransac.compute(correspondences, num_correspondences, params_by_motion);

  } else if (type == CV::MSAC) {
    RTL::MSAC<RTL::Affine, Correspondence, Correspondence *> ransac(estimator);
    ransac.compute(correspondences, num_correspondences, params_by_motion);

  } else if (type == CV::PROSAC) {
    RTL::PROSAC<RTL::Affine, Correspondence, Correspondence *> ransac(
        estimator);
    ransac.compute(correspondences, num_correspondences, params_by_motion);

  } else if (type == CV::LMEDS) {
    RTL::LMedS<RTL::Affine, Correspondence, Correspondence *> ransac(estimator);
    ransac.compute(correspondences, num_correspondences, params_by_motion);

  } else {
    assert(0);
  }

  num_inliers_by_motion[0] = params_by_motion->num_inliers;

  // Pontos: inliers / outliers
  fprintf(stderr, "%d:\t %d / %d\n", num_correspondences,
          num_inliers_by_motion[0],
          num_correspondences - num_inliers_by_motion[0]);

  delete estimator;
}

static void get_inliers_from_indices(MotionModel *params,
                                     const int *correspondences) {
  int *inliers_tmp = (int *)aom_malloc(2 * MAX_CORNERS * sizeof(*inliers_tmp));
  memset(inliers_tmp, 0, 2 * MAX_CORNERS * sizeof(*inliers_tmp));

  for (int i = 0; i < params->num_inliers; i++) {
    int index = params->inliers[i];
    inliers_tmp[2 * i] = correspondences[4 * index];
    inliers_tmp[2 * i + 1] = correspondences[4 * index + 1];
  }

  memcpy(params->inliers, inliers_tmp, sizeof(*inliers_tmp) * 2 * MAX_CORNERS);
  aom_free(inliers_tmp);
}