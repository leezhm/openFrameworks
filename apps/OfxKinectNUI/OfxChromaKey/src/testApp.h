#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

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

	bool Kinect_ProcessDepthAndAlphaPixels(float fInpaintRadius, float fInpaintResizeScale, int nInpaintUsersDilateSteps, 
										float fInpaintDepthValueNonUsers, int nMedianFilterDepthSize, int nAlphaErodeSteps, 
										int nAlphaDilateSteps, float fAlphaErodeDilateScale, int nAlphaErodeDilateKernelType, 
										int nAlphaErodeDilateKernelSize, int nAlphaOffsetX, int nAlphaOffsetY, int nMedianFilterAlphaSize, 
										float fMedianFilterAlphaScale, int nAlphaBlurFilterSize);

private:
	
	ofxKinectNui KinectSensor;

	// Color Image 
	ofImage ColorImage;

	
	int ColorWidth ;
	int ColorHeight;

	// Label Image
	//
	//	Notice, Getting the player image(or Label Image) is a very 
	//	expensive operating.
	//
	//ofxCvGrayscaleImage LabelImage;

	ofxCvGrayscaleImage thresholdImage;

	// Player Image
	ofxCvColorImage ColorPlayer;

	// Background Image
	ofImage backgroundImage;

	// current angle of sensor
	int Angle;

	int threshold;

	unsigned short nearClipping;
	unsigned short farClipping;

	// xml config file
	ofxXmlSettings xmlSetting;
};
