#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "chull.c"
#include "kmeans.c"

#define SCREEN_WIDTH 	640
#define SCREEN_HEIGHT 	480
#define X_CENTER		SCREEN_WIDTH/2
#define Y_CENTER		SCREEN_HEIGHT/2

using namespace cv;
using namespace std;

typedef struct{
	vector<Point> contour;
	Point position;
	std::string name;
	
	double area;
	uchar hue;
} contourMap;


// Set the video stream to the main camera
VideoCapture webcam(0);
// Matricies to hold stream data in different formats
Mat frameBGR, frameHSV; // Blue Green Red, Hue Saturation Value
Mat hsvThres, hsvEdges;
Mat channelsHSV[3];		// Blue Green Red split into separate matricies
double *hDArray = (double *)(malloc(sizeof(double)*SCREEN_WIDTH*SCREEN_HEIGHT));
queue edgePoints;
vector<contourMap> contourMaps;

// Records keyboard presses
int keyPressed;
	
void initWebcam(){
	//check if video device has been initialised
	if (!webcam.isOpened()) { 
		cout << "cannot open camera";
	}

	// Set the parameters of the cameras
	webcam.set(CV_CAP_PROP_FRAME_WIDTH, SCREEN_WIDTH);
	webcam.set(CV_CAP_PROP_FRAME_HEIGHT, SCREEN_HEIGHT);
	webcam.set(CV_CAP_PROP_FPS, 30);
}


bool error(contourMap check, contourMap reference){
	double area = check.area<reference.area?check.area/reference.area:reference.area/check.area;
	double hue = check.hue<reference.hue?check.hue/(1.0*reference.hue):reference.hue/(1.0*check.hue);
	return (area+hue)/2.0 > 0.75;
}



int main() {
	int val, fsize;
	
	initWebcam();
	int count = 0;
	
	//unconditional loop
	while (true) {
		// Store the most recent frame from the stream
		webcam.read(frameBGR);
		Mat gtemp;
		GaussianBlur(frameBGR, gtemp, Size(11,11), 7, 0, BORDER_DEFAULT);
		//addWeighted(frameBGR, 1.5, gtemp, -0.5, 0, frameBGR);
		
		cvtColor(gtemp, frameHSV, CV_BGR2HSV);
		val = (int)frameHSV.at<Vec3b>(X_CENTER,Y_CENTER)[0];
		inRange(frameHSV, Scalar(val-15, 100, 100), Scalar(val+15, 255, 255), hsvThres);
		
		split(frameHSV, channelsHSV);
		
		fsize=11;
		Mat element = getStructuringElement( MORPH_ELLIPSE,
                      Size( 2*fsize + 1, 2*fsize+1 ),
                      Point( fsize, fsize ) );
		
		dilate( hsvThres, hsvThres, element );
		erode( hsvThres, hsvThres, element );
		
		Canny(hsvThres, hsvEdges, 0.0, 255.0, 3, false);
		
		Mat mask = Mat::zeros(hsvEdges.rows, hsvEdges.cols, CV_8UC1);
		// find the contours
		vector< vector<Point> > contours;
		findContours(hsvEdges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		
		vector<double> areas(contours.size());
		for(int i = 0; i < contours.size(); i++)
			areas[i] = contourArea(Mat(contours[i]));
		double max;
		Point maxPosition;
		minMaxLoc(Mat(areas),0,&max,0,&maxPosition);
		/*
		contourMap thing = {contours.at(maxPosition.y), maxPosition, "object "+count, areas.at(maxPosition.y), val};
		for(int i = 0; i < contourMaps.size(); i++)
		{
		
			if(!error(thing, contourMaps.at(i)))
			{
				putText(frameBGR, contourMaps.at(i).name, Point(0, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0),2);
			}
			else{
				contourMaps.push_back(thing);
			}
				
		}*/
		
		drawContours(mask, contours, maxPosition.y, Scalar(255), CV_FILLED);
		
		
		 
		/*
		uchar *hArray = (uchar *)(hsvThres.data);
		for(int i = 0; i < SCREEN_WIDTH; i++)
			for(int j = 0; j < SCREEN_HEIGHT; j++)
				hDArray[j*SCREEN_WIDTH+i] = (double)hArray[j*SCREEN_WIDTH+i];
		
		
		
		edgePoints = main2(SCREEN_WIDTH, SCREEN_HEIGHT, hDArray, 2);
		
		int queueSize = edgePoints.numOwned;
		Point tmpPoints[1][queueSize];
		
		for(int i = 0; i < queueSize; i++){
			tmpPoints[0][i] = Point(edgePoints.owned[i]%SCREEN_WIDTH 
								, edgePoints.owned[i]/SCREEN_WIDTH);
								
			//cout <<edgePoints.owned[i]%SCREEN_WIDTH<< " " <<edgePoints.owned[i]/SCREEN_WIDTH << "\n";
		}		
		
		const Point* ppt[1] = {tmpPoints[0]};
		int npt[] ={queueSize};
		fillPoly( frameBGR, ppt, npt, 1, Scalar( 255, 0, 0 ), 8 );
		
		Mat temp = Mat(SCREEN_HEIGHT, SCREEN_WIDTH, CV_64FC1, hDArray);
		
		
		/*
		/// Convert the image to grayscale
		cvtColor( frameBGR, frameBGR, CV_BGR2GRAY );
		Laplacian(frameBGR, frameBGR, CV_16S, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs( frameBGR, frameBGR );*/
		
		
		keyPressed = waitKey(30);

		// If escape is pressed quit
		if (keyPressed == 27)
			break;
		else if(keyPressed == 32)
			cout << "Data " << hDArray[400] << "\n";
			
		imshow("RGB", frameBGR);
		imshow("Thres", hsvThres);
		imshow("Edges", hsvEdges);
		imshow("Contours", mask);
	}
	return 0;
}