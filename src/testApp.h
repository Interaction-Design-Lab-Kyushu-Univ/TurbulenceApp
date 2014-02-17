#pragma once

#include "ofMain.h"
#include "ofxDome.h"

#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include "Sequencer.h"

#include <vector>
#include <list>
#include <ofMath.h>
#include <ofxBox2d.h>
#include "FlyingObject.h"
#include "ofxLibwebsockets.h"

class Cloud;
class HitEffect;

class testApp : public ofBaseApp{
    ofxDome::ofxDome dome;
    ofxLibwebsockets::Client websock;
    
	boost::mutex mutex;
	
	ofxBox2d box2d;
	std::vector<Cloud*> clouds;
	std::list<FlyingObject*> fobjects;
	std::list<HitEffect*> effects;
	
	Sequencer sequencer;
    
    int maxItemKind;
    
    void setupDome();
    void setupWebsock();
    void setupScene();
    
    void updateScene();
    float compassToX(float);
    
	public:
		void setup();
		void update();
		void draw();
        void exit();

        FlyingObject* createFlyingObject(FlyingObject::E_KIND kind, float x);
        void add(FlyingObject* obj);
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void onConnect(ofxLibwebsockets::Event&);
        void onOpen(ofxLibwebsockets::Event&);
        void onClose(ofxLibwebsockets::Event&);
        void onIdle(ofxLibwebsockets::Event&);
        void onMessage(ofxLibwebsockets::Event&);
        void onBroadcast(ofxLibwebsockets::Event&);
};
