

#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/calib3d.hpp"

#include "opencv2/highgui/highgui_c.h"
#include "opencv2/calib3d/calib3d_c.h"

using namespace cv;
using namespace std;

class CameraCalibration {

public:

	void readCameraParams(Mat& cameraMatrix, Mat& distCoeffs);
	void readPoints(vector<Point3f>& objectPoints, vector<Point2f>& imagePoints);
};

void CameraCalibration::readCameraParams(Mat& cameraMatrix, Mat& distCoeffs) {
	FileStorage fs2("src/out_camera_data.xml", FileStorage::READ);

	fs2["Camera_Matrix"] >> cameraMatrix;
	fs2["Distortion_Coefficients"] >> distCoeffs;

//	cout << "camera matrix: " << cameraMatrix << endl
//		 << "distortion coeffs: " << distCoeffs << endl;

}

void CameraCalibration::readPoints(vector<Point3f>& objectPoints, vector<Point2f>& imagePoints) {

	float objectHeight = 4; //temporary value from there and there
	float objectWidth = 1; //temporary value from there and there

	//Initialising the 3D-Points for the chessboard
	float a = 1.0f; //The widht/height of each square of the chessboard object
	Point3f _3DPoint;
	float y = 0;
	float x = 0.0f;

	for (int w = 0; w < objectHeight; w++, y += a) {
		_3DPoint.x = x;
		_3DPoint.y = y;
		_3DPoint.z = 0.0f;
		objectPoints.push_back(_3DPoint);
	}


}

#endif /* CAMERA_CALIBRATION_H_ */


