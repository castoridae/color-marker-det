/*
 * ColorDetection.h
 *
 *  Created on: Jun 1, 2014
 *      Author: bober
 */

#ifndef COLORDETECTION_H_
#define COLORDETECTION_H_

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

#include "marker.h"

using namespace std;
using namespace cv;

Mat view;
Mat lastView;
CvPoint lastPoint;
CvPoint point;
int drag = 0;
int mode = 0; //initial mode CAPTURING

enum {
	CAPTURING = 0,
	CALIBRATED = 1,
	SELECT_MARKER = 2,
	TRACKING = 3,
	MARKER_SELECTED = 4

};

Marker* tmpMarker;
void mouseHandler(int event, int x, int y, int flags, void* param);

class ColorDetection {

public:

	static ColorDetection* instance;

	// poziom tolerancji
	static int **tolerationLevel;

	// wielkośc elementu strukturalnego
	static int *structSize;

	static int *minBlob;

	static int *maxBlob;

	//wskaźnik na oryginalny obraz;
	Mat imgView;

	//nazwa okna z ustawieniami
	const static string SETTINGS_WINDOW_NAME;

	//	static Marker tmpMarker;
	vector<Marker> *markers;

	ColorDetection();

	virtual ~ColorDetection() {
		// TODO Auto-generated destructor stub
	}

	void initSettingsWindow();

	const Mat& getView() const;

	void setView(const Mat& viewa);

	Mat tresholdImage(Mat imgOriginal);

	Mat tresholdImage(Mat imgOriginal, Scalar value);

	void displayMultiTreshold(Mat view);

	void fillHoles(Mat& imgThresholded);

	void static mouseHandler(int event, int x, int y, int flags, void* param);

	void addMarker(Marker m) {
		markers->push_back(m);
	}

	vector<Marker>* getMarkers() {
		return markers;
	}

	void incToleration() {
		ColorDetection::tolerationLevel++;
	}

	void decToleration() {
		ColorDetection::tolerationLevel--;
	}

	int* getTolerationLevel();

	void fillImagePoints();

	void drawLines();

	void drawLines(Mat);

	static ColorDetection& getInstance();

	int getStructSize() const {
		return *structSize;
	}

	void setStructSize(int *structSize) {
		this->structSize = structSize;
	}
};

const string ColorDetection::SETTINGS_WINDOW_NAME = "Settings window";
int *ColorDetection::structSize = new int(1);
int *ColorDetection::minBlob = new int(500);
int *ColorDetection::maxBlob = new int(50000);
int levels[] = { 10, 10, 10 };
int (*p) = levels;
//p = levels;
int **ColorDetection::tolerationLevel = &p;

ColorDetection::ColorDetection() {
	markers = new vector<Marker>();

	initSettingsWindow();
}

void ColorDetection::initSettingsWindow() {

	namedWindow(SETTINGS_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
	createTrackbar("Min blob size", SETTINGS_WINDOW_NAME, this->minBlob, 10000);
	createTrackbar("Max blob size", SETTINGS_WINDOW_NAME, this->maxBlob, 10000);
}

const Mat& ColorDetection::getView() const {
	return view;
}

void ColorDetection::setView(const Mat& viewa) {
	view = viewa;
}

void ColorDetection::displayMultiTreshold(Mat view) {
	int markersCnt = markers->size();
	for (int i = 0; i < markersCnt; i++) {
		stringstream ss;
		ss.str("");
		ss.clear();
		ss << (i + 1);
		string str2 = ss.str();
		Marker m = markers->at(i);
		m.tresholdImage();
		m.fillHoles();
		imshow(m.getWindowName(), m.getTresholdedImage());
	}
}

void ColorDetection::drawLines() {

	int markersCnt = markers->size();
	for (int i = 0; i < markersCnt - 1; i++) {
		line(view, markers->at(i).calculateCenter(),
				markers->at(i + 1).calculateCenter(), Scalar(0, 0, 255), 1, 8,
				0);
	}

}

void ColorDetection::fillImagePoints() {
	int markersCnt = markers->size();
	for (int i = 0; i < markersCnt - 1; i++) {
		cout << "marker " << i << ": " << markers->at(i).calculateCenter()
				<< endl;
	}
}

void ColorDetection::drawLines(Mat view) {

	int markersCnt = markers->size();
	for (int i = 0; i < markersCnt - 1; i++) {

		Point start = markers->at(i).calculateCenter();
		Point end = markers->at(i + 1).calculateCenter();

		int thickness = 2;
		int lineType = 8;
		line(view, start, end, Scalar(255, 50, 50), thickness, 8);

	}

}

void ColorDetection::mouseHandler(int event, int x, int y, int flags,
		void* param) {
	Mat image_rect = Mat::zeros(1, 1, CV_8UC3);
	Scalar avgPixelIntensity = 0;

	/* lewy przycisk myszki wcisniety */
	if (event == CV_EVENT_LBUTTONDOWN && !drag) {
		point = cvPoint(x, y);
		drag = 1;
	}
	/* przeciagniecie myszki */
	if (event == CV_EVENT_MOUSEMOVE && drag) {
		lastPoint = cvPoint(x, y);
		lastView = view;
		image_rect.release();
		image_rect = view.clone();
		rectangle(image_rect, point, lastPoint, Scalar(255, 255, 0), 1, 8);
		imshow("Original", image_rect);
		//		cout << image_rect;

	}

	/* uwolnienie lewego przycisku myszki */
	if (event == CV_EVENT_LBUTTONUP && drag) {

		int roiX = point.x;
		int roiY = point.y;
		int roiWidth = abs(roiX - lastPoint.x);
		int roiHeight = abs(roiY - lastPoint.y);

		cout << "roiX: " << roiX << endl;
		cout << "roiY: " << roiY << endl;
		cout << "lastPoint.x: " << lastPoint.x << endl;
		cout << "lastPoint.y: " << lastPoint.y << endl;
		cout << "roiWidth: " << roiWidth << endl;
		cout << "roiHeight: " << roiHeight << endl;

		int startPointX = (roiX < lastPoint.x) ? roiX : lastPoint.x;
		int startPointY = (roiY < lastPoint.y) ? roiY : lastPoint.y;

		if (roiWidth > 0 && roiHeight > 0) {
			Rect roi(startPointX, startPointY, roiWidth, roiHeight);

			Mat image_roi(lastView, roi);
			cvtColor(image_roi, image_roi, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

			avgPixelIntensity = mean(image_roi);

			int blue = avgPixelIntensity.val[0];
			int green = avgPixelIntensity.val[1];
			int red = avgPixelIntensity.val[2];

			if (mode == SELECT_MARKER) {

				int* toleration[3];
				int* structSize;

				structSize = new int(3);

				tmpMarker = new Marker(view, avgPixelIntensity, structSize);
				tmpMarker->setBlobSize(ColorDetection::minBlob,
						ColorDetection::maxBlob);
				mode = MARKER_SELECTED;
			}

			cout << "B:G:R avg intensity: " << blue << ":" << green << ":"
					<< red << endl;
		}

		drag = 0;
	}

	/* prawy przycisk myszki: reset */
	if (event == CV_EVENT_RBUTTONUP) {
		drag = 0;
	}
}

#endif /* COLORDETECTION_H_ */
