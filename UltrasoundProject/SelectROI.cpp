#include "stdafx.h"
#include "SelectROI.h"

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
#include "atlstr.h"

using namespace std;
using namespace cv;



IplImage* roi_mask = 0;
IplImage* roi_img = 0;
bool hasCut = false;
CvPoint prev_pt = {-1,-1};
CvPoint init_pt = {-1,-1};
CvPoint min_temp, max_temp;

void on_mouseROI( int event, int x, int y, int flags, void* param)
{
	if( !roi_img )
		return;
	if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
		prev_pt = cvPoint(-1,-1);
	else if( event == CV_EVENT_LBUTTONDOWN )
	{
		prev_pt = cvPoint(x,y);
		init_pt = cvPoint(x,y);
		min_temp = init_pt;
		max_temp = init_pt;
	}
	else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
	{
		CvPoint pt = cvPoint(x,y);
		if( prev_pt.x < 0 )
			prev_pt = pt;
		cvLine( roi_mask, prev_pt, pt, cvScalarAll(255), 2, 8, 0 ); // 模板上画
		cvLine( roi_img, prev_pt, pt, cvScalarAll(255), 2, 8, 0 ); // 原图上画
		prev_pt = pt;
		min_temp.x = min(min_temp.x, pt.x);
		min_temp.y = min(min_temp.y, pt.y);
		max_temp.x = max(max_temp.x, pt.x);
		max_temp.y = max(max_temp.y, pt.y);
		cvShowImage( "ultrasound tongue", roi_img );
	}
	if(event == CV_EVENT_RBUTTONUP)
	{
		cvFloodFill(roi_mask, cvPoint(x, y), cvScalarAll(255)); // 填充抠图模板
	//	cvShowImage( "roi mask", roi_mask ); // 显示模板
		cvSaveImage("whatwahtwahtw.png",roi_mask);
		//	cvSaveImage("maskImg.png", img); // 保存在原图上画线后的图，如结果中第一个图

		hasCut = true;

		TCHAR minx[10];
		TCHAR miny[10];
		TCHAR maxx[10];
		TCHAR maxy[10];

		_itow_s(min_temp.x,minx,10);
		_itow_s(min_temp.y,miny,10);
		_itow_s(max_temp.x,maxx,10);
		_itow_s(max_temp.y,maxy,10);


		WritePrivateProfileString(_T("min_pt"), _T("x"), minx, _T("..\\config.ini"));
		WritePrivateProfileString(_T("min_pt"), _T("y"), miny, _T("..\\config.ini"));
		WritePrivateProfileString(_T("max_pt"), _T("x"), maxx, _T("..\\config.ini"));
		WritePrivateProfileString(_T("max_pt"), _T("y"), maxy, _T("..\\config.ini"));

		MessageBox(NULL, _T("Successful!"), _T("Updated ROI"), MB_OK);
	}
}



SelectROI::SelectROI(void)
{
	char const* filename = "videoplayback.avi";
	CvCapture* videoplayback;
	IplImage* videoframe=0;
	int delay = 30;

	hasCut = false;
	/*
	//-- 2. Read the video stream
	capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera
	cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 60);
	*/
	videoplayback = cvCaptureFromAVI(filename); // read AVI video




	cvNamedWindow("ultrasound tongue", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("ultrasound tongue", 0, 0);
	cvSetWindowProperty("ultrasound tongue", CV_WINDOW_FULLSCREEN, CV_WINDOW_FULLSCREEN);


	while( true )
	{

		videoframe = cvQueryFrame( videoplayback );
		if(!videoframe)
		{
			break;
		}

		if(!hasCut)
		{
			cvShowImage("ultrasound tongue", videoframe);
		}

		if(delay>=0&&cvWaitKey (delay)>=0)  
		{
			//cvNamedWindow("watershed transform", 0);
			roi_img = cvCloneImage( videoframe );
			roi_mask = cvCreateImage( cvGetSize(roi_img), 8, 1 );
			cvSet( roi_mask, cvScalarAll(0)); // 将模板设为黑色
			//			cvShowImage( "image", img ); // 显示原图
			//cvShowImage("w", videoframe);

		//	cvShowImage( "watershed transform", roi_mask ); // 显示模板原图
			cvSetMouseCallback( "ultrasound tongue", on_mouseROI, 0 ); 

			cvWaitKey(0);
		}

	}
}



SelectROI::~SelectROI(void)
{
}
