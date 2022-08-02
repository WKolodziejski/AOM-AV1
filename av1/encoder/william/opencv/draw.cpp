#include "draw.hpp"

void draw(Mat &src_img, Mat &ref_img, std::vector<KeyPoint> &src_keypoints,
          std::vector<KeyPoint> &ref_keypoints,
          std::vector<DMatch> &good_matches, const string &name) {
  Mat img_matches;
  drawMatches(src_img, src_keypoints, ref_img, ref_keypoints, good_matches,
              img_matches, Scalar::all(1), Scalar::all(-1),
              std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  vector<int> params;
  params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  params.push_back(0);
  imwrite(name, img_matches, params);
}

void draw(Mat &src_img, std::vector<KeyPoint> &src_keypoints,
          const string &name) {
  Mat img_keypoints;

  for (auto &k : src_keypoints) {
    k.size = 1;
  }

  drawKeypoints(src_img, src_keypoints, img_keypoints, Scalar::all(-1),
                DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  vector<int> params;
  params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  params.push_back(0);
  imwrite(name, img_keypoints, params);
}
