#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Apa102.h"
#include "LedController.h"

// media files
#define VIDEO_STARTUP_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/video/startup/"
#define VIDEO_WAIT_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/video/wait/"
#define VIDEO_PULLED_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/video/pulled/"
#define VIDEO_REVEAL_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/video/reveal/"

#define LED_STARTUP_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/led/startup/"
#define LED_WAIT_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/led/wait/"
#define LED_PULLED_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/led/pulled/"
#define LED_REVEAL_DIRECTORY "/home/trypdeck/projects/tripdeck_basscoast/media/led/reveal/"

// gpio
#define SERIAL_BAUD 9600 // networking
#define SPI_BAUD 4000000 // led matrix
#define ENABLE_DEBUG 0	// when true, stops all calls to pigpio (this disables leds!)

// led (do not modify)
#define MAIN_LEDS 0
#define CENTRE_LEDS 1
#define NO_LEDS 2

// led (make modifications for different setups below)
#define LED_SETTING MAIN_LEDS
#define PIXEL_BRIGHTNESS 31

#if (LED_SETTING == MAIN_LEDS)
// settings for main LED grid
#define LED_MATRIX_WIDTH 50
#define LED_MATRIX_HEIGHT 50
#define LED_MATRIX_SPLIT 25
#define LED_GRID_CONFIGURATION_OPTION Apa102::VerticalTopLeft		// configuration of first led grid in main light chamber
#define LED_GRID_CONFIGURATION_OPTION_2 Apa102::VerticalTopRight	// configuration of second led grid in main light chamber
#define LED_GRID_AB_ORIENTATION LedController::Horizontal			// orientation of grid A and grid B (Horizontal -> A B -- Vertical -> A / B)
#elif (LED_SETTING == CENTRE_LEDS)
// settings for centre LED grid
#define LED_MATRIX_WIDTH 25
#define LED_MATRIX_HEIGHT 30
#define LED_MATRIX_SPLIT 0
#define LED_GRID_CONFIGURATION_OPTION Apa102::VerticalTopLeft
#define LED_GRID_AB_ORIENTATION LedController::None
#endif

#endif