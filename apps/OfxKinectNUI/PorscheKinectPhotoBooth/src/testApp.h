#pragma once

#include "ofxZbar.h"

#include "ofxJSONElement.h"
#include "ofxKinectNui.h"
#include "ofxKinectNuiDraw.h"

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofxGui.h"

#include "opencv2/opencv.hpp"

class testApp : public ofBaseApp
{
public:

	void setup();
	void update();
	void draw();

	void exit();
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

private:

	void MappingColorPlayer(ofImage & player);

	void Processing();

	void GestureGenerator();

private:
	
	ofxKinectNui KinectSensor;

	//ofxKinectNuiDrawSkeleton * skeltonDraw;

	// Color Image 
	ofxCvColorImage ColorImage;

	
	int ColorWidth ;
	int ColorHeight;

	int DepthWidth;
	int DepthHeight;

	// Label Image
	//
	//	Notice, Getting the player image(or Label Image) is a very 
	//	expensive operating.
	//
	//ofxCvGrayscaleImage LabelImage;

	//ofxCvGrayscaleImage depthImage;
	//ofxCvGrayscaleImage userImage;

	// Player Image
	//ofxCvColorImage ColorPlayer;

	// Background Image
	ofImage backgroundImage;

	// current angle of sensor
	int Angle;

	int threshold;

	unsigned short nearClipping;
	unsigned short farClipping;

	// xml config file
	ofxXmlSettings xmlSetting;



private:
	//////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//////////////////////////////////////////////////////////////////////////
	//ofxCvColorImage ofxCvKinectRGB;
	//ofxCvGrayscaleImage ofxCvKinectUsers;
	//
	//ofxCvShortImage ofxCvKinectDepth;

	//ofxCvGrayscaleImage ofxCvKinectMask;
	//ofxCvGrayscaleImage ofxCvKinectAlpha;


	//ofxCvGrayscaleImage ofxKinectResult;


private:
	//////////////////////////////////////////////////////////////////////////
	//////ofxPanel gui;	// the gui panel

	//////ofxToggle filled;
	//////ofxFloatSlider InpaintResizeScaleSlider;
	//////ofxFloatSlider InpaintRadiusSlider;
	//////ofxIntSlider   InpaintUsersDilateStepSlider;

	//////ofxFloatSlider InpaintDepthValueNonUserSlider;
	//////ofxFloatSlider DepthRangeStartMeters;
	//////ofxFloatSlider DepthRangeEndMeters;

	//////ofxIntSlider MedianFilterDepthSizeSlider;
	//////ofxIntSlider MedianFilterAlphaSizeSlider;
	//////ofxIntSlider AlphaBlurFilterSizeSlider;
	//////
	//////ofxIntSlider AlphaErodeDilateKernelSizeSlider;

	//////ofxIntSlider AlphaErodeStepSlider;
	//////ofxIntSlider AlphaDilateStepSlider;

	//////ofxIntSlider AlphaOffSetXSlider;
	//////ofxIntSlider AlphaOffSetYSlider;

	//////ofxFloatSlider AlphaErodeDilateScaleSlider;
	//////ofxFloatSlider MedianFilterAlphaScaleSlider;

//private:
//	const ofPoint * skeletonDatas[6];
//
//
//	
//
//	unsigned long leftSwipeTime;
//	unsigned long leftRaiseTime;
//
//	unsigned long rightSwipeTime;
//	unsigned long rightRaiseTime;


private:
	void InitKinectSensor();

private:
	bool hasGetQRCode;
	ofxZbar qrDecoder;
	ofxCvGrayscaleImage ofxCvQrScanImage;



private:
	ofxJSONElement jsonGetInfo;
};