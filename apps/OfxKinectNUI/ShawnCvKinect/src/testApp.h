/******************************************************************/
/**
 * @file	testApp.h
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#pragma once

#include "ofxKinectNuiPlayer.h"
#include "ofxKinectNuiRecorder.h"
#include "ofxBase3DVideo.h"
#include "ofxKinectNuiDraw.h"

#include "ofxKinectNui.h"

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"



class ofxKinectNuiDrawTexture;
class ofxKinectNuiDrawSkeleton;

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS


class testApp : public ofBaseApp{
	public:

		void setup();
		void update();
		void draw();

		/**
		 * @brief	example for adjusting video images to depth images
		 * @note	inspired by akira's video http://vimeo.com/17146552
		 */
		void drawCalibratedTexture();
		void exit();
		void drawCircle3f(int n, int radius, ofVec3f cur);
		

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void kinectPlugged();
		void kinectUnplugged();
		
		void startRecording();
		void stopRecording();
		void startPlayback();
		void stopPlayback();

		ofxKinectNui kinect;

#ifdef USE_TWO_KINECTS
		ofxKinectNui kinect2;
#endif
		ofxKinectNuiPlayer kinectPlayer;
		ofxKinectNuiRecorder kinectRecorder;

		ofxBase3DVideo* kinectSource;

		ofTexture calibratedTexture;

		bool bRecord;
		bool bPlayback;
		bool bDrawVideo;
		bool bDrawDepthLabel;
		bool bDrawSkeleton;
		bool bDrawCalibratedTexture;
		bool bPlugged;
		bool bUnplugged;
		
		unsigned short nearClipping;
		unsigned short farClipping;
		int angle;
		
		int mRotationX, mRotationY;

		ofxKinectNuiDrawTexture*	videoDraw_;
		ofxKinectNuiDrawTexture*	depthDraw_;
		ofxKinectNuiDrawTexture*	labelDraw_;
		ofxKinectNuiDrawSkeleton*	skeletonDraw_;


//p  ofImages

    	ofxCvColorImage vidCalibImage;
	//	ofImage  vidCalibImageOrig;
		ofxCvColorImage vidCalibImageOrig;
		
//p  make a gradually fading away  depth mat which make the core mat totally  transparent but the outer one partly transparent.
		ofxCvGrayscaleImage depthImage;
		ofxCvGrayscaleImage depthImagePrev;  //the original depth will copy itself to Prev and 
		ofxCvGrayscaleImage depthImageOutSubRing;
		ofxCvGrayscaleImage depthImageResult;   //the combination of many layers  of  depth  layer of different transparent value.
		




			//ofxCvColorImage labelImage;
		ofxCvContourFinder contourFinderDepth;  		


//p  ofImages  about bkground
         ofxCvColorImage ofxCvbkGround;
		 ofImage  ofbkGound;

		 int count_for_Dilate;
		 int count_for_Erode;
		 int Kine_depth_width;
		 int Kine_depth_height;

};
