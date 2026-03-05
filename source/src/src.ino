/*
 * Galagino - Galaga arcade for ESP32 (c) 2025 speckhoiler: https://github.com/speckhoiler/galagino 
 * This is a port of Till Harbaum's awesome Galaga emulator: https://github.com/harbaum/galagino
 * Published under GPLv3
 *
 * GALAGIN0 V3.0 by Davide Gatti www.survivalhacking.it a porting for Galagino V2.0 hardware https://youtu.be/YmvyNwJLqJM
 *
 * V3.0 - 25/02/2026
 * Conversion from speckhoiler VsCode version to Arduino Ide by Paolo Sambinello
 * Adding a Bluethoot Controller support, by Paolo Sambinello
 * Adding a Bluethooot enabler/disabler and increase lives to 5 by Marco Prunca
 * Adding a COIN animation by Marco Prunca
 *
 * V3.1 - 02/03/2026
 * Added 2 more games, MrDo and BombJack, by Davide Gatti
 *
 * 
 * Version adapted for yellow cheap Aliexpress Display 2.4" : https://s.click.aliexpress.com/e/_DECY96V
 * Add this url in your arduino ide board source preferences: https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json 
 * then add a ESP32 bluepad board
 *
 * select board "ESP 32 DEV module" family:esp32_bluepad32
 * Use huge partition
 * 
 * USAGE:
 * Joystick UP/DOWN to select game
 * Any key to enter in the selected game.
 * COIN key to add coins
 * START key to start game or flight loop in 1945 game
 * FIRE key to of a fire in game the uses fire/action
 * COIN + UP/DOWN to setup volume
 * COIN pressed for 10 second to enable/disable BT
 * START for 5 seconds to exit game and return to selection menu
 *
 * Galagino uses code that is not freely available and thus not included in this repository. 
 * Preparing the firmware thus consists of a few additional steps:
 * 
 * If you do not have Python installed, download it from here. https://www.python.org/downloads/release/python-3130 
 * The ROM ZIP files have to be placed in the romszip directory, together with the ZIP file containing the Z80 emulator.
 * A set of python scripts is then being used to convert and patch the ROM data and emulator code and to include the resulting code into the galagino machines directory. For all games, just use conv__all.bat.
 * The ROM conversion create a whole bunch of additional files in the source directory. Please check the README in the romconv directory for further instructions. Please ensure that the stripts run without errors!
 * 
 * 
 * here you can find the needed companion files:
 * Z80 Emulator: https://fms.komkon.org/EMUL8/Z80-081707.zip
 * Galaga: https://www.google.com/search?q=galaga.zip+arcade+rom
 * Pac Man: https://www.google.com/search?q=pacman.zip+arcade+rom
 * Donkey Kong: https://www.google.com/search?q=dkong.zip+arcade+rom
 * Frogger: https://www.google.com/search?q=frogger.zip+arcade+rom
 * DigDug: https://www.google.com/search?q=digdug.zip+arcade+rom
 * 1942: https://www.google.com/search?q=1942.zip+arcade+rom
 * Lizard Wizard: https://www.google.com/search?q=lizwiz.zip+arcade+rom
 * Eyes: https://www.google.com/search?q=eyes.zip+arcade+rom
 * Mr. TNT: https://www.google.com/search?q=mrtnt.zip+arcade+rom
 * The Glob (filename with "p"): https://www.google.com/search?q=theglobp.zip+arcade+rom
 * Crush Roller: https://www.google.com/search?q=crush.zip+arcade+rom
 * Anteater: https://www.google.com/search?q=anteater.zip+arcade+rom
 * BombJack: https://www.google.com/search?q=bombjack.zip+arcade+rom
 * MrDo: https://www.google.com/search?q=mrdo.zip+arcade+rom
 *
 */

#include <Arduino.h>
#include "esp_task_wdt.h"
#include "config.h"
#include "machines/machineBase.h"
#include "emulation/audio.h"
#include "emulation/video.h"
#include "emulation/input.h"
#include "emulation/menu.h"
#include "emulation/emulation.h"
#include "machines/bombjack/bombjack.h"  
#ifdef LED_PIN
  #include "emulation/led.h"
#endif

#include <EEPROM.h>       // volume persistence in EEPROM
#define EEPROM_VOL_ADDR 1 // EEPROM address 0 = Bluetooth state, address 1 = volume

#include "logo.h"         // add flyer logo support www.survivalhacking.it
#include <Wire.h>         // add flyer logo support www.survivalhacking.it
#include "SSD1306Wire.h"  // add flyer logo support www.survivalhacking.it
#ifdef USE_I2C_INPUT
  #include "PCF8574.h"
#endif

#ifdef ENABLE_PACMAN  
  #include "machines/pacman/pacman.h"
#endif

#ifdef ENABLE_GALAGA
  #include "machines/galaga/galaga.h"
#endif

#ifdef ENABLE_DKONG
  #include "machines/dkong/dkong.h"
#endif

#ifdef ENABLE_FROGGER
  #include "machines/frogger/frogger.h"
#endif

#ifdef ENABLE_DIGDUG
  #include "machines/digdug/digdug.h"
#endif

#ifdef ENABLE_1942
  #include "machines/1942/1942.h"
#endif

#ifdef ENABLE_EYES
  #include "machines/eyes/eyes.h"
#endif

#ifdef ENABLE_MRTNT
  #include "machines/mrtnt/mrtnt.h"
#endif

#ifdef ENABLE_LIZWIZ
  #include "machines/lizwiz/lizwiz.h"
#endif

#ifdef ENABLE_THEGLOB
  #include "machines/theglob/theglob.h"
#endif

#ifdef ENABLE_CRUSH 
  #include "machines/crush/crush.h"
#endif

#ifdef ENABLE_ANTEATER 
  #include "machines/anteater/anteater.h"
#endif

#ifdef ENABLE_MRDO
  #include "machines/mrdo/mrdo.h"
#endif

#ifdef ENABLE_BOMBJACK
  #include "machines/bombjack/bombjack.h"
#endif

class bombjack;  // Forward declaration

// change machine order is possible here...
machineBase *machines[] = {
#ifdef ENABLE_PACMAN  
  new pacman(),
#endif
#ifdef ENABLE_GALAGA  
  new galaga(), 
#endif  
#ifdef ENABLE_DIGDUG  
  new digdug(), 
#endif  
#ifdef ENABLE_FROGGER  
  new frogger(), 
#endif  
#ifdef ENABLE_DKONG  
  new dkong(), 
#endif  
#ifdef ENABLE_1942  
  new _1942(), 
#endif  
#ifdef ENABLE_LIZWIZ  
  new lizwiz(),
#endif  
#ifdef ENABLE_EYES  
  new eyes(), 
#endif  
#ifdef ENABLE_MRTNT  
  new mrtnt(), 
#endif  
#ifdef ENABLE_THEGLOB 
  new theglob(),
#endif
#ifdef ENABLE_CRUSH 
  new crush(),
#endif
#ifdef ENABLE_ANTEATER 
  new anteater(),
#endif
#ifdef ENABLE_MRDO 
  new mrdo(),
#endif
#ifdef ENABLE_BOMJACK 
  new bombjack()
#endif
};

signed char machinesCount = (signed char)(sizeof(machines) / sizeof(unsigned short*));

machineBase *currentMachine;

// the hardware supports 64 sprites
struct sprite_S *sprite_buffer;

// buffer space for one row of 28 characters
unsigned short *frame_buffer;

// RAM
unsigned char *memory;

Audio audio = Audio();
Video video = Video();
Input input = Input();
Menu menu = Menu();
#ifdef LED_PIN
  Led led = Led();
#endif
SSD1306Wire flyer(0x3c, 1, 3); // flyer logo support www.survivalhacking.it
#ifdef USE_I2C_INPUT
PCF8574 pcf20(0x3F);  // 0x3F per PCF8574A, 0x27 per PCF8574 www.survivalhacking.it
// PCF8574 pcf20(0x27); // 0x27 is for PCF8574 di MARCO 
#endif
unsigned long volumeShowTime = 0;
unsigned char savedMenu = 1;

void updateAudioVideo(void);
void renderRow(short row, bool isMenu);
void onDoAttractReset();
void onVolumeUpDown(bool up, bool down);
void onDoReset();
bool doReset = false;
volatile bool coinAnimPending = false;

void setup() {
  Serial.begin(230400);
  Serial.println("Galagino");

  // Disable task watchdog for core 0 - emulation is CPU intensive
  disableCore0WDT();
  disableCore1WDT();

  Serial.print("ESP-IDF ");
  Serial.println(ESP_IDF_VERSION, HEX);
  #ifdef USE_I2C_INPUT
    pcf20.begin(1, 3, 255);  // SDA=1, SCL=3, interrupt pin non usato
  #endif

  // flyer logo support www.survivalhacking.it
  flyer.init();  
  flyer.flipScreenVertically();
  flyer.clear();
  flyer.setTextAlignment(TEXT_ALIGN_LEFT);
  flyer.setFont(ArialMT_Plain_24);
  flyer.drawString(0, 0, "GALAGINO");
  flyer.drawString(20, 25, "V3.1 BY");
  flyer.display();
  delay(1000);
  flyer.clear();
  flyer.drawString(15, 0, "DAVIDE");
  flyer.drawString(20, 25, "GATTI");
  flyer.display();
  delay(1000);

#ifdef USE_I2C_INPUT
  Serial.println("Initializing I2C input (PCF8574)...");
  pcf20.begin(1, 3, 255);  // SDA=1, SCL=3, no interrupt
  Serial.println("I2C input ready!");
#endif

#ifdef WORKAROUND_I2S_APLL_PROBLEM
  Serial.println("I2S APLL workaround active");
#endif
  // this should not be needed as the CPU runs by default on 240Mht nowadays
  setCpuFrequencyMhz(240);

  Serial.print("Free heap: "); Serial.println(ESP.getFreeHeap());
  Serial.print("Main core: "); Serial.println(xPortGetCoreID());
  Serial.print("Main priority: "); Serial.println(uxTaskPriorityGet(NULL));  

  // allocate memory for a single tile/character row
  frame_buffer = (unsigned short*)malloc(224 * 8 * 2);
  sprite_buffer = (sprite_S*)malloc(128 * sizeof(sprite_S));
  memory = (uint8_t *)malloc(RAMSIZE);
  currentMachine = machines[0];
  
  for (int i = 0; i < machinesCount; i++)
    machines[i]->init(&input, frame_buffer, sprite_buffer, memory);

  audio.init();
  audio.start(currentMachine);

  input.init();

  // Load saved volume from EEPROM (EEPROM.begin(4) already called in input.init())
  uint8_t savedVol = EEPROM.read(EEPROM_VOL_ADDR);
  if (savedVol >= 1 && savedVol <= 30) {
    audio.volumeSetting = savedVol;
    Serial.printf("Volume loaded from EEPROM: %d\n", savedVol);
  } else {
    Serial.println("No saved volume found, using default");
  }

  input.onVolumeUpDown(onVolumeUpDown);
  input.onDoReset(onDoReset);
  input.onDoAttractReset(onDoAttractReset);

  menu.init(&input, machines, machinesCount, frame_buffer);
#ifdef LED_PIN
  led.init();
#endif

#ifdef SINGLE_MACHINE
  // start machine [0]
  emulation_start();
#endif

  video.begin();
  Serial.print("Free heap: "); Serial.println(ESP.getFreeHeap());
}

void loop(void) {
  // Update Bluetooth controller from main loop (required for proper operation)
  input.updateBluetooth();

  // run video in main task. This will send signals to the emulation task in the background to synchronize video
  updateAudioVideo();

  // Update Bluetooth again at end of loop for more responsive input
  input.updateBluetooth(); 

  // Coin insert animation - solo durante il gioco, non nel menu
  if (coinAnimPending) {
    coinAnimPending = false;
    if (!menu.machineIndexIsMenu()) {
      show_coin_insert_animation();
    }
  }

  // Timeout volume/BT status - ripristina flyer dopo 2 secondi
  if (volumeShowTime > 0 && (millis() - volumeShowTime > 2000)) {
    show_flyer(savedMenu);
    volumeShowTime = 0;
  }

#ifdef LED_PIN
  led.update(machines, menu.machineIndexPreselection(), menu.machineIndexSelected());
#endif
}

void updateAudioVideo(void) {
  bool isMenu = false;
  uint32_t t0 = micros();

#ifdef SINGLE_MACHINE
  currentMachine->prepare_frame();
#else
  isMenu = menu.machineIndexIsMenu();
  if(isMenu) {
    menu.handle();
    if (volumeShowTime == 0) {  // non sovrascrivere volume/BT status sull'OLED
      show_flyer(menu.machineIndexPreselection() + 1);  // flyer logo support www.survivalhacking.it
    }
  }
  else {
    if (menu.startMachine()) {
      currentMachine = machines[menu.machineIndexSelected()];
      audio.start(currentMachine);
      if (currentMachine->videoFlipY()) {
        video.flipVertical(1);
      }
      // start new machine      
      emulation_start();
    }
    currentMachine->prepare_frame();
  }



  if (doReset || menu.attract_gameTimeout()) {
    if (currentMachine->videoFlipY())
      video.flipVertical(0);
    // stop current machine
    emulation_stop();
    menu.show_menu();
    doReset = false;
  }
#endif

#ifndef VIDEO_HALF_RATE
  // render and transmit screen at once as the display running at 80Mhz can update at full 60 hz game frame
  for(int c = 0; c < 36; c += 6) {
    for (int i = 0; i < 6; i++) {
      renderRow(c + i, isMenu); video.write(frame_buffer, 224 * 8);
    }

    // audio is updated 6 times per 60 Hz frame
    audio.transmit();

    // Poll Bluetooth during rendering for responsive input
    input.updateBluetooth();
  } 
 
  // one screen at 60 Hz is 16.6ms
  unsigned long t1 = (micros() - t0) / 1000;  // calculate time in milliseconds
  if(t1 < 16) 
    vTaskDelay(16 - t1);
  else
    vTaskDelay(1);    // at least 1 ms delay to prevent watchdog timeout

  // physical refresh is 60Hz. So send vblank trigger once a frame
  emulation_notifyGive();
#else
  // render and transmit screen in two halfs as the display running at 40Mhz can only update every second 60 hz game frame
  for(int half = 0; half < 2; half++) {
    for(int c = 18 * half; c < 18 * (half + 1); c += 3) {
      renderRow(c + 0, isMenu); video.write(frame_buffer, 224 * 8);
      renderRow(c + 1, isMenu); video.write(frame_buffer, 224 * 8);
      renderRow(c + 2, isMenu); video.write(frame_buffer, 224 * 8);

      // audio is refilled 6 times per screen update. The screen is updated
      // every second frame. So audio is refilled 12 times per 30 Hz frame.
      // Audio registers are udated by CPU3 two times per 30hz frame.
      audio.transmit();

      // Poll Bluetooth during rendering for responsive input
      input.updateBluetooth();
    } 
 
    // one screen at 60 Hz is 16.6ms
    unsigned long t1 = (micros() - t0) / 1000;  // calculate time in milliseconds
    // printf("uspf %d\n", t1);
    if(t1 < (half ? 33 : 16))
      vTaskDelay((half ? 33 : 16) - t1);
    else if(half)
      vTaskDelay(1);    // at least 1 ms delay to prevent watchdog timeout

    // physical refresh is 30Hz. So send vblank trigger twice a frame to the emulation. This will make the game run with 60hz speed
    emulation_notifyGive();
  }
#endif
}

// render one of 36 tile rows (8 x 224 pixel lines)
void renderRow(short row, bool isMenu) {
#ifndef SINGLE_MACHINE 
  if(isMenu) {
    menu.render_row(row);
  } 
  else
#endif  
  {
    memset(frame_buffer, 0, 2 * 224 * 8);
    currentMachine->render_row(row);
  }
}

void onVolumeUpDown(bool up, bool down) {
  short oldVol = audio.volumeSetting;
  audio.volumeUpDown(up, down);
  if (audio.volumeSetting != oldVol) {
    EEPROM.write(EEPROM_VOL_ADDR, (uint8_t)audio.volumeSetting);
    EEPROM.commit();
    Serial.printf("Volume saved to EEPROM: %d\n", audio.volumeSetting);
  }
}

void onDoAttractReset() {
  menu.attract_resetTimer();
}

void onDoReset() {
  if(!menu.machineIndexIsMenu()) {
    doReset = true;    
  }
}

void show_flyer(unsigned char menu_sel) {  // flyer logo support www.survivalhacking.it
  if (menu_sel == 2) { // galaga
    flyer.clear();
    flyer.drawXbm(0, 0, Galaga_Logo_width, Galaga_Logo_height, Galaga_Logo_bits);
    flyer.display();
  } else if (menu_sel == 1) { // pacman
    flyer.clear();
    flyer.drawXbm(0, 0, Pacman_Logo_width, Pacman_Logo_height, Pacman_Logo_bits);
    flyer.display();
  } else if (menu_sel == 3) { // DigDug
    flyer.clear();
    flyer.drawXbm(0, 0, Digdug_Logo_width, Digdug_Logo_height, Digdug_Logo_bits);
    flyer.display();
  } else if (menu_sel == 6) { // 1942
    flyer.clear();
    flyer.drawXbm(0, 0, A1942_Logo_width, A1942_Logo_height, A1942_Logo_bits);
    flyer.display();
  } else if (menu_sel == 4) { // frogger
    flyer.clear();
    flyer.drawXbm(0, 0, Frogger_Logo_width, Frogger_Logo_height, Frogger_Logo_bits);
    flyer.display();
  } else if (menu_sel == 5) { // Sonkey Kong
    flyer.clear();
    flyer.drawXbm(0, 0, Dkong_Logo_width, Dkong_Logo_height, Dkong_Logo_bits);
    flyer.display();
  } else if (menu_sel == 7) { // Lizard wizard
    flyer.clear();
    flyer.drawXbm(0, 0, Lizard_Logo_width, Lizard_Logo_height, Lizard_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 8) { // Eyes
    flyer.clear();
    flyer.drawXbm(0, 0, Eyes_Logo_width, Eyes_Logo_height, Eyes_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 9) { // Mr TNT
    flyer.clear();
    flyer.drawXbm(0, 0, Tnt_Logo_width, Tnt_Logo_height, Tnt_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 10) { // The glob
    flyer.clear();
    flyer.drawXbm(0, 0, Glob_Logo_width, Glob_Logo_height, Glob_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 11) { // crush roller
    flyer.clear();
    flyer.drawXbm(0, 0, Crush_Logo_width, Crush_Logo_height, Crush_Logo_bits);  
    flyer.display();
  } else if (menu_sel == 12) { // Ant
    flyer.clear();
    flyer.drawXbm(0, 0, Ant_Logo_width, Ant_Logo_height, Ant_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 13) { // mrdo
    flyer.clear();
    flyer.drawXbm(0, 0, MrDo_Logo_width, MrDo_Logo_height, MrDo_Logo_bits); 
    flyer.display();
  } else if (menu_sel == 14) { // Bombjack
    flyer.clear();
    flyer.drawXbm(0, 0, BombJack_Logo_width, BombJack_Logo_height, BombJack_Logo_bits); 
    flyer.display();
  }
}

void show_bt_status(bool enabled) {
  savedMenu = menu.machineIndexPreselection() + 1;
  flyer.clear();
  flyer.setFont(ArialMT_Plain_24);
  if (enabled) {
    flyer.drawString(10, 20, "BLU ON");
  } else {
    flyer.drawString(10, 20, "BLU OFF");
  }
  flyer.display();
  volumeShowTime = millis();
}

void show_volume_bar() {
  int currentVol = audio.volumeSetting;
  int maxVol = 30;
  
  // SALVA quale menu era visualizzato
  savedMenu = menu.machineIndexPreselection() + 1;
  
  // PULISCI TUTTO lo schermo
  flyer.clear();
  flyer.setColor(WHITE);

  // Testo GRANDE sopra la barra (font 24px leggibile!)
  flyer.setFont(ArialMT_Plain_24);
  flyer.drawString(5, 5, "VOL");
  
  // Valore numerico GRANDE a destra
  char volText[8];
  sprintf(volText, "%d", maxVol - currentVol + 1);
  flyer.drawString(75, 5, volText);

  // Barra GROSSA al centro (disegnata PRIMA!)
  int barY = 35;
  int barHeight = 18;
  
  // Calcola riempimento
  int fillWidth = (104 * (maxVol - currentVol + 1)) / maxVol;
  if (fillWidth > 104) fillWidth = 104;
  if (fillWidth < 0) fillWidth = 0;
  
  // Riempi barra
  flyer.fillRect(12, barY + 2, fillWidth, barHeight - 4);
  
  // Cornice barra
  flyer.drawRect(10, barY, 108, barHeight+1);

  flyer.display();
  volumeShowTime = millis();
}

// ── Coin insert animation for 128x64 OLED ──
// Animazione: mano con moneta si avvicina alla fessura e la inserisce
void show_coin_insert_animation() {
  const int CY = 32;    // Centro verticale display 128x64
  const int CR = 10;     // Raggio moneta

  // Posizioni X della moneta per i 6 frame di movimento (da destra a sinistra)
  const int coinX[] = {100, 83, 66, 49, 35, 22};
  const int MOVE_FRAMES = 6;
  const int TOTAL_FRAMES = 8;  // 6 movimento + 1 inserimento + 1 "CREDIT!"

  savedMenu = menu.machineIndexPreselection() + 1;

  for (int f = 0; f < TOTAL_FRAMES; f++) {
    flyer.clear();

    // ── PANNELLO SLOT (sempre visibile, lato sinistro) ──
    flyer.drawRect(2, 6, 28, 52);       // Bordo esterno
    flyer.drawRect(4, 8, 24, 48);       // Bordo interno
    // Fessura verticale
    flyer.fillRect(12, 20, 8, 24);      // Sfondo fessura (bianco)
    flyer.setColor(BLACK);
    flyer.fillRect(14, 22, 4, 20);      // Apertura fessura (nero)
    flyer.setColor(WHITE);

    if (f < MOVE_FRAMES) {
      int cx = coinX[f];

      // ── MANO (disegnata per prima, dietro la moneta) ──
      // Palmo
      flyer.fillCircle(cx + 18, CY, 12);
      flyer.fillRect(cx + 18, CY - 12, 22, 24);
      // Dito indice (sopra)
      flyer.fillRect(cx + 5, CY - 16, 33, 7);
      flyer.fillCircle(cx + 5, CY - 13, 3);
      // Pollice (sotto)
      flyer.fillRect(cx + 5, CY + 10, 30, 7);
      flyer.fillCircle(cx + 5, CY + 13, 3);

      // Ritaglio spazio attorno alla moneta (crea la presa)
      flyer.setColor(BLACK);
      flyer.fillCircle(cx, CY, CR + 3);
      flyer.setColor(WHITE);

      // ── MONETA ──
      flyer.fillCircle(cx, CY, CR);          // Disco pieno
      flyer.setColor(BLACK);
      flyer.fillCircle(cx, CY, CR - 3);      // Interno nero
      flyer.setColor(WHITE);
      flyer.drawCircle(cx, CY, CR - 3);      // Anello interno
      // Simbolo $
      flyer.drawLine(cx, CY - 5, cx, CY + 5);
      flyer.drawLine(cx - 3, CY - 2, cx + 1, CY - 2);
      flyer.drawLine(cx - 1, CY + 2, cx + 3, CY + 2);

    } else if (f == MOVE_FRAMES) {
      // ── MONETA CHE ENTRA NELLA FESSURA ──
      flyer.fillCircle(16, CY, 6);
      flyer.setColor(BLACK);
      flyer.fillCircle(16, CY, 3);
      flyer.setColor(WHITE);
      flyer.drawCircle(16, CY, 3);
      // Scintille alla fessura
      flyer.drawLine(8, CY - 10, 24, CY - 10);
      flyer.drawLine(8, CY + 10, 24, CY + 10);
      flyer.drawLine(16, CY - 14, 16, CY - 10);
      flyer.drawLine(16, CY + 10, 16, CY + 14);
      // Mano che si ritira
      flyer.fillRect(50, CY - 14, 22, 7);
      flyer.fillRect(50, CY + 8, 20, 7);
      flyer.fillCircle(65, CY, 10);

    } else {
      // ── TESTO "CREDIT!" ──
      // Scintilla al centro della fessura
      flyer.setPixel(16, CY);
      flyer.drawCircle(16, CY, 4);
      flyer.drawCircle(16, CY, 8);
      // Testo grande
      flyer.setFont(ArialMT_Plain_24);
      flyer.setTextAlignment(TEXT_ALIGN_CENTER);
      flyer.drawString(80, 20, "CREDIT!");
      flyer.setTextAlignment(TEXT_ALIGN_LEFT);
    }

    flyer.display();
    delay(f == TOTAL_FRAMES - 1 ? 500 : 100);
  }

  // Usa il meccanismo di timeout per ripristinare il flyer dopo 2 secondi
  volumeShowTime = millis();
}

