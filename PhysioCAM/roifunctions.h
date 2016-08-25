#ifndef ROIFUNCTIONS_H
#define ROIFUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "geomfunctions.h"
#include <cmath>
#include <vector>

#define ROI_THR 8

/*output the Rect of ROIs given the tracker points*/
cv::Rect adjustROI(const cv::Rect input, const cv::Size frameDim);
cv::Rect getNoseDim(const cv::Point c1, const cv::Point c2);
cv::Rect getNoseDim(const cv::Point c1, const cv::Point c2, const cv::Rect faceDim);
cv::Rect getCheekDim(const cv::Point c1, const cv::Point c2);
cv::Rect getForeDim(const cv::Point c1, const cv::Point c2);
/*Given a frame and a ROI, output the RGB signal and the YCrCb signal */
void getSignalValue(const cv::Mat &inputRGB, const cv::Rect signalROI, std::vector<cv::Scalar> *avgRGB, std::vector<cv::Scalar> *avgYCC);
void updateROIState(cv::Point currentPoint, std::vector<unsigned int> *stateVector, cv::Point *stateCenter, unsigned int *currentState);
#endif // ROIFUNCTIONS_H
