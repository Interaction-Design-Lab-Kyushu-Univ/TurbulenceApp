//
//  HitEffect.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/09/19.
//
//

#include <boost/lexical_cast.hpp>
#include "HitEffect.h"
#include "Resource.h"

HitEffect::HitEffect(const ofVec2f& vec)
: position(vec)
{
	rotation = ofRandom(0, 360.0f);
}

bool HitEffect::update() {
	count++;
	return count >= 8;
}

void HitEffect::draw() {
	ofImage& image = Resource::getInstance()->getImage("hiteffect_" + boost::lexical_cast<std::string>(count) + ".png");
	
	ofPushMatrix();
	ofTranslate(position);
	ofRotate(rotation);
	image.draw(- image.width * 0.5f, - image.height * 0.5f);
	ofPopMatrix();
}
