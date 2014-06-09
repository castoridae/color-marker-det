

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
	void readPoints(vector<Point3f>& objectPoints, vector<Point3f>& imagePoints);
};

void CameraCalibration::readCameraParams(Mat& cameraMatrix, Mat& distCoeffs) {
	FileStorage fs2("src/out_camera_data.xml", FileStorage::READ);

	fs2["Camera_Matrix"] >> cameraMatrix;
	fs2["Distortion_Coefficients"] >> distCoeffs;

//	cout << "camera matrix: " << cameraMatrix << endl
//		 << "distortion coeffs: " << distCoeffs << endl;

	FileNode features = fs2["features"];
	FileNodeIterator it = features.begin(), it_end = features.end();
	int idx = 0;
	std::vector<uchar> lbpval;

	// iterate through a sequence using FileNodeIterator
	for( ; it != it_end; ++it, idx++ )
	{
		cout << "feature #" << idx << ": ";
		cout << "x=" << (int)(*it)["x"] << ", y=" << (int)(*it)["y"] << ", lbp: (";
		// you can also easily read numerical arrays using FileNode >> std::vector operator.
		(*it)["lbp"] >> lbpval;
		for( int i = 0; i < (int)lbpval.size(); i++ )
			cout << " " << (int)lbpval[i];
		cout << ")" << endl;
	}

	fs2.release();
}

void CameraCalibration::readPoints(vector<Point3f>& objectPoints, vector<Point3f>& imagePoints) {

	float objectHeight = 10; //temporary value from ass
	float objectWidth = 10; //temporary value from ass

	//Initialising the 3D-Points for the chessboard
	float a = 0.2f; //The widht/height of each square of the chessboard object
	float rot = 0.0f;
	Point3f _3DPoint;
	float y = (((objectHeight - 1.0f) / 2.0f) * a) + (a / 2.0f);
	float x = 0.0f;
	for (int h = 0; h < objectHeight; h++, y += a) {
		x = (((objectWidth - 2.0f) / 2.0f) * (-a)) - (a / 2.0f);
		for (int w = 0; w < objectWidth; w++, x += a) {
			_3DPoint.x = x;
			_3DPoint.y = y;
			_3DPoint.z = 0.0f;
			objectPoints.push_back(_3DPoint);
		}
	}

}

#endif /* CAMERA_CALIBRATION_H_ */


