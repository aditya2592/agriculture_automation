#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include <stdio.h>

#include <iostream>
#include <string>

#include "tserial.h"
#include "bot_control.h"


using namespace std;
serial comm;


// Serial to Arduino global declarations
  int arduino_command;
  Tserial *arduino_com;
  short MSBLSB = 0;
  unsigned char MSB = 0;
  unsigned char LSB = 0;

// Serial to Arduino global declarations

  /*// serial to Arduino setup 
  arduino_command = new Tserial();
  if (arduino_com!=0) 
  {
    arduino_com->connect("COM3", 9600, spNONE);
  } */




const int alpha_slider_max = 640;
int alpha_slider;

const int alpha_slider_max_2 = 100;
int alpha_slider_2;

const int alpha_slider_max_3 = 320;
int alpha_slider_3;


void on_trackbar( int, void* )
{


}

void on_trackbar_2( int, void* )
{


}

void on_trackbar_3( int, void* )
{


}


using namespace cv;

int main(int, char**)
{
	// serial to Arduino setup 
  arduino_com = new Tserial();
  if (arduino_com!=0) {
       arduino_com->connect("COM5",9600, spNONE); } 
  // serial to Arduino setup 

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;

    namedWindow("edges", 1);

    alpha_slider = 150;
    alpha_slider_2 = 37;
	alpha_slider_3 = 10;

    char TrackbarName[50];
    sprintf( TrackbarName, "Black Pixels Threshold(%d)", alpha_slider_max );

    createTrackbar( TrackbarName, "edges", &alpha_slider, alpha_slider_max, on_trackbar );

    sprintf( TrackbarName, "Edge Threshold(%d)", alpha_slider_max_2 );

    createTrackbar( TrackbarName, "edges", &alpha_slider_2, alpha_slider_max_2, on_trackbar_2 );

	sprintf( TrackbarName, "Range Threshold(%d)", alpha_slider_max_3 );

    createTrackbar( TrackbarName, "edges", &alpha_slider_3, alpha_slider_max_3, on_trackbar_3 );

    for(;;)
    {
        Mat img;
        cap >> img; 

          int scale = 1;
          int delta = 0;
          Mat grad, src_gray;
          int ddepth = CV_16S;
          Mat grad_x, grad_y;
          Mat abs_grad_x, abs_grad_y;
          cvtColor( img, src_gray, CV_RGB2GRAY );

          Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
          convertScaleAbs( grad_x, abs_grad_x );

          Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
          convertScaleAbs( grad_y, abs_grad_y );

          addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );


          Mat thresh;
          double threshVal = alpha_slider_2;
          threshold(grad, thresh, threshVal, 255, CV_THRESH_BINARY_INV);
          medianBlur(thresh, thresh, 5);

          int nodes[550]; int count = 0;
          int node_start, node_end; double ratio_prev;

          Mat out = img.clone();

          for(int i = 0; i < thresh.cols; i ++)
          {
            double pix_count = 0, true_pix_count = 0;
            for(int j = 0; j < thresh.rows; j ++)
            {
              if(thresh.at<uchar>(j,i) == 0)
                true_pix_count++;
              pix_count++;
            }
            double ratio = true_pix_count/pix_count;
            double sliderVal = alpha_slider;
            double RATIO = sliderVal/img.rows;
            if(ratio > RATIO && ratio_prev < RATIO)
            {
              node_start = i;
            }
            if(ratio < RATIO && ratio_prev > RATIO)
            {
              node_end = i;
              nodes[count++] = (node_start + node_end)/2;



            }
            ratio_prev = ratio;
          }

          for(int i = 0; i < count; i ++)
          {
              for (int k = 0; k < out.rows; ++k)
              {
                out.at<Vec3b>(k,nodes[i]) = Vec3b(0,0,255);
              }
			  for (int k = 0; k < out.rows; ++k)
              {
				  out.at<Vec3b>(k,out.cols/2 + alpha_slider_3) = Vec3b(255,0,0);
              }
			  for (int k = 0; k < out.rows; ++k)
              {
				  out.at<Vec3b>(k,out.cols/2 - alpha_slider_3) = Vec3b(255,0,0);
              }

          }

		for(int i = 0; i < count; i ++)
          {
              
			  if(nodes[i] < (out.cols/2 + alpha_slider_3) && nodes[i] > (out.cols/2 - alpha_slider_3))
				{
				 //If location is near the middle of the image send signal
   				  arduino_com->sendChar('1');
  			 
				}
				else
   				  arduino_com->sendChar('0');
			}

        Mat full(480, 640, CV_8UC3);
        resize(full, full, Size(640, 240));
        resize(out, out, Size(320,240));
        cvtColor( thresh, thresh, CV_GRAY2RGB );
        resize(thresh, thresh, Size(320,240));
        Mat image_roi = full(Rect(0, 0, 320, 240));
        Mat image_roi_2 = full(Rect(320, 0, 320, 240));
        out.copyTo(image_roi_2);
        thresh.copyTo(image_roi);
        imshow("edges", full);
	
		
   
  

        if(waitKey(30) >= 0) break;
    }

	// Serial to Arduino - shutdown
     arduino_com->disconnect();
     delete arduino_com;
     arduino_com = 0;
// Serial to Arduino - shutdown
    return 0;
}
