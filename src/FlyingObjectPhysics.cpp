//
//  FlyingObjectPhysics.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#include "FlyingObjectPhysics.h"

#include "constants.h"

#include "ofxBox2d.h"
#include "ofxBox2dBaseShape.h"

#include <ofMain.h>
#include "Resource.h"

using namespace std;

FlyingObjectPhysics::FlyingObjectPhysics(E_KIND kind)
: FlyingObject(kind)
{
	
}

void FlyingObjectPhysics::setup(ofxBox2d& box2d, Sequencer& sequencer, int x, int y, const boost::property_tree::ptree& ptree, float lifetime) {
	ofxBox2dCircle* circle = new ofxBox2dCircle();
	circle->setPhysics(ptree.get<float>("density", 1.0f), ptree.get<float>("bounce", 1.0f), ptree.get<float>("friction", 1.0f));
	auto radius = ptree.get<float>("radius", 60.0f);
	circle->setup(box2d.getWorld(), x, y - radius, radius);
	this->radius = radius;
	
	circle->setDamping(ptree.get<float>("damping", 60.0f));
	
	auto imageName = ptree.get<string>("image", "");
	
	circle->setVelocity(ofRandom(-2.0f, 2.0f), 10.0f + ofRandom(-2.0f, 2.0f));
	
	
	if (imageName != "") {
		image = Resource::getInstance()->getImage(imageName);
		centerVector.x = ptree.get<float>("centerx", image->getWidth() * 0.5f);
		centerVector.y = ptree.get<float>("centery", image->getHeight() * 0.5f);
	}
	
	shape = circle;
	setupTime = ofGetElapsedTimeMillis();
	this->lifetime = lifetime;
	
	circle->setData(this);
	circle->body->SetBullet(true);
	
	midiChannel = ptree.get("channel", 1);
	midiKey = ptree.get("key", -1);
	
	this->sequencer = sequencer;
}

bool FlyingObjectPhysics::onCollide(const FlyingObjectPhysics* fbp) {
	if ((prevVelocity - shape->getVelocity()).length() > 10.0f ) {
		if (midiKey == -1) {
			int keys[] = {48, 50, 52, 55, 57, 60, 62, 64, 67, 69};
			int key = keys[ (int)ofRandom(sizeof(keys)/sizeof(int)) ];
			sequencer->scheduleNote(midiChannel, key, 100, 1, 1);
		} else {
			sequencer->scheduleNote(midiChannel, midiKey, 100, 1, 1);
		}
		return true;
	}
	return false;
}


void FlyingObjectPhysics::destroy(ofxBox2d& box2d) {
	shape->destroy();
}

bool FlyingObjectPhysics::update() {
	shape->update();
	
	prevVelocity = shape->getVelocity();
	
	if (shape->getPosition().x < 0) {
		auto rotation = shape->body->GetAngle();
		auto position = shape->body->GetPosition();
		
		position.x += VIRTUAL_WIDTH / OFX_BOX2D_SCALE;
		
		shape->body->SetTransform(position, rotation);
	}
	
	if (ofGetElapsedTimeMillis() - setupTime > lifetime * 1000.0f) {
		return true;
	} else {
		return false;
	}
}

float FlyingObjectPhysics::getAlpha() {
	const float FADE_TIME = 2.0f;
	float alpha;
	int elapsed = (ofGetElapsedTimeMillis() - setupTime) / 1000.0f;
	if (elapsed < lifetime - FADE_TIME) {
		alpha = 1.0f;
	} else {
		alpha = (lifetime - elapsed) / FADE_TIME;
	}
	return alpha;
}

void FlyingObjectPhysics::draw() {
	if (image) {
		ofPushMatrix();
		ofPushStyle();
		
		ofSetColor(255, 255, 255, getAlpha() * 255);
		
		ofTranslate(shape->getPosition());
		ofRotate(shape->getRotation());
		image->draw(- centerVector.x, - centerVector.y);
		
		ofPopStyle();
		ofPopMatrix();
	} else {
		shape->draw();
	}
}


ofVec2f FlyingObjectPhysics::getPosition() const {
	return shape->getPosition();
}

float FlyingObjectPhysics::getRadius() const {
	return radius;
}