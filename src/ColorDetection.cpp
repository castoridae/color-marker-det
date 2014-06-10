/*
 * ColorDetection.cpp
 *
 *  Created on: Jun 1, 2014
 *      Author: bober
 */

#include "ColorDetection.h"
#include "CameraCalibration.h"

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
	CameraCalibration *cc = new CameraCalibration();

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	setMouseCallback("Original", ColorDetection::mouseHandler, NULL);

	VideoCapture cap(0); //capture the video from webcam
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}


	Mat imgTmp;
	cap.read(imgTmp);

	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	view = imgTmp;

	Mat rvec(3, 1, CV_64F);
	Mat tvec(3, 1, CV_64F);
	Mat cameraMatrix(3, 3, CV_64F);
	Mat distCoeffs(3, 3, CV_64F);
	vector<Point3f> objectPoints;
	vector<Point2f> imagePoints;

	cc->readCameraParams(cameraMatrix, distCoeffs);
	cc->readPoints(objectPoints, imagePoints);



	cout 	<< "camera matrix: " << cameraMatrix << endl
			<< "distortion coeffs: " << distCoeffs << endl
			<< "imagePoints: " << imagePoints << endl
			<< "objectPoints: " << objectPoints << endl
			<< "tvec: " << tvec << endl
			<< "rvec: " << rvec << endl;



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

		} else if (mode == MARKER_SELECTED){
			c->addMarker(*tmpMarker);
			mode = CAPTURING;

		}

		//show 3D position of marker
		if (c->markers->size() == 4) {

			imagePoints.clear();
			for (int i = 0; i < 4; i++) {
				imagePoints.push_back(c->markers->at(i).calculateCenter());
			}

			if (distCoeffs.empty() != 1 && objectPoints.empty() != 1) {
				solvePnP(Mat(objectPoints), Mat(imagePoints), cameraMatrix,
						distCoeffs, rvec, tvec, false);
			}

			cout 	<< "rvec: " << rvec << endl
					<< "tvec: " << tvec << endl;
		}

	}

	delete c;
	return 0;

}
