/******************************************************************/
/**
 * @file	testApp.cpp
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = true;
	initSetting.grabDepth = true;
	initSetting.grabAudio = true;
	initSetting.grabLabel = true;
	initSetting.grabSkeleton = true;
	initSetting.grabCalibratedVideo = true;
	initSetting.grabLabelCv = true;

	initSetting.depthResolution = NUI_IMAGE_RESOLUTION_640x480;
	initSetting.videoResolution = NUI_IMAGE_RESOLUTION_640x480;

	kinect.init(initSetting);
	kinect.open(false);
//	kinect.open(true); // when you want to use near mode

	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	
#ifdef USE_TWO_KINECTS
	// watch out that only the first kinect can grab label and skeleton.
	kinect2.init(true, true, false, false, false, false, false, true);
	kinect2.open();
#endif
	ofSetVerticalSync(true);

	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bRecord = false;
	bPlayback = false;
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();
	
	bDrawVideo = false;
	bDrawDepthLabel = false;
	bDrawSkeleton = false;
	bDrawCalibratedTexture = false;

	ofSetFrameRate(60);
	
	calibratedTexture.allocate(640, 480, GL_RGB);

	videoDraw_ = ofxKinectNuiDrawTexture::createTextureForVideo();
	depthDraw_ = ofxKinectNuiDrawTexture::createTextureForDepth();
	labelDraw_ = ofxKinectNuiDrawTexture::createTextureForLabel();
	skeletonDraw_ = new ofxKinectNuiDrawSkeleton();
	kinect.setVideoDrawer(videoDraw_);
	kinect.setDepthDrawer(depthDraw_);
	kinect.setLabelDrawer(labelDraw_);
	kinect.setSkeletonDrawer(skeletonDraw_);


//p
     vidCalibImage.allocate(640, 480);
	 vidCalibImageOrig.allocate(640, 480);
	 

	 Kine_depth_width=kinect.getDepthPixels().getWidth();
	 Kine_depth_height=kinect.getDepthPixels().getHeight();
	 depthImage.allocate(Kine_depth_width,Kine_depth_height);
	 depthImagePrev.allocate(Kine_depth_width,Kine_depth_height);
	 depthImageOutSubRing.allocate(Kine_depth_width,Kine_depth_height);
	 depthImageResult.allocate(Kine_depth_width,Kine_depth_height);


//init bkground   
	 ofbkGound.loadImage("bk1.jpg");
	 ofbkGound.resize(640,480);
	 //bkGround.getCvImage()->=cvLoadImage("bk1.jpg",CV_LOAD_IMAGE_COLOR);
	
	

	// labelImage.allocate(kinect.getLabelPixels().getWidth(),kinect.getLabelPixels().getHeight());

}

//--------------------------------------------------------------
void testApp::update() {
	kinectSource->update();
	if(bRecord){
		kinectRecorder.update();
	}
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(100, 100, 100);
	// Draw video only
	if(bDrawVideo){
		// draw video images from kinect camera
		videoDraw_->setDrawArea(0, 0, 1024, 768);
		kinect.drawVideo();
	// Draw depth + users label only
	}else if(bDrawDepthLabel){
		ofEnableAlphaBlending();
		// draw depth images from kinect depth sensor
		depthDraw_->setDrawArea(0, 0, 1024, 768);
		kinect.drawDepth();
		// draw players' label images on video images
		labelDraw_->setDrawArea(0, 0, 1024, 768);
		kinect.drawLabel();
		ofDisableAlphaBlending();
	// Draw skeleton only
	}else if(bDrawSkeleton){
		kinect.drawSkeleton(0, 0, 1024, 768);	// draw skeleton images on video images
	// Draw calibrated image only
	}else if(bDrawCalibratedTexture){
		ofPushMatrix();
		drawCalibratedTexture();	// draw calibrated images coodinates to depth images
		ofPopMatrix();
	}else{
		if(!bPlayback){
			// draw video images from kinect camera
			videoDraw_->setDrawArea(20, 20, 400, 300);
			kinect.drawVideo();
			ofEnableAlphaBlending();
			// draw depth images from kinect depth sensor
			depthDraw_->setDrawArea(20, 340, 400, 300);
			kinect.drawDepth();
			// draw players' label images on video images
			labelDraw_->setDrawArea(20, 340, 400, 300);
			kinect.drawLabel();
			ofDisableAlphaBlending();
			// draw skeleton images on video images
			kinect.drawSkeleton(20, 20, 400, 300);

#ifdef USE_TWO_KINECTS
			kinect2.draw(440, 20, 400, 300);
			kinect2.drawDepth(440, 340, 400, 300);
#endif
		}else{
			kinectPlayer.draw(20, 20, 400, 300);
			ofEnableAlphaBlending();
			kinectPlayer.drawDepth(20, 340, 400, 300);
			kinectPlayer.drawLabel(20, 340, 400, 300);

			ofDisableAlphaBlending();
			kinectPlayer.drawSkeleton(20, 20, 400, 300);
		}
	}

	ofPushMatrix();
	ofTranslate(35, 35);
	ofFill();
	if(bRecord) {
		ofSetColor(255, 0, 0);
		ofCircle(0, 0, 10);
	}
	if(bPlayback) {
		ofSetColor(0, 255, 0);
		ofTriangle(-10, -10, -10, 10, 10, 0);
	}
	ofPopMatrix();

	stringstream kinectReport;
	if(bPlugged && !kinect.isOpened() && !bPlayback){
		ofSetColor(0, 255, 0);
		kinectReport << "Kinect is plugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}else if(!bPlugged){
		ofSetColor(255, 0, 0);
		kinectReport << "Kinect is unplugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}

	// draw instructions
	ofSetColor(255, 255, 255);


//p

	     depthImage.setROI(0,0,depthImage.width,depthImage.height);

	     vidCalibImageOrig.setFromPixels(kinect.getCalibratedVideoPixels());
		 vidCalibImage=vidCalibImageOrig;
        
		//vidCalibImage.allocate(320,240);
		 depthImage.setFromPixels(kinect.getDepthPixels());
		// depthImage.setFromPixels(kinect.getLabelPixels());
		//  labelImage.setFromPixels(kinect.getLabelPixels());

		 depthImage.threshold(100);
		 contourFinderDepth.findContours(depthImage, 20, (depthImage.width*depthImage.height)/2, 10, true);
	
		  ofRectangle roirect;
		 if(contourFinderDepth.nBlobs>0)
		 {
			  roirect=contourFinderDepth.blobs[0].boundingRect;
			// roirect.height/=2.5;
			 //roirect.y-=roirect.height/3;
			  roirect.y-=roirect.height/5;
			 
			  depthImage.setROI(roirect);
			// // contourFinderDepth.blobs[0].boundingRect.
		 }
		
		 for(int i=0;i<Kine_depth_width;i++)
		 {
		    for(int j=0;j<Kine_depth_height;j++)
			{
			      depthImageOutSubRing.getPixels()[i*Kine_depth_height+j]=0;
				  depthImagePrev.getPixels()[i*Kine_depth_height+j]=0;
				  depthImageResult.getPixels()[i*Kine_depth_height+j]=0;
			}
		 }
		 depthImageOutSubRing.flagImageChanged();
		 depthImagePrev.flagImageChanged();
		 depthImageResult.flagImageChanged();
		 depthImageOutSubRing.setROI(roirect);
		 depthImagePrev.setROI(roirect);
		 depthImageResult.setROI(roirect);
		 depthImagePrev=depthImage;
		 depthImageResult=depthImage;

		 /* for(int i=0;i<depthImage.width;i++)
		 {
		 for(int j=0;j<depthImage.height;j++)
		 {

		 depthImageResult.getCvImage()[i*depthImage.height+j]=depthImage.getCvImage()[i*depthImage.height+j];

		 }
		 }*/
		   
		 float floatscale=1.0f;
		 for(int i=0;i<count_for_Dilate;i++)
		 {

			 depthImage.dilate();
			 depthImageOutSubRing=depthImage;


			 depthImageOutSubRing.absDiff(depthImagePrev);
			 //depthImageOutSubRing.rangeMap(depthImageOutSubRing.getCvImage(),0,255,0,128);
			 cvConvertScale( depthImageOutSubRing.getCvImage(), depthImageOutSubRing.getCvImage(), floatscale*=0.8, 0);
			 depthImageOutSubRing.flagImageChanged();
			 depthImagePrev=depthImage;
			 depthImageResult+=depthImageOutSubRing;
			 //labelImages[0].translate(10,10);
		 }
		 for(int i=0;i<count_for_Erode;i++)
		 {
			 depthImage.erode();
			 //labelImages[0].translate(10,10);
		 }
	


		 unsigned char* depthpixel =depthImageResult.getPixels();//depthImage.getPixels();		 
		 unsigned char* vidCalibpixel =vidCalibImage.getPixels();
		 unsigned char* bkpixels=ofbkGound.getPixels();
	
         unsigned char* depthpixelCombination=    depthImageResult.getPixels();   //
	//for(int ii=0;ii<vidCalibImage.height;ii++)
	//{
	//	for(int jj=0;jj<vidCalibImage.width;jj++)
	//	{  
	//	// if(depthpixel[ii*vidCalibImage.width+jj]<50)
	//	//if(depthpixel[ii*vidCalibImage.width+jj]>50)
	//		{
	//			int graylevel=depthpixel[ii*vidCalibImage.width+jj];
	//			int bk0=bkpixels[int((ii*vidCalibImage.width+jj)*3)];
	//			int bk1=bkpixels[int((ii*vidCalibImage.width+jj)*3)+1];
	//			int bk2=bkpixels[int((ii*vidCalibImage.width+jj)*3)+2];

	//			int f0=vidCalibpixel[int((ii*vidCalibImage.width+jj)*3)];
	//			int f1=vidCalibpixel[int((ii*vidCalibImage.width+jj)*3)+1];
	//			int f2=vidCalibpixel[int((ii*vidCalibImage.width+jj)*3)+2];

	//			vidCalibpixel[int((ii*vidCalibImage.width+jj)*3)]=(f0*graylevel+bk0*(255-graylevel))/255;
	//			vidCalibpixel[int((ii*vidCalibImage.width+jj)*3+1)]=(f1*graylevel+bk1*(255-graylevel))/255;
	//			vidCalibpixel[int((ii*vidCalibImage.width+jj)*3+2)]=(f2*graylevel+bk2*(255-graylevel))/255;


	//	/*	 vidCalibpixel[int((ii*vidCalibImage.width+jj)*3)]=bkpixels[int((ii*vidCalibImage.width+jj)*3)];
	//			vidCalibpixel[int((ii*vidCalibImage.width+jj)*3+1)]=bkpixels[int((ii*vidCalibImage.width+jj)*3)+1];
	//			vidCalibpixel[int((ii*vidCalibImage.width+jj)*3+2)]=bkpixels[int((ii*vidCalibImage.width+jj)*3)+2];*/
	//		}
	//	}
	//}


     vidCalibImage.flagImageChanged();
     vidCalibImage.draw(500,600);
	// ofbkGound.draw(500,300);
	 depthImage.draw(500,20);
	 depthImagePrev.draw(1140,20);
	 depthImageOutSubRing.draw(820,20);
	 depthImageResult.draw(1140,300);
	//depthImage.dra
	ofNoFill();
	ofSetHexColor(0x2bdbe6);
	 ofRectangle roirect2=depthImage.getROI();
	 roirect2.x+=500;
	 roirect2.y+=20;
	ofRect(roirect2);
	ofSetColor(255);

	// depthImage.drawROI(500,20);
	//labelImage.draw(900,20);
		// depthImage.setROI(50,50,100,100);
			 // depthImage.drawROI(720,400);
		 
	//for(int i = 0; i < contourFinderDepth.nBlobs; i++){
		//contourFinderDepth.blobs[i].draw(620, 400);
	//}
	if(contourFinderDepth.nBlobs>0)
	{
	   contourFinderDepth.blobs[0].draw(500,20);
	 	
	}



	stringstream reportStream;
	reportStream << "fps: " << ofGetFrameRate() << "  Kinect Nearmode: " << kinect.isNearmode() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl
				 << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 << "press LEFT and RIGHT to change the far clipping distance: " << farClipping << " mm" << endl
				 << "press '+' and '-' to change the near clipping distance: " << nearClipping << " mm" << endl
				 << "press 'r' to record and 'p' to playback, record is: " << bRecord << ", playback is: " << bPlayback << endl
				 << "press 'v' to show video only: " << bDrawVideo << ",      press 'd' to show depth + users label only: " << bDrawDepthLabel << endl
				 << "press 's' to show skeleton only: " << bDrawSkeleton << ",   press 'q' to show point cloud sample: " << bDrawCalibratedTexture<<endl
				 <<kinect.getLabelPixels().getWidth();
	ofDrawBitmapString(reportStream.str(), 20, 648);
	
}

//--------------------------------------------------------------
void testApp::drawCalibratedTexture(){
	int offsetX = -400;
	int offsetY = -300;
	glTranslatef(512, 386, 0);
	calibratedTexture.loadData(kinect.getCalibratedVideoPixels());
	for(int y = 0; y < 240; y++){
		for(int x = 0; x < 320; x++){
			float distance = kinect.getDistanceAt(x, y);
			if(distance > 500 && distance < 1500){
				glPushMatrix();
				float radius = (1500 - distance);
				ofSetColor(kinect.getCalibratedColorAt(x, y));
				ofRotateY(mRotationY);
				ofRotateX(mRotationX);
				glTranslatef(x * 2.5 + offsetX, y * 2.5 + offsetY, radius);
				ofBox(5);
				glPopMatrix();
			}else{
				glPushMatrix();
				ofSetColor(kinect.getCalibratedColorAt(x, y));
				ofRotateY(mRotationY);
				ofRotateX(mRotationX);
				ofRect(x * 2.5 + offsetX, y * 2.5 + offsetY, 5, 5);
				glPopMatrix();
			}
		}
	}
}


//--------------------------------------------------------------
void testApp::exit() {
	if(calibratedTexture.bAllocated()){
		calibratedTexture.clear();
	}

	if(videoDraw_) {
		videoDraw_->destroy();
		videoDraw_ = NULL;
	}
	if(depthDraw_) {
		depthDraw_->destroy();
		depthDraw_ = NULL;
	}
	if(labelDraw_) {
		labelDraw_->destroy();
		labelDraw_ = NULL;
	}
	if(skeletonDraw_) {
		delete skeletonDraw_;
		skeletonDraw_ = NULL;
	}

	kinect.setAngle(0);
	kinect.close();
	kinect.removeKinectListener(this);
	kinectPlayer.close();
	kinectRecorder.close();


#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch(key){
	case 'v': // draw video only
	case 'V':
		bDrawVideo = !bDrawVideo;
		if(bDrawVideo){
			bDrawCalibratedTexture = false;
			bDrawSkeleton = false;
			bDrawDepthLabel = false;
			glDisable(GL_DEPTH_TEST);
		}
		break;
	case 'd': // draw depth + users label only
	case 'D':
		bDrawDepthLabel = !bDrawDepthLabel;
		if(bDrawDepthLabel){
			bDrawCalibratedTexture = false;
			bDrawVideo = false;
			bDrawSkeleton = false;
			glDisable(GL_DEPTH_TEST);
		}
		break;
	case 's': // draw skeleton only
	case 'S':
		bDrawSkeleton = !bDrawSkeleton;
		if(bDrawSkeleton){
			bDrawCalibratedTexture = false;
			bDrawVideo = false;
			bDrawDepthLabel = false;
			glDisable(GL_DEPTH_TEST);
		}
		break;
	case 'q': // draw point cloud example
	case 'Q':
		bDrawCalibratedTexture = !bDrawCalibratedTexture;
		if(bDrawCalibratedTexture){
			bDrawVideo = false;
			bDrawDepthLabel = false;
			bDrawSkeleton = false;
			glEnable(GL_DEPTH_TEST);
		}
		break;
	case 'o': // open stream
	case 'O':
		kinect.open();
		break;
	case 'c': // close stream
	case 'C':
		kinect.close();
		break;
	case 'r': // record stream
	case 'R':
		if(!bRecord){
			startRecording();
		}else{
			stopRecording();
		}
		break;
	case 'p': // playback recorded stream
	case 'P':
		if(!bPlayback){
			startPlayback();
		}else{
			stopPlayback();
		}
		break;
	case OF_KEY_UP: // up the kinect angle
		angle++;
		if(angle > 27){
			angle = 27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_DOWN: // down the kinect angle
		angle--;
		if(angle < -27){
			angle = -27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_LEFT: // increase the far clipping distance
		if(farClipping > nearClipping + 10){
			farClipping -= 10;
			kinectSource->setFarClippingDistance(farClipping);
		}
		break;
	case OF_KEY_RIGHT: // decrease the far clipping distance
		if(farClipping < 4000){
			farClipping += 10;
			kinectSource->setFarClippingDistance(farClipping);
		}
		break;
	case '+': // increase the near clipping distance
		if(nearClipping < farClipping - 10){
			nearClipping += 10;
			kinectSource->setNearClippingDistance(nearClipping);
		}
		break;
	case '-': // decrease the near clipping distance
		if(nearClipping >= 10){
			nearClipping -= 10;
			kinectSource->setNearClippingDistance(nearClipping);
		}
		break;
	
	case '4':
		if(count_for_Dilate>0)
		{
			count_for_Dilate--;
		}
		break;
	
	case '6':
		count_for_Dilate++;
		break;


	case '8':
		if(count_for_Erode>0)
		{
			count_for_Erode--;
		}
		break;
	
	case '2':
		count_for_Erode++;
		break;


	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
	mRotationY = (x - 512) / 5;
	mRotationX = (384 - y) / 5;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::kinectPlugged(){
	bPlugged = true;
}

//--------------------------------------------------------------
void testApp::kinectUnplugged(){
	bPlugged = false;
}

//--------------------------------------------------------------
void testApp::startRecording(){
	if(!bRecord){
		// stop playback if running
		stopPlayback();

		kinectRecorder.setup(kinect, "recording.dat");
		bRecord = true;
	}
}

//--------------------------------------------------------------
void testApp::stopRecording(){
	if(bRecord){
		kinectRecorder.close();
		bRecord = false;
	}
}

//--------------------------------------------------------------
void testApp::startPlayback(){
	if(!bPlayback){
		stopRecording();
		kinect.close();

		// set record file and source
		kinectPlayer.setup("recording.dat");
		kinectPlayer.loop();
		kinectPlayer.play();
		kinectSource = &kinectPlayer;
		bPlayback = true;
	}
}

//--------------------------------------------------------------
void testApp::stopPlayback(){
	if(bPlayback){
		kinectPlayer.close();
		kinect.open();
		kinectSource = &kinect;
		bPlayback = false;
	}
}
