#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>
#include<math.h>
using namespace cv;
using namespace std;


Point2f point;
static const double pi = 3.14159265358979323846;
bool addRemovePt = false;
inline static double square(int a)
{
	return a * a;
}

static void onMouse( int event, int x, int y, int /*flags*/, void* /*param*/ )
{
    if( event == EVENT_LBUTTONDOWN )
    {
        point = Point2f((float)x, (float)y);
        addRemovePt = true;
    }
}

int main( int argc, char** argv )
{


    VideoCapture cap;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    bool needToInit = true;
    bool nightMode = false;
    bool nowDraw=false;
    int initCounter=0;
    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        cap.open(argc == 2 ? argv[1][0] - '0' : 0);
    else if( argc == 2 )
        cap.open(argv[1]);

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }

    namedWindow( "LK Demo", 1 );
    setMouseCallback( "LK Demo", onMouse, 0 );

    Mat gray, prevGray, image;
    vector<Point2f> points[2];

    for(;;)
    {

    	Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        frame.copyTo(image);
        cvtColor(image, gray, COLOR_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

        if(++initCounter==50) {needToInit=true; initCounter=0;}

        if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
            addRemovePt = false;
            needToInit=0;
        }
        else if( !points[0].empty() )
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0, 0.001);
            size_t i, k;
            for( i = k = 0; i < points[1].size(); i++ )
            {
                if( addRemovePt )
                {
                    if( norm(point - points[1][i]) <= 5 )
                    {
                        addRemovePt = false;
                        continue;
                    }
                }

                // if( !status[i] )
                //     continue;

                points[1][k++] = points[1][i];
                //circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
    			Point p,q;
    			p.x = (int) points[0][i].x;
    			p.y = (int) points[0][i].y;
    			q.x = (int) points[1][i].x;
    			q.y = (int) points[1][i].y;;

    			double angle;		angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
    			double hypotenuse;	hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );

    			/* Here we lengthen the arrow by a factor of three. */
    			q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
    			q.y = (int) (p.y - 3 * hypotenuse * sin(angle));

    			/* Now we draw the main line of the arrow. */
    			/* "frame1" is the frame to draw on.
    			 * "p" is the point where the line begins.
    			 * "q" is the point where the line stops.
    			 * "CV_AA" means antialiased drawing.
    			 * "0" means no fractional bits in the center cooridinate or radius.
    			 */
    			cv::Scalar colorScalar = cv::Scalar( 255, 0, 0 );
    			Scalar line_color=cv::Scalar( 0, 0, 255 );
    			if(nowDraw)

    				line( image, p, q,colorScalar);
    			//cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
    			/* Now draw the tips of the arrow.  I do some scaling so that the
    			 * tips look proportional to the main line of the arrow.
    			 */

    			p.x = (int) (q.x + 9 * cos(angle + pi / 4));
    			p.y = (int) (q.y + 9 * sin(angle + pi / 4));
    			line( image, p, q, line_color);
    			p.x = (int) (q.x + 9 * cos(angle - pi / 4));
    			p.y = (int) (q.y + 9 * sin(angle - pi / 4));
    			line( image, p, q, line_color  );

            }
            points[1].resize(k);
        }

        /*if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }*/

        //needToInit = false;
        imshow("LK Demo", image);

        char c = (char)waitKey(10);

        if( c == 27 )
            break;
        switch( c )
        {
        case 'r':
            needToInit = true;
            break;
        case 'c':
            points[0].clear();
            points[1].clear();
            break;
        case 'n':
            nightMode = !nightMode;
            break;
        }
        //swapping points and frames
        std::swap(points[1], points[0]);
        cv::swap(prevGray, gray);
        nowDraw=true;
    }

    return 0;
}
