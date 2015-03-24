//#include "F:\projects abhilash\Project Herbicide applicator\green 6 cameras tentative\exp_2a/SerialClass.h"
#include <string>
#include <stdio.h>
// #include <conio.h>
#include "tserial.h"
#include "bot_control.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
//#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

using namespace cv;
using namespace std;
//serial comm;

// Serial to Arduino global declarations
  //int arduino_command;
  //Tserial *arduino_com;
  short MSBLSB = 0;
  unsigned char MSB = 0;
  unsigned char LSB = 0;
// Serial to Arduino global declarations

int iLowH = 38;
int iHighH = 75;

int iLowS = 150; 
int iHighS = 255;

int iLowV = 60;
int iHighV = 255;

int GP1,TP1,GP2,TP2,GP3,TP3,GP4,TP4,GP5,TP5,GP6,TP6;
double GI1,SA1,GI2,SA2,GI3,SA3,GI4,SA4,GI5,SA5,GI6,SA6;
float ar=500;  //application rate
float A=(0.61*0.3)*(10^(-4));
int test = 1;

char output[6]; 
Mat combineImage(Mat img1, Mat img2)
{
  Mat full(img1.rows, img1.cols * 2, CV_8UC3);
  

  Mat image_roi = full(Rect(0, 0, img1.cols, img1.rows));
  Mat image_roi_2 = full(Rect(img1.cols, 0, img1.cols, img1.rows));
  img1.copyTo(image_roi);
  img2.copyTo(image_roi_2);

  return full;
}
Mat combineImageVertical(Mat img1, Mat img2, Mat img3)
{
  Mat full(img1.rows * 3, img1.cols, CV_8UC3);
 
  Mat image_roi = full(Rect(0, 0, img1.cols, img1.rows));
  Mat image_roi_2 = full(Rect(0,  img1.rows, img1.cols, img1.rows));
  Mat image_roi_3 = full(Rect(0,  img1.rows * 2, img1.cols, img1.rows));
  img1.copyTo(image_roi);
  img2.copyTo(image_roi_2);
  img3.copyTo(image_roi_3);

  return full;
}

Mat getGreenIndex(String name, Mat imgOriginal1, int &x0)
{

  

 Mat imgHSV1;
 Mat imgLines1 = Mat::zeros( imgOriginal1.size(), CV_8UC3 );
 //imshow("Control", imgOriginal1); //show the original image
 cvtColor(imgOriginal1, imgHSV1, CV_BGR2HSV); //Convert the captured frame from BGR to HSV 
 Mat imgThresholded1;
 inRange(imgHSV1, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded1); //Threshold the image
     
 //morphological opening (removes small objects from the foreground)
 erode(imgThresholded1, imgThresholded1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
 dilate( imgThresholded1, imgThresholded1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  //morphological closing (removes small holes from the foreground)
 dilate( imgThresholded1, imgThresholded1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
 erode(imgThresholded1, imgThresholded1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
 

  int TotalNumberOfPixels1 = imgThresholded1.rows * imgThresholded1.cols;// to calculate green index
  int GreenPixels1 = countNonZero(imgThresholded1);
  GI1=(double)(GreenPixels1)/(double)(TotalNumberOfPixels1);
  SA1=-ar*GI1*A/100;
  char out;
   if(GI1>=0.05 && GI1<=.25) 
   {
	 out = '1';
     x0=1; 
   }
   else if(GI1>=.25 && GI1<=.50) 
   {   
	 out = '2';
     x0=2;
   }   
   else if(GI1>=.5 && GI1<=.75) 
   {
     out = '3';
     x0=3; 
   }
   else if(GI1>=.75 && GI1<=1) 
   {
     out = '4';
     x0=4;
   }
   else if(GI1 < 0.05)
   {
	 out = '0';
     x0 = 0;
   }

  //cout<<"\n\n\TP1 = "<<TotalNumberOfPixels1;
  //cout<<"\n\GP1 = "<<GreenPixels1;
  cout<<"\n\GI";
  cout<<test;
  cout<<" = "<<GI1;
  output[test-1] = out;
  test++;
  if(test == 7)
  {
	  test = 1;
  }
  //displayStatusBar(name, "text", 0);
  //cout<<"\n\SA1 = "<<SA1;
  //cout<<"\n\x0 = "<<x0;

  cvtColor( imgThresholded1, imgThresholded1, CV_GRAY2RGB );
  Mat full = combineImage(imgOriginal1, imgThresholded1);
  return full;

 imgOriginal1 = imgOriginal1 + imgLines1;
}
int main( string[], int argc, char** argv )
 {
  
  VideoCapture cap1(0),cap2(1),cap3(2),cap4(3),cap5(4),cap6(5); //capture the video from webcam

	int x0;
	int a0;
	int b0;
	int c0;
    int d0;
	int e0;
	

// serial to Arduino setup 
  Tserial *arduino_com;
  arduino_com = new Tserial();
  if (arduino_com!=0) 
  {
    arduino_com->connect("COM3", 9600, spNONE);
  } 
  
	

	char input = ' ';

  if ( !cap1.isOpened() )  // if not success, exit program
  {
       cout << "Cannot open the web cam" << endl;
       return -1;
  }


int height = 480;
int width = 640;

//cap1.set(CV_CAP_PROP_FRAME_WIDTH,width); cap1.set(CV_CAP_PROP_FRAME_HEIGHT,height);
//cap2.set(CV_CAP_PROP_FRAME_WIDTH,width); cap2.set(CV_CAP_PROP_FRAME_HEIGHT,height);
//cap3.set(CV_CAP_PROP_FRAME_WIDTH,width); cap3.set(CV_CAP_PROP_FRAME_HEIGHT,height); 
//cap4.set(CV_CAP_PROP_FRAME_WIDTH,width); cap4.set(CV_CAP_PROP_FRAME_HEIGHT,height); 
//cap5.set(CV_CAP_PROP_FRAME_WIDTH,width); cap5.set(CV_CAP_PROP_FRAME_HEIGHT,height); 
//cap6.set(CV_CAP_PROP_FRAME_WIDTH,width); cap6.set(CV_CAP_PROP_FRAME_HEIGHT,height); 

Mat imgOriginal1;
Mat imgOriginal2,imgOriginal3,imgOriginal4,imgOriginal5,imgOriginal6;

String name = "Cam1";
namedWindow(name, CV_WINDOW_AUTOSIZE); //create a window called "Control"

createTrackbar("LowH", name, &iLowH, 179); //Hue (0 - 179)
createTrackbar("HighH", name, &iHighH, 179);

createTrackbar("LowS", name, &iLowS, 255); //Saturation (0 - 255)
createTrackbar("HighS", name, &iHighS, 255);

createTrackbar("LowV", name, &iLowV, 255);//Value (0 - 255)
createTrackbar("HighV", name, &iHighV, 255);

//name = "Cam2";
Mat scene1, scene2, scene3, scene4, scene5, scene6;
while (true)
{     
    //char c= (char) arduino_com->getChar();
    //cout<<c;

    bool bSuccess = cap1.read(imgOriginal1); // read a new frame from video


    if (!bSuccess) //if not success, break loop
    {
         cout << "Cannot read a frame from video stream" << endl;
         break;
    }
    
    if(cap1.read(imgOriginal1))
      scene1 = getGreenIndex("Cam1", imgOriginal1, x0);


    if(cap2.read(imgOriginal2))
      scene2 = getGreenIndex("Cam1", imgOriginal2, a0);
    else
      scene2 =  Mat::zeros(scene1.size(), CV_8UC3 );
     
    if(cap3.read(imgOriginal3))
      scene3 = getGreenIndex("Cam1", imgOriginal3, b0);
    else
      scene3 =  Mat::zeros(scene1.size(), CV_8UC3 );

    if(cap4.read(imgOriginal4))
      scene4 = getGreenIndex("Cam1", imgOriginal4, c0);
    else
      scene4 =  Mat::zeros(scene1.size(), CV_8UC3 );

    if(cap5.read(imgOriginal5))
      scene5 = getGreenIndex("Cam1", imgOriginal5, d0);
    else
      scene5 =  Mat::zeros(scene1.size(), CV_8UC3 );

    if(cap6.read(imgOriginal6))
      scene6 = getGreenIndex("Cam1", imgOriginal6, e0);
    else
      scene6 =  Mat::zeros(scene1.size(), CV_8UC3 );


	if(scene1.rows == 240)
		resize(scene1, scene1, Size(scene1.cols*2, scene1.rows * 2));
	if(scene2.rows == 240)
		resize(scene2, scene2, Size(scene2.cols*2, scene2.rows * 2));
	if(scene3.rows == 240)
		resize(scene3, scene3, Size(scene3.cols*2, scene3.rows * 2));
	if(scene4.rows == 240)
		resize(scene4, scene4, Size(scene4.cols*2, scene4.rows * 2));
	if(scene5.rows == 240)
		resize(scene5, scene5, Size(scene5.cols*2, scene5.rows * 2));
	if(scene6.rows == 240)
		resize(scene6, scene6, Size(scene6.cols*2, scene6.rows * 2));
   
    Mat scene1_comb= combineImage(scene1, scene2);
    resize(scene1_comb, scene1_comb, Size(scene1_comb.cols/2, scene1_comb.rows/2));

    Mat scene2_comb= combineImage(scene3, scene4);
    resize(scene2_comb, scene2_comb, Size(scene2_comb.cols/2, scene2_comb.rows/2));

	

    Mat scene3_comb= combineImage(scene5, scene6);
    resize(scene3_comb, scene3_comb, Size(scene3_comb.cols/2, scene3_comb.rows/2));

    Mat scene_vertical = combineImageVertical(scene1_comb, scene2_comb, scene3_comb);
    imshow(name, scene_vertical);
    
	cout<<"\n";
	for(int i = 0; i < 6; i++)
	{
		cout<<output[i];
		arduino_com->sendChar(output[i]);
	}
    cout<<"\n";
 
  input=cvWaitKey(30);

 }


// Serial to Arduino - shutdown
    //arduino_com->disconnect();
     //delete arduino_com;
     //arduino_com = 0;
// Serial to Arduino - shutdown
   return 0;
}
	








