#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    original.loadImage("original.JPG");
    
    processed.allocate(original.getWidth(), original.getHeight(), OF_IMAGE_COLOR);
    
    for (int h = 0; h < original.getHeight(); ++ h)
    {
        for (int w = 0; w < original.getWidth(); ++ w)
        {
            //ofColor color = original.getColor(w, h);
            
            //color.r = 255 - color.r;
            //color.g = 255 - color.g;
            //color.b = 255 - color.b;
            
            processed.setColor(w, h, original.getColor(w, h));
        }
    }
}

//--------------------------------------------------------------
void testApp::update()
{
    //ofSetBackground();
}

//--------------------------------------------------------------
void testApp::draw()
{
    original.draw(0, 0, 800, 600);
    
    processed.draw(680, 0, 800, 600);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}