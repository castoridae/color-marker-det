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

class Settings {

public:

	//marker properties
	int colorsCount;
	float markerHeight;
	float markerWidth;

	//camera properties
	Mat cameraMatrix;
	Mat distCoeffs;

	//rotation and translation vectors of marker
	Mat rvec;
	Mat tvec;

	//marker physical 3d points
	vector<Point3f> objectPoints;

	//marker measured 2d points
	vector<Point2f> imagePoints;

	//marker projected points
	vector<Point2f> projectedPoints;

	void readMarkerData() {
		FileStorage fs("src/marker_data.xml", FileStorage::READ);

		fs["colorsCount"] >> colorsCount;
		fs["markerHeight"] >> markerHeight;
		fs["markerWidth"] >> markerWidth;
	}

	void readCameraParams() {
		FileStorage fs("src/out_camera_data.xml", FileStorage::READ);

		fs["Camera_Matrix"] >> cameraMatrix;
		fs["Distortion_Coefficients"] >> distCoeffs;
	}

	void setObjectPoints() {

		//Initialising the 3D-Points for the chessboard
		Point3f _3DPoint;
		float a = markerHeight/colorsCount;
		float y = 0.0f;
		float x = 0.5f;

		for (int w = 0; w < markerHeight; w++, y += a) {
			_3DPoint.x = x;
			_3DPoint.y = y;
			_3DPoint.z = 0.0f;
			objectPoints.push_back(_3DPoint);
		}
	}
};

int main(int argc, char* argv[]) {

	Settings s;
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

	s.readCameraParams();
	s.readMarkerData();
	s.setObjectPoints();

	cout << "camera matrix: " << s.cameraMatrix << endl << "distortion coeffs: "
			<< s.distCoeffs << endl << "imagePoints: " << s.imagePoints << endl
			<< "objectPoints: " << s.objectPoints << endl << "tvec: " << s.tvec
			<< endl << "rvec: " << s.rvec << endl;

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

		if (mode == SELECT_MARKER) {

		} else if (mode == MARKER_SELECTED) {
			c->addMarker(*tmpMarker);
			mode = SELECT_MARKER;
			if (c->markers->size() == 4) {
				mode = CAPTURING;
				cout << "Marker detected" << endl;
			}
		}

		//show 3D position of marker
		if (c->markers->size() == 4) {

			s.imagePoints.clear();
			for (int i = 0; i < 4; i++) {
				s.imagePoints.push_back(c->markers->at(i).calculateCenter());
			}

			if (s.distCoeffs.empty() != 1 && s.objectPoints.empty() != 1) {
				solvePnP(Mat(s.objectPoints), Mat(s.imagePoints), s.cameraMatrix,
						s.distCoeffs, s.rvec, s.tvec);
			}

			cout 	<< "rvec: " << s.rvec << endl
					<< "tvec: " << s.tvec << endl;

//			projectPoints(s.objectPoints, s.rvec, s.tvec, s.cameraMatrix,
//					s.distCoeffs, s.projectedPoints);
//
//			for (unsigned int i = 0; i < s.projectedPoints.size(); ++i) {
//				cout << "Image point: " << s.imagePoints[i] << " Projected to "
//						<< s.projectedPoints[i] << endl;
//			}
		}

	}

	delete c;
	return 0;

}
