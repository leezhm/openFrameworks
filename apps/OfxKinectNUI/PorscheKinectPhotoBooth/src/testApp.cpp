#include "ofxKinectNuiDraw.h"

#include "testApp.h"

#include <iostream>

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
	atexit(Exit);

	ofSetLogLevel(OF_LOG_VERBOSE);

	ofSetVerticalSync(true);

	// Set Frame Rate
	ofSetFrameRate(60);

	//leftSwipeTime = 0;
	//leftRaiseTime = 0;

	//rightSwipeTime = 0;
	//rightRaiseTime = 0;

	InitKinectSensor();

	// Get current angle of sensor
	Angle = KinectSensor.getCurrentAngle();

	// Allocate image memory
	ColorImage.allocate(ColorWidth, ColorHeight);

	// Allocate the threshold image
	//depthImage.allocate(DepthWidth, DepthHeight);
	//userImage.allocate(DepthWidth, DepthHeight);

	// Allocate colorful player image
	// Allocate image memory
	////ColorPlayer.allocate(ColorWidth, ColorHeight);

	// Loading the background image
	backgroundImage.loadImage("Background.jpg");

	// load the config
	xmlSetting.loadFile("config.xml");
	nearClipping = xmlSetting.getValue("KINECT:PROCESSING:DEPTH:START", 1.6) * 1000.0f;
	farClipping = xmlSetting.getValue("KINECT:PROCESSING:DEPTH:END", 1.9) * 1000.0f;
	threshold = xmlSetting.getValue("KINECT:OPENCV:THRESHOLD", 25);
	KinectSensor.setFarClippingDistance(farClipping);
	KinectSensor.setNearClippingDistance(nearClipping);

	//////////////////////////////////////////////////////////////////////////
	// Init processing variable
	//////////////////////////////////////////////////////////////////////////
	//ofxCvKinectRGB.allocate(ColorWidth, ColorHeight);
	//ofxCvKinectAlpha.allocate(DepthWidth, DepthHeight);
	//ofxCvKinectDepth.allocate(DepthWidth, DepthHeight);
	//ofxCvKinectMask.allocate(DepthWidth, DepthHeight);
	//ofxCvKinectUsers.allocate(DepthWidth, DepthHeight);

	//ofxKinectResult.allocate(DepthWidth, DepthHeight);

	////////////////////////////////////////////////////////////////////////////
	//skeltonDraw = new ofxKinectNuiDrawSkeleton();
	//KinectSensor.setSkeletonDrawer(skeltonDraw);

	//for (int i = 0; i < 6; ++ i)
	//{
	//	ofPoint p(0, 0, 0);
	//	skeletonDatas[i] = &p;
	//}

	// QR Code Scan
	hasGetQRCode = false;
	ofxCvQrScanImage.allocate(ColorWidth, ColorHeight);

	// Json Data
}

//--------------------------------------------------------------
void testApp::update()
{
	// update the kinect sensor
	KinectSensor.update();

	if(KinectSensor.isOpened())
	{
		ColorImage.setFromPixels(KinectSensor.getVideoPixels());

		//// depth Image
		//depthImage.setFromPixels(KinectSensor.getDepthPixels());

		//// user Image
		//userImage.setFromPixels(KinectSensor.getLabelPixelsCv(0));

		//////////////////////////////////////////////////////////////////////////
		//
		//
		if(!hasGetQRCode)
		{
			ofxCvQrScanImage = ColorImage;
			ofxCvQrScanImage.mirror(false, true);

			std::string qrString = qrDecoder.FindQRString(ofxCvQrScanImage);
#ifdef _DEBUG
			std::cout<<"QR Code = "<<qrString<<std::endl;
#endif // _DEBUG

//			//string url = "http://porsche.herokuapp.com/api/getuser?uuid=c49e1c83-96e8-42a9-9003-0d3217f1d070";
//			//bool parsingSuccessful = jsonGetInfo.open(url);
//
//			if(!parsingSuccessful)
//			{
//#ifdef _DEBUG
//				std::cout<<"Failed to parse JSON ... \n";
//#endif // _DEBUG
//			}
//
//			string profile = "Name: " + jsonGetInfo["data"]["name"].asString();
//
//#ifdef _DEBUG
//			std::cout<<profile<<std::endl;
//#endif // _DEBUG
		}

		// the ofxCvGrayscaleImage had override the operator '='
		//ofxCvKinectUsers = userImage;
		//ofxCvKinectRGB = ColorImage;
		//
		//ofxCvKinectMask.set(128);
		//ofxCvKinectAlpha.set(128);

		//KinectSensor.getSkeletonPoints(skeletonDatas);
		//GestureGenerator();

		//ColorPlayer.setFromPixels (KinectSensor.getCalibratedVideoPixels());

		//Processing();
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	//// Set background
	//ofBackgroundGradient(ofColor::white, ofColor::gray);

	//ofEnableAlphaBlending();
	//ColorImage.draw(0, 10, 640, 480);

	////ColorPlayer.flagImageChanged();
	//ColorPlayer.draw(1280, 20, 640, 480);

	//ofDisableAlphaBlending();

	//// Draw skeleton
	//KinectSensor.drawSkeleton(320, 320);


	//// draw title
	//ofSetColor(255, 255, 255);

	//stringstream strDepth("KinectSensor.getDepthPixels()");
	//ofDrawBitmapString(strDepth.str(), 0, 12);

	//stringstream strUserImage("KinectSensor.getLabelPixelsCv(0)");
	//ofDrawBitmapString(strUserImage.str(), 640, 12);

	//stringstream strColorPlayer("KinectSensor.getCalibratedVideoPixels() and userImage");
	//ofDrawBitmapString(strColorPlayer.str(), 1280, 12);

	//stringstream strKRGB("KinectSensor.getVideoPixels()");
	//ofDrawBitmapString(strKRGB.str(), 0, 512);

	//stringstream strKUsers("KinectSensor.getLabelPixelsCv(0)");
	//ofDrawBitmapString(strKUsers.str(), 320, 512);

	//stringstream strKMask("ofxCvKinectMask.set(128)");
	//ofDrawBitmapString(strKMask.str(), 640, 512);

	//stringstream strKAlpha("ofxCvKinectAlpha.set(255)");
	//ofDrawBitmapString(strKAlpha.str(), 1280, 512);

	//// draw instructions
	//stringstream reportStream;
	//reportStream << "(press: < >), fps: " << ofGetFrameRate() << endl
	//	<< "press 'c' to close the stream and 'o' to open it again, stream is: " << KinectSensor.isOpened() << endl
	//	<< "press UP and DOWN to change the tilt angle: " << Angle << " degrees" << endl
	//	<< "press UP and DOWN to change the threshold: " << threshold << " degrees" << endl
	//	<< "press PgUp and PgDn to change the far clipping distance: " << farClipping * 1000.0f << " mm" << endl
	//	<< "press Home and End to change the near clipping distance: " << nearClipping * 1000.0f << " mm" << endl
	//	<< "press 's' or 'S' to save the near/ far clipping distance to xml file." << endl;
	//ofDrawBitmapString(reportStream.str(), 10, 820);

	//gui.draw();

	backgroundImage.draw(0, 0, 1366, 768);


	if (!hasGetQRCode)
	{
		ofxCvQrScanImage.draw(1046, 528, 320, 240);
	}
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
	//case OF_KEY_LEFT:
	//	{
	//		threshold --;
	//		depthImage.threshold(threshold);

	//		break;
	//	}
	//case OF_KEY_RIGHT:
	//	{
	//		threshold ++;
	//		depthImage.threshold(threshold);

	//		break;
	//	}
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

			break;
		}
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

void testApp::InitKinectSensor()
{
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
}


//////////////////////////////////////////////////////////////////////////
//
//	depth image ����
//
//////////////////////////////////////////////////////////////////////////

//void testApp::Processing()
//{
//	// set the depth pixels
//	//
//	//	Caution, Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP);
//	//
//	//	First param is row and second is col.
//	//
//	cv::Mat ofxMatKinectDepth(DepthHeight, DepthWidth, CV_16UC1, KinectSensor.getDistancePixels().getPixels());
//
//	cv::Mat ofxMatKinectUsers(ofxCvKinectUsers.getCvImage());
//	cv::Mat ofxMatKinectMask(ofxCvKinectMask.getCvImage());
//	cv::Mat ofxMatKinectAlpha(ofxCvKinectAlpha.getCvImage());
//	cv::Mat ofxMatKinectRGB(ofxCvKinectRGB.getCvImage());
//
//	cv::Mat ofxMatKinectProcessed;
//
//	if(0.0001f < fInpaintRadius)
//	{
//		// Dilate users buffer
//		if(0 < nInpaintUsersDilateSteps)
//		{
//			// ��Player��ͨ��ͼ��������
//			cv::Mat matUsersDilated;
//			cv::dilate(ofxMatKinectUsers, matUsersDilated, cv::Mat(), cv::Point(-1, -1), nInpaintUsersDilateSteps);
//			matUsersDilated.copyTo(ofxMatKinectUsers);
//		}
//
//		// Inpaint manually unknown z pixels outside users buffer with a constant value and discard them from the opencv inpaint
//		unsigned short usDepthValue = (unsigned short)(fInpaintDepthValueNonUsers * 1000.0f);
//
//		for (int row = 0; row < ofxMatKinectUsers.rows; ++ row)
//		{
//			unsigned char * userRow = ofxMatKinectUsers.ptr<unsigned char>(row);
//			unsigned char *	maskRow = ofxMatKinectMask.ptr<unsigned char>(row);
//			unsigned short * depthRow = ofxMatKinectDepth.ptr<unsigned short>(row);
//
//			for (int col = 0; col < ofxMatKinectUsers.cols; ++ col)
//			{
//				if(0 != maskRow[col] && 0 != userRow[col])
//				{
//					depthRow[col] = usDepthValue;
//					maskRow[col] = 0;
//				}
//			}
//		}
//
//		// Inpaint Z-index
//		if(0.999f > fInpaintResizeScale)
//		{
//			cv::Mat kinectDepth8;
//			cv::Mat kinectDepth16;
//			cv::Mat resizedDepth;
//			cv::Mat resizedMask;
//			cv::Mat resizedProcessed;
//			cv::Mat processedResizedBack;
//			cv::Mat blurred;
//
//			double minVal, maxVal;
//
//			// Get the min and max distance in depth image
//			cv::minMaxLoc(ofxMatKinectDepth, &minVal, &maxVal, NULL, NULL);
//
//			// Inpaint Z-index values in a reduced buffer and copy them back only in the masked positions
//			ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0/maxVal);
//
//			//cv::Mat outResult;
//			//kinectDepth8.copyTo(outResult);
//			ofxKinectResult.setFromPixels(kinectDepth8.data, DepthWidth, DepthHeight);
//
//			int newSizeCols = ofxMatKinectDepth.cols * fInpaintResizeScale;
//			int newSizeRows = ofxMatKinectDepth.rows * fInpaintResizeScale;
//
//			// rescale
//			cv::resize(kinectDepth8, resizedDepth, cvSize(newSizeCols, newSizeRows), 0.0f, 0.0f, cv::INTER_NEAREST);
//			cv::resize(ofxMatKinectMask, resizedMask, cvSize(newSizeCols, newSizeRows), 0.0f, 0.0f, cv::INTER_NEAREST);
//
//			// Inpaint
//			cv::inpaint(resizedDepth, (resizedMask == 255), resizedProcessed, fInpaintRadius, cv::INPAINT_TELEA);
//
//			// recover
//			cv::resize(resizedProcessed, processedResizedBack, ofxMatKinectDepth.size());
//			
//			// recover
//			processedResizedBack.convertTo(kinectDepth16, CV_16UC1, maxVal / 255.0);
//			kinectDepth16.copyTo(ofxMatKinectDepth, (ofxMatKinectMask == 255));
//
//			// Apply median filter to the result
//			if(0 < nMedianFilterDepthSize)
//			{
//				// this is the main trick, apply median to mask, not to depth values
//				ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0 / maxVal);
//				cv::medianBlur(kinectDepth8, blurred, nMedianFilterDepthSize);
//				blurred.convertTo(ofxMatKinectProcessed, CV_16UC1, maxVal / 255.0);
//			}
//			else
//			{
//				ofxMatKinectDepth.copyTo(ofxMatKinectProcessed);
//			}
//
//			bProcessed = true;
//		}
//	}
//	else if (0 < nMedianFilterDepthSize)
//	{
//		// only median filter for the depth, main trick
//		double maxVal, minVal;
//		cv::minMaxLoc(ofxMatKinectDepth, &minVal, &maxVal, NULL, NULL);
//
//		cv::Mat blurred;
//		cv::Mat kinectDepth8;
//		
//		ofxMatKinectDepth.convertTo(kinectDepth8, CV_8UC1, 255.0 / maxVal);
//		cv::medianBlur(kinectDepth8, blurred, nMedianFilterDepthSize);
//		blurred.convertTo(ofxMatKinectProcessed, CV_16UC1, maxVal / 255.0);
//
//		bProcessed = true;
//	}
//
//	if(bProcessed)
//	{
//		// Remove depths out of range and create alpha mask(0 or 255)
//		float fInvRange255 = 255.0f / (gDepthRangeEndMeters - gDepthRangeStartMeters);
//		float fToMeters = 1.0f / 1000.0f;
//
//		for (int row = 0; row < DepthHeight; ++ row)
//		{
//			unsigned short * pDepthRows = ofxMatKinectProcessed.ptr<unsigned short>(row);
//			unsigned char  * pAlphaRows = ofxMatKinectAlpha.ptr<unsigned char>(row);
//
//			for (int col = 0; col < DepthWidth; ++ col)
//			{
//				float fDepth = pDepthRows[col] * fToMeters;
//				int nDepth = 0;
//
//				if (fDepth >= gDepthRangeStartMeters || fDepth <= gDepthRangeEndMeters)
//				{
//					nDepth = (int)((fDepth - gDepthRangeStartMeters) * fInvRange255);
//
//					if(0 > nDepth || 255 < nDepth)
//					{
//						nDepth = 0;
//					}
//
//					pAlphaRows[col] = nDepth > 0 ? 255 : 0;
//				}
//			}
//		}
//	}
//
//	// Offset alpha mask
//	// this is the main other thing to get the the depth to align properly to color
//	if(0 != nAlphaOffsetX || 0 != nAlphaOffsetY)
//	{
//		cv::Point2f srcPoints[3], dstPoints[3];
//
//		srcPoints[0].x = 0;
//		srcPoints[0].y = 0;
//		srcPoints[1].x = DepthWidth;
//		srcPoints[1].y = 0;
//		srcPoints[2].x = 0;
//		srcPoints[2].y = DepthHeight;
//
//		float fAlphaScale = 1.0f;
//		
//		for (int i = 0; i < 3; ++ i)
//		{
//			dstPoints[i].x = (((srcPoints[i].x - (DepthWidth  / 2)) * fAlphaScale) + (DepthWidth  / 2)) + nAlphaOffsetX;
//			dstPoints[i].y = (((srcPoints[i].y - (DepthHeight / 2)) * fAlphaScale) + (DepthHeight / 2)) + nAlphaOffsetY;
//		}
//
//		cv::Mat affine;
//		cv::Mat warped;
//		affine = cv::getAffineTransform(srcPoints, dstPoints);
//		cv::warpAffine(ofxMatKinectAlpha, warped, affine, ofxMatKinectAlpha.size());
//		warped.copyTo(ofxMatKinectAlpha);
//		
//		bProcessed = true;
//	}
//
//	// Erode & Dilate
//	bool bErodeDilateResize = false;
//	cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
//
//	if(0 == nAlphaErodeDilateKernelType)
//	{
//		structuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
//	}
//	else if (1 == nAlphaErodeDilateKernelType)
//	{
//		structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
//	}
//	else if (2 == nAlphaErodeDilateKernelType)
//	{
//		structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
//	}
//
//	if((nAlphaErodeSteps > 0 || nAlphaDilateSteps > 0) && fAlphaErodeDilateScale != 1.0f)
//	{
//		cv::Mat erodeDilateScale;
//		cv::resize(ofxMatKinectAlpha, erodeDilateScale, cvSize(DepthWidth * fAlphaErodeDilateScale, DepthHeight * fAlphaErodeDilateScale),
//				   0.0f, 0.0f, cv::INTER_LINEAR);
//		erodeDilateScale.copyTo(ofxMatKinectAlpha);
//
//		bErodeDilateResize = true;
//	}
//
//	if(0 < nAlphaErodeSteps && 0 < nAlphaDilateSteps)
//	{
//		cv::Mat intermediate;
//		cv::erode(ofxMatKinectAlpha, intermediate, structuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
//		cv::dilate(intermediate, ofxMatKinectAlpha, structuringElement, cv::Point(-1, -1), nAlphaDilateSteps);
//
//		bProcessed = true;
//	}
//	else if (0 < nAlphaErodeSteps)
//	{
//		cv::Mat erode;
//		cv::dilate(ofxMatKinectAlpha, erode, structuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
//		erode.copyTo(ofxMatKinectAlpha);
//
//		bProcessed = true;
//	} 
//	else if(0 < nAlphaDilateSteps)
//	{
//		cv::Mat dilate;
//		cv::dilate(ofxMatKinectAlpha, dilate, structuringElement, cv::Point(-1, -1), nAlphaDilateSteps);
//		dilate.copyTo(ofxMatKinectAlpha);
//
//		bProcessed = true;
//	}
//
//	if(0 < nMedianFilterAlphaSize)
//	{
//		bool bAlphaMedianResized = false;
//		
//		if(1.0f != fMedianFilterAlphaScale)
//		{
//			cv::Mat scaled;
//			cv::resize(ofxMatKinectAlpha, scaled, cvSize(DepthWidth * fMedianFilterAlphaScale, DepthHeight * fMedianFilterAlphaScale),
//				       0.0f, 0.0f, cv::INTER_LINEAR);
//			scaled.copyTo(ofxMatKinectAlpha);
//
//			bAlphaMedianResized = true;
//		}
//
//		cv::Mat filtered;
//		cv::medianBlur(ofxMatKinectAlpha, filtered, nMedianFilterAlphaSize);
//
//		if(bAlphaMedianResized)
//		{
//			cv::resize(filtered, ofxMatKinectAlpha, cvSize(DepthWidth, DepthHeight), 0.0f, 0.0f, cv::INTER_LINEAR);
//		}
//		else
//		{
//			filtered.copyTo(ofxMatKinectAlpha);
//		}
//
//		bProcessed = true;
//	}
//
//	// Blur Alpha
//	if(0 < nAlphaBlurFilterSize)
//	{
//		cv::Mat blurred;
//		cv::GaussianBlur(ofxMatKinectAlpha, blurred, cvSize(nAlphaBlurFilterSize, nAlphaBlurFilterSize), 0.0f, 0.0f);
//		blurred.copyTo(ofxMatKinectAlpha);
//
//		bProcessed = true;
//	}
//
//
//	ofColor color;
//	color.set(0, 0, 0);
//
//	for(int hIndex = 0; hIndex < DepthHeight; hIndex ++)
//	{
//		unsigned char * alpha = ofxMatKinectAlpha.ptr<unsigned char>(hIndex);
//
//		for(int wIndex = 0; wIndex < DepthWidth; wIndex ++)
//		{
//			if((int)alpha[wIndex] == 0)
//			{
//				ColorPlayer.getPixelsRef().setColor(wIndex, hIndex, backgroundImage.getColor(wIndex, hIndex));
//			}
//		}
//
//		alpha = NULL;
//	}
//
//	//ofxCvKinectUsers.clear();
//	//ofxCvKinectRGB.clear();
//	//ofxCvKinectMask.clear();
//	//ofxCvKinectDepth.clear();
//}
//
//
//void testApp::GestureGenerator()
//{
//	const ofPoint * playerSkeleton = NULL;
//
//	for (int i = 0; i < 6; ++ i)
//	{
//		if((int)skeletonDatas[i][0].z > 0 && (int)skeletonDatas[i][0].z < 40000)
//		{
//			//std::cout<<i<<" ("<<(int)skeletonDatas[i][0].x<<", "<<(int)skeletonDatas[i][0].y<<", "<<(int)skeletonDatas[i][0].z<<")"<<std::endl;
//			//for (int j = 0; j < 20; ++ j)
//			//{
//			//	std::cout<<skeletonDatas[i][j]<<", ";
//			//}
//
//
//			//cout<<"HEAD -> "<<(int)skeletonDatas[i][NUI_SKELETON_POSITION_HEAD].x<<" "<<(int)skeletonDatas[i][NUI_SKELETON_POSITION_HEAD].y<<" ";
//			//
//			//cout<<"HIP CENTER -> "<<(int)skeletonDatas[i][NUI_SKELETON_POSITION_HIP_CENTER].x<<" "<<(int)skeletonDatas[i][NUI_SKELETON_POSITION_HIP_CENTER].y<<" ";
//
//			if(NULL == playerSkeleton)
//			{
//				playerSkeleton = skeletonDatas[i];
//			}
//			else
//			{
//				if(playerSkeleton[NUI_SKELETON_POSITION_SHOULDER_CENTER].z > skeletonDatas[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].z)
//				{
//					playerSkeleton = skeletonDatas[i];
//				}
//			}
//		}
//	}
//
//	if(NULL == playerSkeleton) return;
//	
//	int HeadX = (int)playerSkeleton[NUI_SKELETON_POSITION_HEAD].x;
//	int HeadY = (int)playerSkeleton[NUI_SKELETON_POSITION_HEAD].y;
//	int SpineX = (int)playerSkeleton[NUI_SKELETON_POSITION_SPINE].x;
//	int SpineY = (int)playerSkeleton[NUI_SKELETON_POSITION_SPINE].y;
//
//	int leftHandX = (int)playerSkeleton[NUI_SKELETON_POSITION_HAND_LEFT].x;
//	int leftHandY = (int)playerSkeleton[NUI_SKELETON_POSITION_HAND_LEFT].y;
//	int leftWristX = (int)playerSkeleton[NUI_SKELETON_POSITION_WRIST_LEFT].x;
//	int leftWristY = (int)playerSkeleton[NUI_SKELETON_POSITION_WRIST_LEFT].y;
//	int leftElbowX = (int)playerSkeleton[NUI_SKELETON_POSITION_ELBOW_LEFT].x;
//	int leftElbowY = (int)playerSkeleton[NUI_SKELETON_POSITION_ELBOW_LEFT].y;
//	int leftShoulderX = (int)playerSkeleton[NUI_SKELETON_POSITION_SHOULDER_LEFT].x;
//	int leftShoulderY = (int)playerSkeleton[NUI_SKELETON_POSITION_SHOULDER_LEFT].y;
//
//	if(0 == leftSwipeTime && 0 < (leftWristX - leftHandX))
//	{
//		leftSwipeTime = ofGetSystemTime();
//	}
//
//	if((ofGetSystemTime() - leftSwipeTime) < 1500)
//	{
//
//		if(0 < (leftHandX-leftWristX) && 0 < (leftWristX-leftShoulderX) && 0 < (leftElbowY - HeadY) && 0 < (SpineY - leftElbowY))
//		{
//			std::cout<<"H-W "<<leftHandX-leftWristX<<"  W-S "<<leftWristX-leftShoulderX<<std::endl;
//			std::cout<<"Left Swipe ....\n";
//			leftSwipeTime = 0;
//		}
//	}
//	else
//	{
//		leftSwipeTime = 0;
//	}
//
//	int rightHandX = (int)playerSkeleton[NUI_SKELETON_POSITION_HAND_RIGHT].x;
//	int rightHandY = (int)playerSkeleton[NUI_SKELETON_POSITION_HAND_RIGHT].y;
//	int rightWristX = (int)playerSkeleton[NUI_SKELETON_POSITION_WRIST_RIGHT].x;
//	int rightWristY = (int)playerSkeleton[NUI_SKELETON_POSITION_WRIST_RIGHT].y;
//	int rightElbowX = (int)playerSkeleton[NUI_SKELETON_POSITION_ELBOW_RIGHT].x;
//	int rightElbowY = (int)playerSkeleton[NUI_SKELETON_POSITION_ELBOW_RIGHT].y;
//	int rightShoulderX = (int)playerSkeleton[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x;
//	int rightShoulderY = (int)playerSkeleton[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y;
//
//	if(0 == rightSwipeTime && 0 < (rightHandX - rightWristX))
//	{
//		rightSwipeTime = ofGetSystemTime();
//	}
//
//	if((ofGetSystemTime() - rightSwipeTime) < 1500)
//	{
//
//		if(0 < (rightWristX - rightHandX) && 0 < (rightShoulderX - rightWristX) &&  0 < (rightElbowY - HeadY) && 0 < (SpineY - rightElbowY))
//		{
//			std::cout<<"H-W "<<rightWristX - rightHandX<<"  W-S "<<rightShoulderX - rightWristX<<std::endl;
//			std::cout<<"Right Swipe ....\n";
//			rightSwipeTime = 0;
//		}
//	}
//	else
//	{
//		rightSwipeTime = 0;
//	}
//
//
//	if(0 == rightRaiseTime && 0 < (rightHandY - rightWristY))
//	{
//		rightRaiseTime = ofGetSystemTime();
//	}
//
//	if((ofGetSystemTime() - rightRaiseTime) < 1500)
//	{
//
//		if(0 < (rightWristY - rightHandY) && 0 < (rightShoulderY - rightWristY) && 0 < (rightHandX - HeadX))
//		{
//			std::cout<<"H-W "<<rightWristY - rightHandY<<"  W-S "<<rightShoulderY - rightWristY<<std::endl;
//			std::cout<<"Right Raise ....\n";
//			rightRaiseTime = 0;
//		}
//	}
//	else
//	{
//		rightRaiseTime = 0;
//	}
//
//
//	if(0 == leftRaiseTime && 0 < (leftHandY - leftWristY))
//	{
//		leftRaiseTime = ofGetSystemTime();
//	}
//
//	if((ofGetSystemTime() - leftRaiseTime) < 1500)
//	{
//
//		if(0 < (leftWristY - leftHandY) && 0 < (leftShoulderY - leftWristY) && 0 < (HeadX - leftHandX))
//		{
//			std::cout<<"H-W "<<leftWristY - leftHandY<<"  W-S "<<leftShoulderY - leftWristY<<std::endl;
//			std::cout<<"Left Raise ....\n";
//			leftRaiseTime = 0;
//		}
//	}
//	else
//	{
//		leftRaiseTime = 0;
//	}
//}