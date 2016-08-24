#ifndef TRACKERFUNCTIONS_H
#define TRACKERFUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define TRDIM 30 //dimension of tracking windows
#define TRPOINTS 7 //# of tracking points
#define TRSIGMA 3 //gaussian filter sigma

void initTracker(cv::Point2f start_point,const cv::Mat &input, cv::Mat &initFrame, std::vector<cv::Point2f> *initPoints, cv::Point2f *drawpoint, cv::TermCriteria termcrit);
int vjInit(cv::CascadeClassifier *haar_face, cv::CascadeClassifier *haar_reye, cv::CascadeClassifier *haar_leye);
bool init_irisDetect(cv::Mat &input, cv::CascadeClassifier *haar_face, cv::CascadeClassifier *haar_reye,
                     cv::CascadeClassifier *haar_leye, cv::Point *r_eye_c, cv::Point *l_eye_c, cv::Rect *facePos);
void LKTracker(const cv::Mat &input, cv::Mat &initFrame, const cv::Rect window, std::vector<cv::Point2f> *initPoints,
               std::vector<cv::Point2f> *trackPoints, cv::Point2f *drawpoint, float threshold, cv::TermCriteria termcrit);
void initFullTracker(cv::Mat &inputFrame, cv::Mat &trFrame, cv::Point r_eye_c, cv::Point l_eye_c, cv::Rect facePos,
                     cv::Rect *window_t, cv::Mat *prev_t, cv::Point2f *drawpoint, std::vector<cv::Point2f> *points,
                     cv::TermCriteria termcrit);
void getPoint(int event, int x, int y, int flags, void* ptr);
#endif // TRACKERFUNCTIONS_H
