#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCv.h"

const int KIN_CAM_WIDTH = 640;
const int KIN_CAM_HEIGHT = KIN_CAM_WIDTH * 0.75;

class KinApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	private:
		ofxKinectNui kinect;

		ofxCvGrayscaleImage depthImage;
		ofxCvGrayscaleImage labelImage;

		ofxCvGrayscaleImage distanceImage;
};
