#ifndef _INPUT_THREADED_SERIAL_H_
#define _INPUT_THREADED_SERIAL_H_

#include <mutex>

#include "td_util.h"
#include "Serial.h"

using namespace td_util;

class InputThreadedSerial : public Input {
	public:
		InputThreadedSerial(uint32_t id, Serial* serial);
		~InputThreadedSerial();
		bool read() override;
	private:
		Serial* _serial = NULL;
		bool _currentlyReading = false;
		bool _dataAvailable = false;
		std::mutex _stateMutex;

		void _readInternal();
		void _setCurrentlyReading(bool value);
		bool _getCurrentlyReading();
		void _setDataAvailable(bool value);
		bool _getDataAvailable();
};

#endif