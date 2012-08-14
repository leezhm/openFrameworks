#include "testApp.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////////

double fInpaintRadius = 2.5;
int nInpaintUsersDilateSteps = 3; // three times

int nAlphaErodeSteps = 2;
int nAlphaDilateSteps = 2;

int nAlphaOffsetX = 1;
int nAlphaOffsetY = 1;

float fInpaintDepthValueNonUsers = 1.6f;
float fAlphaErodeDilateScale = 0.8f;
float fMedianFilterAlphaScale = 0.8f;

float fInpaintResizeScale = 0.98f;

int nMedianFilterDepthSize = 3; // kernel size for median blur
int nMedianFilterAlphaSize = 3;
int nAlphaBlurFilterSize = 3;

int nAlphaErodeDilateKernelSize = 3;
int nAlphaErodeDilateKernelType = 2;

bool bProcessed = false;

float gDepthRangeStartMeters = 1.4f;
float gDepthRangeEndMeters = 1.8f;

//////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------
void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);

	ofSetVerticalSync(true);

	// Set Frame Rate
	ofSetFrameRate(60);

	// Init the kinect sensor
	ofxKinectNui::InitSetting setting;
	setting.grabAudio = false;
	setting.grabCalibratedVideo = true;
	setting.grabDepth = true;
	setting.grabLabel = true;		
	setting.grabLabelCv = true;	
	setting.grabSkeleton = true;
	setting.grabVideo = true;

	// High Color Resolution is better
	setting.depthResolution = NUI_IMAGE_RESOLUTION_640x480;
	setting.videoResolution = NUI_IMAGE_RESOLUTION_640x480;
	
	switch(setting.videoResolution)
	{
	case NUI_IMAGE_RESOLUTION_640x480:
		{
			ColorWidth = 640;
			ColorHeight = 480;
			break;
		}
	case NUI_IMAGE_RESOLUTION_1280x960:
		{
			ColorWidth = 1280;
			ColorHeight = 960;
			break;
		}
	default:
		{
			ColorWidth = 640;
			ColorHeight = 480;

			ofLog(OF_LOG_ERROR)<<"Color -> Unavailable NUI_IMAGE_RESOLUTION ... "<<endl;
			break;
		}
	}

	switch(setting.depthResolution)
	{
	case NUI_IMAGE_RESOLUTION_640x480:
		{
			DepthWidth = 640;
			DepthHeight = 480;
			break;
		}
	case NUI_IMAGE_RESOLUTION_320x240:
		{
			DepthWidth = 320;
			DepthHeight = 240;
			break;
		}
	default:
		{
			DepthWidth = 640;
			DepthHeight = 480;

			ofLog(OF_LOG_ERROR)<<"Depth -> Unavailable NUI_IMAGE_RESOLUTION ... "<<endl;
			break;
		}
	}

	KinectSensor.init(setting);

	// Open the kinect sensor
	KinectSensor.open();

	// Get current angle of sensor
	Angle = KinectSensor.getCurrentAngle();

	// Allocate image memory
	ColorImage.allocate(ColorWidth, ColorHeight);

	// Allocate the threshold image
	depthImage.allocate(DepthWidth, DepthHeight);
	userImage.allocate(DepthWidth, DepthHeight);

	// Allocate colorful player image
	// Allocate image memory
	ColorPlayer.allocate(ColorWidth, ColorHeight);

	// Loading the background image
	backgroundImage.loadImage("Background.jpg");

	// load the config
	xmlSetting.loadFile("config.xml");
	nearClipping = xmlSetting.getValue("KINECT:CLIPPING:NEAR", KinectSensor.getNearClippingDistance());
	farClipping = xmlSetting.getValue("KINECT:CLIPPING:FAR", KinectSensor.getFarClippingDistance());
	threshold = xmlSetting.getValue("KINECT:OPENCV:THRESHOLD", 25);
	KinectSensor.setFarClippingDistance(farClipping);
	KinectSensor.setNearClippingDistance(nearClipping);

	//////////////////////////////////////////////////////////////////////////
	// Init processing variable
	//////////////////////////////////////////////////////////////////////////
	ofxCvKinectRGB.allocate(ColorWidth, ColorHeight);
	ofxCvKinectAlpha.allocate(DepthWidth, DepthHeight);
	ofxCvKinectDepth.allocate(DepthWidth, DepthHeight);
	ofxCvKinectMask.allocate(DepthWidth, DepthHeight);
	ofxCvKinectUsers.allocate(DepthWidth, DepthHeight);

	resultImage.allocate(DepthWidth, DepthHeight, OF_IMAGE_COLOR_ALPHA);


	//////////////////////////////////////////////////////////////////////////
	gui.setup("Adjust Panel");
	gui.add(InpaintRadiusSlider.setup("Radius", xmlSetting.getValue("KINECT:PROCESSING:INPAINT:RADIUS", 2.8), 0.01, 100));
	gui.add(InpaintResizeScaleSlider.setup("ResizeScale", xmlSetting.getValue("KINECT:PROCESSING:INPAINT:RESIZESCALE", 800), 10, 998));
	gui.add(InpaintUsersDilateStepSlider.setup("UsersDilateStep", xmlSetting.getValue("KINECT:PROCESSING:INPAINT:USERDILATESTEP", 2), 1, 10));
	
	gui.add(InpaintDepthValueNonUserSlider.setup("DepthValueNonUser", xmlSetting.getValue("KINECT:PROCESSING:INPAINT:DEPTHNONUSER", 1.75), 1.5, 3.0));
	gui.add(DepthRangeStartMeters.setup("RangeStart", xmlSetting.getValue("KINECT:PROCESSING:DEPTH:START", 1.6), 1.2, 2.0));
	gui.add(DepthRangeEndMeters.setup("RangeEnd", xmlSetting.getValue("KINECT:PROCESSING:DEPTH:END", 1.9), 1.4, 2.4));
	
	gui.add(MedianFilterDepthSizeSlider.setup("MedianFilterDepthSize", xmlSetting.getValue("KINECT:PROCESSING:MEDIAN:DEPTHSIZE", 3), 3, 9));
	gui.add(MedianFilterAlphaSizeSlider.setup("MedianFilterAlphaSize", xmlSetting.getValue("KINECT:PROCESSING:MEDIAN:ALPHASIZE", 3), 3, 9));
	
	gui.add(AlphaBlurFilterSizeSlider.setup("AlphaBlurFilterSize", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:BLURSIZE", 3), 3, 9));
	gui.add(AlphaErodeDilateKernelSizeSlider.setup("AlphaErodeDilateKernelSize", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:EDKERNELSIZE", 2), 1, 9));

	gui.add(AlphaErodeStepSlider.setup("AlphaErodeStep", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:ERODESTEP", 2), 1, 10));
	gui.add(AlphaDilateStepSlider.setup("AlphaDilateStep", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:DILATESTEP", 2), 1, 10));

	gui.add(AlphaOffSetXSlider.setup("OffSetX", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:OFFSETX", 1), -20, 20));
	gui.add(AlphaOffSetYSlider.setup("OffSetY", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:OFFSETY", 1), -20, 20));

	gui.add(AlphaErodeDilateScaleSlider.setup("AlphaErodeDilateScale", xmlSetting.getValue("KINECT:PROCESSING:ALPHA:EDSCALE", 40), 10, 90));
	gui.add(MedianFilterAlphaScaleSlider.setup("MedianFilterAlphaScale", xmlSetting.getValue("KINECT:PROCESSING:MEDIAN:ALPHASCALE", 40), 10, 90));

	// Do some testing
	//////////////////////////////////////////////////////////////////////////
	////cv::Mat doMat(6, 8, CV_16UC1, cv::Scalar::all(8));
	////cv::randu(doMat, cv::Scalar::all(2), cv::Scalar::all(10));
	////std::cout<<"doMat "<<doMat<<std::endl;

	////cv::Mat outMat;
	////doMat.convertTo(outMat, CV_8UC1, 0.5);
	////std::cout<<"\noutMat "<<outMat<<std::endl;

	////int nSizeX = 8 * 0.75;
	////int nSizeY = 6 * 0.75;
	////std::cout<<"\nnSizeX = "<<nSizeX<<"  nSizeY = "<<nSizeY<<std::endl;

	////cv::Mat rsOutMat;

	////cv::resize(outMat, rsOutMat, cvSize(nSizeX, nSizeY), 0.0f, 0.0f, cv::INTER_NEAREST);
	////std::cout<<"\nrsOutMat "<<rsOutMat<<std::endl;

	////cv::Mat equalMat = (rsOutMat == 4);
	////std::cout<<"\nequalMat "<<equalMat<<std::endl;

	////cv::Mat rsProcessedMat;
	////cv::Mat rsMask(nSizeY, nSizeX, CV_8UC1, cv::Scalar::all(250));
	////cv::randu(rsMask, cv::Scalar::all(1), cv::Scalar::all(255));
	////cv::inpaint(rsOutMat, rsMask, rsProcessedMat, 3, cv::INPAINT_TELEA);
	////std::cout<<"\nrsMask "<<rsMask<<std::endl;
	////std::cout<<"\nrsProcessedMat "<<rsProcessedMat<<std::endl;
}

//--------------------------------------------------------------
void testApp::update()
{
	//std::cout<<"radius -> "<<fInpaintResizeScaleSlider<<std::endl;
	fInpaintResizeScale = InpaintResizeScaleSlider / 1000.0f;
	fInpaintRadius = InpaintRadiusSlider;
	nInpaintUsersDilateSteps = InpaintUsersDilateStepSlider;

	fInpaintDepthValueNonUsers = InpaintDepthValueNonUserSlider;
	gDepthRangeStartMeters = DepthRangeStartMeters;
	gDepthRangeEndMeters = DepthRangeEndMeters;

	if(((MedianFilterDepthSizeSlider % 2) == 0) && (MedianFilterDepthSizeSlider > 0))
	{
		MedianFilterDepthSizeSlider--;
	}
	nMedianFilterDepthSize = MedianFilterDepthSizeSlider;

	if(((MedianFilterAlphaSizeSlider % 2) == 0) && (MedianFilterAlphaSizeSlider > 0))
	{
		MedianFilterAlphaSizeSlider--;
	}
	nMedianFilterAlphaSize = MedianFilterAlphaSizeSlider;

	if(((AlphaBlurFilterSizeSlider % 2) == 0) && (AlphaBlurFilterSizeSlider > 0))
	{
		AlphaBlurFilterSizeSlider--;
	}
	nAlphaBlurFilterSize = AlphaBlurFilterSizeSlider;

	nAlphaErodeDilateKernelSize = AlphaErodeDilateKernelSizeSlider;

	nAlphaErodeSteps = AlphaErodeStepSlider;
	nAlphaDilateSteps = AlphaDilateStepSlider;

	nAlphaOffsetX = AlphaOffSetXSlider;
	nAlphaOffsetY = AlphaOffSetYSlider;

	fAlphaErodeDilateScale = AlphaErodeDilateScaleSlider / 50.0f;
	fMedianFilterAlphaScale = MedianFilterAlphaScaleSlider / 50.0f;

	// update the kinect sensor
	KinectSensor.update();

	if(KinectSensor.isOpened())
	{
		ColorImage.setFromPixels(KinectSensor.getVideoPixels());

		// depth Image
		depthImage.setFromPixels(KinectSensor.getDepthPixels());

		// user Image
		userImage.setFromPixels(KinectSensor.getLabelPixelsCv(0));


		//////////////////////////////////////////////////////////////////////////
		//
		//	Copy data
		//
		//////////////////////////////////////////////////////////////////////////
		//ofxCvKinectDepth.setFromPixels(KinectSensor.getDistancePixels().getPixels(), ColorWidth, ColorHeight);

		// the ofxCvGrayscaleImage had override the operator '='
		ofxCvKinectUsers = userImage;
		ofxCvKinectRGB = ColorImage;
		
		ofxCvKinectMask.set(128);
		ofxCvKinectAlpha.set(128);


		ColorPlayer.setFromPixels(KinectSensor.getCalibratedVideoPixels());

		Processing();

		////ofColor color;
		////color.set(0, 0, 0);
		////
		////for(int hIndex = 0; hIndex < userImage.getHeight(); hIndex ++)
		////{
		////	for(int wIndex = 0; wIndex < userImage.getWidth(); wIndex ++)
		////	{
		////		ofColor color = userImage.getPixelsRef().getColor(wIndex, hIndex);

		////		if(color.r == 0 && color.g == 0 && color.b == 0)
		////		{
		////			ColorPlayer.getPixelsRef().setColor(wIndex, hIndex, backgroundImage.getColor(wIndex, hIndex));
		////		}
		////	}
		////}
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	// Set background
	ofBackgroundGradient(ofColor::white, ofColor::gray);

	ofEnableAlphaBlending();
	//ColorImage.draw(0, 10, 640, 480);

	//ColorPlayer.flagImageChanged();
	ColorPlayer.draw(1280, 20, 640, 480);

	ofDisableAlphaBlending();

	// depth Image
	depthImage.draw(0, 20, DepthWidth, DepthHeight);

	// user image
	userImage.draw(640, 20, DepthWidth, DepthHeight);

	// 
	// draw the ofx...
	ofxCvKinectRGB.draw(0, 520, 320, 240);
	ofxCvKinectUsers.draw(320, 520, 320, 240);
	ofxCvKinectMask.draw(640, 520, 640, 480);
	ofxCvKinectAlpha.draw(1280, 520, 640, 480);

	resultImage.draw(0, 520, 640, 480);

	// draw title
	ofSetColor(255, 255, 255);

	stringstream strDepth("KinectSensor.getDepthPixels()");
	ofDrawBitmapString(strDepth.str(), 0, 12);

	stringstream strUserImage("KinectSensor.getLabelPixelsCv(0)");
	ofDrawBitmapString(strUserImage.str(), 640, 12);

	stringstream strColorPlayer("KinectSensor.getCalibratedVideoPixels() and userImage");
	ofDrawBitmapString(strColorPlayer.str(), 1280, 12);

	stringstream strKRGB("KinectSensor.getVideoPixels()");
	ofDrawBitmapString(strKRGB.str(), 0, 512);

	stringstream strKUsers("KinectSensor.getLabelPixelsCv(0)");
	ofDrawBitmapString(strKUsers.str(), 320, 512);

	stringstream strKMask("ofxCvKinectMask.set(128)");
	ofDrawBitmapString(strKMask.str(), 640, 512);

	stringstream strKAlpha("ofxCvKinectAlpha.set(255)");
	ofDrawBitmapString(strKAlpha.str(), 1280, 512);

	// draw instructions
	stringstream reportStream;
	reportStream << "(press: < >), fps: " << ofGetFrameRate() << endl
		<< "press 'c' to close the stream and 'o' to open it again, stream is: " << KinectSensor.isOpened() << endl
		<< "press UP and DOWN to change the tilt angle: " << Angle << " degrees" << endl
		<< "press UP and DOWN to change the threshold: " << threshold << " degrees" << endl
		<< "press PgUp and PgDn to change the far clipping distance: " << farClipping << " mm" << endl
		<< "press Home and End to change the near clipping distance: " << nearClipping << " mm" << endl
		<< "press 's' or 'S' to save the near/ far clipping distance to xml file." << endl;
	ofDrawBitmapString(reportStream.str(), 10, 820);

	gui.draw();
}


//--------------------------------------------------------------
void testApp::exit()
{
	// Reset the angle = 0;
	KinectSensor.setAngle(0);

	// Close the kinect sensor
	if(KinectSensor.isOpened())
	{
		KinectSensor.close();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed (int key)
{
	switch(key)
	{
	case OF_KEY_LEFT:
		{
			threshold --;
			depthImage.threshold(threshold);

			break;
		}
	case OF_KEY_RIGHT:
		{
			threshold ++;
			depthImage.threshold(threshold);

			break;
		}
	case OF_KEY_UP:
		{
			Angle ++;

			if(27 < Angle)
			{
				Angle = 27;
			}

			KinectSensor.setAngle(Angle);
			break;
		}
	case OF_KEY_DOWN:
		{
			Angle --;

			if(-27 > Angle)
			{
				Angle = -27;
			}

			KinectSensor.setAngle(Angle);

			break;
		}
	case OF_KEY_PAGE_UP:
		{
			if(farClipping < 4000)
			{
				farClipping += 10;
				KinectSensor.setFarClippingDistance(farClipping);
			}
							
			break;
		}
	case OF_KEY_PAGE_DOWN:
		{
			if (farClipping > nearClipping + 10)
			{
				farClipping -= 10;
				KinectSensor.setFarClippingDistance(farClipping);
			}
			
			break;
		}
	case OF_KEY_HOME:
		{
			if (nearClipping < farClipping -10)
			{
				nearClipping += 10;
				KinectSensor.setNearClippingDistance(nearClipping);
			}

			break;
		}
	case OF_KEY_END:
		{
			if (nearClipping >= 800)
			{
				nearClipping -= 10;
				KinectSensor.setNearClippingDistance(nearClipping);
			}

			break;
		}
	case 's':
	case 'S':
		{
			xmlSetting.setValue("KINECT:CLIPPING:NEAR", nearClipping);
			xmlSetting.setValue("KINECT:CLIPPING:FAR", farClipping);
			xmlSetting.setValue("KINECT:OPENCV:THRESHOLD", threshold);

			xmlSetting.setValue("KINECT:PROCESSING:INPAINT:RADIUS", fInpaintRadius);
			xmlSetting.setValue("KINECT:PROCESSING:INPAINT:RESIZESCALE", fInpaintResizeScale * 1000.0f);
			xmlSetting.setValue("KINECT:PROCESSING:INPAINT:USERDILATESTEP", nInpaintUsersDilateSteps);

			xmlSetting.setValue("KINECT:PROCESSING:INPAINT:DEPTHNONUSER", fInpaintDepthValueNonUsers);
			xmlSetting.setValue("KINECT:PROCESSING:DEPTH:START", gDepthRangeStartMeters);
			xmlSetting.setValue("KINECT:PROCESSING:DEPTH:END", gDepthRangeEndMeters);

			xmlSetting.setValue("KINECT:PROCESSING:MEDIAN:DEPTHSIZE", nMedianFilterDepthSize);
			xmlSetting.setValue("KINECT:PROCESSING:MEDIAN:ALPHASIZE", nMedianFilterAlphaSize);

			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:BLURSIZE", nAlphaBlurFilterSize);
			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:EDKERNELSIZE", nAlphaErodeDilateKernelSize);

			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:ERODESTEP", nAlphaErodeSteps);
			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:DILATESTEP", nAlphaDilateSteps);

			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:OFFSETX", nAlphaOffsetX);
			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:OFFSETY", nAlphaOffsetY);

			xmlSetting.setValue("KINECT:PROCESSING:ALPHA:EDSCALE", fAlphaErodeDilateScale * 50.0f);
			xmlSetting.setValue("KINECT:PROCESSING:MEDIAN:ALPHASCALE", fMedianFilterAlphaScale * 50.0f);


			xmlSetting.saveFile("config.xml");
		}
		
		break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
}

//
void testApp::MappingColorPlayer(ofImage & player)
{

}


//////////////////////////////////////////////////////////////////////////
//
//	depth image 处理
//
//////////////////////////////////////////////////////////////////////////

void testApp::Processing()
{
	// set the depth pixels
	cv::Mat ofxMatKinectDepth(DepthWidth, DepthHeight, CV_16UC1, KinectSensor.getDistancePixels().getPixels());

	cv::Mat ofxMatKinectUsers(ofxCvKinectUsers.getCvImage());
	cv::Mat ofxMatKinectMask(ofxCvKinectMask.getCvImage());
	cv::Mat ofxMatKinectAlpha(ofxCvKinectAlpha.getCvImage());
	cv::Mat ofxMatKinectRGB(ofxCvKinectRGB.getCvImage());

	cv::Mat ofxMatKinectProcessed;

	unsigned char * ofxKinectAlphaPixels = new unsigned char[DepthWidth * 4 * DepthHeight];

	if(0.0001f < fInpaintRadius)
	{
		// Dilate users buffer
		if(0 < nInpaintUsersDilateSteps)
		{
			// 对Player单通道图进行膨胀
			cv::Mat matUsersDilated;
			cv::dilate(ofxMatKinectUsers, matUsersDilated, cv::Mat(), cv::Point(-1, -1), nInpaintUsersDilateSteps);
			matUsersDilated.copyTo(ofxMatKinectUsers);
		}

		// Inpaint manually unknown z pixels outside users buffer with a constant value and discard them from the opencv inpaint
		unsigned short usDepthValue = (unsigned short)(fInpaintDepthValueNonUsers * 1000.0f);

		for (int row = 0; row < ofxMatKinectUsers.rows; ++ row)
		{
			unsigned char * userRow = ofxMatKinectUsers.ptr<unsigned char>(row);
			unsigned char *	maskRow = ofxMatKinectMask.ptr<unsigned char>(row);
			unsigned short * depthRow = ofxMatKinectDepth.ptr<unsigned short>(row);

			for (int col = 0; col < ofxMatKinectUsers.cols; ++ col)
			{
				if(0 != maskRow[col] && 0 != userRow[col])
				{
					depthRow[col] = usDepthValue;
					maskRow[col] = 0;
				}
			}
		}

		// Inpaint Z-index
		if(0.999f > fInpaintResizeScale)
		{
			cv::Mat kinectDepth8(DepthWidth, DepthHeight, CV_8UC1, cv::Scalar::all(0));
			cv::Mat kinectDepth16;
			cv::Mat resizedDepth;
			cv::Mat resizedMask;
			cv::Mat resizedProcessed;
			cv::Mat processedResizedBack;
			cv::Mat blurred;

			double minVal, maxVal;

			// Get the min and max distance in depth image
			cv::minMaxLoc(ofxMatKinectDepth, &minVal, &maxVal, NULL, NULL);

			// Inpaint Z-index values in a reduced buffer and copy them back only in the masked positions
			ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0/maxVal);

			int newSizeCols = ofxMatKinectDepth.cols * fInpaintResizeScale;
			int newSizeRows = ofxMatKinectDepth.rows * fInpaintResizeScale;

			// rescale
			cv::resize(kinectDepth8, resizedDepth, cvSize(newSizeCols, newSizeRows), 0.0f, 0.0f, cv::INTER_NEAREST);
			cv::resize(ofxMatKinectMask, resizedMask, cvSize(newSizeCols, newSizeRows), 0.0f, 0.0f, cv::INTER_NEAREST);

			// Inpaint
			cv::inpaint(resizedDepth, (resizedMask == 255), resizedProcessed, fInpaintRadius, cv::INPAINT_TELEA);

			// recover
			cv::resize(resizedProcessed, processedResizedBack, ofxMatKinectDepth.size());
			
			// recover
			processedResizedBack.convertTo(kinectDepth16, CV_16UC1, maxVal / 255.0);
			kinectDepth16.copyTo(ofxMatKinectDepth, (ofxMatKinectMask == 255));

			// Apply median filter to the result
			if(0 < nMedianFilterDepthSize)
			{
				// this is the main trick, apply median to mask, not to depth values
				ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0 / maxVal);
				cv::medianBlur(kinectDepth8, blurred, nMedianFilterDepthSize);
				blurred.convertTo(ofxMatKinectProcessed, CV_16UC1, maxVal / 255.0);
			}
			else
			{
				ofxMatKinectDepth.copyTo(ofxMatKinectProcessed);
			}

			bProcessed = true;
		}
	}
	else if (0 < nMedianFilterDepthSize)
	{
		// only median filter for the depth, main trick
		double maxVal, minVal;
		cv::minMaxLoc(ofxMatKinectDepth, &minVal, &maxVal, NULL, NULL);

		cv::Mat blurred;
		cv::Mat kinectDepth8;
		
		ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0 / maxVal);
		cv::medianBlur(kinectDepth8, blurred, nMedianFilterDepthSize);
		blurred.convertTo(ofxMatKinectProcessed, CV_16UC1, maxVal / 255.0);

		bProcessed = true;
	}

	if(bProcessed)
	{
		// Remove depths out of range and create alpha mask(0 or 255)
		float fInvRange255 = 255.0f / (gDepthRangeEndMeters - gDepthRangeStartMeters);
		float fToMeters = 1.0f / 1000.0f;

		for (int row = 0; row < DepthHeight; ++ row)
		{
			unsigned short * pDepthRows = ofxMatKinectProcessed.ptr<unsigned short>(row);
			unsigned char  * pAlphaRows = ofxMatKinectAlpha.ptr<unsigned char>(row);

			for (int col = 0; col < DepthWidth; ++ col)
			{
				float fDepth = pDepthRows[col] * fToMeters;
				int nDepth = 0;

				//if (fDepth >= gDepthRangeStartMeters || fDepth <= gDepthRangeEndMeters)
				{
					nDepth = (int)((fDepth - gDepthRangeStartMeters) * fInvRange255);

					if(0 > nDepth || 255 < nDepth)
					{
						nDepth = 0;
					}

					pAlphaRows[col] = nDepth > 0 ? 255 : 0;
				}
			}
		}
	}

	// Offset alpha mask
	// this is the main other thing to get the the depth to align properly to color
	if(0 != nAlphaOffsetX || 0 != nAlphaOffsetY)
	{
		cv::Point2f srcPoints[3], dstPoints[3];

		srcPoints[0].x = 0;
		srcPoints[0].y = 0;
		srcPoints[1].x = DepthWidth;
		srcPoints[1].y = 0;
		srcPoints[2].x = 0;
		srcPoints[2].y = DepthHeight;

		float fAlphaScale = 1.0f;
		
		for (int i = 0; i < 3; ++ i)
		{
			dstPoints[i].x = (((srcPoints[i].x - (DepthWidth  / 2)) * fAlphaScale) + (DepthWidth  / 2)) + nAlphaOffsetX;
			dstPoints[i].y = (((srcPoints[i].y - (DepthHeight / 2)) * fAlphaScale) + (DepthHeight / 2)) + nAlphaOffsetY;
		}

		cv::Mat affine;
		cv::Mat warped;
		affine = cv::getAffineTransform(srcPoints, dstPoints);
		cv::warpAffine(ofxMatKinectAlpha, warped, affine, ofxMatKinectAlpha.size());
		warped.copyTo(ofxMatKinectAlpha);
		
		bProcessed = true;
	}

	// Erode & Dilate
	bool bErodeDilateResize = false;
	cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));

	if(0 == nAlphaErodeDilateKernelType)
	{
		structuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
	}
	else if (1 == nAlphaErodeDilateKernelType)
	{
		structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
	}
	else if (2 == nAlphaErodeDilateKernelType)
	{
		structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
	}

	if((nAlphaErodeSteps > 0 || nAlphaDilateSteps > 0) && fAlphaErodeDilateScale != 1.0f)
	{
		cv::Mat erodeDilateScale;
		cv::resize(ofxMatKinectAlpha, erodeDilateScale, cvSize(DepthWidth * fAlphaErodeDilateScale, DepthHeight * fAlphaErodeDilateScale),
				   0.0f, 0.0f, cv::INTER_LINEAR);
		erodeDilateScale.copyTo(ofxMatKinectAlpha);

		bErodeDilateResize = true;
	}

	if(0 < nAlphaErodeSteps && 0 < nAlphaDilateSteps)
	{
		cv::Mat intermediate;
		cv::erode(ofxMatKinectAlpha, intermediate, structuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
		cv::dilate(intermediate, ofxMatKinectAlpha, structuringElement, cv::Point(-1, -1), nAlphaDilateSteps);

		bProcessed = true;
	}
	else if (0 < nAlphaErodeSteps)
	{
		cv::Mat erode;
		cv::dilate(ofxMatKinectAlpha, erode, structuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
		erode.copyTo(ofxMatKinectAlpha);

		bProcessed = true;
	} 
	else if(0 < nAlphaDilateSteps)
	{
		cv::Mat dilate;
		cv::dilate(ofxMatKinectAlpha, dilate, structuringElement, cv::Point(-1, -1), nAlphaDilateSteps);
		dilate.copyTo(ofxMatKinectAlpha);

		bProcessed = true;
	}

	if(0 < nMedianFilterAlphaSize)
	{
		bool bAlphaMedianResized = false;
		
		if(1.0f != fMedianFilterAlphaScale)
		{
			cv::Mat scaled;
			cv::resize(ofxMatKinectAlpha, scaled, cvSize(DepthWidth * fMedianFilterAlphaScale, DepthHeight * fMedianFilterAlphaScale),
				       0.0f, 0.0f, cv::INTER_LINEAR);
			scaled.copyTo(ofxMatKinectAlpha);

			bAlphaMedianResized = true;
		}

		cv::Mat filtered;
		cv::medianBlur(ofxMatKinectAlpha, filtered, nMedianFilterAlphaSize);

		if(bAlphaMedianResized)
		{
			cv::resize(filtered, ofxMatKinectAlpha, cvSize(DepthWidth, DepthHeight), 0.0f, 0.0f, cv::INTER_LINEAR);
		}
		else
		{
			filtered.copyTo(ofxMatKinectAlpha);
		}

		bProcessed = true;
	}

	// Blur Alpha
	if(0 < nAlphaBlurFilterSize)
	{
		cv::Mat blurred;
		cv::GaussianBlur(ofxMatKinectAlpha, blurred, cvSize(nAlphaBlurFilterSize, nAlphaBlurFilterSize), 0.0f, 0.0f);
		blurred.copyTo(ofxMatKinectAlpha);

		bProcessed = true;
	}


	ofColor color;
	color.set(0, 0, 0);

	for(int hIndex = 0; hIndex < DepthHeight; hIndex ++)
	{
		unsigned char * alpha = ofxMatKinectAlpha.ptr<unsigned char>(hIndex);

		for(int wIndex = 0; wIndex < DepthWidth; wIndex ++)
		{
			if((int)alpha[wIndex] == 0)
			{
				ColorPlayer.getPixelsRef().setColor(wIndex, hIndex, backgroundImage.getColor(wIndex, hIndex));
			}
		}
	}
}