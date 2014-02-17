//
//  HitEffect.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/09/19.
//
//

#ifndef __Turbulence__HitEffect__
#define __Turbulence__HitEffect__

#include <iostream>

#include <ofMain.h>

#include "SceneNode.h"

class HitEffect : public SceneNode {
	int count = 0;
	ofVec2f position;
	float rotation;
	
public:
	HitEffect(const ofVec2f& position);
	
	virtual bool update();
	virtual void draw();
};

#endif /* defined(__Turbulence__HitEffect__) */
