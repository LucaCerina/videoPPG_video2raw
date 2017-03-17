/*Program to extract mean signal from fore,nose,cheek.
  argv[1] activate the video output verbose.
  argv[2] activate the tracker initialization division,
  argv[3] activate the subDivision of frames
  argv[4] filename*/

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <numeric>

#ifdef __unix__
extern "C"
{
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
};
#endif

#include "geomfunctions.h"
#include "roifunctions.h"
#include "trackerfunctions.h"

#define TRPOINTS 7 //# of tracking points

using namespace std;
using namespace cv;

/*void getVidRotation(std::string filename)
{
    AVFormatContext *ptFormatCtx = NULL;
    AVDictionaryEntry *tag = NULL;

    av_register_all();

    if(avformat_open_input(&ptFormatCtx,filename.c_str(),NULL,NULL)!=0)
        return;

    //if(avformat_find_stream_info(ptFormatCtx,&ptFormatCtx->metadata)<0)
      //  return;

    while ((tag = av_dict_get(ptFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        cout << tag->key << " = " << tag->value << endl;

    int val = av_dict_count(ptFormatCtx->metadata);

    cout << "tag valido" << val << endl;

    std::string rotation(tag->value);

    cout << "rotation is: " << rotation << endl;

    avformat_close_input(&ptFormatCtx);
}*/

int main( int argc, const char* argv[])
{
    //configuration from arguments
    //modulus for frequency subsampling
    int subFreq = 1;
    if(argc>3 && atoi(argv[3])!=0)
        subFreq = atoi(argv[3]);
    //modulus for tracker reinitialization
    int subInit = 10;
    if(argc>2 && atoi(argv[2])!=0)
        subInit = atoi(argv[2]);
    //visualization
    int videoResp = 0;
    if(argc>1)
        videoResp = atoi(argv[1]);
    int videoRespStep = subInit;

    //info video
    double fps=0.0;
    int nFrames = 0;
    int videoPos = 0;
    Size frameSize;

    //file IO
    string videoname,csvname,outname;
    fstream file_output;

    //acquisition matrices
    Mat captureFrame;
    Mat grayFrame;
    int f_t[] = {0,0}; //use of mixChannels function instead of cvtcolor
    //matrices used for flicker intensity reduction
    Mat t_frame, p_frame;
    Mat outFrame;

    //face and eye detection
    CascadeClassifier haar_face, haar_reye, haar_leye;
    vjInit(&haar_face, &haar_reye, &haar_leye);
    vector<Rect> faceTemp;
    faceTemp.push_back(Rect(0,0,1,1)); //temporary ROI initialization
    Rect facePos = Rect(0,0,0,0);

    //ROIs
    Point r_eye_c = Point(0,0);
    Point l_eye_c = Point(0,0); //position of eyes
    Rect nose_p; //Rect of ROIs
    Rect cheek_p;
    Rect fore_p;

    //signal vectors
    vector<Scalar> fore_avg_rgb(nFrames);
    vector<Scalar> nose_avg_rgb(nFrames);
    vector<Scalar> cheek_avg_rgb(nFrames);
    vector<Scalar> fore_avg_ycc(nFrames);
    vector<Scalar> nose_avg_ycc(nFrames);
    vector<Scalar> cheek_avg_ycc(nFrames);

    //motion vectors
    vector<Point2f> fore_motion(nFrames);
    vector<Point2f> nose_motion(nFrames);
    vector<Point2f> cheek_motion(nFrames);
    vector<Point2f> ground_motion(nFrames);
    vector<unsigned int> motion_state(nFrames);
    unsigned int currentState = 1;
    Point stateCenter;

    //KLT tracking
    TermCriteria termcrit(TermCriteria::EPS|TermCriteria::COUNT, 20, 0.01);
    Rect window_t[TRPOINTS];
    Size winSize(TRDIM,TRDIM);
    Mat prev_t[TRPOINTS];
    fill_n(prev_t,TRPOINTS, Mat(winSize,CV_8UC1));
    Mat trFrame;
    Point2f drawpoint[TRPOINTS];
    Point2f tempPoint(0,0);
    int p = 0; //number of points iterator
    vector<Point2f> points[2*TRPOINTS];

    //framerate of execution
    double tStart;
    double currFps =0.0;
    double tInit;

    //video selection
    if(argc>4)
    {
        videoname = argv[4];
        cout << videoname << endl;
    }
    else
    {
        cout << "insert video file name" << endl;
        cin >> videoname;
    }
    //CSV file selection
    if(argc>5)
    {
        csvname = argv[5];
        cout << csvname << endl;
    }
    else
    {
        cout << "insert CSV output file name" << endl;
        cin >> csvname;
    }
    //video rotation (handles smartphone videos)
    int rotation = 0;
    if(argc>6)
    {
        rotation = atoi(argv[6]);
    }
    else
    {
        cout << "insert known camera rotation" << endl;
        cin >> rotation;
    }


    //video open
    VideoCapture cap(videoname);
    if(!cap.isOpened())
    {
        cout << "cannot open video file" << endl;
        return 1;
    }
    else
    {
        cout << "video file opened" << endl;
    }

    //TEMP VideoWriter vidOut("video prova.avi",VideoWriter::fourcc('M','J','P','G'),cap.get(CAP_PROP_FPS),grayFrame.size(),0);

    //obtain video info
    //obtain number of frames
    int j=0;
    while(cap.read(captureFrame))
    {
        j++;
    }
    cap.release();
    cap.open(videoname);
    nFrames = j-1;
    //fps
    fps = cap.get(CAP_PROP_FPS);
    cout << "Frame per seconds: " << fps <<endl;
    //nFrames
    //nFrames = (int)cap.get(CAP_PROP_FRAME_COUNT);
    cout << "Total frames: " << nFrames <<endl;
    //framesize
    frameSize.width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
    frameSize.height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
    if(rotation != 180 && rotation !=0)
        swap(frameSize.width, frameSize.height);
    grayFrame = Mat(frameSize, CV_8UC1);
    trFrame = Mat(frameSize, CV_8UC1);
    //initialization
    subInit = 1800; //(int)nFrames / subInit; FORCED TO 1800 frames
    cout << "Tracker initialization every " << subInit << " frames" << endl;
    //REDUNDANT reset to starting frame
    cap.set(CAP_PROP_POS_FRAMES, -1);

    //video elaboration

    tInit = (double) getTickCount();
    //for(int j=0; videoPos<nFrames; j++)
    for(j=0; j<nFrames;j++)
    {
        tStart = (double) getTickCount();
        //read a frame
        if(!cap.read(captureFrame))
        {
            cout << "cannot read from video file" << endl;
            cout << cap.get(CAP_PROP_POS_FRAMES) << endl;
            break;
        }
        videoPos = (int)cap.get(CAP_PROP_POS_FRAMES);
        cout << "frame: " << videoPos << "|" << nFrames << endl;
        //angle rotation
        rotateFrame(captureFrame, rotation);
        //color conversion
        mixChannels(&captureFrame, 1, &grayFrame, 1, f_t, 1);
        //flicker reduction
        if(j>0)
        {
            grayFrame.copyTo(t_frame);
            //grayFrame = 0.5*(grayFrame+p_frame);
            grayFrame = grayFrame/2.0 + p_frame/2.0;
            t_frame.copyTo(p_frame);
        }
        else
        {
            //flicker data
            p_frame = grayFrame.clone();
        }

        //first frame operations
        if(j==0 || j%subInit==0)
        {
            cout << "Tracker re-initialization at frame: " << videoPos << endl;
            //			if(init_irisDetect(grayFrame, &haar_face, &haar_reye, &haar_leye, &r_eye_c, &l_eye_c, &facePos))
            //			{
            //				cout << "Eyes at " << r_eye_c << " " << l_eye_c << endl;
            //				initFullTracker(grayFrame, trFrame, r_eye_c, l_eye_c, facePos, window_t, prev_t, drawpoint, points, termcrit);
            //				tempPoint = Point(drawpoint[6]);
            //			}
            //			else
            //			{
            //				cout << "Face or eyes could not be found." << endl;
            //				break;
            //			}
            while(!init_irisDetect(grayFrame, &haar_face, &haar_reye, &haar_leye, &r_eye_c, &l_eye_c, &facePos))
            {
                if(!cap.read(captureFrame))
                    break;
                videoPos = (int)cap.get(CAP_PROP_POS_FRAMES);
                rotateFrame(captureFrame, rotation);
                //color conversion
                mixChannels(&captureFrame, 1, &grayFrame, 1, f_t, 1);
                cout << "Tracker re-initialization at frame: " << videoPos << endl;
            }
            if(facePos.area() >0)
            {
                initFullTracker(grayFrame, trFrame, r_eye_c, l_eye_c, facePos, window_t, prev_t, drawpoint, points, termcrit);
                tempPoint = Point(drawpoint[6]);
            }
            else
                break;

            //initialize ROI state control
            stateCenter = Point(drawpoint[6]);
        }

        //MOTION FLOW TRACKING
        if(j>0 && j%subFreq==0 && j%subInit!=0)
        {
            //points update
            for(p = 0; p<TRPOINTS*2-2; p=p+2)
            {
                LKTracker(grayFrame, prev_t[p/2], window_t[p/2], &points[p], &points[p+1], &drawpoint[p/2], 0.003, termcrit);
            }
            LKTracker(grayFrame, trFrame, window_t[6], &points[12], &points[13], &drawpoint[6], 0.001, termcrit);
            //search window correction
            for(p=0; p<TRPOINTS; p++)
            {
                window_t[p].x += Point(drawpoint[6]).x - Point(tempPoint).x;
                window_t[p].y += Point(drawpoint[6]).y - Point(tempPoint).y;
                window_t[p] = adjustROI(window_t[p], captureFrame.size());
            }
            tempPoint = drawpoint[6];
        }

        //update ROIs
        nose_p = getNoseDim(Point(drawpoint[2]), Point(drawpoint[3]), facePos);
        cheek_p = getCheekDim(Point(drawpoint[4]), Point(drawpoint[5]));
        fore_p = getForeDim(Point(drawpoint[0]), Point(drawpoint[1]));

        //signal extraction
        if(j%subFreq==0)
        {
            //color signal
            getSignalValue(captureFrame, fore_p, &fore_avg_rgb, &fore_avg_ycc);
            getSignalValue(captureFrame, nose_p, &nose_avg_rgb, &nose_avg_ycc);
            getSignalValue(captureFrame, cheek_p, &cheek_avg_rgb, &cheek_avg_ycc);
            //motion signal
            fore_motion.push_back(getCenter(fore_p));
            nose_motion.push_back(getCenter(nose_p));
            cheek_motion.push_back(getCenter(cheek_p));
            ground_motion.push_back(drawpoint[6]);
            //update ROI state
            updateROIState(ground_motion.back(), &motion_state, &stateCenter, &currentState);
        }

        //draw the tracking results
        if(videoResp==1)//verbose execution
        {
            if(j%videoRespStep==0)
            {
                grayFrame.copyTo(outFrame);
                for(int kk=0; kk<TRPOINTS; kk++)
                {
                    circle(outFrame, drawpoint[kk], 3, Scalar(0,255,255,0), -1);
                    rectangle(outFrame, window_t[kk], Scalar(0,255,0,0), 2, 8, 0);
                }
                circle(outFrame, r_eye_c, 2, Scalar(0,255,255,0), -1);
                circle(outFrame, l_eye_c, 2, Scalar(0,255,255,0), -1);
                rectangle(outFrame, facePos, Scalar(0,255,0,0), 1, 8, 0);
                rectangle(outFrame, nose_p, Scalar(0,255,0,0), 1, 8, 0);
                rectangle(outFrame, cheek_p, Scalar(0,255,0,0), 1, 8, 0);
                rectangle(outFrame, fore_p, Scalar(0,255,0,0), 1, 8, 0);
                imshow("drawpoints", outFrame);
                //vidOut.write(outFrame);
            }

            //wait for ESC to end the analysis
            if(waitKey(1)==27)
            {
                cout << "Esc pressed" << endl;
                break;
            }

        }

        //output mean framerate
        tStart = ((double)getTickCount() - tStart) / getTickFrequency();
        currFps += 1.0/tStart;
        cout << "Currfps " << currFps/(j+1) << endl;
        //progress frame
        //j++;
    }

    //if(videoPos == nFrames)
    //{
        tInit = ((double)getTickCount() - tInit) / getTickFrequency();
        cout << "Video analysis completed" << endl;
        cout << "Timing: " << tInit << " seconds." << endl;
    //}
//    else
//    {
//        cout << "Execution ended before the last frame.  Exiting program." << endl;
//        return 1;
//    }

    //CSV signal output
    cout << "CSV file opening" << endl;
    //RGB
    outname.append(csvname);
    outname.append("_rgb.csv");
    cout << "Output file RGB: " << outname << endl;
    cout << fore_avg_rgb.size() << " samples" << endl;
    file_output.open(outname.c_str(), fstream::out);
    for(unsigned int k=0; k< fore_avg_rgb.size(); k++)
    {
        file_output << fore_avg_rgb[k][0] << "," << fore_avg_rgb[k][1] << "," << fore_avg_rgb[k][2] << ",";
        file_output << nose_avg_rgb[k][0] << "," << nose_avg_rgb[k][1] << "," << nose_avg_rgb[k][2] << ",";
        file_output << cheek_avg_rgb[k][0] << "," << cheek_avg_rgb[k][1] << "," << cheek_avg_rgb[k][2] << "\n";
    }
    //	file_output.close();
    //	//YCC
    //	outname.clear();
    //	outname.append(csvname);
    //	outname.append("_ycc.csv");
    //	cout << "Output file YCrCb: " << outname << endl;
    //	cout << fore_avg_ycc.size() << " samples" << endl;
    //	file_output.open(outname.c_str(), fstream::out);
    //	for(unsigned int k=0; k< fore_avg_ycc.size(); k++)
    //	{
    //		file_output << fore_avg_ycc[k][0] << "," << fore_avg_ycc[k][1] << "," << fore_avg_ycc[k][2] << ",";
    //		file_output << nose_avg_ycc[k][0] << "," << nose_avg_ycc[k][1] << "," << nose_avg_ycc[k][2] << ",";
    //		file_output << cheek_avg_ycc[k][0] << "," << cheek_avg_ycc[k][1] << "," << cheek_avg_ycc[k][2] << "\n";
    //	}
    file_output.close();
    //motion record
    outname.clear();
    outname.append(csvname);
    outname.append("_motion.csv");
    cout << "Output file motion: " << outname << endl;
    cout << fore_motion.size() << " samples" << endl;
    file_output.open(outname.c_str(), fstream::out);
    for(unsigned int k=0; k< fore_motion.size(); k++)
    {
        file_output << fore_motion[k].x << "," << fore_motion[k].y << ",";
        file_output << nose_motion[k].x << "," << nose_motion[k].y << ",";
        file_output << cheek_motion[k].x << "," << cheek_motion[k].y << ",";
        file_output << ground_motion[k].x << "," << ground_motion[k].y << ",";
        file_output << motion_state[k] << "\n";
    }
    file_output.close();
    //output number of frames
    file_output.open("tempFrames.txt",fstream::out);
    file_output << nFrames;
    file_output.close();
    if(argc>1 && atoi(argv[1]) == 1)
    {
        destroyWindow("output");
    }

    cout << "computation completed" << endl;
    cap.release();
    //vidOut.release();
    return 0;
}



