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

	Marker(){}
	Marker(Mat);
	Marker(Mat,int[]);
	Marker(Mat originalImage, Scalar value, int* structSize);


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

	void setImage(Mat& imgTresholded){
		this->imgTresholded = imgTresholded;
	}

	Mat& getImage(){
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
};

Marker::Marker(Mat originalImage){

	this->img = originalImage;
	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->tresholdImage();
}

Marker::Marker(Mat originalImage, Scalar value, int* structSize){

	this->structSize = structSize;

	this->img = originalImage;

	this->setValue(value);

	this->tolH = this->tolS = this->tolV = new int(10);
	cout << endl << "123" << endl;

	this->levelsMin[0] = new int(value[0]-10);
	this->levelsMin[1] = new int(value[1]-10);
	this->levelsMin[2] = new int(value[2]-10);

	this->levelsMax[0] = new int(value[0]+10);
	this->levelsMax[1] = new int(value[1]+10);
	this->levelsMax[2] = new int(value[2]+10);

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

Mat Marker::tresholdImage() {

	int hMin = (*(this->levelsMin)[0]>0) ? *(this->levelsMin)[0] : 1;
	int sMin = (*(this->levelsMin)[1]>0) ? *(this->levelsMin)[1] : 1;
	int vMin = (*(this->levelsMin)[2]>0) ? *(this->levelsMin)[2] : 1;

	int hMax = (*(this->levelsMax)[0]>0) ? *(this->levelsMax)[0] : 1;
	int sMax = (*(this->levelsMax)[1]>0) ? *(this->levelsMax)[1] : 1;
	int vMax = (*(this->levelsMax)[2]>0) ? *(this->levelsMax)[2] : 1;

	Scalar levelsMin( hMin,sMin,vMin);
	Scalar levelsMax( hMax,sMax,vMax);

	Mat imgHSV;
	Mat tresh;
	cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	inRange(imgHSV, levelsMin, levelsMax, tresh);
	this->imgTresholded = tresh;
	return this->imgTresholded;
}


void Marker::initWindowTrackbars(){
	stringstream ss;
	ss << ++this->markerCnt;
	string str = "Marker " + ss.str();

	this->windowName = str;

	namedWindow(str, CV_WINDOW_NORMAL);

	createTrackbar("Hue min", str, (this->levelsMin[0]),255);
	createTrackbar("Saturation min", str, (this->levelsMin[1]),255);
	createTrackbar("Value min", str, (this->levelsMin[2]),255);

	createTrackbar("Hue max", str, (this->levelsMax[0]),255);
	createTrackbar("Saturation max", str, (this->levelsMax[1]),255);
	createTrackbar("Value max", str, (this->levelsMax[2]),255);

	createTrackbar("Struct size", str, this->structSize ,30);
	createTrackbar("Median filter size", str, this->medianSize ,50);
	createTrackbar("Closure size", str, this->closureSize ,10);
	createTrackbar("Opening size", str, this->openingSize ,10);
}

Mat Marker::getTresholdedImage() {
	return this->imgTresholded;
}


Point& Marker::calculateCenter() {

	Moments oMoments = moments(this->tresholdImage());

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
	if (dArea > 10000)
	{
		//calculate the position of the ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		center.x = posX;
		center.y = posY;
	}

	return center;
}

void Marker::setBlobSize(int* min, int* max){
	delete this->minBlob;
	delete this->maxBlob;
	this->minBlob = min;
	this->maxBlob = max;
}

void Marker::fillHoles() {

	//median size has to be odd
		int medianSize = ((*this->medianSize) % 2) ? *(this->medianSize)  : *(this->medianSize) +1;
		medianBlur(imgTresholded,imgTresholded,medianSize);

	int structSize = *(this->structSize) > 0 ? *(this->structSize) : 1;

	//morphological opening (removes small objects from the foreground)
	for(int i = 0; i< (*this->openingSize); i++){
		erode(imgTresholded, imgTresholded,
					getStructuringElement(MORPH_ELLIPSE, Size(structSize,structSize)));
			dilate(imgTresholded, imgTresholded,
					getStructuringElement(MORPH_ELLIPSE, Size(structSize,structSize)));
	}


	//morphological closing (removes small holes from the foreground)
	for(int i = 0; i< (*this->closureSize); i++){
		dilate(imgTresholded, imgTresholded,
					getStructuringElement(MORPH_ELLIPSE, Size(structSize,structSize)));
			erode(imgTresholded, imgTresholded,
					getStructuringElement(MORPH_ELLIPSE, Size(structSize,structSize)));
		}

	// convert cv::Mat to IplImage
//	IplImage img2 = img;
//
//
//	// convert to grayscale
//	IplImage gray = imgTresholded;
//
//	// get blobs
//	IplImage *labelImg = cvCreateImage( cvGetSize(&gray), IPL_DEPTH_LABEL, 1 );
//	CvBlobs blobs;
//	unsigned int result = cvLabel( &gray, labelImg, blobs );
//
//	cvFilterByArea(blobs,*minBlob,*maxBlob);
//
//	// render blobs in original image
//	cvRenderBlobs( labelImg, blobs, &img2, &img2 );
//
//	// *always* remember freeing unused IplImages
//	cvReleaseImage( &labelImg );
//
//	// convert back to cv::Mat
//	//cv::Mat output( &img );
}

int Marker::markerCnt = 0;


#endif /* MARKER_H_ */
