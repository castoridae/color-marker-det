/*
 * marker.h
 *
 *  Created on: May 1, 2014
 *      Author: Kamil Kopczyk
 */

#ifndef MARKER_H_
#define MARKER_H_

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"

#include "opencv2/highgui/highgui_c.h"
#include "cvblob.h"

#include "ColorDetection.h"

using namespace cv;
using namespace std;
using namespace cvb;

class Marker {

public:

	static int markerCnt;

	int id;

	Point center;
	Point lastPos;
	Scalar value;

	Mat img;
	Mat imgTresholded;

	int* levelsMin[3];
	int* levelsMax[3];

	int* structSize;
	int* openingSize;
	int* closureSize;
	int* medianSize;
	int* tolH;
	int* tolS;
	int* tolV;

	int* minBlob;
	int* maxBlob;

	CvBlobs blobs;

	String windowName;

	int* hh;

	vector<Point2f> imagePoints;

	Marker() {
	}
	Marker(Mat);
	Marker(Mat, int[]);
	Marker(Mat originalImage, Scalar value, int* structSize);
	Marker(Mat originalImage, const FileNode& node);

	Point& calculateCenter();

	Mat getTresholdedImage();

	Mat tresholdImage();

	void fillHoles();

	Scalar getMean() {
		return mean(value);
	}

	const Point& getCenter() const {
		return center;
	}

	void setCenter(const Point2f& center) {
		this->center = center;
	}

	Scalar& getValue() {
		return value;
	}

	void setValue(Scalar& value) {
		this->value = value;
	}

	void setImage(Mat& imgTresholded) {
		this->imgTresholded = imgTresholded;
	}

	Mat& getImage() {
		return this->imgTresholded;
	}

	const String& getWindowName() const {
		return windowName;
	}

	void setWindowName(const String& windowName) {
		this->windowName = windowName;
	}

	void setBlobSize(int*, int*);

	void initWindowTrackbars();

	void write(FileStorage& fs) const       //Write serialization for this class
				{

			stringstream ss;
			ss << id;
			string str = "marker_" + ss.str();
			fs << str << "{"
					<< "id" << id
					<< "center" << center
					<< "lastPos" << lastPos
					<< "valueH" << value[0]
					<< "valueS" << value[1]
					<< "valueV" << value[2]
					<< "levelsMin_1" << *(levelsMin[0])
					<< "levelsMin_2" << *(levelsMin[1])
					<< "levelsMin_3" << *(levelsMin[2])
					<< "levelsMax_1" << *(levelsMin[0])
					<< "levelsMax_2" << *(levelsMax[1])
					<< "levelsMax_3" << *(levelsMax[2])
					<< "structSize" << *structSize
					<< "openingSize" << *openingSize
					<< "closureSize" << *closureSize
					<< "medianSize" << *medianSize
					<< "minBlob" << *minBlob
					<< "maxBlob" << *maxBlob
					<< "windowName" << windowName
					<< "imagePoints" << imagePoints
					<< "}";
		}

	void read(const FileNode& node);
};

Marker::Marker(Mat originalImage) {

	this->img = originalImage;
	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->tresholdImage();
}

Marker::Marker(Mat originalImage, Scalar value, int* structSize) {

	this->id = ++this->markerCnt;

	this->structSize = structSize;

	this->img = originalImage;

	this->setValue(value);

	this->tolH = this->tolS = this->tolV = new int(20);

	this->levelsMin[0] = new int(value[0] - 10);
	this->levelsMin[1] = new int(value[1] - 10);
	this->levelsMin[2] = new int(value[2] - 10);

	this->levelsMax[0] = new int(value[0] + 10);
	this->levelsMax[1] = new int(value[1] + 10);
	this->levelsMax[2] = new int(value[2] + 10);

	this->medianSize = new int(9);
	this->closureSize = new int(1);
	this->openingSize = new int(1);

	this->initWindowTrackbars();

	this->minBlob = new int(500);
	this->maxBlob = new int(6000);

	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->getTresholdedImage();

}

Marker::Marker(Mat originalImage, const FileNode& node){
	this->id = ++this->markerCnt;
	this->img = originalImage;

	this->minBlob = new int(500);
	this->maxBlob = new int(6000);

	this->read(node);

	this->initWindowTrackbars();

	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->getTresholdedImage();
}

void Marker::read(const FileNode& node)          //Read serialization for this class
				{

		this->id = ++this->markerCnt;

		this->structSize = new int((int)node["structSize"]);

		Scalar valueTmp((double)node["valueH"],(double)node["valueS"],(double)node["valueV"]);
				this->setValue(valueTmp);

				this->levelsMin[0] = new int((int)node["levelsMin_1"]);
				this->levelsMin[1] = new int((int)node["levelsMin_2"]);
				this->levelsMin[2] = new int((int)node["levelsMin_3"]);

				this->levelsMax[0] = new int((int)node["levelsMax_1"]);
				this->levelsMax[1] = new int((int)node["levelsMax_2"]);
				this->levelsMax[2] = new int((int)node["levelsMax_3"]);

				this->medianSize = new int((int)node["medianSize"]);
				this->closureSize = new int((int)node["closureSize"]);
				this->openingSize = new int((int)node["openingSize"]);

		}

Mat Marker::tresholdImage() {

	int hMin = (*(this->levelsMin)[0] > 0) ? *(this->levelsMin)[0] : 1;
	int sMin = (*(this->levelsMin)[1] > 0) ? *(this->levelsMin)[1] : 1;
	int vMin = (*(this->levelsMin)[2] > 0) ? *(this->levelsMin)[2] : 1;

	int hMax = (*(this->levelsMax)[0] > 0) ? *(this->levelsMax)[0] : 1;
	int sMax = (*(this->levelsMax)[1] > 0) ? *(this->levelsMax)[1] : 1;
	int vMax = (*(this->levelsMax)[2] > 0) ? *(this->levelsMax)[2] : 1;

	Scalar levelsMin(hMin, sMin, vMin);
	Scalar levelsMax(hMax, sMax, vMax);

	Mat imgHSV;
	Mat tresh;

	cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	inRange(imgHSV, levelsMin, levelsMax, tresh);

	if (hMin > hMax) {
		Mat treshTmp;
		Scalar levelsMin(hMin, sMin, vMin);
		Scalar levelsMax(255, sMax, vMax);
		inRange(imgHSV, levelsMin, levelsMax, treshTmp);

		Scalar levelsMin2(1, sMin, vMin);
		Scalar levelsMax2(hMax, sMax, vMax);
		inRange(imgHSV, levelsMin2, levelsMax2, tresh);
		cout << "min: " << levelsMin << endl;
		cout << "max: " << levelsMax << endl;
		tresh = tresh | treshTmp;
	} else {
		inRange(imgHSV, levelsMin, levelsMax, tresh);
	}

	this->imgTresholded = tresh;
	return this->imgTresholded;
}

void Marker::initWindowTrackbars() {
	stringstream ss;
	ss << id;
	string str = "Marker " + ss.str();

	this->windowName = str;

	namedWindow(str, CV_WINDOW_NORMAL);

	createTrackbar("Hue min", str, (this->levelsMin[0]), 255);
	createTrackbar("Saturation min", str, (this->levelsMin[1]), 255);
	createTrackbar("Value min", str, (this->levelsMin[2]), 255);

	createTrackbar("Hue max", str, (this->levelsMax[0]), 255);
	createTrackbar("Saturation max", str, (this->levelsMax[1]), 255);
	createTrackbar("Value max", str, (this->levelsMax[2]), 255);

	createTrackbar("Struct size", str, this->structSize, 30);
	createTrackbar("Median filter size", str, this->medianSize, 50);
	createTrackbar("Closure size", str, this->closureSize, 10);
	createTrackbar("Opening size", str, this->openingSize, 10);
}

Mat Marker::getTresholdedImage() {
	return this->imgTresholded;
}

Point& Marker::calculateCenter() {

	Moments oMoments = moments(this->tresholdImage());

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// jesli dArea <= 10000, zakladamy, ze to nie obiekt, tylko szum
	if (dArea > 10000) {
		//wspolrzedne srodka ciezkosci
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		center.x = posX;
		center.y = posY;
	}

	return center;
}

void Marker::setBlobSize(int* min, int* max) {
	delete this->minBlob;
	delete this->maxBlob;
	this->minBlob = min;
	this->maxBlob = max;
}

void Marker::fillHoles() {

	//median size has to be odd
	int medianSize =
			((*this->medianSize) % 2) ?
					*(this->medianSize) : *(this->medianSize) + 1;
	medianBlur(imgTresholded, imgTresholded, medianSize);

	int structSize = *(this->structSize) > 0 ? *(this->structSize) : 1;

	//morfologiczne otwarcie
	for (int i = 0; i < (*this->openingSize); i++) {
		erode(imgTresholded, imgTresholded,
				getStructuringElement(MORPH_ELLIPSE,
						Size(structSize, structSize)));
		dilate(imgTresholded, imgTresholded,
				getStructuringElement(MORPH_ELLIPSE,
						Size(structSize, structSize)));
	}

	//morfologiczne zamkniecie
	for (int i = 0; i < (*this->closureSize); i++) {
		dilate(imgTresholded, imgTresholded,
				getStructuringElement(MORPH_ELLIPSE,
						Size(structSize, structSize)));
		erode(imgTresholded, imgTresholded,
				getStructuringElement(MORPH_ELLIPSE,
						Size(structSize, structSize)));
	}

	// convert cv::Mat to IplImage
	IplImage img2 = img;

	// convert to grayscale
	IplImage gray = imgTresholded;

	// get blobs
	IplImage *labelImg = cvCreateImage(cvGetSize(&gray), IPL_DEPTH_LABEL, 1);
	CvBlobs blobs;
	unsigned int result = cvLabel(&gray, labelImg, blobs);

	cvFilterByArea(blobs, *minBlob, *maxBlob);

	// render blobs in original image
	cvRenderBlobs(labelImg, blobs, &img2, &img2);

	// *always* remember freeing unused IplImages
	cvReleaseImage(&labelImg);

	// convert back to cv::Mat
	//cv::Mat output( &img );
}

int Marker::markerCnt = 0;

#endif /* MARKER_H_ */
