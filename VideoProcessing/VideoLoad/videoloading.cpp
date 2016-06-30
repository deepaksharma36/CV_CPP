#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>

int notmain( int argc, char** argv ) {
cv::namedWindow( "output", cv::WINDOW_AUTOSIZE );
cv::VideoCapture cap;
cap.open( "/home/deepak/Dropbox/CplusWorkPlace/VideoProcessing/data/Video_001.avi" );
//cap.open(0);
cv::Mat frame;
while( 1 ) {
cap >> frame;
if( !frame.data ) break;

cv::imshow( "output", frame );
if( cv::waitKey(33) >= 0 ) break;

}
return 0;
}
