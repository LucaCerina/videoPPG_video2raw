#ifndef GEOMFUNCTIONS_H
#define GEOMFUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>

/*Functions for central point calculation, with Rect, vector of points or matrix*/
cv::Point2f getCenter(cv::Rect input);
cv::Point2f getCenter(const std::vector<cv::Point2f> inputPoints);
cv::Point2f meanPosition(cv::Mat &input);
cv::Point2f getMidPoint(cv::Point p1, cv::Point p2);
void rotateFrame(cv::Mat &input, int angle);
cv::Rect rotateRect(cv::Rect input, double angle);
#endif // GEOMFUNCTIONS_H
