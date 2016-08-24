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
    for(uint i = 0; i<inputPoints.size(); i++)
    {
        //center.x += inputPoints[i].x;
        //center.y += inputPoints[i].y;
        center += inputPoints[i];
    }
    center.x = center.x/inputPoints.size();
    center.y = center.y/inputPoints.size();
    return center;
}

cv::Point2f meanPosition(cv::Mat &input)
{
    cv::Point2f output = cv::Point2f(0.0,0.0);
    float counter=0.0;
    for(int i=0; i<input.rows;i++)
    {
        for(int j=0; j < input.cols;j++)
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
        //output.x /= counter;
        //output.y /= counter;
        output /= counter;
    }

    //return value
    return output;
}

cv::Point2f getMidPoint(cv::Point p1,cv::Point p2)
{
    cv::Point2f midPoint;
    //midPoint.x = (p1.x+p2.x)/2.0;
    //midPoint.y = (p1.y+p2.y)/2.0;
    midPoint = (p1+p2)/2.0;
    return midPoint;
}

void rotateFrame(cv::Mat &input, int angle)
{
    if(angle == 270)
    {
        cv::transpose(input,input);
        cv::flip(input,input,0);
    }
    else if(angle == 90)
    {
        cv::transpose(input,input);
        cv::flip(input,input,1);
    }
    else if(angle == 180)
        cv::flip(input,input,1);
}
