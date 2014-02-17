//
//  Node.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/09/19.
//
//

#ifndef Turbulence_Node_h
#define Turbulence_Node_h

class SceneNode {
public:
	virtual ~SceneNode() {};
	virtual bool update() {};
	virtual void draw() {};
};

#endif
