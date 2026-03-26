#include "emulation/input.h"
#include <EEPROM.h>
#include "esp_task_wdt.h"

#define EEPROM_BT_ADDR 0

#ifdef USE_I2C_INPUT
  #include "PCF8574.h"
  
  extern PCF8574* pcfPtr; 
  #ifndef pcf20
    #define pcf20 (*pcfPtr)
  #endif
#endif

#ifdef BLUETOOTH_GAMEPAD
  #include <Bluepad32.h>
  // Workaround: Bluepad32 might define something that conflicts
  #ifdef Input
    #undef Input
  #endif
#endif

#ifdef NUNCHUCK_INPUT
  Nunchuck nunchuck;
#endif

#ifdef BLUETOOTH_GAMEPAD
  static ControllerPtr myController = nullptr;
  static volatile unsigned char bt_input_states = 0;
  static volatile unsigned char i2c_input_states = 0;
  static bool bt_y_button = false;  // Y/Triangle state for Tutankham flash bomb

  // Bluetooth toggle via COIN long-press (5 seconds)
  static bool bluetoothEnabled = true;
  static unsigned long coinHoldStart = 0;
  static bool btToggleTriggered = false;

  extern void show_bt_status(bool enabled);

  // Callback when controller is connected
  void onConnectedController(ControllerPtr ctl) {
    if (myController == nullptr) {
      Serial.println("CONTROLLER CONNECTED");
      Serial.print("Model: ");
      Serial.println(ctl->getModelName().c_str());
      myController = ctl;
    }
  }

  // Callback when controller is disconnected
  void onDisconnectedController(ControllerPtr ctl) {
    if (myController == ctl) {
      Serial.println("CONTROLLER DISCONNECTED");
      myController = nullptr;
      bt_input_states = 0;
    }
  }

  // Read input from Bluetooth controller
  unsigned char readBluetoothInput() {
    unsigned char states = 0;

    if (myController && myController->isConnected()) {
      // Debug: show raw dpad value
      static uint8_t lastDpad = 0;
      uint8_t currentDpad = myController->dpad();
      if (currentDpad != lastDpad) {
        Serial.printf("RAW DPAD: 0x%02X\n", currentDpad);
        lastDpad = currentDpad;
      }

      // D-pad (unless analog-only mode)
      // Bluepad32 dpad(): 0x01=UP, 0x02=DOWN, 0x04=RIGHT, 0x08=LEFT
      #ifndef BT_INPUT_ANALOG_ONLY
      if (myController->dpad() & 0x01) states |= BUTTON_UP;
      if (myController->dpad() & 0x02) states |= BUTTON_DOWN;
      if (myController->dpad() & 0x04) states |= BUTTON_RIGHT;
      if (myController->dpad() & 0x08) states |= BUTTON_LEFT;
      #endif

      // Analog stick (unless dpad-only mode)
      #ifndef BT_INPUT_DPAD_ONLY
      int axisX = myController->axisX();  // -511 to 512
      int axisY = myController->axisY();  // -511 to 512

      #ifndef BLUETOOTH_DEADZONE
        #define BLUETOOTH_DEADZONE 30
      #endif

      int deadzone = BLUETOOTH_DEADZONE * 4;  // Scale to axis range

      if (axisX < -deadzone) states |= BUTTON_LEFT;
      if (axisX > deadzone)  states |= BUTTON_RIGHT;
      if (axisY < -deadzone) states |= BUTTON_UP;
      if (axisY > deadzone)  states |= BUTTON_DOWN;
      #endif

      // Pulsante in basso (A/Cross) = FIRE
      if (myController->buttons() & 0x01) states |= BUTTON_FIRE;
      // Pulsante a destra (B/Circle) = loop/barrel roll in 1942 (BUTTON_EXTRA -> B2)
      if (myController->buttons() & 0x02) states |= BUTTON_EXTRA;
      // Pulsante a sinistra (X/Square) e in alto (Y/Triangle) = FIRE alternativo
      if (myController->buttons() & 0x0C) states |= BUTTON_FIRE;
      // Shoulder e trigger = FIRE
      if (myController->l1() || myController->r1() ||
          myController->l2() > 100 || myController->r2() > 100) {
        states |= BUTTON_FIRE;
      }

      // SELECT/Share (miscButtons bit 1) = inserisce coin
      if (myController->miscButtons() & 0x02) states |= BUTTON_COIN;
      // START/Options (miscButtons bit 2) = START (avvia gioco)
      if (myController->miscButtons() & 0x04) {
        states |= BUTTON_START;
        // Also set EXTRA so it works as game select in menu
        states |= BUTTON_EXTRA;
      }

      // Quick exit to menu: L1 + R1 + Options (immediate reset)
      if (myController->l1() && myController->r1() &&
          (myController->miscButtons() & 0x04)) {
        Serial.println("EXIT COMBO DETECTED! Returning to menu...");
        extern bool doReset;
        doReset = true;
      }
    }

    return states;
  }
#endif

#ifdef USE_I2C_INPUT
static unsigned char lastValidI2C = 0;

unsigned char readI2CInput() {
  if (pcfPtr == nullptr) return 0;  // PCF8574 non trovato, nessun input
  uint8_t raw = pcf20.read8();

  // Error check: 0x00 means bus error (with pull-ups, resting state is 0xFF)
  // Also reject if all 7 button bits are active (impossible to press all at once)
  if (raw == 0x00) {
    return lastValidI2C;  // Keep previous valid state
  }

  uint8_t i2c_buttons = ~raw;
  unsigned char states = 0;

  // Direzioni joystick
  if (i2c_buttons & 0x01) states |= BUTTON_LEFT;
  if (i2c_buttons & 0x02) states |= BUTTON_RIGHT;
  if (i2c_buttons & 0x04) states |= BUTTON_UP;
  if (i2c_buttons & 0x08) states |= BUTTON_DOWN;

  // Pulsanti
  if (i2c_buttons & 0x10) states |= BUTTON_FIRE;
  if (i2c_buttons & 0x20) states |= BUTTON_START | BUTTON_EXTRA;  // Pin 5 = START
  if (i2c_buttons & 0x40) states |= BUTTON_COIN;                  // Pin 6 = COIN

  lastValidI2C = states;
  return states;
}
#endif


bool Input::button_y_pressed(void) {
#ifdef BLUETOOTH_GAMEPAD
  return bt_y_button;
#else
  return false;
#endif
}


// Initialize Bluetooth early (can be called before init())
// Returns true if BT initialized OK, false if it failed
bool Input::initBluetooth() {
#ifdef BLUETOOTH_GAMEPAD
  static bool done = false;
  if (done) return true;
  done = true;

  // Load Bluetooth enabled state from EEPROM
  uint8_t btSaved = EEPROM.read(EEPROM_BT_ADDR);
  if (btSaved == 0) {
    bluetoothEnabled = false;
    Serial.println("Bluetooth DISABLED (loaded from EEPROM)");
  } else {
    bluetoothEnabled = true;  // 1 or 0xFF (uninitialized) = enabled
    Serial.println("Bluetooth ENABLED (loaded from EEPROM)");
  }

  Serial.println("Initializing Bluetooth gamepad...");

  // Enable watchdog (10s timeout) to recover from BT init hang
  // panic=false per evitare reset se Core 1 IDLE task non è stato rimosso dal WDT
  esp_task_wdt_init(10, false);
  esp_task_wdt_add(NULL);

  BP32.setup(&onConnectedController, &onDisconnectedController);

  // BT init succeeded, disable watchdog
  esp_task_wdt_delete(NULL);

  if (bluetoothEnabled) {
    BP32.enableNewBluetoothConnections(true);
    Serial.println("Bluetooth ready!");
  } else {
    BP32.enableNewBluetoothConnections(false);
    Serial.println("Bluetooth connections disabled. Hold COIN 5s to enable.");
  }
  return true;
#else
  return true;
#endif
}

// Poll Bluetooth and return current button state
unsigned char Input::getBluetoothButtons() {
#ifdef BLUETOOTH_GAMEPAD
  if (bluetoothEnabled) {
    BP32.update();
    bt_input_states = readBluetoothInput();
  }
  return bt_input_states;
#else
  return 0;
#endif
}

void Input::init() {
  // Initialize member variables
  virtual_coin_state = 0;
  virtual_coin_timer = 0;
  reset_timer = 0;
  input_states_last = 0;

#ifdef BLUETOOTH_GAMEPAD
  // Init Bluetooth if not already done by initBluetooth()
  initBluetooth();
#endif

#if !defined(NUNCHUCK_INPUT) && !defined(BLUETOOTH_GAMEPAD)
  !defined(USE_I2C_INPUT)
  pinMode(BTN_START_PIN, INPUT_PULLUP);
  #ifdef BTN_COIN_PIN
    pinMode(BTN_COIN_PIN, INPUT_PULLUP);
  #endif
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_FIRE_PIN, INPUT_PULLUP);
#elif defined(NUNCHUCK_INPUT)
  nunchuck.setup();
#endif
}

void Input::enable() {
#ifdef NUNCHUCK_INPUT
  nunchuck.enable();
#endif
}

void Input::disable() {
#ifdef NUNCHUCK_INPUT
  nunchuck.disable();
  vTaskDelay(100);
#endif
}

auto Input::onVolumeUpDown(THandlerVolume fn) -> Input& {
  _volume_callback = fn;
  return *this;
}

auto Input::onDoReset(THandlerDoReset fn) -> Input& {
  _doReset_callback = fn;
  return *this;
}

auto Input::onDoAttractReset(THandlerDoAttractReset fn) -> Input& {
  _doAttractReset_callback = fn;
  return *this;
}

// Check if Bluetooth is currently enabled
bool Input::isBluetoothEnabled() {
#ifdef BLUETOOTH_GAMEPAD
  return bluetoothEnabled;
#else
  return false;
#endif
}

// Call this from the main loop to update Bluetooth state
void Input::updateBluetooth(void) {
#ifdef BLUETOOTH_GAMEPAD
  // Only process Bluetooth if enabled
  if (bluetoothEnabled) {
    // Heartbeat to verify this function is being called
    static unsigned long lastHB = 0;
    if (millis() - lastHB > 2000) {
      if (myController && myController->isConnected()) {
        // Show raw controller state
        Serial.printf("BT HB: dpad=0x%02X btns=0x%04X misc=0x%02X\n",
          myController->dpad(), myController->buttons(), myController->miscButtons());
      } else {
        Serial.println("BT HB: controller not connected");
      }
      lastHB = millis();
    }

    BP32.update();
    unsigned char newState = readBluetoothInput();

    // Debug: show when buttons change
    if (newState != bt_input_states) {
      Serial.printf("BT UPDATE: 0x%02X -> 0x%02X\n", bt_input_states, newState);
    }

    bt_input_states = newState;
  } else {
    bt_input_states = 0;
  }
#endif
  // I2C input update (throttled to every 20ms to avoid slowing down rendering)
#ifdef USE_I2C_INPUT
  static unsigned long lastI2CRead = 0;
  unsigned long now = millis();
  if (now - lastI2CRead < 20) return;  // skip if less than 20ms since last read
  lastI2CRead = now;

  unsigned char newI2CState = readI2CInput();

  // Detect COIN long-press (5 seconds) to toggle Bluetooth (pin 6)
  #ifdef BLUETOOTH_GAMEPAD
  {
    if (newI2CState & BUTTON_COIN) {  // Pin 6 pressed
      if (coinHoldStart == 0) {
        coinHoldStart = millis();
      }
      // Reset timer se si regola il volume (COIN + UP/DOWN) per evitare toggle BT accidentale
      if (newI2CState & (BUTTON_UP | BUTTON_DOWN)) {
        coinHoldStart = millis();
      }
      if ((millis() - coinHoldStart >= 5000) && !btToggleTriggered) {
        bluetoothEnabled = !bluetoothEnabled;
        EEPROM.write(EEPROM_BT_ADDR, bluetoothEnabled ? 1 : 0);
        EEPROM.commit();
        if (!bluetoothEnabled) {
          BP32.enableNewBluetoothConnections(false);
          if (myController && myController->isConnected()) {
            myController->disconnect();
            myController = nullptr;
            bt_input_states = 0;
          }
          Serial.println("Bluetooth DISABLED via COIN long-press (saved to EEPROM)");
        } else {
          BP32.enableNewBluetoothConnections(true);
          Serial.println("Bluetooth ENABLED via COIN long-press (saved to EEPROM)");
        }
        show_bt_status(bluetoothEnabled);
        btToggleTriggered = true;
      }
      // Sopprimi COIN dopo 1s per evitare crediti multipli, ma NON durante regolazione volume
      if (millis() - coinHoldStart >= 1000 && !(newI2CState & (BUTTON_UP | BUTTON_DOWN))) {
        newI2CState &= ~BUTTON_COIN;
      }
    } else {  // Pin 6 released
      coinHoldStart = 0;
      btToggleTriggered = false;
    }
  }
  #endif

  if (newI2CState != i2c_input_states) {
    Serial.printf("I2C UPDATE: 0x%02X -> 0x%02X\n", i2c_input_states, newI2CState);
  }
  i2c_input_states = newI2CState;
#endif
}

unsigned char Input::buttons_get(void) {
  // Note: Bluetooth is now updated via updateBluetooth() called from main loop
#ifdef BLUETOOTH_GAMEPAD
  // Debug: show all button presses
  static unsigned char lastBtStates = 0;
  if (bt_input_states != lastBtStates) {
    Serial.printf("BT INPUT: 0x%02X (", bt_input_states);
    if (bt_input_states & BUTTON_UP) Serial.print("UP ");
    if (bt_input_states & BUTTON_DOWN) Serial.print("DOWN ");
    if (bt_input_states & BUTTON_LEFT) Serial.print("LEFT ");
    if (bt_input_states & BUTTON_RIGHT) Serial.print("RIGHT ");
    if (bt_input_states & BUTTON_FIRE) Serial.print("FIRE ");
    if (bt_input_states & BUTTON_EXTRA) Serial.print("OPTIONS ");
    Serial.println(")");
    lastBtStates = bt_input_states;
  }
#endif

  // galagino can be compiled without coin button. This will then
  // be implemented by the start button. Whenever the start button
  // is pressed, a virtual coin button will be sent first
  unsigned char input_states = 0;

  // Priority 1: Bluetooth input
#ifdef BLUETOOTH_GAMEPAD
  input_states |= bt_input_states;
#endif

  // Priority 2: I2C input (independent from Bluetooth)
#ifdef USE_I2C_INPUT
  input_states |= i2c_input_states;
  
  // Debug I2C input
  static unsigned char lastI2CStates = 0;
  if (i2c_input_states != lastI2CStates) {
    Serial.printf("I2C INPUT: 0x%02X (", i2c_input_states);
    if (i2c_input_states & BUTTON_UP) Serial.print("UP ");
    if (i2c_input_states & BUTTON_DOWN) Serial.print("DOWN ");
    if (i2c_input_states & BUTTON_LEFT) Serial.print("LEFT ");
    if (i2c_input_states & BUTTON_RIGHT) Serial.print("RIGHT ");
    if (i2c_input_states & BUTTON_FIRE) Serial.print("FIRE ");
    if (i2c_input_states & BUTTON_EXTRA) Serial.print("EXTRA ");
    Serial.println(")");
    lastI2CStates = i2c_input_states;
  }
#endif

  // Priority 3: Nunchuck input
#ifdef NUNCHUCK_INPUT
  input_states |= nunchuck.getInput();
#endif

  // Priority 4: Physical buttons (only if no BT, no I2C, no Nunchuck)
#if !defined(BLUETOOTH_GAMEPAD) && !defined(USE_I2C_INPUT) && !defined(NUNCHUCK_INPUT)
  #ifdef BTN_COIN_PIN
  input_states = (!digitalRead(BTN_COIN_PIN)) ? BUTTON_EXTRA : 0;
  #else
  input_states = (!digitalRead(BTN_START_PIN)) ? BUTTON_EXTRA : 0;
  #endif
  input_states |=
    (digitalRead(BTN_LEFT_PIN) ? 0 : BUTTON_LEFT) |
    (digitalRead(BTN_RIGHT_PIN) ? 0 : BUTTON_RIGHT) |
    (digitalRead(BTN_UP_PIN) ? 0 : BUTTON_UP) |
    (digitalRead(BTN_DOWN_PIN) ? 0 : BUTTON_DOWN) |
    (digitalRead(BTN_FIRE_PIN) ? 0 : BUTTON_FIRE);
#endif


  unsigned char startAndCoinState = 0;
#ifdef BTN_COIN_PIN
  // there is a coin pin -> coin and start work normal
  startAndCoinState = (digitalRead(BTN_START_PIN) ? 0 : BUTTON_START) |
    (digitalRead(BTN_COIN_PIN) ? 0 : BUTTON_COIN);
#else
  #ifdef USE_I2C_INPUT
    // I2C provides START (Pin 5) and COIN (Pin 6) directly via input_states.
    // BT provides COIN via SELECT and START via Options, also in input_states.
    // Both are already OR'd into input_states above, no extra logic needed.
    startAndCoinState = 0;
  #else
    // No I2C, no COIN pin: virtual coin from EXTRA (BT Options or GPIO)
    switch(virtual_coin_state)  {
      case 0:  // idle state
        if(input_states & BUTTON_EXTRA) {
          virtual_coin_state = 1;   // virtual coin pressed
          virtual_coin_timer = millis();
          Serial.println("COIN: Inserting credit");
        }
        break;
      case 1:  // coin signal being sent
        // Send COIN for 200ms
        if(millis() - virtual_coin_timer > 200) {
          virtual_coin_state = 2;   // coin signal ended
          virtual_coin_timer = millis();
        }
        break;
      case 2:  // waiting for button release
        if(!(input_states & BUTTON_EXTRA))
          virtual_coin_state = 0;   // button released, return to idle
        break;
    }
    startAndCoinState = (virtual_coin_state == 1) ? BUTTON_COIN : 0;
  #endif

  // Debug: show when coin/start is being sent
  static unsigned char lastCoinState = 0;
  if (startAndCoinState != lastCoinState) {
    Serial.printf("COIN/START: coinState=0x%02X\n", startAndCoinState);
    lastCoinState = startAndCoinState;
  }
#endif

  // volume control (solo Pin 6 COIN + UP/DOWN)
  if ((input_states & BUTTON_COIN) && _volume_callback) {
    _volume_callback(input_states & BUTTON_UP, input_states & BUTTON_DOWN);
    if ((input_states & BUTTON_UP) | (input_states & BUTTON_DOWN)) {
      reset_timer = 0;

      // Mostra barra volume sull'OLED
      extern void show_volume_bar();
      show_volume_bar();
    }
  }


#ifndef SINGLE_MACHINE
  bool buttonExtraRisingEdge = (input_states & BUTTON_EXTRA) && !(input_states_last & BUTTON_EXTRA);
  bool buttonUpRisingEdge = (input_states & BUTTON_UP) && !(input_states_last & BUTTON_UP);
  bool buttonDownRisingEdge = (input_states & BUTTON_DOWN) && !(input_states_last & BUTTON_DOWN);

  // joystick up/down (menu) or extra disables attract mode
  if (buttonUpRisingEdge | buttonDownRisingEdge | buttonExtraRisingEdge ) {
    if (_doAttractReset_callback)
      _doAttractReset_callback();
  }

  // reset control - richiede START+EXTRA insieme (Pin 5 o BT START), B/Circle (solo EXTRA) non resetta
  if((input_states & BUTTON_EXTRA) && (input_states & BUTTON_START)) {
    if(!reset_timer)
      reset_timer = millis();

    // reset if held for more than 3 seconds
    if(millis() - reset_timer > 3000) {
      if (_doReset_callback)
        _doReset_callback();
    }
  }
  else
    reset_timer = 0;

  input_states_last = input_states;
#endif

  // Coin insert animation flag on BUTTON_COIN rising edge
  unsigned char finalState = input_states | startAndCoinState;
  static unsigned char prevCoinBit = 0;
  if ((finalState & BUTTON_COIN) && !prevCoinBit) {
    extern volatile bool coinAnimPending;
    coinAnimPending = true;
  }
  prevCoinBit = finalState & BUTTON_COIN;

  // Turbo fire: when FIRE is held, toggle it on/off rapidly
  // so games that check rising edge see repeated shots
  if (finalState & BUTTON_FIRE) {
    static unsigned long lastFireToggle = 0;
    static bool firePhase = true;
    unsigned long now = millis();
    if (now - lastFireToggle >= 50) {  // 50ms = ~10 shots/sec per cycle
      firePhase = !firePhase;
      lastFireToggle = now;
    }
    if (!firePhase) {
      finalState &= ~BUTTON_FIRE;  // Remove FIRE during off phase
    }
  }

  // Debug: show final returned state
  static unsigned char lastFinalState = 0;
  if (finalState != lastFinalState) {
    Serial.printf("RETURN: 0x%02X (input=0x%02X coin/start=0x%02X)\n", finalState, input_states, startAndCoinState);
    lastFinalState = finalState;
  }

  return finalState;
}

// Lettura diretta dei tasti BT + I2C senza side-effects (no coin/volume/reset logic)
unsigned char Input::buttons_get_raw(void) {
  unsigned char states = 0;
#ifdef BLUETOOTH_GAMEPAD
  states |= bt_input_states;
#endif
#ifdef USE_I2C_INPUT
  states |= i2c_input_states;
#endif
  return states;
}
