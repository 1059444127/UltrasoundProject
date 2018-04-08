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
#include <stdlib.h>
#include <wchar.h>

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
CvPoint min_pt, max_pt;
int mouthcenterx, mouthcentery = 0;
int tonguex = 180;
int tonguey = 300;
int facewinx = 500;
int facewiny = 200;
bool hasInit = false;

bool stopFaceDetect = false;
bool manualMove = false;
bool manualScale = false;
bool manualRotate = false;

IplImage* org = 0;
IplImage* tmp = 0;
IplImage* dst = 0; 
CvRect rect; 

Mat imageROI;
Mat liveframe;
double m_scale;
int m_rotate;

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
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Scale);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Rotate);
}


BEGIN_MESSAGE_MAP(LiveCam, CDialogEx)
	ON_BN_CLICKED(IDOPEN, &LiveCam::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_CLOSE, &LiveCam::OnBnClickedClose)
	ON_BN_CLICKED(IDC_MOVE, &LiveCam::OnBnClickedMove)
	ON_BN_CLICKED(IDC_SCALE, &LiveCam::OnBnClickedScale)
	ON_BN_CLICKED(IDC_ROTATE, &LiveCam::OnBnClickedRotate)
END_MESSAGE_MAP()

BOOL LiveCam::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Edit_Scale.SetWindowText(_T("1"));
	m_Edit_Rotate.SetWindowText(_T("0"));
	flag = true;

	cvNamedWindow("view", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND) cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_OpenCamera)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);

	TCHAR minx[5];
	GetPrivateProfileString(_T("min_pt"), _T("x"), _T("") ,minx, 5, _T("..\\config.ini"));
	TCHAR miny[5];
	GetPrivateProfileString(_T("min_pt"), _T("y"), _T("") ,miny, 5, _T("..\\config.ini"));
	TCHAR maxx[5];
	GetPrivateProfileString(_T("max_pt"), _T("x"), _T("") ,maxx, 5, _T("..\\config.ini"));
	TCHAR maxy[5];
	GetPrivateProfileString(_T("max_pt"), _T("y"), _T("") ,maxy, 5, _T("..\\config.ini"));
	min_pt = Point(_tstoi(minx),_tstoi(miny));
	max_pt = Point(_tstoi(maxx),_tstoi(maxy));

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
	//	Mat liveframe;
	IplImage* videoframe=0;
	int delay = 30;
	char const* mysideProfile = "FullSizeRender.avi";
	//	char const* mysideProfile = "korea cropping profile.mov";

	char const* filename = "videoplayback.avi";







	//-- 1. Load the cascades
	face_cascade.load(face_cascade_name);
	mouth_cascade.load(mouth_cascade_name);

	//	str.Format("%d",m_nXPos);

	//WritePrivateProfileString(_T("min_pt"), _T("x"), _T("111"), _T("..\\config.ini"));
	//WritePrivateProfileString(_T("min_pt"), _T("y"), _T("41"), _T("..\\config.ini"));
	//WritePrivateProfileString(_T("max_pt"), _T("x"), _T("430"), _T("..\\config.ini"));
	//WritePrivateProfileString(_T("max_pt"), _T("y"), _T("215"), _T("..\\config.ini"));


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

			//inpaint_mask=cvLoadImage("watershed.png");
			//for test
			inpaint_mask=cvLoadImage("whatwahtwahtw.png");



			//smooth border
			//cvSmooth(inpaint_mask,inpaint_mask,CV_BLUR,3,3);

			cvCopy(videoframe, segImage,inpaint_mask); // 使用模板拷贝
			//	cvShowImage( "w", segImage ); // 显示抠图结果
			cvSaveImage("segImage.png", segImage); // 保存抠图结果

			//cvSaveImage("watershed.png",inpaint_mask); // 保存watershed

			//need to be predefined
			/*	min_pt = Point(111,41);
			max_pt = Point(430,215);*/


			Mat tb_dst = cv::cvarrToMat(segImage)(Rect(min_pt.x, min_pt.y, max_pt.x - min_pt.x,  max_pt.y - min_pt.y));
			//				printf("%d, %d, %d, %d",min_pt.x, min_pt.y, max_pt.x - min_pt.x,  max_pt.y - min_pt.y );


			if(manualRotate){


				cv::Point2f center( tb_dst.cols / 2, tb_dst.rows / 2);
				cv::Mat rot = cv::getRotationMatrix2D(center, m_rotate, 1);

				cv::Rect bbox = cv::RotatedRect(center, tb_dst.size(), m_rotate).boundingRect();


				rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
				rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

				cv::warpAffine(tb_dst, tb_dst, rot, bbox.size(), INTER_LINEAR, BORDER_CONSTANT, cvScalarAll(255)); 
			}

			cv::imwrite("transparentBlack.png", tb_dst);



			Mat logo = imread("transparentBlack.png");
			Mat mask = imread("transparentBlack.png",0); //need to be png

			double scale = 0.3;
			if(manualScale && m_scale!=0){
				scale *=m_scale;
			}
			Size ResImgSiz = Size(logo.cols*scale, logo.rows*scale);
			Mat ResImg = Mat(ResImgSiz, logo.type());

			resize(logo, ResImg, ResImgSiz, CV_INTER_CUBIC);

			Size MaskImgSiz = Size(mask.cols*scale, mask.rows*scale);
			Mat MaskImg = Mat(ResImgSiz, mask.type());

			resize(mask, MaskImg, ResImgSiz, CV_INTER_CUBIC);

			threshold(mask,mask,254,255,CV_THRESH_BINARY);
			Mat mask1 = 255 - MaskImg; 


			//imshow("img",mask1);
			//Mat imageROI;
			//		printf("mouthcenterx: %d + mouthcentery: %d -- framecoles: %d + framerows: %d ", mouthcenterx, mouthcentery, liveframe.cols, liveframe.rows );

			int roix = mouthcenterx>0?mouthcenterx: tonguex;
			int roiy = mouthcentery-ResImg.rows/2>0?mouthcentery-ResImg.rows/2: tonguey;
			if(mouthcenterx>0) stopFaceDetect = true;
			if(roiy + ResImg.rows < liveframe.rows && roix + ResImg.cols < liveframe.cols){
				imageROI = liveframe(Rect(roix, roiy, ResImg.cols, ResImg.rows));
			}
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

void on_mouse( int event, int x, int y, int flags, void* ustc)
{
	static CvPoint p={-1,-1};
	static int xrect;
	static int yrect;


	if(event==CV_EVENT_LBUTTONDOWN)
	{
		p=cvPoint(x,y);
		xrect=rect.x;
		yrect=rect.y;
	}
	else if(event==CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		int dx=x-p.x;
		int dy=y-p.y;
		if(dx>0){
			mouthcenterx +=2;
		}		
		if(dy>0){
			mouthcentery +=2;
		}
		if(dx<0){
			mouthcenterx -=2;
		}		
		if(dy<0){
			mouthcentery -=2;
		}
	}
}


void LiveCam::OnBnClickedMove()
{
	// TODO: Add your control notification handler code here
	manualMove = true;
	cvSetMouseCallback( "view", on_mouse, 0 );
}



void LiveCam::OnBnClickedScale()
{
	// TODO: Add your control notification handler code here
	CString c_scale;  
	m_Edit_Scale.GetWindowText(c_scale);
	m_scale = _wtof(c_scale);
	if(m_scale==0){
		MessageBox(_T("scale cannot be zero!"));
	}else{
		manualScale = true;
	}

}



void LiveCam::OnBnClickedRotate()
{
	// TODO: Add your control notification handler code here
	CString c_rotate;  
	m_Edit_Rotate.GetWindowText(c_rotate);
	m_rotate = _wtoi(c_rotate);
	if(m_rotate>360 || m_rotate<0){
		MessageBox(_T("Over degree range!"));
	}else{
		manualRotate = true;
	}
}
