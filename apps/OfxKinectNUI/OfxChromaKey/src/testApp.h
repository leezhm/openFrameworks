#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

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

private:
	
	ofxKinectNui KinectSensor;

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

	ofxCvGrayscaleImage depthImage;
	ofxCvGrayscaleImage userImage;

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



private:
	//////////////////////////////////////////////////////////////////////////
	//
	//
	//
	//////////////////////////////////////////////////////////////////////////
	ofxCvColorImage ofxCvKinectRGB;
	ofxCvGrayscaleImage ofxCvKinectUsers;
	
	ofxCvShortImage ofxCvKinectDepth;

	ofxCvGrayscaleImage ofxCvKinectMask;
	ofxCvGrayscaleImage ofxCvKinectAlpha;
};
