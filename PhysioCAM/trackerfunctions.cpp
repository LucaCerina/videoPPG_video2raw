#include "trackerfunctions.h"
#include "geomfunctions.h"

void initTracker(cv::Point2f start_point,const cv::Mat &input, cv::Mat &initFrame, std::vector<cv::Point2f> *initPoints, cv::Point2f *drawpoint, cv::TermCriteria termcrit)
{
	cv:: Rect window = cv::Rect(start_point.x-initFrame.cols/2,start_point.y-initFrame.rows/2,
								initFrame.cols,initFrame.rows);
	cv::Size subPixWinSize(5,5);
	cv::Size winSize(TRDIM,TRDIM);
	float eigenDim = 3;

	input(window).copyTo(initFrame);
	//GaussianBlur(initFrame,initFrame,Size(5,5),TRSIGMA);
	do
	{
		goodFeaturesToTrack(initFrame, *initPoints, 100, 0.1, 7, cv::Mat(), eigenDim, 0, 0.04);
		eigenDim +=2;
	}while(initPoints->size() < 5);

	cornerSubPix(initFrame, *initPoints, subPixWinSize, cv::Size(-1,-1), termcrit);
	//cout << initPoints->size() << " " << eigenDim-2 << endl;

	*drawpoint = getCenter(initPoints[0]);
	//drawpoint->x += window.x;
	//drawpoint->y += window.y;
	*drawpoint += cv::Point2f(window.tl());
	//*drawpoint = start_point;
}

void LKTracker(const cv::Mat &input, cv::Mat &initFrame, const cv::Rect window, std::vector<cv::Point2f> *initPoints,
			   std::vector<cv::Point2f> *trackPoints, cv::Point2f *drawpoint, float threshold, cv::TermCriteria termcrit)
{
	cv::Size winSize(TRDIM,TRDIM);
	std::vector<uchar> status;
	std::vector<float> err;
	std::vector<cv::Point2f> goodPoints;
	cv::Point2f t_flow;
	cv::Mat next_frame;// = Mat(window.size(),CV_8UC1);

	input(window).copyTo(next_frame);
	GaussianBlur(next_frame,next_frame,cv::Size(5,5),TRSIGMA);
	//cout << initPoints[0] << endl;
	cv::calcOpticalFlowPyrLK(initFrame,next_frame,*initPoints,*trackPoints,status,err,winSize,
							 5,termcrit,0,threshold);
	//cout << trackPoints[0] << endl;
	/*for(int kk = 0; kk< status.size();kk++)
  {
	if((int)status[kk]==1)
	  goodPoints.push_back(trackPoints[0][kk]);
  }*/
	for(unsigned int kk=0;kk<trackPoints->size();kk++)
	{
		goodPoints.push_back(trackPoints[0][kk]-initPoints[0][kk]);
	}
	t_flow = getCenter(goodPoints);
	//cout << t_flow << endl;
	*drawpoint = getCenter(initPoints[0]);
	//*drawpoint = getCenter(window);
	drawpoint->x += t_flow.x + window.x;
	drawpoint->y += t_flow.y + window.y;
	std::swap(trackPoints[0],initPoints[0]);
	next_frame.copyTo(initFrame);
}

bool init_irisDetect(cv::Mat &input, cv::CascadeClassifier *haar_face, cv::CascadeClassifier *haar_reye,
					 cv::CascadeClassifier *haar_leye, cv::Point *r_eye_c, cv::Point *l_eye_c, cv::Rect *facePos)
{
	cv::Rect midface_r,midface_l;
	//Rect r_temp,l_temp;
	std::vector<cv::Rect> faces;
	std::vector<cv::Rect> reye_p,leye_p;
	//Mat ltempImg,rtempImg;
	cv::Point tempeye_r = cv::Point(0,0);
	cv::Point tempeye_l = cv::Point(0,0);

	//face identification
	haar_face->detectMultiScale(input,faces,1.2,3,cv::CASCADE_FIND_BIGGEST_OBJECT|cv::CASCADE_SCALE_IMAGE, cv::Size(30,30));
	std::cout << "faces size = " << faces.size() << std::endl;
	if(faces.size() != 0)
	{
		*facePos = faces[0];
		//midface positions
		midface_r = cv::Rect(faces[0].x,faces[0].y,(int)faces[0].width/2,faces[0].height);
		midface_l = cv::Rect(faces[0].x+(int)faces[0].width/2,faces[0].y,(int)faces[0].width/2,faces[0].height);
		//eyes detection
		haar_reye->detectMultiScale(input(midface_r),reye_p,1.1, 20, cv::CASCADE_FIND_BIGGEST_OBJECT|cv::CASCADE_SCALE_IMAGE, cv::Size(5,5));
		haar_leye->detectMultiScale(input(midface_l),leye_p,1.1, 20, cv::CASCADE_FIND_BIGGEST_OBJECT|cv::CASCADE_SCALE_IMAGE, cv::Size(5,5));
		//eyes position correction
		if(reye_p.size() != 0 && leye_p.size() != 0)
		{
			//correction
			reye_p[0] += midface_r.tl();
			leye_p[0] += midface_l.tl();
			//            //blur and color inversion
			//            medianBlur(255-input(leye_p[0]),ltempImg,5);
			//            medianBlur(255-input(reye_p[0]),rtempImg,5);
			//            //exclude eyebrows and convert to binary
			//            //exclude upper eye portion to exclude eyebrows
			//            l_temp = Rect(ltempImg.cols/4,ltempImg.rows/2,ltempImg.cols/2,ltempImg.rows/2);
			//            r_temp = Rect(rtempImg.cols/4,rtempImg.rows/2,rtempImg.cols/2,rtempImg.rows/2);
			//            ltempImg = (ltempImg(l_temp)>210)*255;
			//            rtempImg = (rtempImg(r_temp)>210)*255;
			//            //central point and coordinate correction
			//            tempeye_l = meanPosition(ltempImg);
			//            tempeye_l.x += leye_p[0].x + l_temp.x;
			//            tempeye_l.y += leye_p[0].y + l_temp.y;
			//            tempeye_r = meanPosition(rtempImg);
			//            tempeye_r.x += reye_p[0].x + r_temp.x;
			//            tempeye_r.y += reye_p[0].y + r_temp.y;
			tempeye_r = getCenter(reye_p[0]);
			tempeye_l = getCenter(leye_p[0]);
			//pointer return
			*r_eye_c = tempeye_r;
			*l_eye_c = tempeye_l;
			return true;
		}
		else
		{
			cv::namedWindow("Set eyes manually",cv::WINDOW_NORMAL);
			cv::imshow("Set eyes manually",input);
			do{
				setMouseCallback("Set eyes manually",getPoint,&(*r_eye_c));
				std::cout << "Select right eye, then press any key" << std::endl;
				cv::waitKey(0);
			}while(r_eye_c->x == 0 && r_eye_c->y == 0);
			do{
				setMouseCallback("Set eyes manually",getPoint,&(*l_eye_c));
				std::cout << "Select left eye, then press any key" << std::endl;
				cv::waitKey(0);
			}while(l_eye_c->x == 0 && l_eye_c->y == 0);
			std::cout << "Right eye " << *r_eye_c << " left eye " << *l_eye_c << std::endl;
			cv::destroyWindow("Set eyes manually");
			return true;
		}
	}
	return false;
}

void initFullTracker(cv::Mat &inputFrame, cv::Mat &trFrame, cv::Point r_eye_c, cv::Point l_eye_c, cv::Rect facePos,
					 cv::Rect *window_t, cv::Mat *prev_t, cv::Point2f *drawpoint, std::vector<cv::Point2f> *points,
					 cv::TermCriteria termcrit)
{
	cv::Point start_point;
	//points forehead
	start_point.x = r_eye_c.x;
	start_point.y = getMidPoint(r_eye_c,l_eye_c).y -0.45*abs(r_eye_c.x-l_eye_c.x);
	window_t[0] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[0] = inputFrame(window_t[0]);
	initTracker(start_point,inputFrame,prev_t[0],&points[0],&drawpoint[0],termcrit);

	start_point.x = l_eye_c.x;
	start_point.y = getMidPoint(r_eye_c,l_eye_c).y -0.45*abs(r_eye_c.x-l_eye_c.x);
	window_t[1] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[1] = inputFrame(window_t[1]);
	initTracker(start_point,inputFrame,prev_t[1],&points[2],&drawpoint[1],termcrit);

	//points nose
	start_point = getMidPoint(r_eye_c,l_eye_c);
	start_point.x -= 0.18*abs(r_eye_c.x-l_eye_c.x);
	window_t[2] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[2] = inputFrame(window_t[2]);
	initTracker(start_point,inputFrame,prev_t[2],&points[4],&drawpoint[2],termcrit);

	start_point = getMidPoint(r_eye_c,l_eye_c);
	start_point.x += 0.18*abs(r_eye_c.x-l_eye_c.x);
	window_t[3] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[3] = inputFrame(window_t[3]);
	initTracker(start_point,inputFrame,prev_t[3],&points[6],&drawpoint[3],termcrit);

	//points cheek
	start_point = r_eye_c;
	start_point.x -= 0.2*abs(r_eye_c.x-l_eye_c.x);
	start_point.y += 0.4*abs(r_eye_c.x-l_eye_c.x);
	window_t[4] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[4] = inputFrame(window_t[4]);
	initTracker(start_point,inputFrame,prev_t[4],&points[8],&drawpoint[4],termcrit);

	start_point = r_eye_c;
	start_point.x += 0.2*abs(r_eye_c.x-l_eye_c.x);
	start_point.y += 0.4*abs(r_eye_c.x-l_eye_c.x);
	window_t[5] = cv::Rect(start_point.x-prev_t[0].cols/2,start_point.y-prev_t[0].rows/2,prev_t[0].cols,prev_t[0].rows);
	prev_t[5] = inputFrame(window_t[5]);
	initTracker(start_point,inputFrame,prev_t[5],&points[10],&drawpoint[5],termcrit);

	//points central face
	start_point = getCenter(facePos);
	window_t[6] = cv::Rect(start_point.x-(int)(0.1*facePos.width),start_point.y-(int)(0.25*facePos.height),(int)(0.2*facePos.width),(int)(0.3333*facePos.height));
	inputFrame(window_t[6]).copyTo(trFrame);
	initTracker(getCenter(window_t[6]),inputFrame,trFrame,&points[12],&drawpoint[6],termcrit);

	std::cout << "init completed " << std::endl;
}

int vjInit(cv::CascadeClassifier *haar_face, cv::CascadeClassifier *haar_reye, cv::CascadeClassifier *haar_leye)
{
	//if(!haar_face->load("//usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml")) //cartella LINUX!
	if(!haar_face->load("3rdparty/haarcascade_frontalface_alt.xml"))
	{
		std::cout << "cannot load face detector" << std::endl;
		return 0;
	}
	//if(!haar_reye->load("//usr/local/share/OpenCV/haarcascades/haarcascade_righteye_2splits.xml")) //cartella LINUX!
	if(!haar_reye->load("3rdparty/haarcascade_righteye_2splits.xml"))
	{
		std::cout << "cannot load right eye detector" << std::endl;
		return 0;
	}
	//if(!haar_leye->load("//usr/local/share/OpenCV/haarcascades/haarcascade_lefteye_2splits.xml")) //cartella LINUX!
	if(!haar_leye->load("3rdparty/haarcascade_lefteye_2splits.xml"))
	{
		std::cout << "cannot load left eye detector" << std::endl;
		return 0;
	}
	std::cout << "Detectors loaded" << std::endl;
	return 1;
}

void getPoint(int event, int x, int y, int flags, void* ptr)
{
	if(event == cv::EVENT_LBUTTONDOWN)
	{
		cv::Point *p = (cv::Point *)ptr;
		p->x = x;
		p->y = y;
		std::cout << *p << std::endl;
	}
}
