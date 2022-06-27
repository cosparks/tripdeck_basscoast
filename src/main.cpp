#include <iostream>

#include "settings.h"
#include "Clock.h"
#include "Serial.h"
#include "VideoPlayer.h"
#include "LedPlayer.h"
#include "TripdeckMedia.h"
#include "TripdeckLeader.h"
#include "TripdeckFollower.h"
#include "MockButton.h"

const char* VideoFolders[] = { VIDEO_CONNECTING_DIRECTORY, VIDEO_WAIT_DIRECTORY, VIDEO_PULLED_DIRECTORY, VIDEO_REVEAL_DIRECTORY };
const char* LedFolders[] = { LED_CONNECTING_DIRECTORY, LED_WAIT_DIRECTORY, LED_PULLED_DIRECTORY, LED_REVEAL_DIRECTORY };

InputManager inputManager;
Serial serial("/dev/ttyS0", O_RDWR);

int main(int argc, char** argv) {
	system("sudo sh -c \"TERM=linux setterm -foreground black -clear all >/dev/tty0\"");
	DataManager dataManager;
	VideoPlayer videoPlayer;

	#ifdef Leader
	TripdeckLeader behavior(&inputManager, &serial);
	#else
	TripdeckFollower behavior(&inputManager, &serial);
	#endif

	// set up leds
	#if RUN_LEDS
	#if (LED_SETTING == MAIN_LEDS)
	LedController ledController(LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, LED_MATRIX_SPLIT, LED_CONTROLLER_ORIENTATION, LED_GRID_CONFIGURATION_OPTION_A, LED_GRID_CONFIGURATION_OPTION_B);
	#else
	LedController ledController(LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, 0, LED_CONTROLLER_ORIENTATION, LED_GRID_CONFIGURATION_OPTION, Apa102::GridConfigurationOption(0));
	#endif
	LedPlayer ledPlayer(&ledController);
	TripdeckMedia application(&dataManager, &behavior, &videoPlayer, &ledPlayer);
	#else
	TripdeckMedia application(&dataManager, &behavior, &videoPlayer);
	#endif

	// add media folders for different application states
	for (int32_t state = Tripdeck::TripdeckState::Connecting; state <= Tripdeck::TripdeckState::Reveal; state++) {
		application.addVideoFolder(Tripdeck::TripdeckState(state), VideoFolders[state]);

		#if RUN_LEDS
		application.addLedFolder(Tripdeck::TripdeckState(state), LedFolders[state]);
		#endif
	}


	// initialize and run application
	application.init();
	application.run();

	system("sudo sh -c \"TERM=linux setterm -foreground white >/dev/tty0\"");
	return 1;
}