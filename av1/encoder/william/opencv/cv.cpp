#include "draw.hpp"
#include "av1/encoder/william/william.h"
#include "av1/encoder/global_motion.h"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

#define RATIO_THRESH 0.75

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

inline void detect(Mat &img, vector<KeyPoint> &kpts, Mat &desc, CV type) {
  kpts.clear();

  if (type == CV::FAST) {
    Ptr<FastFeatureDetector> detector = FastFeatureDetector::create(18);
    detector->detect(img, kpts);

    Ptr<BEBLID> beblid = BEBLID::create(5);
    beblid->compute(img, kpts, desc);

  } else if (type == CV::SURF) {
    Ptr<Feature2D> surf = SURF::create(400);
    surf->detectAndCompute(img, noArray(), kpts, desc);

  } else if (type == CV::SIFT) {
    Ptr<Feature2D> sift = SIFT::create();
    sift->detectAndCompute(img, noArray(), kpts, desc);

  } else if (type == CV::ORB) {
    Ptr<Feature2D> orb = ORB::create();
    orb->detectAndCompute(img, noArray(), kpts, desc);

  } else if (type == CV::BRISK) {
    Ptr<Feature2D> brisk = BRISK::create();
    brisk->detectAndCompute(img, noArray(), kpts, desc);
    //
    //  } else if (type == "kaze") {
    //    Ptr<KAZE> kaze = KAZE::create();
    //    kaze->detectAndCompute(img, Mat(), kpts, desc);
    //
    //  } else if (type == "akaze") {
    //    Ptr<AKAZE> akaze = AKAZE::create();
    //    akaze->detectAndCompute(img, Mat(), kpts, desc);
    //
    //  } else if (type == "freak") {
    //    Ptr<FREAK> freak = FREAK::create();
    //    freak->compute(img, kpts, desc);
    //
    //  } else if (type == "daisy") {
    //    Ptr<DAISY> daisy = DAISY::create();
    //    daisy->compute(img, kpts, desc);
    //
    //  } else if (type == "brief") {
    //    Ptr<BriefDescriptorExtractor> brief =
    //    BriefDescriptorExtractor::create(64); brief->compute(img, kpts, desc);
    //
    //  } else if (type == "beblid") {
    //    Ptr<BEBLID> beblid = BEBLID::create(0.75);
    //    beblid->compute(img, kpts, desc);
    //
    //  } else if (type == "blob") {
    //    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();
    //    detector->detect(img, kpts);
    //
  } else {
    assert(0);
  }
}

inline void match(Mat &desc1, Mat &desc2, vector<DMatch> &matches, CV type) {
  matches.clear();

  vector<vector<DMatch> > vmatches;

  if (type == CV::BF) {
    Ptr<DescriptorMatcher> matcher =
        DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
    matcher->knnMatch(desc1, desc2, vmatches, 2);

  } else if (type == CV::FLANN) {
    FlannBasedMatcher matcher(new flann::LshIndexParams(12, 20, 2));
    matcher.knnMatch(desc1, desc2, vmatches, 2);

  } else {
    assert(0);
  }

  for (auto &knn_match : vmatches) {
    if (knn_match.empty()) {
      continue;
    }

    if (knn_match[0].distance < RATIO_THRESH * knn_match[1].distance) {
      matches.push_back(knn_match[0]);
    }
  }

  std::sort(matches.begin(), matches.end());

  //  while (matches.front().distance * kDistanceCoef < matches.back().distance)
  //  {
  //    matches.pop_back();
  //  }
  //
  while (matches.size() > MAX_CORNERS) {
    matches.pop_back();
  }
}

void draw(unsigned char *src_buffer, int src_width, int src_height,
          int src_stride, unsigned char *ref_buffer, int ref_width,
          int ref_height, int ref_stride, const int *correspondences,
          int num_correspondences) {
  Mat src_img(src_height, src_width, CV_8U, src_buffer, src_stride);
  Mat ref_img(ref_height, ref_width, CV_8U, ref_buffer, ref_stride);

  std::vector<DMatch> good_matches;
  std::vector<KeyPoint> src_keypoints, ref_keypoints;

  for (int i = 0; i < num_correspondences; i++) {
    int sx = correspondences[i * 4];
    int sy = correspondences[i * 4 + 1];
    int rx = correspondences[i * 4 + 2];
    int ry = correspondences[i * 4 + 3];

    src_keypoints.emplace_back(sx, sy, 1);
    ref_keypoints.emplace_back(rx, ry, 1);
    good_matches.emplace_back(i, i, 0);
  }

  draw(src_img, ref_img, src_keypoints, ref_keypoints, good_matches,
       "conv.png");
}

void draw2(unsigned char *src_buffer, int src_width, int src_height,
           int src_stride, const int *src_corners, int num_src_corners) {
  Mat src_img(src_height, src_width, CV_8U, src_buffer, src_stride);

  std::vector<KeyPoint> src_keypoints;

  for (int i = 0; i < num_src_corners; i++) {
    int sx = src_corners[i * 2];
    int sy = src_corners[i * 2 + 1];

    src_keypoints.emplace_back(sx, sy, 1);
  }

  draw(src_img, src_keypoints, "aom_fast.png");
}