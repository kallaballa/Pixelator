/*
 * Features.cpp
 *
 *  Created on: Jun 1, 2017
 *      Author: elchaschab
 */


#include "features.hpp"


int ratioTest(std::vector<std::vector<cv::DMatch> >
    &matches) {
        int removed=0;
        // for all matches
        for (std::vector<std::vector<cv::DMatch> >::iterator
            matchIterator= matches.begin();
            matchIterator!= matches.end(); ++matchIterator) {
                // if 2 NN has been identified
                if (matchIterator->size() > 1) {
                    // check distance ratio
                    if ((*matchIterator)[0].distance/
                        (*matchIterator)[1].distance > 0.7) {
                            matchIterator->clear(); // remove match
                            removed++;
                    }
                } else { // does not have 2 neighbours
                    matchIterator->clear(); // remove match
                    removed++;
                }
        }
        return removed;
}

cv::Mat ransacTest(
    const std::vector<cv::DMatch>& matches,
    const std::vector<cv::KeyPoint>& keypoints1,
    const std::vector<cv::KeyPoint>& keypoints2,
    std::vector<cv::DMatch>& outMatches)
{
    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;
    for (std::vector<cv::DMatch>::
        const_iterator it= matches.begin();
        it!= matches.end(); ++it) {
            // Get the position of left keypoints
            float x= keypoints1[it->queryIdx].pt.x;
            float y= keypoints1[it->queryIdx].pt.y;
            points1.push_back(cv::Point2f(x,y));
            // Get the position of right keypoints
            x= keypoints2[it->trainIdx].pt.x;
            y= keypoints2[it->trainIdx].pt.y;
            points2.push_back(cv::Point2f(x,y));
    }
    // Compute F matrix using RANSAC
    std::vector<uchar> inliers(points1.size(),0);
    std::vector<cv::Point2f> out;
    //cv::Mat fundemental= cv::findFundamentalMat(points1, points2, out, CV_FM_RANSAC, 3, 0.99);


    cv::Mat fundemental= findFundamentalMat(
        cv::Mat(points1),cv::Mat(points2), // matching points
        inliers,      // match status (inlier or outlier)
        CV_FM_RANSAC, // RANSAC method
        3.0,     // distance to epipolar line
        0.99);  // confidence probability

    // extract the surviving (inliers) matches
    std::vector<uchar>::const_iterator
        itIn= inliers.begin();
    std::vector<cv::DMatch>::const_iterator
        itM= matches.begin();
    // for all matches
    for ( ;itIn!= inliers.end(); ++itIn, ++itM) {
        if (*itIn) { // it is a valid match
            outMatches.push_back(*itM);
        }
    }
    return fundemental;
}

void symmetryTest(
    const std::vector<std::vector<cv::DMatch>>& matches1,
    const std::vector<std::vector<cv::DMatch>>& matches2,
    std::vector<cv::DMatch>& symMatches) {
        // for all matches image 1 -> image 2
        for (std::vector<std::vector<cv::DMatch>>::
            const_iterator matchIterator1= matches1.begin();
            matchIterator1!= matches1.end(); ++matchIterator1) {
                // ignore deleted matches
                if (matchIterator1->size() < 2)
                    continue;
                // for all matches image 2 -> image 1
                for (std::vector<std::vector<cv::DMatch>>::
                    const_iterator matchIterator2= matches2.begin();
                    matchIterator2!= matches2.end();
                ++matchIterator2) {
                    // ignore deleted matches
                    if (matchIterator2->size() < 2)
                        continue;
                    // Match symmetry test
                    if ((*matchIterator1)[0].queryIdx ==
                        (*matchIterator2)[0].trainIdx  &&
                        (*matchIterator2)[0].queryIdx ==
                        (*matchIterator1)[0].trainIdx) {
                            // add symmetrical match
                            symMatches.push_back(
                                cv::DMatch((*matchIterator1)[0].queryIdx,
                                (*matchIterator1)[0].trainIdx,
                                (*matchIterator1)[0].distance));
                            break; // next match in image 1 -> image 2
                    }
                }
        }
}
double diff_features(SDL_Surface * test_surf, Mat& goalRGB, Mat& featureMat) {
  Mat testRGB(test_surf->w,test_surf->h,CV_8UC3,test_surf->pixels,test_surf->pitch);
  Mat img_1;
  Mat img_2;

  cvtColor(goalRGB, img_1, CV_RGB2GRAY);
  cvtColor(testRGB, img_2, CV_RGB2GRAY);

  OrbFeatureDetector detector;

   std::vector<KeyPoint> keypoints_1, keypoints_2;

   detector.detect( img_1, keypoints_1 );
   detector.detect( img_2, keypoints_2 );

   //-- Step 2: Calculate descriptors (feature vectors)
   OrbDescriptorExtractor extractor;

   Mat descriptors1, descriptors2;

   extractor.compute( img_1, keypoints_1, descriptors1 );
   extractor.compute( img_2, keypoints_2, descriptors2 );

   if(descriptors1.empty() || descriptors2.empty()) {
     return 1;
   }
   //-- Step 3: Matching descriptor vectors using FLANN matcher
   cv::Ptr<cv::flann::IndexParams> indexParams = new cv::flann::LshIndexParams(12, 20, 2);
   FlannBasedMatcher matcher1(indexParams);
   FlannBasedMatcher matcher2(indexParams);
   std::vector<std::vector<cv::DMatch>> matches1, matches2;
   std::vector<DMatch> goodMatches;
   std::vector<DMatch> symMatches;

   matcher1.knnMatch(descriptors1,descriptors2, matches1,2);
   matcher2.knnMatch(descriptors2,descriptors1, matches2,2);

   ratioTest(matches1);
   ratioTest(matches2);

   symmetryTest(matches1,matches2,symMatches);

   if(symMatches.empty()) {
     return 1;
   }
   cv::Mat fundemental= ransacTest(symMatches,
           keypoints_1, keypoints_2, goodMatches);

   if(goodMatches.empty()) {
     return 1;
   }

   double totalDist = 0;
   double avgDist = 0;
   float minDist = 9999999;
   /*
   if(good_matches.size() > 0) {
     std::sort(good_matches.begin(), good_matches.end(), [](const DMatch& m1, const DMatch& m2){
       return m1.distance < m2.distance;
     });

     assert(good_matches.back().distance >= good_matches.front().distance);

     if(good_matches.size() > 10) {
       good_matches.resize(10);
     }
   }
   */
   //-- Quick calculation of max and min distances between keypoints
   for( size_t i = 0; i < goodMatches.size(); i++ ) {
     totalDist += goodMatches[i].distance;
     minDist = std::min(goodMatches[i].distance, minDist);
   }

   avgDist = totalDist / goodMatches.size();

   Mat img_matches;
   drawMatches( img_1, keypoints_1, img_2, keypoints_2,
       goodMatches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

   featureMat = img_matches;
   if(avgDist > 100) {
     avgDist = 100;
   }

   return (avgDist / 100.0);
}
