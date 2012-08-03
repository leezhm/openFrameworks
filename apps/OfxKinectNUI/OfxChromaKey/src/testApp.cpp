#include "testApp.h"

#include "opencv2/opencv.hpp"

#include <iostream>

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);

	// Set Frame Rate
	ofSetFrameRate(60);

	// Init the kinect sensor
	ofxKinectNui::InitSetting setting;
	setting.grabAudio = false;
	setting.grabCalibratedVideo = true;
	setting.grabDepth = true;
	setting.grabLabel = false;		// we do not use the label image
	setting.grabLabelCv = false;	//
	setting.grabSkeleton = false;
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

			ofLog(OF_LOG_ERROR)<<"Unavailable NUI_IMAGE_RESOLUTION ... "<<endl;
			break;
		}
	}

	KinectSensor.init(setting);

	// Open the kinect sensor
	KinectSensor.open();

	// Get current angle of sensor
	Angle = KinectSensor.getCurrentAngle();

	// Allocate image memory
	ColorImage.allocate(ColorWidth, ColorHeight, OF_IMAGE_COLOR);

	// Allocate the threshold image
	thresholdImage.allocate(640, 480);

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
	// Do some testing
	//////////////////////////////////////////////////////////////////////////
	cv::Mat doMat(6, 8, CV_16UC1, cv::Scalar::all(8));
	cv::randu(doMat, cv::Scalar::all(2), cv::Scalar::all(10));
	std::cout<<"doMat "<<doMat<<std::endl;

	cv::Mat outMat;
	doMat.convertTo(outMat, CV_8UC1, 0.5);
	std::cout<<"\noutMat "<<outMat<<std::endl;

	int nSizeX = 8 * 0.75;
	int nSizeY = 6 * 0.75;
	std::cout<<"\nnSizeX = "<<nSizeX<<"  nSizeY = "<<nSizeY<<std::endl;

	cv::Mat rsOutMat;

	cv::resize(outMat, rsOutMat, cvSize(nSizeX, nSizeY), 0.0f, 0.0f, cv::INTER_NEAREST);
	std::cout<<"\nrsOutMat "<<rsOutMat<<std::endl;

	cv::Mat equalMat = (rsOutMat == 4);
	std::cout<<"\nequalMat "<<equalMat<<std::endl;

	cv::Mat rsProcessedMat;
	cv::Mat rsMask(nSizeY, nSizeX, CV_8UC1, cv::Scalar::all(250));
	cv::randu(rsMask, cv::Scalar::all(1), cv::Scalar::all(255));
	cv::inpaint(rsOutMat, rsMask, rsProcessedMat, 3, cv::INPAINT_TELEA);
	std::cout<<"\nrsMask "<<rsMask<<std::endl;
	std::cout<<"\nrsProcessedMat "<<rsProcessedMat<<std::endl;
}

//--------------------------------------------------------------
void testApp::update()
{
	// update the kinect sensor
	KinectSensor.update();

	if(KinectSensor.isOpened())
	{
		ColorImage.setFromPixels(KinectSensor.getVideoPixels());

		// threshold Image
		thresholdImage.setFromPixels(KinectSensor.getDepthPixels());
		//thresholdImage.threshold(threshold);

		//
		//	Process
		//
		//thresholdImage.blurGaussian(3);
		//thresholdImage.erode_3x3();
		//thresholdImage.dilate_3x3();

		//thresholdImage.threshold()
		//LabelImage.adaptiveThreshold(5, 0, false, true);

		ColorPlayer.setFromPixels(KinectSensor.getCalibratedVideoPixels());

		ofColor color;
		color.set(0, 0, 0);
		
		for(int hIndex = 0; hIndex < thresholdImage.getHeight(); hIndex ++)
		{
			for(int wIndex = 0; wIndex < thresholdImage.getWidth(); wIndex ++)
			{
				ofColor color = thresholdImage.getPixelsRef().getColor(wIndex, hIndex);

				if(color.r == 0 && color.g == 0 && color.b == 0)
				{
					ColorPlayer.getPixelsRef().setColor(wIndex, hIndex, backgroundImage.getColor(wIndex, hIndex));
				}
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	// Set background
	ofBackground(100, 100, 100);

	ofEnableAlphaBlending();
	ColorImage.draw(0, 10, 640, 480);

	//ColorPlayer.flagImageChanged();
	ColorPlayer.draw(1280, 10, 640, 480);

	ofDisableAlphaBlending();

	// threshold Image
	thresholdImage.draw(640, 10, 640, 480);

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << "(press: < >), fps: " << ofGetFrameRate() << endl
		<< "press 'c' to close the stream and 'o' to open it again, stream is: " << KinectSensor.isOpened() << endl
		<< "press UP and DOWN to change the tilt angle: " << Angle << " degrees" << endl
		<< "press UP and DOWN to change the threshold: " << threshold << " degrees" << endl
		<< "press PgUp and PgDn to change the far clipping distance: " << farClipping << " mm" << endl
		<< "press Home and End to change the near clipping distance: " << nearClipping << " mm" << endl
		<< "press 's' or 'S' to save the near/ far clipping distance to xml file." << endl;
	ofDrawBitmapString(reportStream.str(), 10, 520);
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
			thresholdImage.threshold(threshold);

			break;
		}
	case OF_KEY_RIGHT:
		{
			threshold ++;
			thresholdImage.threshold(threshold);

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
	double fInpaintRadius = 3;
	int nInpaintUsersDilateSteps = 3; // three times

	if(0.0001f < fInpaintRadius)
	{
		// Dilate users buffer
		if(0 < nInpaintUsersDilateSteps)
		{
			// 对Player单通道图进行膨胀

		}
	}
}