// LiveCam.cpp : implementation file
//

#include "stdafx.h"
#include "UltrasoundProject.h"
#include "LiveCam.h"
#include "afxdialogex.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgproc/types_c.h>   
#include <opencv2/core/core.hpp>          
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <cv.h>

#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;
using namespace cv;


/** Function Headers */
void detectAndDisplay( Mat frame, int x, int y );

/** Global variables */
string face_cascade_name = "haarcascade_profileface.xml";
string mouth_cascade_name = "haarcascade_mcs_mouth.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier mouth_cascade;
RNG rng(12345);

IplImage* inpaint_mask = 0;
IplImage* img = 0;
CvPoint prev_pt = {-1,-1};
CvPoint init_pt = {-1,-1};
CvPoint end_pt = {-1,-1};
CvPoint min_pt, max_pt;
bool hasCut = false;
int mouthcenterx, mouthcentery = 0;
Rect box; 
int tonguex = 180;
int tonguey = 300;
int facewinx = 500;
int facewiny = 200;
bool hasInit = false;
bool stopFaceDetect = false;



// LiveCam dialog

IMPLEMENT_DYNAMIC(LiveCam, CDialogEx)

	LiveCam::LiveCam(CWnd* pParent /*=NULL*/)
	: CDialogEx(LiveCam::IDD, pParent)
{

}

LiveCam::~LiveCam()
{
}

void LiveCam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LiveCam, CDialogEx)
	ON_BN_CLICKED(IDOPEN, &LiveCam::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_CLOSE, &LiveCam::OnBnClickedClose)
END_MESSAGE_MAP()

BOOL LiveCam::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	flag = true;

	cvNamedWindow("view", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND) cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_OpenCamera)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	return TRUE;

} 


// LiveCam message handlers


void LiveCam::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here

	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_OpenCamera);
	CDC* pDC = pWnd->GetDC(); 
	HDC hDC = pDC ->GetSafeHdc();
	pWnd->GetClientRect(&rect); //get picture control size
	int x = rect.Width();
	int y = rect.Height();

	CvCapture* capture; 
	CvCapture* videoplayback;
	Mat liveframe;
	IplImage* videoframe=0;
	int delay = 30;
	char const* mysideProfile = "FullSizeRender.avi";
	char const* filename = "videoplayback.avi";



	//-- 1. Load the cascades
	face_cascade.load(face_cascade_name);
	mouth_cascade.load(mouth_cascade_name);


	/*
	//-- 2. Read the video stream
	cv::VideoCapture capture(0);
	while(flag)
	{
	cv::Mat frame;
	capture>>frame;
	cv::Mat dst;
	cv::resize(frame, dst, cv::Size(x,y), 0, 0, 1);
	cv::imshow("view", dst);
	cv::waitKey(30);
	}

	*/

	capture = cvCaptureFromAVI(mysideProfile); // read AVI video
	videoplayback = cvCaptureFromAVI(filename); // read AVI video

	while( flag )
	{
		liveframe = cvQueryFrame( capture );
		videoframe = cvQueryFrame( videoplayback );

		if(videoframe &&  !liveframe.empty()){

			img = cvCloneImage( videoframe );
			IplImage *segImage=cvCreateImage(cvGetSize(img),8,3);
			//	cvZero(segImage); // 加这句可使结果第三个图的背景为黑色
			cvSet( segImage, cvScalarAll(255)); // 将模板设为白色
			videoframe = cvQueryFrame( videoplayback );


			inpaint_mask=cvLoadImage("watershed.png");



			cvCopy(videoframe,segImage, inpaint_mask); // 使用模板拷贝
			//	cvShowImage( "w", segImage ); // 显示抠图结果
			cvSaveImage("segImage.png",segImage); // 保存抠图结果

			//cvSaveImage("watershed.png",inpaint_mask); // 保存watershed


			min_pt = Point(111,41);
			max_pt = Point(430,215);
			Mat dst = cv::cvarrToMat(segImage)(Rect(min_pt.x, min_pt.y, max_pt.x - min_pt.x,  max_pt.y - min_pt.y));
			//				printf("%d, %d, %d, %d",min_pt.x, min_pt.y, max_pt.x - min_pt.x,  max_pt.y - min_pt.y );

			//namedWindow("dst");
			//imshow("dst", dst);
			cv::imwrite("transparentBlack.png", dst);



			Mat logo = imread("transparentBlack.png");
			Mat mask = imread("transparentBlack.png",0); //need to be png

			double scale = 0.3;
			Size ResImgSiz = Size(logo.cols*scale, logo.rows*scale);
			Mat ResImg = Mat(ResImgSiz, logo.type());

			resize(logo, ResImg, ResImgSiz, CV_INTER_CUBIC);

			Size MaskImgSiz = Size(mask.cols*scale, mask.rows*scale);
			Mat MaskImg = Mat(ResImgSiz, mask.type());

			resize(mask, MaskImg, ResImgSiz, CV_INTER_CUBIC);

			threshold(mask,mask,254,255,CV_THRESH_BINARY);
			Mat mask1 = 255 - MaskImg; 
			//imshow("img",mask1);
			Mat imageROI;
			//		printf("mouthcenterx: %d + mouthcentery: %d -- framecoles: %d + framerows: %d ", mouthcenterx, mouthcentery, liveframe.cols, liveframe.rows );

			int roix = mouthcenterx>0?mouthcenterx: tonguex;
			int roiy = mouthcentery-ResImg.rows/2>0?mouthcentery-ResImg.rows/2: tonguey;
			if(mouthcenterx>0) stopFaceDetect = true;
			imageROI = liveframe(Rect(roix, roiy, ResImg.cols, ResImg.rows));
			ResImg.copyTo(imageROI,mask1);

		}else{
			//if(videoframe){
			//	videoplayback = cvCaptureFromAVI(filename); //loop playing the video

			//}else if(liveframe.empty()){
			//	capture = cvCaptureFromAVI(mysideProfile); //loop playing the video

			//}
			break;
		}




		if(!liveframe.empty() &&videoframe){

			//cv::Mat dst;
			//cv::resize(liveframe, dst, cv::Size(x,y), 0, 0, 1);
			//cv::imshow("view", dst);

			//-- 3. Apply the classifier to the frame
			if(!stopFaceDetect){

				detectAndDisplay( liveframe, x, y);
			}else{
				cv::Mat dst;
				cv::resize(liveframe, dst, cv::Size(x,y), 0, 0, 1);
				cv::imshow("view", dst);
			}
			cv::waitKey(30);
		}else{
			//if(videoframe){
			//	videoplayback = cvCaptureFromAVI(filename); //loop playing the video

			//}else if(liveframe.empty()){
			//	capture = cvCaptureFromAVI(mysideProfile); //loop playing the video

			//}
			break;
		}
	}




}



void detectAndDisplay( Mat frame, int x, int y )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

	if(!stopFaceDetect){

		for (size_t i = 0; i < faces.size(); i++)
		{
			cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			//ellipse(frame, center, cv::Size(faces[i].width*0.8, faces[i].height*0.8), 0, 0, 360, Scalar(0, 255, 0), 4, 8, 0);

			Mat faceROI = frame_gray(faces[i]);
			//imshow("face", faceROI);
			//std::vector<Rect> nose;
			std::vector<Rect> mouth;

			Rect mouthROI((int)faces[i].width / 4,(int)(faces[i].height/4)*3.2 ,(int)faces[i].width / 2, (int)faces[i].height / 5);
			//Rect noseROI((int)faces[i].width / 4, (int)(faces[i].height / 2), (int)faces[i].width / 2, (int)faces[i].height / 4);
			//	imshow("nose",faceROI(noseROI));
			//imshow("mouth", faceROI(mouthROI));

			//-- In each face, detect mouth
			mouth_cascade.detectMultiScale(faceROI(mouthROI), mouth, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

			//	for (size_t j = 0; j < mouth.size(); j++)
			{
				cv::Point center(faces[i].x  + mouthROI.x, faces[i].y  + mouthROI.y );
				int radius = cvRound((mouthROI.width)*0.6);

				cv::Point topleft(faces[i].x+ mouthROI.tl().x, faces[i].y+ mouthROI.tl().y);
				cv::Point bottomright(faces[i].x+ mouthROI.br().x, faces[i].y+ mouthROI.br().y);

			//	rectangle(frame,topleft, bottomright,Scalar(85, 100, 255), 3, 8, 0);
				//circle(frame, center, radius, Scalar(85, 100, 255), 2, 8, 0);
				if(!hasInit){
					mouthcenterx = faces[i].x+(mouthROI.tl()+mouthROI.br()).x/2 - 20;
					mouthcentery = faces[i].y+(mouthROI.tl()+mouthROI.br()).y/2;
					hasInit = true;
				}


			}

		}
	}

	cv::Mat showframe;
	cv::resize(frame, showframe, cv::Size(x,y), 0, 0, 1);
	//-- Show what you got
	cv::imshow("view", showframe);
	//	moveWindow(window_name, 500, 100);

}


void LiveCam::OnBnClickedClose()
{
	// TODO: Add your control notification handler code here
	flag = false;
	CDialogEx::OnCancel();
}
