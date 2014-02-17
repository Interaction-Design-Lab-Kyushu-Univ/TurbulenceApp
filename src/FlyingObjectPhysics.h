//
//  FlyingObjectPhysics.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#ifndef __Turbulence__FlyingObjectPhysics__
#define __Turbulence__FlyingObjectPhysics__

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include "FlyingObject.h"
#include <ofMain.h>
#include "Sequencer.h"

class FlyingObjectPhysics : public FlyingObject {
protected:
	ofxBox2dBaseShape* shape = NULL;
	float lifetime = 0.0f;
	unsigned long long setupTime;
	boost::optional<ofImage&> image;
	boost::optional<Sequencer&> sequencer;
	ofVec2f centerVector;
	ofVec2f prevVelocity;
	int midiChannel, midiKey;
	float radius;
	// bool firstTime = true;
	// std::string soundName;
	
	float getAlpha();
	
public:
	FlyingObjectPhysics(E_KIND kind);
	virtual ~FlyingObjectPhysics() {};
	
	virtual void setup(ofxBox2d& box2d, Sequencer& sequencer, int x, int y, const boost::property_tree::ptree& physicsSettings, float lifetime);
	virtual void destroy(ofxBox2d& box2d);
	virtual bool update();
	virtual void draw();
	
	ofVec2f getPosition() const;
	float getRadius() const;
	
	/*
	 	return true when the velocity has changed
	 */
	virtual bool onCollide(const FlyingObjectPhysics* fbp);
};

#endif /* defined(__Turbulence__FlyingObjectPhysics__) */
