//opencv
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include <NetSend.hpp>


using namespace cv;

/** Global Variables */
const int max_value_H = 360/2;
const int max_value = 255;
const int max_value_R = 20;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
const String window_slider_name = "Slider Window";
int low_H = 50, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

//! [low]
static void on_low_H_thresh_trackbar(int, void *) {
	low_H = min(high_H-1, low_H);
	setTrackbarPos("Low H", window_slider_name, low_H);
}
//! [low]

//! [high]
static void on_high_H_thresh_trackbar(int, void *) {
	high_H = max(high_H, low_H+1);
	setTrackbarPos("High H", window_slider_name, high_H);
}

//! [high]
static void on_low_S_thresh_trackbar(int, void *) {
	low_S = min(high_S-1, low_S);
	setTrackbarPos("Low S", window_slider_name, low_S);
}

static void on_high_S_thresh_trackbar(int, void *) {
	high_S = max(high_S, low_S+1);
	setTrackbarPos("High S", window_slider_name, high_S);
}

static void on_low_V_thresh_trackbar(int, void *) {
	low_V = min(high_V-1, low_V);
	setTrackbarPos("Low V", window_slider_name, low_V);
}

static void on_high_V_thresh_trackbar(int, void *) {
	high_V = max(high_V, low_V+1);
	setTrackbarPos("High V", window_slider_name, high_V);
}

int main(int argc, char* argv[]) {

    NetSend infoSender(8080);
    infoSender.sendData();

	//! [cap]
	VideoCapture cap(argc > 1 ? atoi(argv[1]) : 0);
	//! [cap]

	//! [window]
	namedWindow(window_capture_name);
	namedWindow(window_detection_name);
	namedWindow(window_slider_name);
	
	//! [window]

	//! [trackbar]
	// Trackbars to set thresholds for HSV values
	createTrackbar("Low H", window_slider_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_slider_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S", window_slider_name, &low_S, max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_slider_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V", window_slider_name, &low_V, max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_slider_name, &high_V, max_value, on_high_V_thresh_trackbar);
	//! [trackbar]

	Mat frame, frame_HSV, frame_threshold, frame_contours;
	while (true) {
		//! [while]
		cap >> frame;
		if(frame.empty()) {
			break;
		}

		// Convert from BGR to HSV colorspace
		cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
		// Detect the object based on HSV Range Values
		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
		//! [while]

        // erode + dialate
        Mat kernal = Mat();
        dilate(frame_threshold, frame_threshold, kernal, Point(-1, -1), 10);
        erode(frame_threshold, frame_threshold, kernal, Point(-1, -1), 10);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        frame_contours = frame_threshold.clone();

        findContours(frame_contours, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        vector<Point> largestContour;
        double largestArea = 0;
        for(int i = 0; i < contours.size(); i++) {
            Scalar color = Scalar(0, 0, 255);

            double area = contourArea(contours[i]);
            if (area > largestArea) {
                largestContour = contours[i];
                largestArea = area;
            }
            
            RotatedRect rect = minAreaRect(contours[i]);

            Point2f rect_points[4]; rect.points( rect_points );
            for( int j = 0; j < 4; j++ )
                line(frame, rect_points[j], rect_points[(j+1)%4], Scalar(0, 255, 0), 5, 8);
        }
        
        if (largestArea > 0) {
            RotatedRect rect = minAreaRect(largestContour);

            Point2f rect_points[4]; rect.points( rect_points );
            for( int j = 0; j < 4; j++ )
                line(frame, rect_points[j], rect_points[(j+1)%4], Scalar(0, 0, 255), 5, 8 );
            Point2f center = rect.center;
            circle(frame, Point((int) center.x, (int) center.y), 4, Scalar(0, 0, 255), -1);

            infoSender.sendData(center.x, center.y);
        }

        //! [show]
        // Show the frames
        imshow(window_capture_name, frame);
        imshow(window_detection_name, frame_threshold);
        //! [show]

        char key = (char) waitKey(30);
		if (key == 'q' || key == 27) {
			break;
		}
	}

    infoSender.closePort();
	return 0;
}