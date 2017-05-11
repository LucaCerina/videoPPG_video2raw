#include "geomfunctions.h"

cv::Point2f getCenter(cv::Rect input)
{
	cv::Point2f center;
	center.x = input.x + input.width/2.0;
	center.y = input.y + input.height/2.0;
	return center;
}

cv::Point2f getCenter(const std::vector<cv::Point2f> inputPoints)
{
	cv::Point2f center = cv::Point2f(0,0);
	for(uint i=0; i<inputPoints.size(); i++)
	{
		center += inputPoints[i];
	}
	center.x = center.x / inputPoints.size();
	center.y = center.y / inputPoints.size();
	return center;
}

cv::Point2f meanPosition(cv::Mat &input)
{
	cv::Point2f output = cv::Point2f(0.0,0.0);
	float counter=0.0;
	for(int i=0; i<input.rows; i++)
	{
		for(int j=0; j<input.cols; j++)
		{
			if(input.at<unsigned char>(i,j) != 0)
			{
				output.y += i;
				output.x += j;
				counter++;
			}
		}
	}
	//update mean
	if(counter > 0)
	{
		output /= counter;
	}

	//return value
	return output;
}

cv::Point2f getMidPoint(cv::Point p1,cv::Point p2)
{
	cv::Point2f midPoint;
	midPoint = (p1+p2) / 2.0;
	return midPoint;
}

void rotateFrame(cv::Mat &input, int angle)
{
	if(angle == 270)
	{
		cv::transpose(input, input);
		cv::flip(input, input, 0);
	}
	else if(angle == 90)
	{
		cv::transpose(input, input);
		cv::flip(input, input, 1);
	}
	else if(angle == 180)
		cv::flip(input, input, 1);
	else if(angle != 0)
	{
		cv::Point2f pt(input.cols/2, input.rows/2);
		cv::Mat rot = cv::getRotationMatrix2D(pt, (double)angle, 1.0);
		cv::warpAffine(input, input, rot, cv::Size(input.cols, input.rows));
	}
}

cv::Rect rotateRect(cv::Rect input, double angle)
{
	cv::Rect output;
	cv::Point2f center = getCenter(input);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

	output.x = rot.at<double>(0,0)*input.x + rot.at<double>(0,1)*input.y
			+ rot.at<double>(0,2);
	output.x = rot.at<double>(1,0)*input.x + rot.at<double>(1,1)*input.y
			+ rot.at<double>(1,2);
	output.width = input.width;
	output.height = input.height;
	return output;
}
