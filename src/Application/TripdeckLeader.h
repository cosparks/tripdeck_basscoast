#ifndef _TRIPDECK_BEHAVIOR_LEADER_H_
#define _TRIPDECK_BEHAVIOR_LEADER_H_

#include <string>
#include <vector>

#include "Tripdeck.h"

class TripdeckLeader : public Tripdeck {
	public:
		TripdeckLeader(TripdeckMediaManager* mediaManager, InputManager* inputManager, Serial* serial);
		~TripdeckLeader();
		void init() override;
		void run() override;
		void handleMediaChanged(TripdeckStateChangedArgs& args) override;
	private:
		std::vector<std::string> _nodeIds;
		bool _connected = false;

		void _onStateChanged(TripdeckStateChangedArgs& args) override;
		void _updateFollowers();
		void _handleSerialInput(InputArgs& args) override;
		void _handleUserInput(InputArgs* data);
		void _notifyConnected(const std::string& id);
		void _runStartup();
		void _notifyPulled();
		void _notifyReveal();
};

#endif