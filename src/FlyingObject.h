//
//  FlyingObject.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/03.
//
//

#ifndef __Turbulence__FlyingObject__
#define __Turbulence__FlyingObject__

#include "SceneNode.h"

class ofxBox2d;
class ofxBox2dBaseShape;

class ofxOscMessage;

/*
 Abstract Class
 */
class FlyingObject : public SceneNode {
public:
	typedef int E_KIND;
	
protected:
	E_KIND kind;
	FlyingObject(E_KIND kind) : kind(kind) {};
	
public:
	virtual ~FlyingObject() {};
	virtual void destroy(ofxBox2d& box2d) {};
};

#endif /* defined(__Turbulence__FlyingObject__) */
