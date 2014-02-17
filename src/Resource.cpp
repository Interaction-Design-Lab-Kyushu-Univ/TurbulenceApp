//
//  Resources.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//
#include <boost/property_tree/xml_parser.hpp>

#include "Resource.h"

shared_ptr<Resource> Resource::instance;

shared_ptr<Resource> Resource::getInstance() {
	if (!instance) {
		instance.reset(new Resource());
	}
	return instance;
}

Resource::Resource() {
	boost::property_tree::read_xml(ofToDataPath("settings.xml"), settings);
}

Resource::~Resource() {
}

void Resource::loadFolder(const char * folderName) {
	ofDirectory dir;
	dir.open(ofToDataPath(folderName, true));
	dir.listDir();
	
	for (auto& file : dir.getFiles()) {
		if (file.getExtension() == "png") {
			images.insert(unordered_map<std::string, ofImage>::value_type(file.getFileName(), ofImage()))
				.first->second.loadImage(file.path());
		} else if (file.getExtension() == "wav") {
			sounds.insert(unordered_map<std::string, ofSoundPlayer>::value_type(file.getFileName(), ofSoundPlayer()))
			.first->second.loadSound(file.path());
		}
	}
	
	dir.close();
}

ofImage& Resource::getImage(const std::string& fileName) {
	return images[fileName];
}

ofSoundPlayer& Resource::getSound(const std::string& fileName) {
	return sounds[fileName];
}

boost::property_tree::ptree& Resource::getSettings() {
	return settings;
}

void Resource::saveSettings() {
	boost::property_tree::xml_parser::write_xml(ofToDataPath("settings.xml"), settings);
}