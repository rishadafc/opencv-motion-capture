// OpenCV Motion Capture Assignment - CSCI 442
// @author Rishad Bharucha

// Uncomment the header below for Windows

//#include <stdafx.h>
//#if !(defined (__linux__) || (defined (__APPLE__) && defined(__MACH__)))
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

// Replace main with wmain for Windows
//int wmain(int argc, wchar_t *argv[]) //_TCHAR* argv[])

int main() {
    
    cv::Mat frame;                      // Default image
    cv::Mat frame1;                     // Cloned image
    cv::Mat running_average;            // 32f, 3 channel image for running average
    cv::Mat difference;                 // Difference image
    cv::Mat contours_img;               // Contours image
    
    vector<vector<Point> > contours;    // Vector for contours
    Scalar color = Scalar(200,200,200); // Bounding box color
    
    int largestArea = 0;                // Store the largest area found from the contours
    int largestContourIndex = 0;        // Store the location of the largest contour for creating the boxes
    
    int c = 100;
    
    cv::VideoCapture capture(0);        // Open the default camera
    
    while(1)
    {
        // Capture a frame
        capture >> frame;
        
        // Clone the captured frame
        frame1 = frame.clone();
        
        // Blank image for running average of frame
        running_average = cv::Mat(frame.size(), CV_32FC3);
        
        // Blank image for difference
        difference = cv::Mat(frame.size(), CV_8UC3);
        
        // Calculate running average of frame
        cv::accumulateWeighted(frame, running_average, 0.32);
        
        // Swap running average to same format as frame
        cv::convertScaleAbs(running_average, running_average, 1.0, 0.0);
        
        // Blur image
        cv::GaussianBlur(frame1, frame1, cv::Size(5,5), 1.5);
        
        // Calculate the absolute difference
        cv::absdiff(frame1, running_average, difference);
        
        // Convert 8-bit unsigned 1 channel to 3 channel grayscale
        cv::cvtColor(difference, difference, CV_BGR2GRAY);
        
        // Threshold grayscale
        cv::threshold(difference, difference, 2, 255, CV_THRESH_BINARY);
        
        // Blur grayscale
        cv::GaussianBlur(difference, difference, cv::Size(5,5), 1.5);
        
        // Threshold grayscale
        cv::threshold(difference, difference, 250, 255, CV_THRESH_BINARY);
        
        // Find the contours
        cv::findContours(difference, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        
        // Initialize bounding rectangle box
        vector<Rect> boundingRects(contours.size());
        
        // Initialize contour image
        cv::Mat contourImage(difference.size(), CV_8UC3, cv::Scalar(0,0,0));
        
        // For loop for drawing contours
        for (int i = 0; i < contours.size(); i++) {
            
            // Find the area of the contour
            double area = contourArea(contours[i], false);
            
            // Find the bounds of the rectangle
            boundingRects[i] = boundingRect(Mat(contours[i]));
            
            // Draw the contours
            cv::drawContours(contourImage, contours, i, color, CV_FILLED);
            
            // Throw away all areas out size of this range
            if (area < 3000 || area > 1000000){}
            
            else {
                // Set largest area
                largestArea = area;
                
                // Store the location of the motion for creating the rectangle
                largestContourIndex = i;
            }
            
            // Generate the rectangle bounding box
            rectangle(frame1, boundingRects[largestContourIndex].tl(), boundingRects[largestContourIndex].br(), color, 2, 8, 0);
        }
        
        // Show windows
        cv::imshow("Boxes", frame1);
        cv::imshow("Contours", contourImage);
        
        c = cv::waitKey(10);
        if(c==27)
            break;
    }
    
    return 0;
}

