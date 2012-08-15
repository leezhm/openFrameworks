#include "KinApp.h"
#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
int main()
{
	ofAppGlutWindow window; // create a window
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
	
	ofSetupOpenGL(&window, 1920, 1600, OF_WINDOW);
	ofRunApp(new KinApp()); // start the app
}
