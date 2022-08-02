#include "william.h"
#include "av1/encoder/william/robust/sac.cpp"
#include "av1/encoder/william/opencv/cv.cpp"
#include "av1/encoder/ransac.c"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

void compute(unsigned char *src_buffer, int src_width, int src_height,
             int src_stride, unsigned char *ref_buffer, int ref_width,
             int ref_height, int ref_stride, int *num_inliers_by_motion,
             MotionModel *params_by_motion,
             TransformationType transformation_type, CV detect_type,
             CV match_type, CV estimate_type) {
  Mat src_img(src_height, src_width, CV_8U, src_buffer, src_stride);
  Mat ref_img(ref_height, ref_width, CV_8U, ref_buffer, ref_stride);
  std::vector<KeyPoint> src_keypoints, ref_keypoints;
  std::vector<Point2d> src_matches, ref_matches;
  Mat src_descriptors, ref_descriptors;
  std::vector<DMatch> good_matches;

  fprintf(stderr, "\n");

  //  string d = det(detect);
  //  string comp = com(a1);
  //  string matc = mat(a2);

  detect(src_img, src_keypoints, src_descriptors, detect_type);
  detect(ref_img, ref_keypoints, ref_descriptors, detect_type);
  match(src_descriptors, ref_descriptors, good_matches, match_type);
  //  draw(src_img, ref_img, src_keypoints, ref_keypoints, good_matches,
  //       comp + "_" + matc + ".png");

  // draw(src_img, src_keypoints, "cv_fast.png");

  int num_correspondences = static_cast<int>(good_matches.size());

  // src points | ref points: matches
  fprintf(stderr, "%zu | %zu:\t %d\n", src_keypoints.size(),
          ref_keypoints.size(), num_correspondences);

  if (num_correspondences == 0) {
    num_inliers_by_motion[0] = 0;
    return;
  }

  Correspondence *correspondences = (Correspondence *)malloc(
      num_correspondences * 4 * sizeof(*correspondences));
  Correspondence *ptr = correspondences;

  for (auto &d : good_matches) {
    ptr->x = static_cast<int>(round(src_keypoints[d.queryIdx].pt.x));
    ptr->y = static_cast<int>(round(src_keypoints[d.queryIdx].pt.y));
    ptr->rx = static_cast<int>(round(ref_keypoints[d.trainIdx].pt.x));
    ptr->ry = static_cast<int>(round(ref_keypoints[d.trainIdx].pt.y));
    ptr++;
  }

  //  for (auto &d : good_matches) {
  //    *(ptr++) = static_cast<int>(round(src_keypoints[d.queryIdx].pt.x));
  //    *(ptr++) = static_cast<int>(round(src_keypoints[d.queryIdx].pt.y));
  //    *(ptr++) = static_cast<int>(round(ref_keypoints[d.trainIdx].pt.x));
  //    *(ptr++) = static_cast<int>(round(ref_keypoints[d.trainIdx].pt.y));
  //    num_correspondences++;
  //  }

  //  draw(src_buffer, src_width, src_height, src_stride, ref_buffer, ref_width,
  //       ref_height, ref_stride, correspondences, num_correspondences);

  estimate(correspondences, num_correspondences, num_inliers_by_motion,
           params_by_motion, transformation_type, estimate_type);

  if (num_inliers_by_motion[0] < MIN_INLIER_PROB * num_correspondences) {
    num_inliers_by_motion[0] = 0;
  } else {
    get_inliers_from_indices(&params_by_motion[0], (int *)correspondences);
  }

  free(correspondences);
}
