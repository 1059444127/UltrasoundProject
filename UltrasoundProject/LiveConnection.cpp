#include "stdafx.h"
#include "LiveConnection.h"

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

#define IDC_M_BTN 8888

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
string face_cascade_name = "haarcascade_profileface.xml";
string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
string nose_cascade_name = "haarcascade_nosez.xml";
string mouth_cascade_name = "haarcascade_mcs_mouth.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier mouth_cascade;

char const* filename = "videoplayback.avi";
string window_name = "Capture - Face detection";
RNG rng(12345);



IplImage* inpaint_mask = 0;
IplImage* img0 = 0, *img = 0;
CvPoint prev_pt = {-1,-1};
CvPoint init_pt = {-1,-1};
CvPoint end_pt = {-1,-1};
CvPoint min_pt, max_pt;
bool hasCut = false;
int cropwidth, cropheight, xcor, ycor, mouthcenterx, mouthcentery;
Rect box; 
int tonguex = 180;
int tonguey = 300;
int facewinx = 500;
int facewiny = 200;
bool hasInit = false;
bool stopFaceDetect = false;

void on_mouse( int event, int x, int y, int flags, void* param)
{
	if( !img )
		return;
	if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
		prev_pt = cvPoint(-1,-1);
	else if( event == CV_EVENT_LBUTTONDOWN )
	{
		prev_pt = cvPoint(x,y);
		init_pt = cvPoint(x,y);
		min_pt = init_pt;
		max_pt = init_pt;
	}
	else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
	{
		CvPoint pt = cvPoint(x,y);
		if( prev_pt.x < 0 )
			prev_pt = pt;
		cvLine( inpaint_mask, prev_pt, pt, cvScalarAll(255), 2, 8, 0 ); // 模板上画
		cvLine( img, prev_pt, pt, cvScalarAll(255), 2, 8, 0 ); // 原图上画
		prev_pt = pt;
		min_pt.x = min(min_pt.x, pt.x);
		min_pt.y = min(min_pt.y, pt.y);
		max_pt.x = max(max_pt.x, pt.x);
		max_pt.y = max(max_pt.y, pt.y);
		cvShowImage( "ultrasound tongue", img );
	}
	if(event == CV_EVENT_RBUTTONUP)
	{
		cvFloodFill(inpaint_mask, cvPoint(x, y), cvScalarAll(255)); // 填充抠图模板
		cvShowImage( "watershed transform", inpaint_mask ); // 显示模板
		//	cvSaveImage("maskImg.png", img); // 保存在原图上画线后的图，如结果中第一个图
		hasCut = true;
	}
}

void removeBorder(char* stringName,IplImage* image)
{ 
	wchar_t wtext[20];
	mbstowcs(wtext, stringName, strlen(stringName)+1);//Plus null
	HWND win_handle = FindWindow(NULL, wtext);
	if (!win_handle)
	{
		printf("Failed FindWindow\n");
	}

	// Borderless
	SetWindowLong(win_handle, GWL_STYLE, GetWindowLong(win_handle, GWL_EXSTYLE) | WS_EX_TOPMOST);
	ShowWindow(win_handle, SW_SHOW);

}

BEGIN_MESSAGE_MAP(LiveConnection, CDialogEx)
	ON_COMMAND_RANGE(IDC_M_BTN,IDC_M_BTN,OnButtonClick)
END_MESSAGE_MAP()



/** @function main */
LiveConnection::LiveConnection(void)
{
	CvCapture* capture; 
	CvCapture* videoplayback;
	Mat frame;
	IplImage* videoframe=0;
	int delay = 30;

	char const* mysideProfile = "FullSizeRender.avi";

	//-- 1. Load the cascades
	face_cascade.load(face_cascade_name);
	eyes_cascade.load(eyes_cascade_name);
	nose_cascade.load(nose_cascade_name);
	mouth_cascade.load(mouth_cascade_name);
	/*
	//-- 2. Read the video stream
	capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera
	cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 60);
	*/
	capture = cvCaptureFromAVI(mysideProfile); // read AVI video
	videoplayback = cvCaptureFromAVI(filename); // read AVI video


	cvNamedWindow("Capture - Face detection", 0);
	HWND  hWnd = (HWND)cvGetWindowHandle("Capture - Face detection");
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				CButton* btn = new CButton;  
				DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;  
				btn->Create(_T("Manipulate"), dwStyle,CRect(60, 20, 160,60),pWnd,IDC_M_BTN);

	if( capture )
	{

		/*cvNamedWindow("ultrasound tongue", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("ultrasound tongue", 0, 0);
		cvSetWindowProperty("ultrasound tongue", CV_WINDOW_FULLSCREEN, CV_WINDOW_FULLSCREEN);*/

				 
		while( true )
		{
			frame = cvQueryFrame( capture );

			videoframe = cvQueryFrame( videoplayback );
			if(!videoframe)
			{
				break;
			}
			/*
			if(delay>=0&&cvWaitKey (delay)>=0)  
			{
			cvNamedWindow("watershed transform", 0);
			img = cvCloneImage( videoframe );
			inpaint_mask = cvCreateImage( cvGetSize(img), 8, 1 );
			cvSet( inpaint_mask, cvScalarAll(0)); // 将模板设为黑色
			//			cvShowImage( "image", img ); // 显示原图
			//	cvShowImage("w", videoframe);

			cvShowImage( "watershed transform", inpaint_mask ); // 显示模板原图
			cvSetMouseCallback( "ultrasound tongue", on_mouse, 0 ); 

			cvWaitKey(0);
			}

			if(!hasCut)
			{
			cvShowImage("ultrasound tongue", videoframe);
			}else{
			*/
			{
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
				printf("mouthcenterx: %d + mouthcentery: %d -- framecoles: %d + framerows: %d ", mouthcenterx, mouthcentery, frame.cols, frame.rows );
				int roix = mouthcenterx>0?mouthcenterx: tonguex;
				int roiy = mouthcentery-ResImg.rows/2>0?mouthcentery-ResImg.rows/2: tonguey;

				imageROI = frame(Rect(roix, roiy, ResImg.cols, ResImg.rows));
				ResImg.copyTo(imageROI,mask1);

				//		cvDestroyWindow("ultrasound tongue");
				//		cvDestroyWindow("watershed transform");
				//	stopFaceDetect = true;

			}
			//-- 3. Apply the classifier to the frame
			if( !frame.empty())
			{ 
				detectAndDisplay( frame ); 



			}else
			{ printf(" --(!) No captured frame -- Break!"); break; }

			int c = waitKey(10);
			if( (char)c == 'c' ) { break; }
		}
	}
	//return 0;
}

 void LiveConnection::OnButtonClick(UINT uID)
 {
	 MessageBox(_T("You hit me!"));

 }

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
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
			//	ellipse(frame, center, cv::Size(faces[i].width*0.8, faces[i].height*0.8), 0, 0, 360, Scalar(0, 255, 0), 4, 8, 0);

			Mat faceROI = frame_gray(faces[i]);
			//imshow("face", faceROI);
			std::vector<Rect> nose;
			std::vector<Rect> mouth;

			Rect mouthROI((int)faces[i].width / 4,(int)(faces[i].height/4)*3.2 ,(int)faces[i].width / 2, (int)faces[i].height / 5);
			Rect noseROI((int)faces[i].width / 4, (int)(faces[i].height / 2), (int)faces[i].width / 2, (int)faces[i].height / 4);
			//	imshow("nose",faceROI(noseROI));
			//	imshow("mouth", faceROI(mouthROI));

			//-- In each face, detect nose,mouth

			nose_cascade.detectMultiScale(faceROI(noseROI), nose, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
			for (size_t j = 0; j < nose.size(); j++)
			{
				cv::Point center(faces[i].x + nose[j].x +noseROI.x+ nose[j].width*0.5, faces[i].y + nose[j].y + noseROI.y+ nose[j].height*0.5);
				int radius = cvRound((nose[j].width + nose[j].height)*0.25);
				//		circle(frame, center, radius, Scalar(0, 255, 85), 3, 8, 0);
			}

			mouth_cascade.detectMultiScale(faceROI(mouthROI), mouth, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

			//	for (size_t j = 0; j < mouth.size(); j++)
			{
				cv::Point center(faces[i].x  + mouthROI.x, faces[i].y  + mouthROI.y );
				int radius = cvRound((mouthROI.width)*0.6);

				cv::Point topleft(faces[i].x+ mouthROI.tl().x, faces[i].y+ mouthROI.tl().y);
				cv::Point bottomright(faces[i].x+ mouthROI.br().x, faces[i].y+ mouthROI.br().y);

				//		rectangle(frame,topleft, bottomright,Scalar(85, 100, 255), 3, 8, 0);
				//circle(frame, center, radius, Scalar(85, 100, 255), 2, 8, 0);
				if(!hasInit){
					mouthcenterx = faces[i].x+(mouthROI.tl()+mouthROI.br()).x/2 - 20;
					mouthcentery = faces[i].y+(mouthROI.tl()+mouthROI.br()).y/2;
					hasInit = true;
				}


			}

		}
	}

	
	//-- Show what you got
	//imshow( window_name, frame );
	cvShowImage( "Capture - Face detection", &IplImage(frame) );
	//	resizeWindow(window_name,400,300);
	moveWindow(window_name, 500, 100);
	


}



LiveConnection::~LiveConnection(void)
{
}
