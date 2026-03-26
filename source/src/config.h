#ifndef _CONFIG_H_
#define _CONFIG_H_

// ============================================================================
// ARDUINO IDE BUILD CONFIGURATION
// These defines were previously set in platformio.ini build_flags
// ============================================================================

// Core configuration for FreeRTOS tasks
// Set to 0 to run main loop on core 0, emulation on core 1
// Set to 1 to run main loop on core 1, emulation on core 0
#ifndef ARDUINO_RUNNING_CORE
  #define ARDUINO_RUNNING_CORE 0
#endif

// SPI clock speed for TFT display
// 40000000 = 40 MHz (safer, use VIDEO_HALF_RATE)
// 80000000 = 80 MHz (faster, full frame rate)
#ifndef TFT_SPICLK
  #define TFT_SPICLK 40000000
#endif

// ============================================================================
// INPUT CONFIGURATION
// ============================================================================

// Uncomment to enable Bluetooth gamepad support (PS4, PS5, Xbox, Switch, etc.)
// Requires Bluepad32 library: https://github.com/ricardoquesada/bluepad32
#define BLUETOOTH_GAMEPAD

// Uncomment to enable I2C joystick support (PCF8574 expander)
#define USE_I2C_INPUT

// Analog stick deadzone (0-127, higher = less sensitive)
#define BLUETOOTH_DEADZONE 30

// Controller input mode - uncomment ONE of these:
//#define BT_INPUT_DPAD_ONLY      // Use only D-pad for movement
//#define BT_INPUT_ANALOG_ONLY  // Use only analog stick for movement
// If none defined, both D-pad and analog stick work (recommended)

// ============================================================================
// HARDWARE CONFIGURATION - Uncomment ONE of these options:
// ============================================================================

// Option 1: Cheap Yellow Display (ESP32-2432S028R)
#define CHEAP_YELLOW_DISPLAY_CONF //  decommentato www.survivalhacking.it

// Option 2: Standard ESP32 DevKit with external display (default)
// (no define needed, this is the default configuration)

// ============================================================================
// GAME SELECTION - Comment out games you don't want or don't have ROMs for
// ============================================================================

// disable e.g. if roms are missing
#define ENABLE_PACMAN
#define ENABLE_GALAGA
#define ENABLE_DKONG
#define ENABLE_FROGGER
#define ENABLE_DIGDUG
#define ENABLE_1942
#define ENABLE_EYES
// #define ENABLE_LIZARD
// #define ENABLE_THEGLOB
// #define ENABLE_MRTNT
#define ENABLE_CRUSH
// #define ENABLE_ANTEATER
#define ENABLE_MRDO
#define ENABLE_BOMBJACK
#define ENABLE_MSPACMAN
#define ENABLE_GALAXIAN
#define ENABLE_LADYBUG
#define ENABLE_GYRUSS // to enable this game you must disable Tutankham
#define ENABLE_TIMEPLT
#define ENABLE_PENGO
#define ENABLE_BAGMAN
#define ENABLE_SPACE
#define ENABLE_TUTANKHM // to enable this game you must disable GYRUSS
#if !defined(ENABLE_PACMAN) && !defined(ENABLE_GALAGA) && !defined(ENABLE_DKONG) && !defined(ENABLE_FROGGER) && !defined(ENABLE_DIGDUG) && !defined(ENABLE_1942) && !defined(ENABLE_EYES) && !defined(ENABLE_LIZARD) && !defined(ENABLE_THEGLOB) && !defined(ENABLE_MRTNT) && !defined(ENABLE_CRUSH) && !defined(ENABLE_ANTEATER) && !defined(ENABLE_MRDO) && !defined(ENABLE_BOMBJACK) && !defined(ENABLE_GALAXIAN) && !defined(ENABLE_LADYBUG) && !defined(ENABLE_GYRUSS) && !defined(ENABLE_TIMEPLT) && !defined(ENABLE_PENGO) && !defined(ENABLE_BAGMAN) && !defined(ENABLE_SPACE) && !defined(ENABLE_TUTANKHM)
  #error "At least one machine has to be enabled!"
#endif

// check if only one machine is enabled (preprocessor counter)
#define _MC 0
#ifdef ENABLE_PACMAN
  #undef _MC
  #define _MC 1
#endif
#ifdef ENABLE_GALAGA
  #if _MC == 0
    #undef _MC
    #define _MC 1
  #else
    #undef _MC
    #define _MC 2
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_DKONG
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_FROGGER
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_DIGDUG
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_1942
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_EYES
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_LIZARD
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_THEGLOB
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_MRTNT
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_ANTEATER
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_MRDO
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_BOMBJACK
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_MSPACMAN
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_GALAXIAN
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_LADYBUG
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_GYRUSS
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_TIMEPLT
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_PENGO
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_BAGMAN
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_SPACE
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC < 2
  #ifdef ENABLE_TUTANKHM
    #if _MC == 0
      #undef _MC
      #define _MC 1
    #else
      #undef _MC
      #define _MC 2
    #endif
  #endif
#endif
#if _MC == 1
  #define SINGLE_MACHINE
#endif
#undef _MC


// game config
#define MASTER_ATTRACT_MENU_TIMEOUT  20000      // start games while sitting idle in menu for 20 seconds, undefine to disable
#define MASTER_ATTRACT_GAME_TIMEOUT  60000 * 5  // restart after 5 minutes 

// video config
// TFT_SPICLK is now defined at the top of this file

// max possible video rate:
// 8*224 pixels = 8*224*16 = 28672 bits
// 2790 char rows per sec at 40Mhz = max 38 fps
#if TFT_SPICLK < 80000000
  #define VIDEO_HALF_RATE
#endif

// x and y offset of 224x288 pixels inside the 240x320 screen
#define TFT_X_OFFSET      8
#define TFT_Y_OFFSET      16

// led config
//#define LED_PIN           18 // pin used for optional WS2812 stripe
#define LED_BRIGHTNESS 	  50 // range 0..255

// audio config
//#define SND_DIFF   	 // set to output differential audio on GPIO25 _and_ inverted on GPIO26
#define SND_LEFT_CHANNEL // Use GPIO 26 for audio

// ESP32 model specific pin configurations
// CHEAP_YELLOW_DISPLAY_CONF is defined at the top of this file if needed

#ifdef CHEAP_YELLOW_DISPLAY_CONF
  #define TFT_CS          15
  #define TFT_DC          2
  #define TFT_RST         -1
  #define TFT_BL          27   // don't set if backlight is hard wired
  #define TFT_BL_LEVEL    HIGH  // backlight on with low or high signal
  #define TFT_ILI9341   // define for ili9341, otherwise st7789 -- decommenteato www.survivalhacking.it 
 //#define TFT_VFLIP   // define for upside down

  #define TFT_MISO 	      12
  #define TFT_MOSI 	      13
  #define TFT_SCLK 	      14
  #define TFT_MAC  	      0x48  // test
 // #define TFT_MAC  	    0x20  // some CYD need this to rotate properly and have correct colors

  // Pins used for buttons
  //  #define BTN_START_PIN	  35
  //#define BTN_COIN_PIN    21   // if this is not defined, then start will act as coin & start

  // #define BTN_LEFT_PIN    21
  // #define BTN_RIGHT_PIN   22
  // #define BTN_DOWN_PIN    16
  // #define BTN_UP_PIN      17
  // #define BTN_FIRE_PIN    4
#endif

#ifndef CHEAP_YELLOW_DISPLAY_CONF
  #define TFT_CS          5
  #define TFT_DC          4
  #define TFT_RST         22
  #define TFT_BL          15      // don't set if backlight is hard wired
  #define TFT_BL_LEVEL    LOW     // backlight on with low or high signal
  #define TFT_ILI9341             // define for ili9341, otherwise st7789
  //#define TFT_VFLIP               // define for upside down

  #define TFT_MISO 	      19
  #define TFT_MOSI 	      23
  #define TFT_SCLK 	      18

  // Pins used for buttons
  #define BTN_START_PIN   0       // Boot button on most ESP32 DevKit boards
  //#define BTN_COIN_PIN    21      // if this is not defined, then start will act as coin & start

  #ifndef NUNCHUCK_INPUT
    #define BTN_LEFT_PIN  33
    #define BTN_RIGHT_PIN 14
    #define BTN_DOWN_PIN  16
    #define BTN_UP_PIN    21
    #define BTN_FIRE_PIN  12
  #else
    #define NUNCHUCK_SDA  33
    #define NUNCHUCK_SCL  32
    #define NUNCHUCK_MOVE_THRESHOLD 30 // This is the dead-zone for where minor movements on the stick will not be considered valid movements
  #endif
#endif

#endif // _CONFIG_H_
