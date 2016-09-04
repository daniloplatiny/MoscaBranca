/* -------------------------------------------------------------------------------------------------------------------------
   skinSegmentation.cpp - apply some skin segmentation techniques in real time
   Version: 1.0
   Author: Hemerson Pistori (pistori@ucdb.br)
   Last Updated: 09/02/2012
*/

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "cv.h"
#include "cxcore.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>


using namespace cv;
using namespace std;



//-------------------------------------------------------------------------------------------------------------------------
//--- Global Variables

const char configFileName[] = "../data/config.xml";

int bmin = 0;  // Minimum value for BLUE channel thresholding (BGR)
int bmax = 20; // Maximum value for BLUE channel thresholding (BGR)
int gmin = 30; // Minimum value for GREEN  thresholding
int gmax = 150; // Maximum ...
int rmin = 80; // Minimum value for RED thresholding
int rmax = 255; // Maximum ...
int useMinMax = 1; // Instead of using mean and standard dev, use min and max values
int stdMult = 4; // Standard deviation multiplayer used to calculate the threshold range (RANGE SIZE)


// Initial position for all the windows (adjusted to a 1920x1080 screen). The values are read from the configuration file at configFileName
int wwebcamX, wwebcamY, wthresholdX, wthresholdY, wcontrolX, wcontrolY  = 0;

// Selection of sample pixels that will be used to determine the BGR min a max values
bool selectObject = false;
// Origin of the sample pixels window (dynamically created by the user using press and realease mouse button actions)
Point origin;
// Sample pixels window
Rect selection;

// Accumulator for statistics calculation
int Mean[3] = {0,0,0}; // Mean values for B, G and R converted to int
int Std[3] = {0,0,0}; // Standard Devition values for B, G and R converted to int
int Min[3] = {0,0,0}; // Minimun values for B, G and R converted to int
int Max[3] = {0,0,0}; // Maximun values for B, G and R converted to int


IplImage* originalImage;
IplImage* thresholdedImage;


//-------------------------------------------------------------------------------------------------------------------------
//--- Functions Declarations
void saveConfiguration(void);
void readConfiguration(void);
void changeBGRMinMaxValues(Mat roi);
void statisticsChanged( int, void * );
void onMouse( int event, int x, int y, int, void* );
void createAllTrackBars(void);






//-------------------------------------------------------------------------------------------------------------------------
//--- Main

int main( int argc, char** argv )
{
    readConfiguration();



//---------------------------------------------------------------------
//--- Initialize video capture from webcam
    CvCapture* capture = cvCaptureFromCAM(0);
    if (!cvQueryFrame(capture)) {
        cout<<"Video capture failed"<<endl;
        exit(-1);
    }

    cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 320 );
    cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 240 );

//---------------------------------------------------------------------
//--- Create object for each frame captured
    CvSize sz = cvGetSize(cvQueryFrame( capture));
    originalImage = cvCreateImage( sz, 8, 3 );


//---------------------------------------------------------------------
//--- Create windows e move to saved position (configuration file - data/config.xml)

    cvNamedWindow( "originalImage",1);  // Window for the original frames captured by the WebCam
    setMouseCallback( "originalImage", onMouse, 0 );
    cvMoveWindow("originalImage",wwebcamX,wwebcamY);


    cvNamedWindow( "thrsholdImage",1);  // Window for the thrholded image
    cvMoveWindow("thresholdedImage",wthresholdX,wthresholdY);

    namedWindow( "Configuration", 0);     // Window for the trackbars with adjustable parameters
    cvMoveWindow("Configuration",wcontrolX,wcontrolY);

    createAllTrackBars();

    // Main loop that process each frame coming from the webcam. Use ESC (27) to exit from the loop
    int c = 0;
    while ( c != 27)
    {

        // Get the current frame from the webcam
        originalImage = cvQueryFrame( capture);




//---------------------------------------------------------------------
//--- Apply Color Thresholding


        thresholdedImage = cvCreateImage(cvGetSize(originalImage),originalImage->depth,originalImage->nChannels);
        cvCopy(originalImage,thresholdedImage,NULL);

        // Iterate through all the pixels of the image (one by one)
        uchar * data  = (uchar *)thresholdedImage->imageData;
        int step = thresholdedImage->widthStep;
        int channels = thresholdedImage->nChannels;
        for (int row=0; row<thresholdedImage->height; row++) {
            for (int col=0; col<thresholdedImage->width; col++) {
                int B = data[row*step+col*channels+0]; // Blue (B) value of the pixel
                int G = data[row*step+col*channels+1]; // Green (G)value of the pixel
                int R = data[row*step+col*channels+2]; // Pixel's Red (R) value'
                if( !(  B >= bmin && B <= bmax  &&
                        G >= gmin && G <= gmax  &&
                        R >= rmin && R <= rmax)) {
                    for (int channel=0; channel<channels; channel++) {
                        data[row*step+col*channels+channel]=0;
                    }
                }
            }
        }

        if ( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(originalImage, selection);
            changeBGRMinMaxValues(roi);
            bitwise_not(roi, roi); // Shows that rectangular region indicating what you selected using mouse draging
            cvResetImageROI(originalImage);

        }


        cvShowImage("originalImage", originalImage); // Show the webcam image
        cvMoveWindow("thresholdedImage",wthresholdX,wthresholdY);
        cvShowImage("thresholdedImage", thresholdedImage); // Show thresholded Image


        cvReleaseImage(&thresholdedImage );

        c = cvWaitKey(10);
    }

    saveConfiguration();
    cvReleaseCapture( &capture);
    cvDestroyAllWindows();

}







//------------------------------------------------------------------------------------------------
// Calculate HSV Min and Max from statistics extracted from the image
void adjustBGRMinMax(void) {
    if (useMinMax) {

        bmin = Min[0];
        bmax = Max[0];
        gmin = Min[1];
        gmax = Max[1];
        rmin = Min[2];
        rmax = Max[2];
    }
    else {

        bmin = Mean[0]-Std[0]*stdMult;
        bmax = Mean[0]+Std[0]*stdMult;
        gmin = Mean[1]-Std[1]*stdMult;
        gmax = Mean[1]+Std[1]*stdMult;
        rmin = Mean[2]-Std[2]*stdMult;
        rmax = Mean[2]+Std[2]*stdMult;
    }
    setTrackbarPos( "Bmin", "Configuration", bmin );
    setTrackbarPos( "Bmax", "Configuration", bmax );
    setTrackbarPos( "Gmin", "Configuration", gmin );
    setTrackbarPos( "Gmax", "Configuration", gmax );
    setTrackbarPos( "Rmin", "Configuration", rmin );
    setTrackbarPos( "Rmax", "Configuration", rmax );

}






//------------------------------------------------------------------------------------------------
// Extract min, max, mean and standard deviation statistics from the selected window
void changeBGRMinMaxValues(Mat roi) {


    Mat BGRMeans(4,1,CV_32F);
    Mat BGRStdDevs(4,1,CV_32F);
    double BGRMins[] = {0,0,0,0};
    double BGRMaxs[] = {0,0,0,0};
    Mat channel;

    // Calculate means and standard values for B, G and R channels
    meanStdDev(roi,BGRMeans,BGRStdDevs);

    for (int i=0; i<3; ++i) {

        Mean[i] = (int) BGRMeans.at<double>(i,0);
        Std[i] = (int) BGRStdDevs.at<double>(i,0);


        extractChannel(roi,channel,i);
        minMaxIdx(channel,&BGRMins[i],&BGRMaxs[i]);

        Min[i] = BGRMins[i];
        Max[i] = BGRMaxs[i];
    }

    adjustBGRMinMax();
}





//------------------------------------------------------------------------------------------------
// The statistics must be calculated changed and the values must be recalculated
void statisticsChanged( int, void * ) {
    if(originalImage) {
        Mat roi(originalImage, selection);
        changeBGRMinMaxValues(roi);
        cvResetImageROI(originalImage);
    }
}






//------------------------------------------------------------------------------------------------
// Deals with the selection of a rectangular region from where the mean and standard deviation statistics for pixels BGR values are calculated
void onMouse( int event, int x, int y, int, void* )
{
    if ( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);
        selection &= Rect(0, 0, 650, 500);

    }

    switch ( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
        break;
    }
}







//-------------------------------------------------------------------------------------------------

void createAllTrackBars(void) {
    createTrackbar( "Bmin", "Configuration", &bmin, 256, 0 );
    createTrackbar( "Bmax", "Configuration", &bmax, 256, 0 );
    createTrackbar( "Gmin", "Configuration", &gmin, 256, 0 );
    createTrackbar( "Gmax", "Configuration", &gmax, 256, 0 );
    createTrackbar( "Rmin", "Configuration", &rmin, 256, 0 );
    createTrackbar( "Rmax", "Configuration", &rmax, 256, 0 );
    createTrackbar( "Use Min Max ?", "Configuration", &useMinMax, 1, statisticsChanged );
    createTrackbar( "Standard Deviation", "Configuration", &stdMult, 20, statisticsChanged );

}






//-------------------------------------------------------------------------------------------------
// Save configuration parameters on disk
void saveConfiguration(void) {
    CvFileStorage* fs=cvOpenFileStorage(configFileName, 0,CV_STORAGE_WRITE);
    cvWriteInt( fs, "bmin", bmin );
    cvWriteInt( fs, "bmax", bmax );
    cvWriteInt( fs, "gmin", gmin );
    cvWriteInt( fs, "gmax", gmax );
    cvWriteInt( fs, "rmin", rmin );
    cvWriteInt( fs, "rmax", rmax );
    cvWriteInt( fs, "wwebcamX", wwebcamX);
    cvWriteInt( fs, "wwebcamY", wwebcamY);
    cvWriteInt( fs, "wthresholdX", wthresholdX);
    cvWriteInt( fs, "wthresholdY", wthresholdY);
    cvWriteInt( fs, "wcontrolX", wcontrolX);
    cvWriteInt( fs, "wcontrolY", wcontrolY);
    cvWriteInt( fs, "stdMult", stdMult);
    cvWriteInt( fs, "useMinMax", useMinMax);
    cvReleaseFileStorage( &fs);
}




//-------------------------------------------------------------------------------------------------
// Recover configuration parameters from disk
void readConfiguration(void) {
    CvFileStorage* fs= cvOpenFileStorage(configFileName, 0, CV_STORAGE_READ);
    if (fs) {
        bmin = cvReadIntByName( fs, 0, "bmin" );
        bmax = cvReadIntByName( fs, 0, "bmax" );
        gmin = cvReadIntByName( fs, 0, "gmin" );
        gmax = cvReadIntByName( fs, 0, "gmax" );
        rmin = cvReadIntByName( fs, 0, "rmin" );
        rmax = cvReadIntByName( fs, 0, "rmax" );
        wwebcamX = cvReadIntByName( fs, 0, "wwebcamX");
        wwebcamY = cvReadIntByName( fs, 0,"wwebcamY");
        wthresholdX = cvReadIntByName( fs,0, "wthresholdX");
        wthresholdY = cvReadIntByName( fs,0, "wthresholdY");
        wcontrolX = cvReadIntByName( fs,0, "wcontrolX");
        wcontrolY = cvReadIntByName( fs,0, "wcontrolY");
        stdMult = cvReadIntByName( fs,0, "stdMult");
        useMinMax = cvReadIntByName( fs,0, "useMinMax");
        Mean[0] = (bmin+bmax)/2;
        Mean[1] = (gmin+gmax)/2;
        Mean[2] = (rmin+rmax)/2;
        Std[0] = ((bmax-bmin)/2)/stdMult;
        Std[1] = ((gmax-gmin)/2)/stdMult;
        Std[2] = ((rmax-rmin)/2)/stdMult;
        Min[0] = bmin;
        Min[1] = gmin;
        Min[2] = rmin;
        Max[0] = bmax;
        Max[1] = gmax;
        Max[2] = rmax;
    }
    cvReleaseFileStorage( &fs);
}




