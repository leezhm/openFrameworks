#include "KinApp.h"


//////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef _DEBUG
#define MEMORY_CHECK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define MEMORY_CHECK
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new MEMORY_CHECK
#endif

void Exit()
{
	int i = _CrtDumpMemoryLeaks();

#ifdef _WINDOWS
#include <afx.h>
	assert(0 == i);
#endif
}

//////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------
void KinApp::setup()
{
	// For Debug
	atexit(Exit);

	// set the log tack
	ofSetLogLevel(OF_LOG_VERBOSE);

	// Init the Kinect
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = false;
	initSetting.grabDepth = true;
	initSetting.grabAudio = false;
	initSetting.grabLabel = true;
	initSetting.grabLabelCv = true;
	initSetting.grabSkeleton = false;
	initSetting.grabCalibratedVideo = false;
	initSetting.depthResolution = NUI_IMAGE_RESOLUTION_640x480;

	kinect.init(initSetting);

	// open kinect
	kinect.open();

	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	// allocate for depth and label image
	depthImage.allocate(KIN_CAM_WIDTH, KIN_CAM_HEIGHT);
	labelImage.allocate(KIN_CAM_WIDTH, KIN_CAM_HEIGHT);
	distanceImage.allocate(KIN_CAM_WIDTH, KIN_CAM_HEIGHT);


	//////////////////////////////////////////////////////////////////////////
	//
	//	cv::Mat testing
	//
	//////////////////////////////////////////////////////////////////////////
	cv::Mat mat16(6, 6, CV_16UC1, cv::Scalar::all(0));
	cv::randu(mat16, cv::Scalar::all(1000), cv::Scalar::all(2000));
	std::cout<<" "<<mat16<<std::endl;

	cv::Mat mat8(6, 6, CV_8UC1, cv::Scalar::all(0));
	std::cout<<"\n "<<mat8<<std::endl<<std::endl;

	double min, max;
	cv::minMaxLoc(mat16, &min, &max, NULL, NULL);
	std::cout<<"max = "<<max<<"  min = "<<min<<std::endl;

	float param = 255.0f / max;

	for (int row = 0; row < mat8.rows; ++ row)
	{
		unsigned char * pMat8 = mat8.ptr<unsigned char>(row);
		unsigned short* pMat16 = mat16.ptr<unsigned short>(row);

		for (int col = 0; col < mat8.cols; ++ col)
		{
			pMat8[col] = pMat16[col] * param;
		}
	}

	std::cout<<"\n "<<mat8<<std::endl<<std::endl;

}

//--------------------------------------------------------------
void KinApp::update()
{
	// update
	kinect.update();

	if(kinect.isOpened())
	{
		depthImage.setFromPixels(kinect.getDepthPixels());
		labelImage.setFromPixels(kinect.getLabelPixelsCv(0));

		cv::Mat kinectDistanceMat(KIN_CAM_HEIGHT, KIN_CAM_WIDTH, CV_16UC1, kinect.getDistancePixels().getPixels());
		cv::Mat kinectMask(KIN_CAM_HEIGHT, KIN_CAM_WIDTH, CV_8UC1, cv::Scalar::all(128));
		cv::Mat kinectUser(labelImage.getCvImage());

		for (int row = 0; row < kinectDistanceMat.rows; ++ row)
		{
			unsigned char * userRow = kinectUser.ptr<unsigned char>(row);
			unsigned char *	maskRow = kinectMask.ptr<unsigned char>(row);
			unsigned short * depthRow = kinectDistanceMat.ptr<unsigned short>(row);

			for (int col = 0; col < kinectDistanceMat.cols; ++ col)
			{
				if(0 != maskRow[col] &&0 != userRow[col])
				{
					depthRow[col] = 1800;
					maskRow[col] = 0;
				}
			}
		}

		double max, min;
		cv::minMaxLoc(kinectDistanceMat, &min, &max, NULL, NULL);

		cv::Mat processed;
		kinectDistanceMat.convertTo(processed, CV_8UC1, 255.0f / max);

		distanceImage.setFromPixels(processed.data, KIN_CAM_WIDTH, KIN_CAM_HEIGHT);
	}
}

//--------------------------------------------------------------
void KinApp::draw()
{
	// draw the background
	ofBackground(100, 100, 100);

	// draw the depth image
	depthImage.draw(0, 0);

	// draw label image
	labelImage.draw(640, 0);

	//
	distanceImage.draw(1280, 0);

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << "fps: " << ofGetFrameRate() << endl;
				 //<< "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl
				 //<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 //<< "press LEFT and RIGHT to change the far clipping distance: " << farClipping << " mm" << endl
				 //<< "press '+' and '-' to change the near clipping distance: " << nearClipping << " mm" << endl
				 //<< "press 's' to save the near/ far clipping distance to xml file." << endl;
	ofDrawBitmapString(reportStream.str(), 20, 600);
}

//--------------------------------------------------------------
void KinApp::keyPressed(int key){

}

//--------------------------------------------------------------
void KinApp::keyReleased(int key){

}

//--------------------------------------------------------------
void KinApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void KinApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void KinApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void KinApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void KinApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void KinApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void KinApp::dragEvent(ofDragInfo dragInfo){ 

}