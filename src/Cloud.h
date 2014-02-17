//
//  Cloud.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/09.
//
//

#ifndef __Turbulence__Cloud__
#define __Turbulence__Cloud__

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <ofMain.h>
#include "SceneNode.h"

class ofxOscMessage;

class Cloud : public SceneNode {
	ofVec2f position;
	float velocity;
	float scale;
	
	boost::optional<ofImage&> image;
	
public:
	Cloud(const boost::property_tree::ptree& ptree);
	
	bool update();
	void draw();
};

#endif /* defined(__Turbulence__Cloud__) */
