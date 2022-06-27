#ifndef _TRIPDECK_BEHAVIOR_FOLLOWER_H_
#define _TRIPDECK_BEHAVIOR_FOLLOWER_H_

#include "Tripdeck.h"

class TripdeckFollower : public Tripdeck {
	public:
		TripdeckFollower(TripdeckMediaManager* mediaManager, InputManager* inputManager, Serial* serial);
		~TripdeckFollower();
		void init() override;
		void run() override;
		void handleMediaChanged(TripdeckStateChangedArgs& args) override;
	private:
		int64_t _nextActionMillis = 0;

		void _onStateChanged(TripdeckStateChangedArgs& args) override;
		void _notifyLeader();
		void _handleSerialInput(InputArgs& args) override;
		bool _parseStateChangedMessage(const std::string& buffer, TripdeckStateChangedArgs& args);
};

#endif