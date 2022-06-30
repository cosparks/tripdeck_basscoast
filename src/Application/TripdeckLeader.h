#ifndef _TRIPDECK_BEHAVIOR_LEADER_H_
#define _TRIPDECK_BEHAVIOR_LEADER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "Tripdeck.h"

class TripdeckLeader : public Tripdeck {
	public:
		TripdeckLeader(TripdeckMediaManager* mediaManager, InputManager* inputManager, Serial* serial);
		~TripdeckLeader();
		void init() override;
		void run() override;
	private:
		TripdeckMediaOption _nextMediaActionOption;
		MediaPlayer::MediaPlayerState _nextMediaPlayerState;
		std::unordered_map<char, TripdeckStatus> _nodeIdToStatus;
		std::vector<pair<int64_t, void (TripdeckLeader::*)(void)>> _oneShotActions;
		std::vector<Input*> _buttons;
		bool _followersSynced = false;
		bool _revealTriggered = false;
		bool _chainPulled = false;
		bool _executingPreReveal = false;

		// run
		void _runStartup();
		bool _verifySynced();
		void _runPulled();
		bool _verifyAllPulled();
		void _runOneShotActions();
		// state and input
		void _onStateChanged();
		void _handleSerialInput(InputArgs& args) override;
		void _receiveStartupNotification(char id, const std::string& buffer);
		void _receiveFollowerStatusUpdate(char id, const std::string& buffer);
		void _updateStateFollower(char id, TripdeckStateChangedArgs& args);
		void _updateStateFollowers(TripdeckStateChangedArgs& args);
		void _updateMediaStateFollower(char id, TripdeckMediaOption option, MediaPlayer::MediaPlayerState state);
		void _updateMediaStateUniversal(TripdeckMediaOption option, MediaPlayer::MediaPlayerState state);
		void _handleDigitalInput(InputArgs& data);
		void _handleChainPull(char id);
		void _handleReset();
		void _handleShutdown();
		// actions
		void _addOneShotAction(void (TripdeckLeader::*action)(void), int64_t wait);
		void _setMediaUpdateUniversalAction(TripdeckMediaOption option, MediaPlayer::MediaPlayerState state, int64_t wait = 5);
		void _updateMediaStateUniversalAction();
		void _cancelOneShotActions();
		void _executePreReveal();
		void _executeReveal();
		void _returnToWait();

		class DigitalInputDelegate : public Command {
			public:
				DigitalInputDelegate(TripdeckLeader* owner);
				~DigitalInputDelegate();
				void execute(CommandArgs args) override;
			private:
				TripdeckLeader* _owner;
		};
};

#endif