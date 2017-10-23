/*
 * features.hpp
 *
 *  Created on: Jun 1, 2017
 *      Author: elchaschab
 */

#ifndef SRC_FEATURES_HPP_
#define SRC_FEATURES_HPP_

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#ifndef _NO_OPENCV
#include <opencv2/ocl/ocl.hpp>
#include <opencv2/legacy/legacy.hpp>
#endif
#include <SDL/SDL.h>

using std::vector;
using namespace cv;
#ifndef _NO_OPENCV
using namespace cv::ocl;
#endif

int ratioTest(std::vector<std::vector<cv::DMatch> > &matches);
cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
    const std::vector<cv::KeyPoint>& keypoints1,
    const std::vector<cv::KeyPoint>& keypoints2,
    std::vector<cv::DMatch>& outMatches);
void symmetryTest(const std::vector<std::vector<cv::DMatch> >& matches1,
    const std::vector<std::vector<cv::DMatch>>& matches2,
    std::vector<cv::DMatch>& symMatches);

double diff_features(SDL_Surface * test_surf,Mat& goalRGB, Mat& featureMat);

#endif /* SRC_FEATURES_HPP_ */
