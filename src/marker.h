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

using namespace cv;
using namespace std;

class Marker {

public:

	Point center;
	Point lastPos;
	Scalar value;

	Mat img;
	Mat imgTresholded;

	int** toleration;
	int* structSize;

	int* hh;

	vector<Point2f> imagePoints;

	Marker(){}
	Marker(Mat);
	Marker(Mat,int[]);
	Marker(Mat originalImage, int** toleration, int* structSize);


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

	this->toleration = toleration;


	lastPos.x = -1;
	lastPos.y = -1;
	this->imgTresholded = this->getTresholdedImage();

}

Mat Marker::tresholdImage() {

	Scalar toleration((*this->toleration)[0], (*this->toleration)[1], (*this->toleration)[2]);

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

//	cout << this->img << "asdasdad";
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

	//morphological opening (removes small objects from the foreground)
	erode(imgTresholded, imgTresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(*structSize, *structSize)));
	dilate(imgTresholded, imgTresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(*structSize, *structSize)));

	//morphological closing (removes small holes from the foreground)
	dilate(imgTresholded, imgTresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(*structSize, *structSize)));
	erode(imgTresholded, imgTresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(*structSize, *structSize)));
}


#endif /* MARKER_H_ */
