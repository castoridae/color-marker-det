/*
 * marker.h
 *
 *  Created on: Jun 1, 2014
 *      Author: bober
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

	int* toleration[3];
	int* structSize;
	int* openingSize;
	int* closureSize;
	int* medianSize;
	int* tolH;
	int* tolS;
	int* tolV;

	String windowName;

	int* hh;

	vector<Point2f> imagePoints;

	Marker(){}
	Marker(Mat);
	Marker(Mat,int[]);
	Marker(Mat originalImage, int* toleration[3], int* structSize);


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
};

Marker::Marker(Mat originalImage){

	this->img = originalImage;
	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->tresholdImage();
}

Marker::Marker(Mat originalImage, int** toleration, int* structSize){

	this->structSize = structSize;

	this->img = originalImage;

	this->tolH = this->tolS = this->tolV = new int(10);
	cout << endl << "123" << endl;

	this->toleration[0] = new int(10);
	this->toleration[1] = new int(10);
	this->toleration[2] = new int(10);

	this->medianSize = new int(9);
	this->closureSize = new int(1);
	this->openingSize = new int(1);

	stringstream ss;
	ss << ++this->markerCnt;
	string str = "Marker " + ss.str();

	this->windowName = str;


	namedWindow(str, CV_WINDOW_AUTOSIZE);
	createTrackbar("Toleration value H", str, (this->toleration[0]),100);
	createTrackbar("Toleration value S", str, (this->toleration[1]),100);
	createTrackbar("Toleration value V", str, (this->toleration[2]),100);
	createTrackbar("Struct size", str, this->structSize ,30);
	createTrackbar("Median filter size", str, this->medianSize ,50);
	createTrackbar("Closure size", str, this->closureSize ,10);
	createTrackbar("Opening size", str, this->openingSize ,10);

	cout << "this->windowName: " << this->windowName << "123123 asdasdasd "<< endl;

	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->getTresholdedImage();
	toleration = this->toleration;

}

Mat Marker::tresholdImage() {

	int h = (*(this->toleration)[0]>0) ? *(this->toleration)[0] : 1;
	int s = (*(this->toleration)[1]>0) ? *(this->toleration)[1] : 1;
	int v = (*(this->toleration)[1]>0) ? *(this->toleration)[1] : 1;
	Scalar toleration( h,s,v);

	Mat imgHSV;
	Mat tresh;
	cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	inRange(imgHSV, value - toleration, value + toleration, tresh);
	this->imgTresholded = tresh;
	return this->imgTresholded;
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
//	cout << center << "  dArea  " << dArea << endl;
	return center;
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



}

int Marker::markerCnt = 0;


#endif /* MARKER_H_ */
