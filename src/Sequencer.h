//
//  Sequencer.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/08/03.
//
//

#ifndef __Turbulence__Sequencer__
#define __Turbulence__Sequencer__

#include <fluidsynth.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include <ofMain.h>

class Sequencer {
	fluid_settings_t* _fluidSettings;
	fluid_synth_t *_synth;
	fluid_audio_driver_t *_audioDriver;
	
	boost::mutex _mutex;
	boost::thread _thread;
	
	class scheduled_action_t {
	public:
		boost::function<void(void)> func;
		int reserved;
		
		bool operator<(const scheduled_action_t& b) const {
			return reserved < b.reserved;
		}
		bool operator>(const scheduled_action_t& b) const {
			return reserved > b.reserved;
		}
		
	};
	
	std::list<scheduled_action_t> _queue;
	volatile bool _killFlag;
	volatile float _interval;
	volatile float _initialOffset;
	
	void threadFunc();
	void onBeat();
	
	void initWithSettings();
		
	ofSoundPlayer bgm;
	float bgmLength;
	
public:
	Sequencer();
	virtual ~Sequencer();
	void loadSoundFont(const char*, bool reset);
	
	void programChange(int channel, int program);
	void noteOn(int channel, int key, int velocity);
	void noteOff(int channel, int key);
	
	void scheduleNote(int channel, int key, int velocity, int count, int length);
	void scheduleOnBeatUnsafe(boost::function<void(void)>, int count);
	void scheduleOnBeat(boost::function<void(void)>, int count);
	
	void setBgm(const char* bgm);
	void setTempo(float tempo, float initialOffset = 0.0f);
};

#endif /* defined(__Turbulence__Sequencer__) */
