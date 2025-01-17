#include "fileSource.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/background_segm.hpp"

#include <string>
#include <unistd.h>

#define SMIN 30
#define VMIN 10
#define VMAX 256

using namespace std;
using namespace cv;


Point2f point;
bool addRemovePt = false;

Mat roiFrame, roiAux;
Rect roiBox;
vector<Point> roiPts;		//points defining a ROI
int cnt = 0;				//mouse clicks counter

//EX2
bool type;
int method;
int threshValue;
string filename = "vid/vid1.mp4";
Mat frame;



static void roiSelection(int event, int x, int y, int, void*) {
	// This function waits for user to select points that define a ROI
	switch (event) {
		case CV_EVENT_LBUTTONDOWN:
			cnt++;

			//point selection and ROI definition
			if (cnt <= 2) {
				//point selection and display
				Point selected = Point(x, y);
				roiPts.push_back(selected);

				circle(roiFrame, selected, 5, Scalar(0, 0, 255), 1);
				if (cnt == 2) {
					//ROI display and storage
					rectangle(roiFrame, roiPts[0], roiPts[1], Scalar(255, 0, 0), 2);
					roiBox = Rect(roiPts[0], roiPts[1]);
				}
			}
			else {
				//flushes point vector
				roiFrame = roiAux.clone();
				roiPts.clear();
				cnt = 0;
			}

			imshow("Object Tracking", roiFrame);
	}
}


int ex1(){

//
//  VideoCapture cap(0); // open the default camera
//  if(!cap.isOpened())  // check if we succeeded
//     return -1;
//
//  Mat flow;
//
//  // some faster than mat image container
//    Mat flowUmat, prevgray;
//
//     for (;;)
//     {
//
//       Mat frame;
//       cap >> frame;
//
//       //Mat img;
//       Mat original;
//
//       // capture frame from video file
//       cap.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);
//       resize(frame, frame, Size(640, 480));
//
//       // save original for later
//       frame.copyTo(original);
//
//       // just make current frame gray
//       cvtColor(frame, frame, COLOR_BGR2GRAY);
//
//       if (prevgray.empty() == false ) {
//
//        // calculate optical flow
//        calcOpticalFlowFarneback(prevgray, frame, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
//        // copy Umat container to standard Mat
//        flowUmat.copyTo(flow);
//
//
//               // By y += 5, x += 5 you can specify the grid
//        for (int y = 0; y < original.rows; y += 5) {
//         for (int x = 0; x < original.cols; x += 5)
//         {
//                  // get the flow from y, x position * 10 for better visibility
//            const Point2f flowatxy = flow.at<Point2f>(y, x) * 10;
//                         // draw line at flow direction
//            line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(0,0,255));
//                                                             // draw initial point
//            circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);
//          }
//
//         }
//         // draw the results
//        namedWindow("prew", WINDOW_AUTOSIZE);
//        imshow("prew", original);
//
//        // fill previous image again
//        frame.copyTo(prevgray);
//
//       }
//       else {
//
//        // fill previous image in case prevgray.empty() == true
//        frame.copyTo(prevgray);
//
//       }
//
//       int key1 = waitKey(20);
//     }
}



int ex2(){
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach

	//opens default camera
	VideoCapture cap(0);
  VideoCapture capture(filename);

	//check if success
	if (!cap.isOpened())
		{
			cout << "Error when reading camera!" << endl;
			return -1;
		}
  if(!capture.isOpened())
	{
		cout << "Error when reading video file!" << endl;
		return -1;
	}

	//presents options to user
	system("clear");		//clears terminal window
	cout << endl << "---------------" << endl;
	cout << " Choose Video Capture[0] or load a Video File[1]" << endl;
	cout << "---------------" << endl << endl;
	cin >> type;

  if(type)
  {
    namedWindow( "Video File", 1);
    for( ; ; )
    {
      capture >> frame;
      if(frame.empty())
        break;
      imshow("Video File", frame);

			//update the background model
			pMOG2->apply(frame, fgMaskMOG2);
			//get the frame number and write it on the current frame
			stringstream ss;
			rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
			          cv::Scalar(255,255,255), -1);
			ss << capture.get(CAP_PROP_POS_FRAMES);
			string frameNumberString = ss.str();
			putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
			        FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
			//show the current frame and the fg masks
			imshow("Video File Subtracted", fgMaskMOG2);

      waitKey(20);
    }
    waitKey(0);
  }
	else
	{
		namedWindow("Video Capture", 1);
		for(;;)
		{
			cap >> frame;
			if(frame.empty())
				break;
			imshow("Video Capture", frame);
			pMOG2->apply(frame, fgMaskMOG2);
			imshow("Video Capture Subtracted", fgMaskMOG2);
			if (waitKey(5) >= 0) break;				//waits 30ms for program to render next frame
		}
		waitKey(0);
}


    return 0;

}

static void onMouse( int event, int x, int y, int /*flags*/, void* /*param*/ )
{
    if( event == EVENT_LBUTTONDOWN )
    {
        point = Point2f((float)x, (float)y);
        addRemovePt = true;
    }
}


int ex3(){ VideoCapture cap;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    bool needToInit = false;
    bool nightMode = false;

    string input = "";

    if( input.empty() )
        cap.open(0);
    else if( input.size() == 1 && isdigit(input[0]) )
        cap.open(input[0] - '0');
    else
        cap.open(input);

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }

    namedWindow( "LK Demo", 1 );
    setMouseCallback( "LK Demo", onMouse, 0 );

    Mat gray, prevGray, image, frame;
    vector<Point2f> points[2];

    for(;;)
    {
        cap >> frame;
        if( frame.empty() )
            break;

        frame.copyTo(image);
        cvtColor(image, gray, COLOR_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

        if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
            addRemovePt = false;
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

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i];
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
            }
            points[1].resize(k);
        }

        if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }

        needToInit = false;
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

        std::swap(points[1], points[0]);
        cv::swap(prevGray, gray);
    }

return 0;
}







