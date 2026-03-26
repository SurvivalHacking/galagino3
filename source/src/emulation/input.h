#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <functional>
#include "../config.h"
#ifdef NUNCHUCK_INPUT
  #include "nunchuck.h"
#endif

// a total of 7 button is needed for most games
#define BUTTON_LEFT  0x01
#define BUTTON_RIGHT 0x02
#define BUTTON_UP    0x04
#define BUTTON_DOWN  0x08
#define BUTTON_FIRE  0x10
#define BUTTON_START 0x20
#define BUTTON_COIN  0x40
#define BUTTON_EXTRA 0x80

class Input {
public:
  void init();
  bool initBluetooth();          // Call early in setup() before boot screen, returns false on failure
  unsigned char getBluetoothButtons(); // Poll BT and return button state
  void enable();
  void disable();
  void updateBluetooth(void);  // Call from main loop to update Bluetooth controller
  unsigned char buttons_get(void);
  unsigned char buttons_get_raw(void);  // Lettura diretta senza side-effects
  bool button_y_pressed(void);  // Y/Triangle on BT gamepad (for Tutankham flash bomb)
  char demoSoundsOff() { return 0; } 
  bool isBluetoothEnabled();   // Check if Bluetooth is currently enabled
 
  typedef std::function<void(bool up, bool down)> THandlerVolume;
  Input& onVolumeUpDown(THandlerVolume fn);

  typedef std::function<void(void)> THandlerDoReset;
  Input& onDoReset(THandlerDoReset fn);

  typedef std::function<void(void)> THandlerDoAttractReset;
  Input& onDoAttractReset(THandlerDoAttractReset fn);

private:
  THandlerVolume _volume_callback;
  THandlerDoReset _doReset_callback;
  THandlerDoAttractReset _doAttractReset_callback;
  unsigned char input_states_last;
  int virtual_coin_state;
  unsigned long virtual_coin_timer;
  unsigned long reset_timer;
};

#endif
