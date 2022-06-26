#include "TripdeckBehaviorFollower.h"

#include "Clock.h"

#define STARTUP_NOTIFICATION_INTERVAL 1000

TripdeckBehaviorFollower::TripdeckBehaviorFollower(InputManager* inputManager, Serial* serial) : TripdeckBehavior(inputManager, serial) { }

TripdeckBehaviorFollower::~TripdeckBehaviorFollower() { }

void TripdeckBehaviorFollower::init() {
	TripdeckBehavior::init();
	_currentState = Startup;
	// hook up serial inputs with callbacks
}

void TripdeckBehaviorFollower::run() {
	switch (_currentState) {
		case Startup:
			_notifyLeader();
			break;
		case Wait:
			TripdeckBehavior::run();
			break;
		case Pulled:
			TripdeckBehavior::run();
			break;
		case Reveal:
			TripdeckBehavior::run();
			break;
		default:
			// do nothing
			break;
	}
}

void TripdeckBehaviorFollower::_onStateChanged(TripdeckStateChangedArgs& args) {
	_stateChangedDelegate->execute((CommandArgs)&args);
}

void TripdeckBehaviorFollower::_notifyLeader() {
	int64_t currentTime = Clock::instance().millis();
	if (currentTime >= _nextActionMillis) {
		_nextActionMillis = currentTime + STARTUP_NOTIFICATION_INTERVAL;
		std::string data = STARTUP_NOTIFICATION_HEADER;
		data += ID;
		_serial->transmit(data);
	}
}

void TripdeckBehaviorFollower::_handleSerialInput(const std::string& buffer) {
	// parse state changed transmission for this ID
	if (buffer.substr(0, HEADER_LENGTH).compare(STATE_CHANGED_HEADER) == 0 && buffer.substr(HEADER_LENGTH, 1).compare(ID) == 0 ) {
		TripdeckBehavior::TripdeckStateChangedArgs args = { };
		args.newState = (TripdeckBehavior::TripdeckState)std::stoi(buffer.substr(HEADER_LENGTH + 2, 1));
		
		// parse message data only if we are entering a new state
		if (args.newState != _currentState) {
			// check for extra data in serial message
			if (buffer.substr(HEADER_LENGTH + 3, 1).compare("/") == 0) {
				std::string mediaHashes = buffer.substr(HEADER_LENGTH + 4);
				int32_t slashIndex = mediaHashes.find("/");
				uint32_t videoHash = std::stoul(mediaHashes.substr(0, slashIndex), NULL, 16);
				uint32_t ledHash = std::stoul(mediaHashes.substr(slashIndex + 1));

				if (videoHash != 0) {
					args.videoId = videoHash;
					args.syncVideo = true;
				}

				if (ledHash != 0) {
					args.ledId = ledHash;
					args.syncLeds = true;
				}
			}

			_currentState = args.newState;
			_onStateChanged(args);
		}
	} else {
		// if transmission is not for us, pass it on
		_serial->transmit(buffer);
	}
}

