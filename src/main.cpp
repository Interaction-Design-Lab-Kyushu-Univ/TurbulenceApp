#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"

int main( ){
    ofPtr<ofAppGLFWWindow> window(new ofAppGLFWWindow());
    window->setMultiDisplayFullscreen(true);
    ofSetupOpenGL(window, 1280*3, 800, OF_FULLSCREEN);
    ofSetWindowShape(ofGetScreenWidth()*3, ofGetScreenHeight());
    ofRunApp(new testApp());
}