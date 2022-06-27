#include "TripdeckBehavior.h"

// tripdeck behavior
TripdeckBehavior::TripdeckBehavior(InputManager* inputManager, Serial* serial) : _inputManager(inputManager), _serial(serial) { }

void TripdeckBehavior::init() {
	_inputManager->init();
	_serial->init();
}

void TripdeckBehavior::run() {
	_inputManager->run();
}

TripdeckBehavior::TripdeckState TripdeckBehavior::getState() {
	return _currentState;
}

void TripdeckBehavior::setStateChangedDelegate(Command* delegate) {
	_stateChangedDelegate = delegate;
}

// serial input delegate
TripdeckBehavior::SerialInputDelegate::SerialInputDelegate(TripdeckBehavior* owner) : _owner(owner) { }

TripdeckBehavior::SerialInputDelegate::~SerialInputDelegate() { }

void TripdeckBehavior::SerialInputDelegate::execute(CommandArgs args) {
	_owner->_handleSerialInput(*((InputArgs*)args));
}