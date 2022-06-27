#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "Clock.h"
#include "TripdeckMedia.h"

#define RUN_TIME_MILLIS 330000000
#define VIDEO_PLAY_INTERVAL 10000
#define VLC_DELAY 600

TripdeckMedia::TripdeckMedia(DataManager* dataManager, Tripdeck* behavior, MediaPlayer* videoPlayer, MediaPlayer* ledPlayer) :
	_dataManager(dataManager), _behavior(behavior), _videoPlayer(videoPlayer), _ledPlayer(ledPlayer) {
		if (!_dataManager || !_behavior)
			throw std::runtime_error("Error: Neither TripdeckMedia::_dataManager nor TripdeckMedia::_behavior can be NULL");

		_runnableObjects.push_back(_dataManager);
		_runnableObjects.push_back(_behavior);

		if (_ledPlayer)
			_runnableObjects.push_back(_ledPlayer);
		if (_videoPlayer)
			_runnableObjects.push_back(_videoPlayer);
	}

TripdeckMedia::~TripdeckMedia() { }

void TripdeckMedia::init() {
	// initialize all runnable objects
	for (Runnable* runnable : _runnableObjects) {
		runnable->init();
	}
	
	// add state changed listener delegate to TripdeckBehavior
	_behavior->setStateChangedDelegate(new TripdeckMedia::TripdeckStateChangedDelegate(this));

	// hook up media players to data manager
	if (_ledPlayer)
		_dataManager->addMediaListener(_ledPlayer);
	if (_videoPlayer)
		_dataManager->addMediaListener(_videoPlayer);
	
	_run = true;
	_onStateChanged();
}

void TripdeckMedia::run() {
	while (_run) {
		// check inputs
		for (Runnable* runnable : _runnableObjects) {
			runnable->run();
		}
	}
}

void TripdeckMedia::addVideoFolder(Tripdeck::TripdeckState state, const char* folder) {
	_videoPlayer->addMediaFolder(folder);
	_stateToVideoFolder[state] = folder;
}

void TripdeckMedia::addLedFolder(Tripdeck::TripdeckState state, const char* folder) {
	_ledPlayer->addMediaFolder(folder);
	_stateToLedFolder[state] = folder;
}

void TripdeckMedia::_stateChanged(Tripdeck::TripdeckStateChangedArgs* args) {
	std::cout << "State changed args received" << std::endl;
	std::cout << "New state == " << args->newState << std::endl;
	std::cout << "Synchronize video: " << (args->syncVideo ? "True" : "False") << std::endl;
	std::cout << "Synchronize led: " << (args->syncLeds ? "True" : "False") << std::endl;
	std::cout << "Video hash: " << args->videoId << std::endl;
	std::cout << "Led hash: " << args->ledId << std::endl;
}

void TripdeckMedia::_onStateChanged() {
	srand((uint32_t)Clock::instance().millis());
	
	if (_videoPlayer) {
		// set random video file from folder
		const auto& videoFiles = _dataManager->getFileIdsFromFolder(_stateToVideoFolder[_behavior->getState()]);
		uint32_t randomVideoFile = videoFiles[rand() % videoFiles.size()];
		_videoPlayer->setCurrentMedia(randomVideoFile, MediaPlayer::MediaPlaybackOption::Loop);
		_videoPlayer->play();
	}

	if (_ledPlayer) {
		// set random led file from folder
		const auto& ledFiles = _dataManager->getFileIdsFromFolder(_stateToLedFolder[_behavior->getState()]);
		uint32_t randomLedFile = ledFiles[rand() % ledFiles.size()];
		_ledPlayer->setCurrentMedia(randomLedFile, MediaPlayer::MediaPlaybackOption::Loop);
		_ledPlayer->play();
	}
}

TripdeckMedia::TripdeckStateChangedDelegate::TripdeckStateChangedDelegate(TripdeckMedia* owner) {
	_owner = owner;
}

TripdeckMedia::TripdeckStateChangedDelegate::~TripdeckStateChangedDelegate() { }

void TripdeckMedia::TripdeckStateChangedDelegate::execute(CommandArgs args) {
	_owner->_stateChanged((Tripdeck::TripdeckStateChangedArgs*)args);
}