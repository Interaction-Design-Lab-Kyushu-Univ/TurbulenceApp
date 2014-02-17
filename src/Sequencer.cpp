//
//  Sequencer.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/08/03.
//
//

#include <boost/shared_ptr.hpp>

#include "Sequencer.h"
#include "Resource.h"

using namespace boost;

Sequencer::Sequencer() {
	_fluidSettings = new_fluid_settings();
	fluid_settings_setnum(_fluidSettings, "synth.gain", 2.0);
	fluid_settings_setstr(_fluidSettings, "audio.driver", "coreaudio");
	_synth = new_fluid_synth(_fluidSettings);
	_audioDriver = new_fluid_audio_driver(_fluidSettings, _synth);
	
	_killFlag = false;
	
	_thread = boost::thread(boost::bind(&Sequencer::threadFunc, this));
	
	initWithSettings();
}

void Sequencer::initWithSettings() {
	auto& settings = Resource::getInstance()->getSettings().get_child("settings.sequencer");
	
	ofFile file(settings.get("synth.soundfont", ""));
	loadSoundFont(file.getAbsolutePath().c_str(), true);
	fluid_settings_setnum(_fluidSettings, "synth.gain", settings.get("synth.volume", 1.0));
	
	auto& synthProps = settings.get_child("synth");
	for (auto& prop : synthProps) {
		if (prop.first == "progchan") {
			programChange(prop.second.get<int>("<xmlattr>.chan"), prop.second.get<int>("<xmlattr>.prog"));
		}
	}
	
	bgmLength = settings.get("bgm.length", 1.0f);
	setBgm(settings.get("bgm.file", "").c_str());
	setTempo(settings.get("bgm.tempo", 120.0f), settings.get("bgm.tempooffset", 0.0f));
	bgm.setVolume(settings.get("bgm.volume", 1.0f));
}

Sequencer::~Sequencer() {
	{
		mutex::scoped_lock lock(_mutex);
		_killFlag = true;
	}
	
	if (_thread.joinable()) {
		_thread.join();
	}
	
	delete_fluid_audio_driver(_audioDriver);
	delete_fluid_synth(_synth);
	delete_fluid_settings(_fluidSettings);
}

void Sequencer::threadFunc() {
	float prev_pos = 0.0f;
	
	while (true) {
		{
			mutex::scoped_lock(_mutex);
			if (_killFlag) return;
			
			if (bgm.getIsPlaying()) {
				float pos = bgm.getPosition() * bgmLength;
				
				// loop considered position
				float lppos = (pos < prev_pos)? pos + bgmLength : pos;
				
				if (lppos > prev_pos) {
					if (static_cast<int>((lppos - _initialOffset) / _interval) != static_cast<int>((prev_pos - _initialOffset) / _interval)) {
						onBeat();
					}
				}
				prev_pos = pos;
			}
		}
		usleep(1000);
	}
}

// this function has to be called in threadFunc()
void Sequencer::onBeat() {
	// insert a random rest
	if (ofRandom(1) < 0.1f) return;
	
	// perform scheduled action
	for (auto itr = _queue.begin(); itr != _queue.end(); ) {
		scheduled_action_t& action = const_cast<scheduled_action_t&>(*itr);
		action.reserved--;
		if (action.reserved <= 0) {
			if (!action.func.empty()) action.func();
			itr = _queue.erase(itr);
		} else {
			++itr;
		}
	}
}

void Sequencer::loadSoundFont(const char* filename, bool reset) {
	fluid_synth_sfload(_synth, filename, reset);
}

void Sequencer::programChange(int channel, int program) {
	fluid_synth_program_change(_synth, channel, program);
}

void Sequencer::noteOn(int channel, int key, int velocity) {
	fluid_synth_noteon(_synth, channel, key, velocity);
}

void Sequencer::noteOff(int channel, int key) {
	fluid_synth_noteoff(_synth, channel, key);
}

void Sequencer::scheduleNote(int channel, int key, int velocity, int count, int length) {
    struct {
        Sequencer *self;
        int channel, key, velocity, length;
        void operator()() {
            self->noteOn(channel, key, velocity);
        }
    } noteOnFunc;
    noteOnFunc.self = this;
    noteOnFunc.channel = channel;
    noteOnFunc.key = key;
    noteOnFunc.velocity = velocity;
    noteOnFunc.length = length;
	scheduleOnBeatUnsafe(noteOnFunc, count);

    struct {
        Sequencer *self;
        int channel, key;
        void operator()() {
            self->noteOff(channel, key);
        }
    } noteOffFunc;
    noteOffFunc.self = this;
    noteOffFunc.channel = channel;
    noteOffFunc.key = key;
	scheduleOnBeatUnsafe(noteOffFunc, count + length);
}

void Sequencer::scheduleOnBeatUnsafe(function<void(void)> func, int count) {
	mutex::scoped_lock lock(_mutex);
	scheduled_action_t action = {
		func, count
	};
	_queue.push_back(action);
}

void Sequencer::scheduleOnBeat(function<void(void)> func, int count) {
	scheduleOnBeatUnsafe(func, count);
}

void Sequencer::setBgm(const char *filename) {
	bgm.loadSound(filename);
	bgm.setLoop(true);
	bgm.play();
}

void Sequencer::setTempo(float tempo, float initialOffset) {
	mutex::scoped_lock lock(_mutex);
	_interval = 60.0f / tempo;
	_initialOffset = initialOffset;
}