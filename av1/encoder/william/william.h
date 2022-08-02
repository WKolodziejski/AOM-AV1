#ifndef AOM_WILLIAM_H

#include "av1/encoder/global_motion.h"

enum CV {
  FLANN,
  BF,
  FAST,
  SURF,
  SIFT,
  ORB,
  BRISK,
  RANSAC,
  MLESAC,
  MSAC,
  PROSAC,
  LMEDS
};

#define AOM_WILLIAM_H

#ifdef __cplusplus
extern "C" {
#endif

void compute(unsigned char *src_buffer, int src_width, int src_height,
             int src_stride, unsigned char *ref_buffer, int ref_width,
             int ref_height, int ref_stride, int *num_inliers_by_motion,
             MotionModel *params_by_motion, TransformationType transformation_type,
             enum CV detect, enum CV match, enum CV estimate);

#ifdef __cplusplus
}
#endif

#endif  // AOM_WILLIAM_H
