#include <iostream>
#include <algorithm>

#include "TripdeckLeader.h"
#include "Clock.h"

TripdeckLeader::TripdeckLeader(TripdeckMediaManager* mediaManager, InputManager* inputManager, Serial* serial) : Tripdeck(mediaManager, inputManager, serial) { }

TripdeckLeader::~TripdeckLeader() { }

void TripdeckLeader::init() {
	// sets state to Connecting and _run to true
	Tripdeck::init();
	// hook up button inputs with callback
}

void TripdeckLeader::run() {
	while (_run) {
		Tripdeck::run();

		switch (_currentState) {
			case Connecting:
				_runStartup();
				break;
			case Connected:
				break;
			case Wait:
				break;
			case Pulled:
				break;
			case Reveal:
				break;
			default:
				// do nothing
				break;
		}
	}
}

void TripdeckLeader::handleMediaChanged(TripdeckStateChangedArgs& args) {

}

void TripdeckLeader::_onStateChanged() {
	TripdeckStateChangedArgs args = { };
	args.newState = _currentState;

	switch (_currentState) {
		case Connecting:
			args.loop = true;
			break;
		case Connected:
			args.loop = true;
			break;
		case Wait:
			args.videoId = _mediaManager->getRandomVideoId(_currentState);
			args.ledId = _mediaManager->getRandomLedId(_currentState);
			args.syncVideo = true;
			args.syncLeds = true;
			args.loop = true;
			_updateFollowers(args);
			break;
		case Pulled:
			_mediaManager->stop();
			break;
		case Reveal:
			_notifyReveal();
			break;
		default:
			// do nothing
			break;
	}

	_mediaManager->updateState(args);
}

void TripdeckLeader::_runStartup() {
	if (_nodeIdToStatus.size() < NUM_FOLLOWERS)
		return;
	
	if (!_followersSynced) {
		if (_verifySynced()) {
			_followersSynced = true;
			_currentState = Connected;
			_onStateChanged();
		}

		return;
	}

	if (Clock::instance().millis() > STARTUP_TIME) {
		_currentState = Wait;
		_onStateChanged();
	}
}

void TripdeckLeader::_updateFollowers(TripdeckStateChangedArgs& args) {
	for (auto const& pair : _nodeIdToStatus) {
		_updateFollowerState(pair.first, args);
	}
}

void TripdeckLeader::_handleSerialInput(InputArgs& args) {
	if (args.buffer.length() < HEADER_LENGTH + 2) {
		#if ENABLE_SERIAL_DEBUG
		// TODO: Remove debug code
		std::cout << "Warning: Invalid message received -- length: " << args.buffer.length() << std::endl;
		#endif

		return;
	}

	const std::string header = _parseHeader(args.buffer);
	const std::string id = _parseId(args.buffer);

	// check message header
	if (header.compare(STARTUP_NOTIFICATION_HEADER) == 0) {
		// add node to map and send state update message
		if (_nodeIdToStatus.find(id) == _nodeIdToStatus.end())
			_nodeIdToStatus[id] = TripdeckStatus { 0x0, 0x0, Unknown, false };

		TripdeckStateChangedArgs stateArgs = { };
		stateArgs.newState = Connected;

		_updateFollowerState(id, stateArgs);
	} else if (header.compare(STATUS_UPDATE_HEADER) == 0) {
		// update internal representation of node's state
		if (_nodeIdToStatus.find(id) == _nodeIdToStatus.end())
			_nodeIdToStatus[id] = TripdeckStatus { 0x0, 0x0, _parseState(args.buffer), true };
		else
			_nodeIdToStatus[id].state = _parseState(args.buffer);

		if (_containsMediaHashes(args.buffer)) {
			MediaHashes hashes = _parseMediaHashes(args.buffer);
			_nodeIdToStatus[id].videoMedia = hashes.videoHash;
			_nodeIdToStatus[id].ledMedia = hashes.ledHash;
		}
	}
}

void TripdeckLeader::_updateFollowerState(const std::string& id, TripdeckStateChangedArgs& args) { 
	std::string message(STATE_CHANGED_HEADER);
	message.append(id + "/" + std::to_string(args.newState));
	
	if (args.syncVideo)
		message.append("/" + std::to_string(_mediaManager->getRandomVideoId(args.newState)));
	if (args.syncLeds)
		message.append("/" + std::to_string(_mediaManager->getRandomLedId(args.newState)));

	_serial->transmit(message);
}

bool TripdeckLeader::_verifySynced() {
	for (auto const& pair : _nodeIdToStatus) {
		if (!pair.second.connected)
			return false;
	}

	return true;
}

void TripdeckLeader::_handleUserInput(InputArgs* data) {

}