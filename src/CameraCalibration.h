

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
};

void CameraCalibration::readCameraParams(Mat& cameraMatrix, Mat& distCoeffs) {
	FileStorage fs2("src/out_camera_data.xml", FileStorage::READ);

	fs2["Camera_Matrix"] >> cameraMatrix;
	fs2["Distortion_Coefficients"] >> distCoeffs;

//	cout << "camera matrix: " << cameraMatrix << endl
//		 << "distortion coeffs: " << distCoeffs << endl;

}




#endif /* CAMERA_CALIBRATION_H_ */


