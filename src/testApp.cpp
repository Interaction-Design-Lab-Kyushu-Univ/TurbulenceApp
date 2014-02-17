#include <boost/lexical_cast.hpp>
#include "testApp.h"
#include "constants.h"

#include "FlyingObjectPhysics.h"
#include "constants.h"
#include "Resource.h"
#include "Cloud.h"
#include "HitEffect.h"
#include "json.h"

using mutex = boost::mutex;

void testApp::setupDome() {
    dome.setScreenDivision(3, 1);
    dome.setCoordinateSystemPanorama(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, M_PI_4);
    dome.setup();
	
	dome.loadMeshCompositionString(ofBufferFromFile("mesh.txt").getText());
}

void testApp::setupWebsock() {
	auto& settings = Resource::getInstance()->getSettings();
	
    ofxLibwebsockets::ClientOptions options = ofxLibwebsockets::defaultClientOptions();
    options.host = settings.get<string>("settings.websocket.host");
    options.channel = settings.get<string>("settings.websocket.channel");
    
    websock.addListener(this);
    websock.connect(options);
}

void testApp::setupScene() {
	const float GROUND_HEIGHT = 117.0f;
	const float SKY_HEIGHT = 300.0f;
	
	box2d.init();
	box2d.setGravity(- 5.0f,1.0f);
	box2d.createGround(0.0f, VIRTUAL_HEIGHT - GROUND_HEIGHT, VIRTUAL_WIDTH, VIRTUAL_HEIGHT - GROUND_HEIGHT);
	box2d.createGround(0.0f, - SKY_HEIGHT, VIRTUAL_WIDTH, - SKY_HEIGHT);
	box2d.setFPS(60);
	
	Resource::getInstance()->loadFolder("image");
	Resource::getInstance()->loadFolder("bgimage");
	Resource::getInstance()->loadFolder("sound");
	
	auto& cloudProps = Resource::getInstance()->getSettings().get_child("settings.clouds");
	for (auto& prop : cloudProps) {
		clouds.push_back(new Cloud(prop.second));
	}
}


//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    
    maxItemKind = Resource::getInstance()->getSettings().get_child("settings.flyingobjects").size();
    
    setupDome();
    setupScene();
    setupWebsock();
}

void testApp::updateScene() {
	mutex::scoped_lock lock(mutex);
	
	box2d.update();
	auto contact = box2d.getWorld()->GetContactList();
	
	while (contact) {
		auto a = static_cast<FlyingObjectPhysics*>(contact->GetFixtureA()->GetBody()->GetUserData());
		auto b = static_cast<FlyingObjectPhysics*>(contact->GetFixtureB()->GetBody()->GetUserData());
		
		if (a && b) {
			if (a->onCollide(b) || b->onCollide(a)) {
				auto position = (a->getPosition() * a->getRadius() + b->getPosition() * b->getRadius()) * (1.0f  / (a->getRadius() + b->getRadius()));
				HitEffect* hitEffect = new HitEffect(position);
				effects.push_back(hitEffect);
			}
		} else {
			if (a) a->onCollide(b);
			if (b) b->onCollide(a);
		}
        
		contact = contact->GetNext();
	}
	
	for (auto cloud : clouds) {
		cloud->update();
	}
	
	{
		for (auto itr = effects.begin(); itr != effects.end(); ) {
			if ((*itr)->update()) {
				delete *itr;
				itr = effects.erase(itr);
			} else {
				itr++;
			}
		}
	}
	
	{
		for (auto itr = fobjects.begin(); itr != fobjects.end(); ) {
			if ((*itr)->update()) {
				(*itr)->destroy(box2d);
				delete *itr;
				itr = fobjects.erase(itr);
			} else {
				itr++;
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::update(){
    updateScene();
}

//--------------------------------------------------------------
void testApp::draw(){
	mutex::scoped_lock lock(mutex);
	ofSetColor(255, 255, 255);
    
    dome.beginFbo();
    
	// draw background
    {
		ofPushMatrix();
		for (int j = 1, x = 0; j <= 6; j++) {
			ofImage& image = Resource::getInstance()->getImage(std::string("enkei_0") + (char)('0' + j) + ".png");
			image.draw(x, 0);
			x += image.getWidth();
		}
		ofPopMatrix();
	}
	
    // draw items
    for (int i = -1; i <= 1; i++){
		ofPushMatrix();
        ofTranslate(VIRTUAL_WIDTH * i, 0.0f);
		{
			for (auto cloud : clouds) {
				cloud->draw();
			}
			for (auto effect : effects) {
				effect->draw();
			}
			for (auto fobject : fobjects) {
				fobject->draw();
			}
		}
		ofPopMatrix();
	}

    dome.endFbo();
}

//--------------------------------------------------------------
void testApp::exit() {
	for (auto fobj : fobjects) {
		fobj->destroy(box2d);
		delete fobj;
	}
	for (auto cloud : clouds) {
		delete cloud;
	}
    
    websock.close();
}


float testApp::compassToX(float compass) {
	auto& settings = Resource::getInstance()->getSettings().get_child("settings.compass");
    
    float coef = (settings.get<std::string>("clockwise", "true") == "true")? 1.0f : -1.0f;
    
	auto x = settings.get<float>("standard") + coef * compass / 360.0f;
	while (x < 0.0f) x += 1.0f;
	while (x >= 1.0f) x -= 1.0f;
	return VIRTUAL_WIDTH * x;
}

//--------------------------------------------------------------
FlyingObject* testApp::createFlyingObject(FlyingObject::E_KIND kind, float x) {
	mutex::scoped_lock lock(mutex);
	
	switch (kind) {
		default: {
			FlyingObjectPhysics* fo = new FlyingObjectPhysics(kind);
			
			// find matching property from the settings
			auto& itemProps = Resource::getInstance()->getSettings().get_child("settings.flyingobjects");
			auto itr = itemProps.begin();
			std::advance(itr, kind);
			const auto& prop = itr->second;
			
			fo->setup(box2d, sequencer, x, 0, prop, 20.0f);
			
			return fo;
		}
	}
}

//--------------------------------------------------------------
void testApp::add(FlyingObject* obj) {
	mutex::scoped_lock lock(mutex);
	fobjects.push_back(obj);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
        case 'l':
            dome.setMeshVisiblity(!dome.getMeshVisiblity());
            break;
        case 'f':
            ofToggleFullscreen();
            break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    int rand = ceil(ofRandom(maxItemKind));
    FlyingObject* fobj = createFlyingObject((FlyingObject::E_KIND)rand, VIRTUAL_WIDTH * 0.5f);
    add(fobj);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	ofBuffer buffer(dome.getMeshCompositionString());
	ofBufferToFile("mesh.txt", buffer);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


void testApp::onConnect(ofxLibwebsockets::Event&) {
    
}

void testApp::onOpen(ofxLibwebsockets::Event&) {
    
}
void testApp::onClose(ofxLibwebsockets::Event&) {
    
}

void testApp::onIdle(ofxLibwebsockets::Event&) {
    
}

void testApp::onMessage(ofxLibwebsockets::Event& e) {
    Json::Value data = e.json;
    
    cout << data.toStyledString() << endl;
    
    auto compassValue = data.get("compass", Json::Value::null);
    auto kindValue = data.get("item", Json::Value::null);
    
    if (compassValue.isConvertibleTo(Json::realValue)
        && kindValue.isConvertibleTo(Json::intValue)) {
        float compass = compassValue.asFloat();
        int kind = kindValue.asInt();
        
        if (0 <= kind && kind <= maxItemKind) {
            add(createFlyingObject((FlyingObject::E_KIND)kind, compassToX(compass)));
        }
    } else if (compassValue.isString() && kindValue.isString()) {
        float compass = boost::lexical_cast<float>(compassValue.asString());
        int kind = boost::lexical_cast<int>(kindValue.asString());
        
        if (0 <= kind && kind <= maxItemKind) {
            add(createFlyingObject((FlyingObject::E_KIND)kind, compassToX(compass)));
        }
    } else {
        cout << "Invalid Json passsed" << endl;
    }
}

void testApp::onBroadcast(ofxLibwebsockets::Event&) {
    
}