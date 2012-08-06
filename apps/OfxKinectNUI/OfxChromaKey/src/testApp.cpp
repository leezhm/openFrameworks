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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static cv::Mat matKinectDepth(cv::Size(640, 480), CV_16UC1);
static cv::Mat matKinectMask (cv::Size(640, 480), CV_8UC1);
static cv::Mat matKinectAlpha(cv::Size(640, 480), CV_8UC1);
static cv::Mat matKinectRGB  (cv::Size(640, 480), CV_8UC3);
static cv::Mat matKinectUsers(cv::Size(640, 480), CV_8UC1);

static unsigned int * g_pucPackedKinectRGBDepthPixels;
static unsigned char * g_pucKinectAlphaPixels;

static unsigned int			g_nDepthWidth	= 0;
static unsigned int			g_nDepthHeight	= 0;

static unsigned int			g_fDepthRangeEndMeters = 1400;
static unsigned int			g_fDepthRangeStartMeters = 900;

static bool g_bDebugPixels = true;


bool testApp::Kinect_ProcessDepthAndAlphaPixels(float fInpaintRadius, float fInpaintResizeScale, int nInpaintUsersDilateSteps, 
												float fInpaintDepthValueNonUsers, int nMedianFilterDepthSize, int nAlphaErodeSteps, 
												int nAlphaDilateSteps, float fAlphaErodeDilateScale, int nAlphaErodeDilateKernelType, 
												int nAlphaErodeDilateKernelSize, int nAlphaOffsetX, int nAlphaOffsetY, int nMedianFilterAlphaSize, 
												float fMedianFilterAlphaScale, int nAlphaBlurFilterSize)
{
	if(((nMedianFilterDepthSize % 2) == 0) && (nMedianFilterDepthSize > 0))
	{
		nMedianFilterDepthSize--;
	}

	if(((nMedianFilterAlphaSize % 2) == 0) && (nMedianFilterAlphaSize > 0))
	{
		nMedianFilterAlphaSize--;
	}

	if(((nAlphaBlurFilterSize % 2) == 0) && (nAlphaBlurFilterSize > 0))
	{
		nAlphaBlurFilterSize--;
	}

	bool bProcessed = false;

	cv::Mat matProcessed;

	// Inpaint z and normal filter

	if(fInpaintRadius > 0.0001f) //double类型与0比较
	{
		// Dilate users buffer

		if(nInpaintUsersDilateSteps > 0)
		{
			// 对得到的User单通道图进行膨胀，默认3x3的内核，nInpaintUsersDilateSteps次操作
			cv::Mat matUsersDilated;
			cv::dilate(matKinectUsers, matUsersDilated, cv::Mat(), cv::Point(-1, -1), nInpaintUsersDilateSteps);
			matUsersDilated.copyTo(matKinectUsers);
		}

		// Inpaint manually unknown z pixels outside users buffer with a constant value and discard them from the opencv inpaint
		unsigned short usDepthValue = (unsigned short)(fInpaintDepthValueNonUsers * 1000.0f);

		for(int y = 0; y < matKinectUsers.rows; y++) // 循环整个User图
		{
			unsigned char*  pUsersRow = matKinectUsers.ptr <unsigned char> (y); // 获取一行的像素
			unsigned char*  pMaskRow  = matKinectMask.ptr  <unsigned char> (y); // 获取一行的像素
			unsigned short* pDepthRow = matKinectDepth.ptr <unsigned short>(y); // 获取一行的像素

			int nDest = y * matKinectUsers.cols * 4; // 乘以4是因为后面的g_pucPackedKinectRGBDepthPixels

			for(int x = 0; x < matKinectUsers.cols; x++)
			{
				int nDestPrecalc = nDest + (x << 2); // 乘以4

				// Inpaint mask activated?
				if(pMaskRow[x] != 0) 
				{
					if(pUsersRow[x] == 0) // 如果当前像素为0，说明不是User像素的一部。
					{
						// If it's not a user, inpaint with constant z
						pDepthRow[x] = usDepthValue; 	// 使用一个常量Z来填充matKinectDepth
						pMaskRow[x]  = 0;				// 使用0来填充matKinectMask

						if(g_bDebugPixels)
						{
							g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 0] = 0;
							g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 1] = 0;
							g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 2] = 255;
							g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 3] = 255;
						}
					}
					else if(g_bDebugPixels)
					{
						g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 0] = 0;
						g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 1] = 255;
						g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 2] = 0;
						g_pucPackedKinectRGBDepthPixels[nDestPrecalc + 3] = 255;
					}
				}
			}
		}

		// Inpaint z
		if(fInpaintResizeScale < 0.999f) // fInpaintResizeScale是么意思，么作用
		{
			cv::Mat matKinectDepth8;
			cv::Mat matKinectDepth16;
			cv::Mat matResizedDepth;
			cv::Mat matResizedMask;
			cv::Mat matResizedProcessed;
			cv::Mat matProcessedResizedBack;
			cv::Mat matBlurred;

			Point minLoc;
			double minval,maxval;
			minMaxLoc(matKinectDepth, &minval, &maxval, NULL, NULL);	// 找出填充后的matKinectDepth中的最大和最小值

			// Inpaint z values in a reduced buffer and copy them back only in the masked positions
			matKinectDepth.convertTo(matKinectDepth8, CV_8UC1, 255.0 / maxval); // 类型转换，并且每个item都需要乘以255.0 / maxval

			int nNewSizeX = matKinectDepth.cols * fInpaintResizeScale;
			int nNewSizeY = matKinectDepth.rows * fInpaintResizeScale;

			// 进行矩阵的缩放，明显fInpaintResizeScale < 1所以是缩小
			cv::resize(matKinectDepth8, matResizedDepth, cvSize(nNewSizeX, nNewSizeY), 0.0f, 0.0f, cv::INTER_NEAREST);
			cv::resize(matKinectMask,   matResizedMask,  cvSize(nNewSizeX, nNewSizeY), 0.0f, 0.0f, cv::INTER_NEAREST);

			// 对
			cv::inpaint(matResizedDepth, (matResizedMask == 255), matResizedProcessed, fInpaintRadius, cv::INPAINT_TELEA);

			// 再次缩放回去为原来的尺寸
			cv::resize(matResizedProcessed, matProcessedResizedBack, matKinectDepth.size());

			// 将缩小经过inpaint处理的depth图在上一步再次resize回去之后，进行类型转换，还原为cv_16uc1
			matProcessedResizedBack.convertTo(matKinectDepth16, CV_16UC1, maxval / 255.0);
			matKinectDepth16.copyTo(matKinectDepth, (matKinectMask == 255));

			// Apply median filter to the result?

			if(nMedianFilterDepthSize > 0)
			{
				// this is the main trick.  apply median to mask, not to depth values

				matKinectDepth.convertTo(matKinectDepth8, CV_8UC1, 255.0 / maxval);
				cv::medianBlur(matKinectDepth8, matBlurred, nMedianFilterDepthSize);
				matBlurred.convertTo(matProcessed, CV_16UC1, maxval / 255.0);
			}
			else
			{
				matKinectDepth.copyTo(matProcessed);
			}

			bProcessed = true;
		}
		else
		{
			cv::Mat matKinectDepth8;
			cv::Mat matProcessed8;

			// Inpaint z-values on original image using the provided mask
			Point minLoc;
			double minval,maxval;
			minMaxLoc(matKinectDepth, &minval, &maxval, NULL, NULL);	// 找出填充后的matKinectDepth中的最大和最小值
			matKinectDepth.convertTo(matKinectDepth8, CV_8UC1, 255.0 / maxval);	

			cv::inpaint(matKinectDepth8, (matKinectMask == 255), matProcessed8, fInpaintRadius, cv::INPAINT_TELEA);

			if(nMedianFilterDepthSize > 0)
			{
				cv::Mat matBlurred;
				cv::medianBlur(matProcessed8, matBlurred, nMedianFilterDepthSize);
				matBlurred.convertTo(matProcessed, CV_16UC1, maxval / 255.0);
			}
			else
			{
				matProcessed8.convertTo(matProcessed, CV_16UC1, maxval / 255.0);
			}

			bProcessed = true;
		}
	}
	else if(nMedianFilterDepthSize > 0)
	{
		// Only median filter the depth.  main trick!

		Point minLoc;
		double minval,maxval;
		minMaxLoc(matKinectDepth, &minval, &maxval, NULL, NULL);

		cv::Mat matBlurred;
		cv::Mat matKinectDepth8;
		matKinectDepth.convertTo(matKinectDepth8, CV_8UC1, 255.0 / maxval);
		cv::medianBlur(matKinectDepth8, matBlurred, nMedianFilterDepthSize);
		matBlurred.convertTo(matProcessed, CV_16UC1, maxval / 255.0);

		bProcessed = true;
	}

	if(g_bDebugPixels == false)
	{
		if(bProcessed == true)
		{
			// Remove depths out of range and create alpha mask (0 or 255)
			float fInvRange255 = 255.0f / (g_fDepthRangeEndMeters - g_fDepthRangeStartMeters);
			float fToMeters    = 1.0f / 1000.0f;

			for(int y = 0; y < g_nDepthHeight; y++)
			{
				unsigned short* pCVRowDepths = matProcessed.ptr<unsigned short>(y);
				unsigned char*  pCVRowAlpha  = matKinectAlpha.ptr<unsigned char>(y);

				for(int x = 0; x < g_nDepthWidth; x++)
				{
					float fDepth = pCVRowDepths[x] * fToMeters;
					int   nDepth = 0;

					if(fDepth >= g_fDepthRangeStartMeters || fDepth <= g_fDepthRangeEndMeters)
					{
						nDepth = (int)((fDepth - g_fDepthRangeStartMeters) * fInvRange255);
						if(nDepth < 0 || nDepth > 255)
						{
							nDepth = 0;
						}
					}
					pCVRowAlpha[x] = nDepth > 0 ? 255 : 0;
				}
			}
		}
		else if(nAlphaErodeSteps > 0 || nAlphaDilateSteps > 0 || nMedianFilterAlphaSize > 0 || nAlphaBlurFilterSize > 0 || nAlphaOffsetX != 0 || nAlphaOffsetY != 0)
		{
			// Just use our previously stored values

			for(int y = 0; y < g_nDepthHeight; y++)
			{
				unsigned char* pCVRowAlpha = matKinectAlpha.ptr<unsigned char>(y);
				unsigned char* pKinectRow  = g_pucKinectAlphaPixels + (y * g_nDepthWidth * 4);

				for(int x = 0; x < g_nDepthWidth; x++)
				{
					pCVRowAlpha[x] = pKinectRow[x * 4 + 3];
				}
			}
		}

		// Offset alpha mask
		// this is the main other thing to get the depth to align properly to color.

		if(nAlphaOffsetX != 0 || nAlphaOffsetY != 0)
		{
			cv::Point2f srcPoints[3], dstPoints[3];

			srcPoints[0].x = 0;
			srcPoints[0].y = 0;
			srcPoints[1].x = g_nDepthWidth;
			srcPoints[1].y = 0;
			srcPoints[2].x = 0;
			srcPoints[2].y = g_nDepthHeight;

			float fAlphaScale = 1.0f;

			for(int i = 0; i < 3; i++)
			{
				dstPoints[i].x = (((srcPoints[i].x - (g_nDepthWidth  / 2)) * fAlphaScale) + (g_nDepthWidth  / 2)) + nAlphaOffsetX;
				dstPoints[i].y = (((srcPoints[i].y - (g_nDepthHeight / 2)) * fAlphaScale) + (g_nDepthHeight / 2)) + nAlphaOffsetY;
			}

			cv::Mat matAffine;
			cv::Mat matWarped;

			matAffine = cv::getAffineTransform(srcPoints, dstPoints);
			cv::warpAffine(matKinectAlpha, matWarped, matAffine, matKinectAlpha.size());
			matWarped.copyTo(matKinectAlpha);
			bProcessed = true;
		}

		// Erode & Dilate

		bool bErodeDilateResized   = false;
		cv::Mat  matStructuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));

		if(nAlphaErodeDilateKernelType == 0)
		{
			matStructuringElement = cv::getStructuringElement(cv::MORPH_CROSS, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
		}
		else if(nAlphaErodeDilateKernelType == 1)
		{
			matStructuringElement = cv::getStructuringElement(cv::MORPH_RECT, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
		}
		else if(nAlphaErodeDilateKernelType == 2)
		{
			matStructuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cvSize(nAlphaErodeDilateKernelSize, nAlphaErodeDilateKernelSize));
		}

		if((nAlphaErodeSteps > 0 || nAlphaDilateSteps > 0) && IS_ONE(fAlphaErodeDilateScale) == false)
		{
			cv::Mat matErodeDilateScaled;
			cv::resize(matKinectAlpha, matErodeDilateScaled, cvSize(g_nDepthWidth * fAlphaErodeDilateScale, g_nDepthHeight * fAlphaErodeDilateScale), 0.0f, 0.0f, cv::INTER_LINEAR);
			matErodeDilateScaled.copyTo(matKinectAlpha);
			bErodeDilateResized = true;
		}

		if(nAlphaErodeSteps > 0 && nAlphaDilateSteps > 0)
		{
			cv::Mat matIntermediate;
			cv::erode (matKinectAlpha,  matIntermediate, matStructuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
			cv::dilate(matIntermediate, matKinectAlpha,  matStructuringElement, cv::Point(-1, -1), nAlphaDilateSteps);
			bProcessed = true;
		}
		else if(nAlphaErodeSteps > 0)
		{
			cv::Mat matEroded;
			cv::erode(matKinectAlpha, matEroded, matStructuringElement, cv::Point(-1, -1), nAlphaErodeSteps);
			matEroded.copyTo(matKinectAlpha);
			bProcessed = true;
		}
		else if(nAlphaDilateSteps > 0)
		{
			cv::Mat matDilated;
			cv::dilate(matKinectAlpha, matDilated, matStructuringElement, cv::Point(-1, -1), nAlphaDilateSteps);
			matDilated.copyTo(matKinectAlpha);
			bProcessed = true;
		}

		if(bErodeDilateResized)
		{
			cv::Mat matErodeDilateScaled;
			cv::resize(matKinectAlpha, matErodeDilateScaled, cvSize(g_nDepthWidth, g_nDepthHeight), 0.0f, 0.0f, cv::INTER_LINEAR);
			matErodeDilateScaled.copyTo(matKinectAlpha);
		}

		// Median filter alpha

		if(nMedianFilterAlphaSize > 0)
		{
			bool bAlphaMedianResized = false;

			if(IS_ONE(fMedianFilterAlphaScale) == false)
			{
				cv::Mat matScaled;
				cv::resize(matKinectAlpha, matScaled, cvSize(g_nDepthWidth * fMedianFilterAlphaScale, g_nDepthHeight * fMedianFilterAlphaScale), 0.0f, 0.0f, cv::INTER_LINEAR);
				matScaled.copyTo(matKinectAlpha);
				bAlphaMedianResized = true;
			}

			cv::Mat matFiltered;
			cv::medianBlur(matKinectAlpha, matFiltered, nMedianFilterAlphaSize);

			if(bAlphaMedianResized)
			{
				cv::resize(matFiltered, matKinectAlpha, cvSize(g_nDepthWidth, g_nDepthHeight), 0.0f, 0.0f, cv::INTER_LINEAR);
			}
			else
			{
				matFiltered.copyTo(matKinectAlpha);
			}

			bProcessed = true;
		}

		// Blur alpha

		if(nAlphaBlurFilterSize > 0)
		{
			cv::Mat matBlurred;
			cv::GaussianBlur(matKinectAlpha, matBlurred, cvSize(nAlphaBlurFilterSize, nAlphaBlurFilterSize), 0.0, 0.0);
			matBlurred.copyTo(matKinectAlpha);
			bProcessed = true;
		}

		// Store result

		if(bProcessed)
		{
			for(int y = 0; y < g_nDepthHeight; y++)
			{
				unsigned char* pCVRowAlpha = matKinectAlpha.ptr<unsigned char>(y);
				unsigned char* pKinectRow  = g_pucKinectAlphaPixels + (y * g_nDepthWidth * 4);

				for(int x = 0; x < g_nDepthWidth; x++)
				{
					pKinectRow[x * 4 + 3] = pCVRowAlpha[x];
				}
			}
		}
	}

	return true;
}
