#include "roifunctions.h"

cv::Rect adjustROI(const cv::Rect input,const cv::Size frameDim)
{
	cv::Rect correction = input;
	if(correction.x + correction.width > frameDim.width)
		correction.x = frameDim.width-correction.width;
	if(correction.y + correction.height > frameDim.height)
		correction.y = frameDim.height-correction.height;
	return correction;
}

cv::Rect getNoseDim(const cv::Point c1,const cv::Point c2)
{
	cv::Rect nose;
	int inter_points = abs(c1.x-c2.x);
	cv::Point center = getMidPoint(c1,c2);
	nose = cv::Rect(c1.x,center.y,inter_points,2.0*inter_points);
	return nose;
}

cv::Rect getNoseDim(const cv::Point c1, const cv::Point c2,const cv::Rect faceDim)
{
	cv::Rect nose;
	int inter_points = abs(c1.x-c2.x);
	cv::Point center = getMidPoint(c1,c2);
	nose = cv::Rect(c1.x,center.y,inter_points,0.25*faceDim.height+15.0/inter_points);
	return nose;
}

cv::Rect getCheekDim(const cv::Point c1,const cv::Point c2)
{
	cv::Rect cheek;
	int inter_points = abs(c1.x-c2.x);
	cv::Point center = getMidPoint(c1,c2);
	cheek = cv::Rect(c1.x,center.y,inter_points,inter_points);
	return cheek;
}

cv::Rect getForeDim(const cv::Point c1,const cv::Point c2)
{
	cv::Rect fore;
	cv::Point center = getMidPoint(c1,c2);
	int inter_eye = abs(c1.x-c2.x);
	fore.x = c1.x;
	fore.width = inter_eye;
	fore.y = center.y-0.4*inter_eye;
	if(fore.y<0)
		fore.y=0;
	fore.height = 0.4*inter_eye;
	return fore;
}

void getSignalValue(const cv::Mat &inputRGB, const cv::Rect signalROI, std::vector<cv::Scalar> *avgRGB, std::vector<cv::Scalar> *avgYCC)
{
	cv::Scalar signalValueRGB,signalValueYCC;
	//Scalar tempVal;
	float delta = 128.0;

	//BGR to RGB
	cv::cvtColor(inputRGB,inputRGB,cv::COLOR_BGR2RGB);
	signalValueRGB = mean(inputRGB(signalROI));
	//correct BGR to RGB conversion
	//signalValueRGB = Scalar(signalValueRGB[2],signalValueRGB[1],signalValueRGB[0]);
	//YCC conversion
	signalValueYCC[0] = 0.29900*signalValueRGB[0] + 0.58700*signalValueRGB[1] + 0.11400*signalValueRGB[2];
	signalValueYCC[1] = (signalValueRGB[0]-signalValueYCC[0])*0.71300 + delta;
	signalValueYCC[2] = (signalValueRGB[2]-signalValueYCC[0])*0.56400 + delta;
	avgRGB->push_back(signalValueRGB);
	avgYCC->push_back(signalValueYCC);
}

void updateROIState(cv::Point currentPoint, std::vector<unsigned int> *stateVector, cv::Point *stateCenter, unsigned int *currentState)
{
	//find dx and dy
	cv::Point delta = cv::Point(abs(currentPoint.x-stateCenter->x),abs(currentPoint.y-stateCenter->y));

	if((delta.x+delta.y <= ROI_THR) || (delta.x*delta.x+delta.y*delta.y <= ROI_THR*ROI_THR))
	{
		stateVector->push_back(*currentState);
	}
	else
	{
		(*currentState)++;
		*stateCenter = currentPoint;
		stateVector->push_back(*currentState);
	}
}
