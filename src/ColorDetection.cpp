/*
 * ColorDetection.cpp
 *
 *  Created on: Jun 1, 2014
 *      Author: bober
 */

#include "ColorDetection.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"

#include "opencv2/highgui/highgui_c.h"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <unistd.h>

using namespace cv;
using namespace std;



int main(int argc, char* argv[]) {

	ColorDetection *c = new ColorDetection();

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	setMouseCallback("Original", ColorDetection::mouseHandler, NULL);

	VideoCapture cap(1); //capture the video from webcam
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}


	Mat imgTmp;
	cap.read(imgTmp);

	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	view = imgTmp;
	while (true) {

		bool bSuccess = cap.read(view); // read a new frame from video
//		view = imread("/home/bober/workspace/hello_world/src/color4.0.png", CV_LOAD_IMAGE_COLOR);


		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}


		c->displayMultiTreshold(view);
		c->drawLines(view);

		imshow("Original", view); //show the original image


		char key = (char) waitKey(25);
		if (key == 27) {
			cout << "escape detected lol";
			return 0;
		} else if (key == 'm') {
			cout << "Select roi" << endl;
			mode = SELECT_MARKER;
		} else if (key == 't') {
			cout << "Market tracking stated" << endl;
			mode = TRACKING;
		} else if (key == 'w') {
//			cout << "toleration incremented " << *(c->getTolerationLevel()) << endl;
		} else if (key == 'e') {
//			cout << "toleration decremented " << c->getTolerationLevel() << endl;
		}

		if(mode == SELECT_MARKER){

		}else if(mode == MARKER_SELECTED){
			c->addMarker(*tmpMarker);
			mode = CAPTURING;
		}


	}

	delete c;
	return 0;

}
