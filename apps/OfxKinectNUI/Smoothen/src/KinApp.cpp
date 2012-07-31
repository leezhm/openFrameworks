#include "KinApp.h"

//--------------------------------------------------------------
void KinApp::setup()
{
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

		// opencv
		labelImage.dilate_3x3();
		labelImage.blur(7);
		labelImage.erode_3x3();
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
	labelImage.draw(660, 0);

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