#include <iostream>
#include <iomanip>

#include "Tripdeck.h"

const char ValidHeaders[] = { STARTUP_NOTIFICATION_HEADER, STATE_CHANGED_HEADER, STATUS_UPDATE_HEADER, PLAY_MEDIA_HEADER,
	STOP_MEDIA_HEADER, PAUSE_MEDIA_HEADER, PLAY_MEDIA_FROM_ARGS_HEADER, MEDIA_PLAYBACK_COMPLETE_HEADER, SYSTEM_RESET_HEADER, SYSTEM_SHUTDOWN_HEADER };

// tripdeck behavior
Tripdeck::Tripdeck(TripdeckMediaManager* mediaManager, InputManager* inputManager, Serial* serial) : _mediaManager(mediaManager), _inputManager(inputManager), _serial(serial) { }

void Tripdeck::init() {
	// initialize members
	_inputManager->init();
	_mediaManager->init();
	_serial->init();
	
	// create and add serial input and serial input delegate to InputManager
	// ID does not matter in this case -- objects will be cleaned up by InputManger on Destruction
	_inputManager->addInput(new InputThreadedSerial((char)0xFF, _serial), new Delegate<Tripdeck, InputArgs>(this, &Tripdeck::_handleSerialInput));
	// _inputManager->addInput(new InputThreadedSerial((char)0xFF, _serial), new SerialInputDelegate(this));

	// add playback complete handler
	_mediaManager->setPlaybackCompleteDelegate(new Delegate<Tripdeck, TripdeckStateChangedArgs>(this, &Tripdeck::_mediaManagerPlaybackComplete));

	_status.state = Connecting;
	_run = true;
}

void Tripdeck::run() {
	_inputManager->run();
	_mediaManager->run();
}

bool Tripdeck::_validateSerialMessage(const std::string& buffer) {
	// shortest possible message is HEADER_LENGTH + ID + '\n'
	if (buffer.length() < HEADER_LENGTH + 2) {
		#if ENABLE_SERIAL_DEBUG
		// TODO: Remove debug code
		std::cout << "Warning: Invalid message received -- length: " << buffer.length() << std::endl;
		#endif
		
		return false;
	}

	// check if first character is alphanumeric
	return iswalnum(buffer[0]) != 0 && _validateHeader(buffer[0]);
}

bool Tripdeck::_validateHeader(char header) {
	for (char h : ValidHeaders) {
		if (header == h)
			return true;
	}
	return false;
}

Tripdeck::MediaHashes Tripdeck::_parseMediaHashes(const std::string& buffer) {
	std::string mediaHashes = buffer.substr(HASH_INDEX);
	int32_t slashIndex = mediaHashes.find("/");
	return MediaHashes { std::stoul(mediaHashes.substr(0, slashIndex), NULL, 16), std::stoul(mediaHashes.substr(slashIndex + 1), NULL, 16) };
}

const std::string Tripdeck::_hashToHexString(uint32_t hash) {
	std::stringstream stream;
	stream << std::hex << hash;
	return stream.str();
}

void Tripdeck::_updateStatusFromStateArgs(TripdeckStateChangedArgs& args) {
	_status.videoMedia = args.videoId;
	_status.ledMedia = args.ledId;
	_status.option = args.mediaOption;
	_status.state = args.state;
	_status.lastTransmitMillis = Clock::instance().millis();
	_status.connected = true;
}

void Tripdeck::_populateStateArgsFromBuffer(const std::string& buffer, TripdeckStateChangedArgs& args) {
	args.state = _parseState(buffer);

	// check for media id info
	if (_containsMediaHashes(buffer)) {
		MediaHashes hashes = _parseMediaHashes(buffer);
		args.videoId = hashes.videoHash;
		args.ledId = hashes.ledHash;
	}

	args.mediaOption = _parseMediaOption(buffer);
	args.playbackOption = _parsePlaybackOption(buffer);
}

std::string Tripdeck::_populateBufferFromStateArgs(const TripdeckStateChangedArgs& args, char header, char id) {
	std::string message = DEFAULT_MESSAGE;
	message[0] = header;
	message[ID_INDEX] = id;
	message[STATE_INDEX] = _singleDigitIntToChar((int32_t)args.state);
	message[MEDIA_OPTION_INDEX] = _singleDigitIntToChar((int32_t)args.mediaOption);
	message[PLAYBACK_OPTION_INDEX] = _singleDigitIntToChar((int32_t)args.playbackOption);
	
	if (args.videoId || args.ledId)
		message.append("/" + _hashToHexString(args.videoId) + "/" + _hashToHexString(args.ledId));

	return message;
}

void Tripdeck::_mediaManagerPlaybackComplete(const TripdeckStateChangedArgs& args) {
	// at the moment, we only care about Cycling Led videos
	if (args.mediaOption == Led && args.playbackOption == MediaPlayer::Cycle) {
		_handleMediaPlayerPlaybackComplete(args);
	}
}

void Tripdeck::_reset() {
	system("sudo reboot");
}

void Tripdeck::_shutdown() {
	system("sudo shutdown -h now");
}

// Serial Input Delegate
Tripdeck::SerialInputDelegate::SerialInputDelegate(Tripdeck* owner) : _owner(owner) { }

Tripdeck::SerialInputDelegate::~SerialInputDelegate() { }

void Tripdeck::SerialInputDelegate::execute(CommandArgs args) {
	_owner->_handleSerialInput(*((InputArgs*)args));
}
