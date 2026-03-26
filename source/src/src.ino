/*
 * Galagino - Galaga arcade for ESP32 (c) 2025 speckhoiler: https://github.com/speckhoiler/galagino 
 * This is a port of Till Harbaum's awesome Galaga emulator: https://github.com/harbaum/galagino
 * Published under GPLv3
 *
 * GALAGIN0 V3.0 by Davide Gatti www.survivalhacking.it a porting for Galagino V2.0 hardware https://youtu.be/YmvyNwJLqJM
 *
 * V3.0 1/2/2026  
 * Conversion from speckhoiler VsCode version to Arduino Ide by Paolo Sambinello
 * Adding a Bluethoot Controller support, by Paolo Sambinello
 * Adding a Bluethooot enabler/disabler and increase lives to 5 by Marco Prunca
 * Adding a COIN animation by Marco Prunca
 *
 * V3.1 11/3/2026
 * Added Lady Bug by Paolo Sambinello and Davide Gatti 
 * Added Gyrruss by Paolo Sambinello and Marco Prunca (Playable but with reducete audio features AY3-8910 #3 emulated instead of #5)
 * Added MsPacman by Marco Prunca (work 100%)
 * Added Pengo by Spek Hoiler and Paolo Sambinello and Marco Prunca
 * Added Time Pilot by Marco Prunca
 * Added Bagman By Spek Hoiler
 * Added SpaceInvaders by Marco Prunca
 * Detect automaticallu PCF8574 / PCF8574A
 * Added Name list at boot
 * Fixed conditional compile to allow remove/add games without problems
 *
 * V3.2 13/4/2026 
 * Added QR code by Paolo Sambinello
 * Added Configuration Menu by Paolo Sambinello
 * Fixed insert coin animation by Marco Prunca
 * Added Galaxian game by Marco Prunca (minor audio problems)
 * Fixed Time Pilot game by Marco Prunca (minor glitch on clouds graphichs)
 * Added Tutankham game by Marco Prunca (not playable at full speed)
 *
 * Version adapted for yellow cheap Aliexpress Display 2.4" : https://s.click.aliexpress.com/e/_DECY96V
 * Add this url in your arduino ide board source preferences: https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json 
 * then add a ESP32 bluepad board
 *
 * select board "ESP 32 DEV module" family:esp32_bluepad32
 * Use huge partition and enable PSRAM
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
 * [Z80 Emulator] https://fms.komkon.org/EMUL8/Z80-081707.zip
 * [Galaga (Namco Rev. B ROM)](https://www.google.com/search?q=galaga.zip+arcade+rom)
 * [Pac-Man (Midway)](https://www.google.com/search?q=pacman.zip+arcade+rom)
 * [Donkey Kong (US set 1)](https://www.google.com/search?q=dkong.zip+arcade+rom)
 * [Frogger](https://www.google.com/search?q=frogger.zip+arcade+rom)
 * [Digdug](https://www.google.com/search?q=digdug.zip+arcade+rom)
 * [1942](https://www.google.com/search?q=1942.zip+arcade+rom)
 * [Lizard Wizard](https://www.google.com/search?q=lizwiz.zip+arcade+rom)
 * [Eyes](https://www.google.com/search?q=eyes.zip+arcade+rom)
 * [Mr. TNT](https://www.google.com/search?q=mrtnt.zip+arcade+rom)
 * [The Glob](https://www.google.com/search?q=theglobp.zip+arcade+rom) (Important: filename with "p")
 * [Crush Roller](https://www.google.com/search?q=crush.zip+arcade+rom)
 * [Anteater](https://www.google.com/search?q=anteater.zip+arcade+rom)
 * [Bombjack](https://www.google.com/search?q=bombjack.zip+arcade+rom)
 * [Mr. Do!](https://www.google.com/search?q=mrdo.zip+arcade+rom)
 * [Bagman](https://www.google.com/search?q="bagmanm2.zip"+download) (Important: filename with "m2")
 * [Pengo](https://www.google.com/search?q=pengo2u.zip+arcade+rom) (Important: filename with "2u")
 * [Lady Bug](https://www.google.com/search?q=ladybug.zip+arcade+rom)
 * [Gyrrus](https://www.google.com/search?q=gyruss.zip+arcade+rom)
 * [Time Pilot](https://www.google.com/search?q=timeplt.zip+arcade+rom)
 * [Galaxian](https://www.google.com/search?q=glaxian.zip+arcade+rom)
 * [MsPacman](https://www.google.com/search?q=mspacman.zip+arcade+rom)
 * [SpaceInvaders](https://www.google.com/search?q=invaders.zip+arcade+rom)
 * [Thuthankam](https://www.google.com/search?q=tutankhm.zip+arcade+rom) 
 * 
 * 
 *  MAPPA COMPLETA COMANDI JOYSTICK

  CONTROLLER BLUETOOTH (PS4/PS5/Xbox/Switch)

  ┌──────────────────────┬─────────────────────────────┬────────────────────────────────────────┐
  │   Tasto Controller   │       Segnale interno       │                Funzione                │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ D-pad / Analog stick │ UP/DOWN/LEFT/RIGHT          │ Movimento nel gioco + navigazione menu │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ A / Cross (X)        │ BUTTON_FIRE                 │ Fuoco / Sparo                          │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ X / Square           │ BUTTON_FIRE                 │ Fuoco alternativo                      │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ Y / Triangle         │ BUTTON_FIRE                 │ Fuoco alternativo                      │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ B / Circle (O)       │ BUTTON_EXTRA                │ Loop/barrel roll in 1942               │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ L1 / R1 / L2 / R2    │ BUTTON_FIRE                 │ Fuoco (shoulder/trigger)               │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ SELECT / Share       │ BUTTON_COIN                 │ Inserisce credito                      │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ OPTIONS / Start      │ BUTTON_START + BUTTON_EXTRA │ Avvia gioco / seleziona nel menu       │
  ├──────────────────────┼─────────────────────────────┼────────────────────────────────────────┤
  │ L1 + R1 + OPTIONS    │ doReset immediato           │ Uscita immediata al menu               │
  └──────────────────────┴─────────────────────────────┴────────────────────────────────────────┘

  JOYSTICK I2C (PCF8574)

  ┌─────────────┬─────────────────────────────┬──────────────────────────────────┐
  │ Pin PCF8574 │       Segnale interno       │             Funzione             │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 0       │ BUTTON_LEFT                 │ Sinistra                         │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 1       │ BUTTON_RIGHT                │ Destra                           │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 2       │ BUTTON_UP                   │ Su                               │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 3       │ BUTTON_DOWN                 │ Giu                              │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 4       │ BUTTON_FIRE                 │ Fuoco                            │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 5       │ BUTTON_START + BUTTON_EXTRA │ Avvia gioco / seleziona nel menu │
  ├─────────────┼─────────────────────────────┼──────────────────────────────────┤
  │ Pin 6       │ BUTTON_COIN                 │ Inserisce credito                │
  └─────────────┴─────────────────────────────┴──────────────────────────────────┘

  COMBINAZIONI E TEMPI

  ┌────────────────────────────┬──────────────────┬───────────────────────────────┬──────────┐
  │           Combo            │      Tempo       │           Funzione            │ Sorgente │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ COIN (Pin6/SELECT)         │ tap              │ Inserisce 1 credito           │ I2C / BT │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ COIN + UP                  │ tenere           │ Volume su                     │ I2C / BT │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ COIN + DOWN                │ tenere           │ Volume giu                    │ I2C / BT │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ COIN (Pin6)                │ tenere 5 secondi │ Abilita/Disabilita Bluetooth  │ Solo I2C │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ START+EXTRA (Pin5/OPTIONS) │ tenere 3 secondi │ Esce dal gioco, torna al menu │ I2C / BT │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ L1 + R1 + OPTIONS          │ tap              │ Uscita immediata al menu      │ Solo BT  │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ UP / DOWN                  │ tap (nel menu)   │ Naviga tra i giochi           │ I2C / BT │
  ├────────────────────────────┼──────────────────┼───────────────────────────────┼──────────┤
  │ FIRE / START / EXTRA       │ tap (nel menu)   │ Seleziona gioco               │ I2C / BT │
  └────────────────────────────┴──────────────────┴───────────────────────────────┴──────────┘

  DURANTE IL BOOT SCREEN (QR code)

  ┌───────────┬─────────────────────────────────────┐
  │   Tasto   │              Funzione               │
  ├───────────┼─────────────────────────────────────┤
  │ FIRE      │ Entra nel menu ROM Settings         │
  ├───────────┼─────────────────────────────────────┤
  │ UP / DOWN │ (letti ma non usati durante scroll) │
  └───────────┴─────────────────────────────────────┘

  NEL MENU ROM SETTINGS

  ┌───────────┬──────────────────────────────────┐
  │   Tasto   │             Funzione             │
  ├───────────┼──────────────────────────────────┤
  │ UP / DOWN │ Naviga tra le ROM                │
  ├───────────┼──────────────────────────────────┤
  │ FIRE      │ Attiva/Disattiva ROM selezionata │
  ├───────────┼──────────────────────────────────┤
  │ START     │ Salva e esce                     │
  └───────────┴──────────────────────────────────┘

  NOTE IMPORTANTI

  - Volume: devi tenere premuto COIN (Pin 6 o SELECT su BT) e poi premere UP o DOWN. Non funziona al contrario (prima UP
   poi COIN).
  - Toggle BT: solo da Pin 6 I2C, tenere 5 secondi. Se durante il long-press muovi UP/DOWN il timer si resetta (per
  evitare toggle accidentale durante regolazione volume). Dopo 1 secondo il COIN viene soppresso per non inserire
  crediti multipli.
  - Exit gioco: START+EXTRA per 3 secondi. Su BT OPTIONS li manda entrambi. Su I2C Pin 5 li manda entrambi. Oppure
  L1+R1+OPTIONS su BT per uscita immediata.
  - Attract mode: dopo 20 secondi di inattivita nel menu, parte la demo automatica. Premere UP/DOWN/EXTRA resetta il
  timer. Dopo 5 minuti di demo, torna al menu.

 */

#include <Arduino.h>
#include "esp_task_wdt.h"
#include "esp_system.h"
#include "config.h"
#include "machines/machineBase.h"
#include "emulation/audio.h"
#include "emulation/video.h"
#include "emulation/input.h"
#include "emulation/menu.h"
#include "emulation/emulation.h"

#include "cpus/i8048/i8048.c"
#include "cpus/m6809/m6809.c"
#include "cpus/z80/Z80.c"

#include "emulation/video.h"


#ifdef LED_PIN
  #include "emulation/led.h"
#endif
#ifdef ENABLE_1942
  #include "machines/1942/1942.h"
  #include "machines/1942/1942.cpp"
#endif
#ifdef ENABLE_ANTEATER
  #include "machines/anteater/anteater.h"
  #include "machines/anteater/anteater.cpp"
#endif
#ifdef ENABLE_BOMBJACK
  #include "machines/bombjack/bombjack.h"
  #include "machines/bombjack/bombjack.cpp"
#endif
#ifdef ENABLE_CRUSH
  #include "machines/crush/crush.h"
  #include "machines/crush/crush.cpp"
#endif
#ifdef ENABLE_DIGDUG
  #include "machines/digdug/digdug.h"
  #include "machines/digdug/digdug.cpp"
#endif
#ifdef ENABLE_DKONG
  #include "machines/dkong/dkong.h"
  #include "machines/dkong/dkong.cpp"
#endif
#ifdef ENABLE_EYES
  #include "machines/eyes/eyes.h"
  #include "machines/eyes/eyes.cpp"
#endif
#ifdef ENABLE_FROGGER
  #include "machines/frogger/frogger.h"
  #include "machines/frogger/frogger.cpp"
#endif
#ifdef ENABLE_GALAGA
  #include "machines/galaga/galaga.h"
  #include "machines/galaga/galaga.cpp"
#endif
#ifdef ENABLE_GALAXIAN
  #include "machines/galaxian/galaxian.h"
  #include "machines/galaxian/galaxian.cpp"
#endif
#ifdef ENABLE_GYRUSS
  #include "machines/gyruss/gyruss.h"
  #include "machines/gyruss/gyruss.cpp"
#endif
#ifdef ENABLE_LADYBUG
  #include "machines/ladybug/ladybug.h"
  #include "machines/ladybug/ladybug.cpp"
#endif
#ifdef ENABLE_LIZARD
  #include "machines/lizwiz/lizwiz.h"
  #include "machines/lizwiz/lizwiz.cpp"
#endif
#ifdef ENABLE_MRDO
  #include "machines/mrdo/mrdo.h"
  #include "machines/mrdo/mrdo.cpp"
#endif
#ifdef ENABLE_MRTNT
  #include "machines/mrtnt/mrtnt.h"
  #include "machines/mrtnt/mrtnt.cpp"
#endif
#ifdef ENABLE_MSPACMAN
  #include "machines/mspacman/mspacman.h"
  #include "machines/mspacman/mspacman.cpp"
#endif
#ifdef ENABLE_PACMAN
  #include "machines/pacman/pacman.h"
  #include "machines/pacman/pacman.cpp"
#endif
#ifdef ENABLE_PENGO
  #include "machines/pengo/pengo.h"
  #include "machines/pengo/pengo.cpp"
#endif
#ifdef ENABLE_THEGLOB
  #include "machines/theglob/theglob.h"
  #include "machines/theglob/theglob.cpp"
#endif
#ifdef ENABLE_TIMEPLT
  #include "machines/timeplt/timeplt.h"
  #include "machines/timeplt/timeplt.cpp"
#endif
#ifdef ENABLE_BAGMAN 
  #include "machines/bagman/bagman.h"
  #include "machines/bagman/bagman.cpp"
#endif
#ifdef ENABLE_SPACE 
  #include "machines/spaceinvaders/spaceinvaders.h"
  #include "machines/spaceinvaders/spaceinvaders.cpp"
#endif
#ifdef ENABLE_TUTANKHM
  #include "machines/tutankhm/tutankhm.h"
  #include "machines/tutankhm/tutankhm.cpp"
#endif


#include <EEPROM.h>       // volume persistence in EEPROM
#define EEPROM_VOL_ADDR 1 // EEPROM address 0 = Bluetooth state, address 1 = volume

#include "logo.h"         // add flyer logo support www.survivalhacking.it
#include "qr_gen.h"       // QR code generation for boot screen
// qr_logo_bitmap.h removed - QR code is now generated at runtime via qr_gen.h
#include "qr_logo_bitmap.h"  // Pre-generated QR bitmap 220x220 1-bit
#include "tft_font.h"     // 5x7 bitmap font for TFT text rendering
#include <Wire.h>         // add flyer logo support www.survivalhacking.it
#include "SSD1306Wire.h"  // add flyer logo support www.survivalhacking.it
#ifdef USE_I2C_INPUT
  #include "PCF8574.h"
  PCF8574* pcfPtr = nullptr; 
  #define pcf20 (*pcfPtr)
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
#ifdef ENABLE_LIZARD
  new lizwiz(),
#endif  
#ifdef ENABLE_THEGLOB 
  new theglob(),
#endif  
#ifdef ENABLE_MRTNT  
  new mrtnt(),
#endif  
#ifdef ENABLE_EYES  
  new eyes(), 
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
#ifdef ENABLE_BOMBJACK
  new bombjack(),
#endif
#ifdef ENABLE_MSPACMAN
  new mspacman(),
#endif
#ifdef ENABLE_GALAXIAN
  new galaxian(),
#endif
#ifdef ENABLE_LADYBUG
  new ladybug(),
#endif
#ifdef ENABLE_GYRUSS
  new gyruss(),
#endif
#ifdef ENABLE_TIMEPLT
  new timeplt(),
#endif
#ifdef ENABLE_PENGO
  new pengo(),
#endif
#ifdef ENABLE_BAGMAN
  new bagman(),
#endif
#ifdef ENABLE_SPACE
  new spaceinvaders(),
#endif
#ifdef ENABLE_TUTANKHM
  new tutankhm()
#endif
};

signed char machinesCount = (signed char)(sizeof(machines) / sizeof(unsigned short*));

// Machine names for ROM settings menu (must match machines[] order)
const char* const machineNames[] = {
#ifdef ENABLE_PACMAN
  "Pac-Man",
#endif
#ifdef ENABLE_GALAGA
  "Galaga",
#endif
#ifdef ENABLE_DIGDUG
  "Dig Dug",
#endif
#ifdef ENABLE_FROGGER
  "Frogger",
#endif
#ifdef ENABLE_DKONG
  "D. Kong",
#endif
#ifdef ENABLE_1942
  "1942",
#endif
#ifdef ENABLE_LIZARD
  "Liz Wiz",
#endif
#ifdef ENABLE_THEGLOB
  "The Glob",
#endif
#ifdef ENABLE_MRTNT
  "Mr. TNT",
#endif
#ifdef ENABLE_EYES
  "Eyes",
#endif
#ifdef ENABLE_CRUSH
  "Crush",
#endif
#ifdef ENABLE_ANTEATER
  "Anteater",
#endif
#ifdef ENABLE_MRDO
  "Mr. Do!",
#endif
#ifdef ENABLE_BOMBJACK
  "BombJack",
#endif
#ifdef ENABLE_MSPACMAN
  "Ms.Pac",
#endif
#ifdef ENABLE_GALAXIAN
  "Galaxian",
#endif
#ifdef ENABLE_LADYBUG
  "LadyBug",
#endif
#ifdef ENABLE_GYRUSS
  "Gyruss",
#endif
#ifdef ENABLE_TIMEPLT
  "TimePlt",
#endif
#ifdef ENABLE_PENGO
  "Pengo",
#endif
#ifdef ENABLE_BAGMAN
  "Bagman",
#endif
#ifdef ENABLE_SPACE
  "Invaders",
#endif
#ifdef ENABLE_TUTANKHM
  "Tutankham",
#endif
};

// ROM enable/disable stored in EEPROM (address 2..23)
#define EEPROM_ROM_BASE 2
#define MAX_COMPILED_MACHINES 23
bool machineEnabled[MAX_COMPILED_MACHINES];
machineBase *activeMachines[MAX_COMPILED_MACHINES];
signed char activeMachinesCount = 0;

void rebuildActiveMachines() {
  activeMachinesCount = 0;
  for (int i = 0; i < machinesCount; i++) {
    if (machineEnabled[i]) {
      activeMachines[activeMachinesCount++] = machines[i];
    }
  }
}

void loadMachineSettings() {
  for (int i = 0; i < machinesCount; i++) {
    uint8_t val = EEPROM.read(EEPROM_ROM_BASE + i);
    machineEnabled[i] = (val != 0); // 0xFF (uninitialized) or 1 = enabled, 0 = disabled
  }
  rebuildActiveMachines();
}

void saveMachineSettings() {
  for (int i = 0; i < machinesCount; i++) {
    EEPROM.write(EEPROM_ROM_BASE + i, machineEnabled[i] ? 1 : 0);
  }
  EEPROM.commit();
}

machineBase *currentMachine;

unsigned char lastKeys = 0xFF;

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
//#ifdef USE_I2C_INPUT
//PCF8574 pcf20(0x3F);  // 0x3F per PCF8574A, 0x27 per PCF8574 www.survivalhacking.it
////PCF8574 pcf20(0x27); // 0x27 is for PCF8574 di MARCO 
//#endif
unsigned long volumeShowTime = 0;
unsigned char savedMenu = 1;

void updateAudioVideo(void);
void renderRow(short row, bool isMenu);
void onDoAttractReset();
void onVolumeUpDown(bool up, bool down);
void onDoReset();
bool doReset = false;

// ── Non-blocking coin animation state ──
volatile bool coinAnimPending = false;
int coinAnim_frame = -1;                // -1 = idle
unsigned long coinAnim_lastTime = 0;

// Callback: entra nel menu ROM Settings dal menu giochi (COIN+FIRE 3s)
void onRomSettingsFromMenu() {
  showRomSettingsMenu();
  // rebuildActiveMachines() già chiamato dentro showRomSettingsMenu()
  // Aggiorna currentMachine per sicurezza
  currentMachine = (activeMachinesCount > 0) ? activeMachines[0] : machines[0];
  menu.init(&input, activeMachines, activeMachinesCount, frame_buffer);
  menu.show_menu();
  // Piccola pausa per stabilizzare dopo il menu bloccante
  delay(100);
}

void setup() {

  // Disable task watchdog for core 0 - emulation is CPU intensive
  disableCore0WDT();
  disableCore1WDT();

  #ifdef USE_I2C_INPUT
      Wire.begin(1, 3);
  // Scansione per determinare l'indirizzo corretto
  bool pcfFound = false;
  uint8_t address = 0x27;

  Wire.beginTransmission(0x3F);
  if (Wire.endTransmission() == 0) {
    address = 0x3F;
    pcfFound = true;
  } else {
    Wire.beginTransmission(0x27);
    if (Wire.endTransmission() == 0) {
      address = 0x27;
      pcfFound = true;
    }
  }

  if (pcfFound) {
    pcfPtr = new PCF8574(address);
    pcf20.begin(1, 3, 255);
    pcf20.write8(0xFF);
  }
  #endif

  // EEPROM init (larger size for ROM enable/disable settings)
  EEPROM.begin(32);

  // Init displays FIRST so we can show messages during init
  flyer.init();
  flyer.flipScreenVertically();
  video.begin();
  delay(100);

  // Load ROM enable/disable settings from EEPROM
  loadMachineSettings();

  // --- Device detection: need at least BT or PCF to proceed ---
  bool btReady = false;
  bool pcfReady = false;

  // Check PCF8574 (already scanned above)
#ifdef USE_I2C_INPUT
  pcfReady = pcfFound;
#endif

  // Init Bluetooth PRIMA del boot screen, così ha tempo di connettersi
#ifdef BLUETOOTH_GAMEPAD
  // BP32.setup() can hang - watchdog (10s) will reboot if it does
  input.initBluetooth();
  btReady = true;  // If we get here, BP32.setup() didn't hang
#endif

  // Show boot screen on TFT + OLED (polling BT durante lo scroll)
  showBootScreen();

/*
  // --- Show result and decide ---
  // --- Show result and decide ---
  flyer.clear();
  flyer.setColor(WHITE);
  oled_drawStringGFXCenter(flyer, 64, 2, "GALAGINO", &FreeSans12pt7b);

#ifdef BLUETOOTH_GAMEPAD
  oled_drawStringGFXCenter(flyer, 64, 28,
    btReady ? "BT: OK" : "BT: NO", &FreeSans9pt7b);
#endif
#ifdef USE_I2C_INPUT
  oled_drawStringGFXCenter(flyer, 64, 44,
    pcfReady ? "PCF: OK" : "PCF: NO", &FreeSans9pt7b);
#endif
  flyer.display();
*/
  // Check if at least one input device is available
  bool hasInput = btReady || pcfReady;

  // If no BT and no I2C compiled, always proceed (GPIO or single input mode)
#if !defined(BLUETOOTH_GAMEPAD) && !defined(USE_I2C_INPUT)
  hasInput = true;
#endif
  // If only BT compiled (no I2C), BT ready is enough
#if defined(BLUETOOTH_GAMEPAD) && !defined(USE_I2C_INPUT)
  hasInput = btReady;
#endif
  // If only I2C compiled (no BT), PCF ready is enough
#if !defined(BLUETOOTH_GAMEPAD) && defined(USE_I2C_INPUT)
  hasInput = pcfReady;
#endif

/*
  if (!hasInput) {
    // No input device found - show error and reboot
    flyer.clear();
    flyer.setColor(WHITE);
    oled_drawStringGFXCenter(flyer, 64, 2, "NESSUN", &FreeSans12pt7b);
    oled_drawStringGFXCenter(flyer, 64, 26, "CONTROLLER", &FreeSans12pt7b);
    oled_drawStringGFXCenter(flyer, 64, 50, "Riavvio...", &FreeSans9pt7b);
    flyer.display();
    delay(3000);
    ESP.restart();
  }

  delay(1000);  // Show status for 1 second

*/



  flyer.setTextAlignment(TEXT_ALIGN_LEFT);

  // Check se START o FIRE è premuto per entrare nel menu ROM Settings
  // (BT e I2C sono già inizializzati a questo punto)
  {
    unsigned char bootKeys = readBootButtons();
    if (bootKeys & (BUTTON_START | BUTTON_FIRE)) {
      showRomSettingsMenu();
    }
  }

  // this should not be needed as the CPU runs by default on 240Mht nowadays
  setCpuFrequencyMhz(240);

  // allocate memory for a single tile/character row
  frame_buffer = (unsigned short*)malloc(224 * 8 * 2);
  sprite_buffer = (sprite_S*)malloc(128 * sizeof(sprite_S));
  memory = (uint8_t *)malloc(RAMSIZE);

  // Init ALL compiled machines (including disabled ones)
  for (int i = 0; i < machinesCount; i++) {
    machines[i]->init(&input, frame_buffer, sprite_buffer, memory);
    yield();
  }

  // Rebuild active machines list and set current
  rebuildActiveMachines();
  currentMachine = (activeMachinesCount > 0) ? activeMachines[0] : machines[0];

  audio.init();
  audio.start(currentMachine);

  input.init();

  // Load saved volume from EEPROM
  uint8_t savedVol = EEPROM.read(EEPROM_VOL_ADDR);
  if (savedVol >= 1 && savedVol <= 30) {
    audio.volumeSetting = savedVol;
  }

  input.onVolumeUpDown(onVolumeUpDown);
  input.onDoReset(onDoReset);
  input.onDoAttractReset(onDoAttractReset);

  menu.init(&input, activeMachines, activeMachinesCount, frame_buffer);

#ifdef LED_PIN
  led.init();
#endif

#ifdef SINGLE_MACHINE
  // start machine [0]
  emulation_start();
#endif

  // video.begin() already called before showBootScreen()

#ifdef SINGLE_MACHINE
  if (currentMachine->videoFlipY()) {
    video.flipVertical(1);
  } else if (currentMachine->videoFlipX()) {
    video.flipHorizontal(1);
  }
#endif

#ifdef ENABLE_GYRUSS
  if (currentMachine->machineType() == MCH_GYRUSS) {
    video.mirror_x = true;
  }
#endif

}

// Read buttons during boot (Bluetooth + I2C or GPIO)
unsigned char readBootButtons() {
  unsigned char states = 0;

  // Bluetooth input (initialized early via input.initBluetooth())
#ifdef BLUETOOTH_GAMEPAD
  states |= input.getBluetoothButtons();
#endif

  // I2C input
#ifdef USE_I2C_INPUT
  if (pcfPtr == nullptr) return states;  // PCF8574 non trovato
  uint8_t i2c_raw = pcf20.read8();
  if (i2c_raw != 0x00) {  // Skip bus errors (pull-ups make resting state 0xFF)
    uint8_t i2c_pressed = ~i2c_raw;
    if (i2c_pressed & 0x04) states |= BUTTON_UP;
    if (i2c_pressed & 0x08) states |= BUTTON_DOWN;
    if (i2c_pressed & 0x10) states |= BUTTON_FIRE;
    if (i2c_pressed & 0x20) states |= BUTTON_START;
  }
#elif defined(BTN_FIRE_PIN)
  // GPIO buttons
  pinMode(BTN_FIRE_PIN, INPUT_PULLUP);
  if (!digitalRead(BTN_FIRE_PIN)) states |= BUTTON_FIRE;
  #ifdef BTN_UP_PIN
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    if (!digitalRead(BTN_UP_PIN)) states |= BUTTON_UP;
  #endif
  #ifdef BTN_DOWN_PIN
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    if (!digitalRead(BTN_DOWN_PIN)) states |= BUTTON_DOWN;
  #endif
  #ifdef BTN_START_PIN
    pinMode(BTN_START_PIN, INPUT_PULLUP);
    if (!digitalRead(BTN_START_PIN)) states |= BUTTON_START;
  #endif
#endif
  return states;
}

// Render ROM settings menu on TFT (240x320) using GFX font
void renderRomMenu(uint16_t *rowBuf, signed char cursor, signed char scrollOffset) {
  const GFXfont *font = &FreeSans9pt7b;
  const int charH = pgm_read_byte(&font->yAdvance);  // 22px per line
  const int baseline = gfxFontBaseline(font);
  const int titleH = charH + 4;     // title area height
  const int entryY0 = titleH;       // entries start here
  // Footer: 2 lines of text (controls + ROM counter) + progress bar
  const int barH = 10;
  const int footerH = charH * 2 + barH + 8;
  const int footerY = 320 - footerH;
  const int maxVisible = (footerY - entryY0) / charH;

  const uint16_t WHITE = 0xFFFF;
  const uint16_t BLACK = 0x0000;
  const uint16_t GRAY  = 0x4208;
  const uint16_t CYAN  = 0x07FF;
  const uint16_t GREEN = 0x07E0;
  const uint16_t YELLOW = 0xFFE0;
  const uint16_t RED   = 0xF800;

  // Count enabled ROMs
  int enabledCount = 0;
  for (int i = 0; i < machinesCount; i++)
    if (machineEnabled[i]) enabledCount++;

  int romPct = (enabledCount * 100) / machinesCount;

  // Build ROM counter string
  char romStr[40];
  snprintf(romStr, sizeof(romStr), "ROM: %d/%d attive (%d%%)",
           enabledCount, machinesCount, romPct);

  // Bar color based on how many are enabled
  uint16_t barColor = GREEN;
  if (romPct > 90) barColor = RED;
  else if (romPct > 75) barColor = YELLOW;

  video.setAddrWindow(0, 0, 240, 320);

  for (int py = 0; py < 320; py++) {
    memset(rowBuf, 0, 240 * 2);

    if (py < titleH - 2) {
      int charRow = py - 1;
      if (charRow >= 0 && charRow < charH) {
        tft_drawStringRowGFX(rowBuf, 4, "ROM SETTINGS", charRow, CYAN, BLACK, font, baseline);
        tft_drawStringRowGFX(rowBuf, 155, "START=OK", charRow, GRAY, BLACK, font, baseline);
      }
    } else if (py == titleH - 2 || py == titleH - 1) {
      for (int x = 0; x < 240; x++) rowBuf[x] = GRAY;
    } else if (py >= entryY0 && py < footerY) {
      int relY = py - entryY0;
      int visIdx = relY / charH;
      int charRow = relY % charH;
      int entryIdx = scrollOffset + visIdx;

      if (entryIdx < machinesCount && visIdx < maxVisible && charRow < charH) {
        bool selected = (entryIdx == cursor);
        uint16_t fg = selected ? BLACK : WHITE;
        uint16_t bg = selected ? WHITE : BLACK;

        if (selected) {
          for (int x = 0; x < 240; x++) rowBuf[x] = WHITE;
        }

        char entryStr[32];
        snprintf(entryStr, sizeof(entryStr), "%s %s",
                 machineEnabled[entryIdx] ? "[X]" : "[ ]",
                 machineNames[entryIdx]);
        tft_drawStringRowGFX(rowBuf, 4, entryStr, charRow, fg, bg, font, baseline);
      }
    } else if (py >= footerY && py < footerY + 2) {
      for (int x = 0; x < 240; x++) rowBuf[x] = GRAY;
    } else if (py >= footerY + 4 && py < footerY + 4 + charH) {
      int charRow = py - (footerY + 4);
      if (charRow < charH) {
        tft_drawStringRowGFX(rowBuf, 4, "UP/DN=MOVE", charRow, GRAY, BLACK, font, baseline);
        tft_drawStringRowGFX(rowBuf, 130, "FIRE=TOGGLE", charRow, GRAY, BLACK, font, baseline);
      }
    } else if (py >= footerY + 4 + charH && py < footerY + 4 + charH * 2) {
      int charRow = py - (footerY + 4 + charH);
      if (charRow < charH) {
        tft_drawStringRowGFX(rowBuf, 4, romStr, charRow, barColor, BLACK, font, baseline);
      }
    } else if (py >= footerY + 4 + charH * 2 + 2 && py < footerY + 4 + charH * 2 + 2 + barH) {
      int barMargin = 10;
      int barWidth = 240 - barMargin * 2;
      int filledW = (barWidth * romPct) / 100;
      int relBarY = py - (footerY + 4 + charH * 2 + 2);

      for (int x = barMargin; x < barMargin + barWidth; x++) {
        if (relBarY == 0 || relBarY == barH - 1 || x == barMargin || x == barMargin + barWidth - 1) {
          rowBuf[x] = GRAY;
        } else if (x - barMargin < filledW) {
          rowBuf[x] = barColor;
        }
      }
    }

    video.write(rowBuf, 240);
  }

  video.waitDMA();
}


// ROM settings menu on TFT - toggle games on/off
void showRomSettingsMenu() {
  // Assicurati che nessun DMA sia in corso prima di usare il display
  video.waitDMA();
  signed char cursor = 0;
  signed char scrollOffset = 0;
  const int charH = pgm_read_byte(&FreeSans9pt7b.yAdvance);
  const int titleH = charH + 4;
  const int entryY0 = titleH;
  const int barH = 10;
  const int footerH = charH * 2 + barH + 8;
  const int footerY = 320 - footerH;
  const int maxVisible = (footerY - entryY0) / charH;

  uint16_t *rowBuf = (uint16_t*)malloc(240 * 2);
  if (!rowBuf) return;

  // Show instructions on OLED
  flyer.clear();
  flyer.setColor(WHITE);
  oled_drawStringGFXCenter(flyer, 64, 0,  "UP/DN = Select", &FreeSans9pt7b);
  oled_drawStringGFXCenter(flyer, 64, 22, "FIRE = Toggle",  &FreeSans9pt7b);
  oled_drawStringGFXCenter(flyer, 64, 44, "START = Save",   &FreeSans9pt7b);
  flyer.display();

  // Initial render
  renderRomMenu(rowBuf, cursor, scrollOffset);

  while (true) {
    delay(30);
    input.updateBluetooth();  // aggiorna BT/I2C dallo stesso percorso del loop
    unsigned char keys = input.buttons_get_raw();
    unsigned char rising = keys & ~lastKeys;
    lastKeys = keys;

    bool needRedraw = false;

    if (rising & BUTTON_UP) {
      cursor--;
      if (cursor < 0) cursor = machinesCount - 1;
      if (cursor < scrollOffset) scrollOffset = cursor;
      if (cursor >= scrollOffset + maxVisible) scrollOffset = cursor - maxVisible + 1;
      if (cursor == machinesCount - 1)
        scrollOffset = (machinesCount > maxVisible) ? machinesCount - maxVisible : 0;
      needRedraw = true;
    }
    if (rising & BUTTON_DOWN) {
      cursor++;
      if (cursor >= machinesCount) cursor = 0;
      if (cursor >= scrollOffset + maxVisible) scrollOffset = cursor - maxVisible + 1;
      if (cursor < scrollOffset) scrollOffset = cursor;
      if (cursor == 0) scrollOffset = 0;
      needRedraw = true;
    }
    if (rising & BUTTON_FIRE) {
      machineEnabled[cursor] = !machineEnabled[cursor];
      // Ensure at least one ROM stays enabled
      int enabledCount = 0;
      for (int i = 0; i < machinesCount; i++)
        if (machineEnabled[i]) enabledCount++;
      if (enabledCount == 0)
        machineEnabled[cursor] = true;
      // Auto-save to EEPROM on every toggle
      saveMachineSettings();
      needRedraw = true;
    }
    if (rising & BUTTON_START) {
      rebuildActiveMachines();

      // Clear screen and restore game window
      video.setAddrWindow(0, 0, 240, 320);
      memset(rowBuf, 0, 240 * 2);
      for (int i = 0; i < 320; i++)
        video.write(rowBuf, 240);
      video.waitDMA();
      video.setAddrWindow(TFT_X_OFFSET, TFT_Y_OFFSET, 224, 288);

      free(rowBuf);

      // Clear OLED
      flyer.clear();
      flyer.display();
      return;
    }

    if (needRedraw) {
      renderRomMenu(rowBuf, cursor, scrollOffset);
    }
  }
}

// Mostra QR code sul TFT (240x320) - bitmap 220x220 1-bit da qr_logo_bitmap.h
void showQRonTFT() {
  const int qrW = QR_LOGO_WIDTH;               // 220
  const int qrH = QR_LOGO_HEIGHT;              // 220
  const int xOff = (240 - qrW) / 2;            // centra orizzontalmente = 10
  const int yOff = 10;                          // margine sopra
  const int textY = yOff + qrH + 8;            // testo sotto il QR
  const GFXfont *font = &FreeSans9pt7b;
  const int charH = pgm_read_byte(&font->yAdvance);
  const int baseline = gfxFontBaseline(font);

  uint16_t *rowBuf = (uint16_t*)malloc(240 * 2);
  if (!rowBuf) return;

  video.setAddrWindow(0, 0, 240, 320);

  for (int py = 0; py < 320; py++) {
    memset(rowBuf, 0, 240 * 2);  // sfondo nero

    // Zona QR code
    if (py >= yOff && py < yOff + qrH) {
      int qrRow = py - yOff;
      for (int qrCol = 0; qrCol < qrW; qrCol++) {
        int byteIdx = qrRow * QR_LOGO_ROW_BYTES + (qrCol / 8);
        int bitIdx = 7 - (qrCol % 8);
        uint8_t val = pgm_read_byte(&qr_logo_bitmap[byteIdx]);
        // bit 1 = bianco (sfondo), bit 0 = nero (modulo QR)
        rowBuf[xOff + qrCol] = (val >> bitIdx) & 1 ? 0xFFFF : 0x0000;
      }
    }
    // Testo "survivalhacking.it" sotto il QR
    else if (py >= textY && py < textY + charH) {
      int charRow = py - textY;
      // Centra il testo
      int textW = gfxStringWidth("survivalhacking.it", font);
      int textX = (240 - textW) / 2;
      tft_drawStringRowGFX(rowBuf, textX, "survivalhacking.it", charRow,
                           0xFFFF, 0x0000, font, baseline);
    }
    // Testo "GALAGINO V3.2"
    else if (py >= textY + charH + 4 && py < textY + charH * 2 + 4) {
      int charRow = py - (textY + charH + 4);
      int textW = gfxStringWidth("GALAGINO V3.2", font);
      int textX = (240 - textW) / 2;
      tft_drawStringRowGFX(rowBuf, textX, "GALAGINO V3.2", charRow,
                           0x07FF, 0x0000, font, baseline);  // cyan
    }

    video.write(rowBuf, 240);
  }
  video.waitDMA();
  free(rowBuf);
}

// Boot screen: QR su TFT + scrolling credits su OLED
void showBootScreen() {
  // Mostra QR code sul display TFT
  showQRonTFT();

  // Show title on OLED for 2 seconds, polling BT nel frattempo
  flyer.clear();
  flyer.setColor(WHITE);
  oled_drawStringGFXCenter(flyer, 64, 4, "GALAGINO", &FreeSans12pt7b);
  oled_drawStringGFXCenter(flyer, 64, 36, "V3.2 BY", &FreeSans12pt7b);
  flyer.display();
  // Polling BT durante i 2 secondi di splash
  for (int i = 0; i < 200; i++) {
    delay(10);
    input.updateBluetooth();
  }

  // Scrolling credits sull'OLED + polling BT continuo
  const char* credits[] = {
    "Paolo Sambi",
    "Davide Gatti",
    "Marco Prunca",
    "Spek Hoiler",
    "",
    "FIRE = ROM cfg",
  };
  const int numLines = sizeof(credits) / sizeof(credits[0]);
  const int lineH = 16;
  const int totalH = numLines * lineH;
  const int screenH = 64;
  const int scrollEnd = totalH + screenH;
  const int scrollSpeed = 1;
  const int frameDelay = 30;

  for (int offset = 0; offset < scrollEnd; offset += scrollSpeed) {
    flyer.clear();
    flyer.setColor(WHITE);

    for (int i = 0; i < numLines; i++) {
      int y = screenH - offset + (i * lineH);
      if (y > -lineH && y < screenH) {
        if (credits[i][0] != '\0') {
          oled_drawStringGFXCenter(flyer, 64, y, credits[i], &FreeSans9pt7b);
        }
      }
    }

    flyer.display();
    delay(frameDelay);
    // Polling BT ad ogni frame dello scroll
    input.updateBluetooth();
  }

  // Clear OLED
  flyer.clear();
  flyer.display();

  // Pulisci TFT e ripristina finestra di gioco (224x288)
  {
    uint16_t *clrBuf = (uint16_t*)malloc(240 * 2);
    if (clrBuf) {
      video.setAddrWindow(0, 0, 240, 320);
      memset(clrBuf, 0, 240 * 2);
      for (int i = 0; i < 320; i++)
        video.write(clrBuf, 240);
      video.waitDMA();
      free(clrBuf);
    }
    video.setAddrWindow(TFT_X_OFFSET, TFT_Y_OFFSET, 224, 288);
  }
}


// Timer per combo COIN+FIRE → ROM Settings (funziona sempre, menu o gioco)
static unsigned long romSettingsComboTimer = 0;

void loop(void) {
  // Update Bluetooth controller from main loop (required for proper operation)
  input.updateBluetooth();

  // COIN + FIRE tenuti 3 secondi = menu ROM Settings (funziona da qualsiasi stato)
  {
    unsigned char keys = input.buttons_get_raw();
    bool comboActive = (keys & BUTTON_COIN) && (keys & BUTTON_FIRE);
    if (comboActive) {
      if (romSettingsComboTimer == 0)
        romSettingsComboTimer = millis();
      else if (millis() - romSettingsComboTimer > 3000) {
        romSettingsComboTimer = 0;
        // Se un gioco è in esecuzione, fermalo prima
        if (!menu.machineIndexIsMenu()) {
          if (currentMachine->videoFlipY()) video.flipVertical(0);
          else if (currentMachine->videoFlipX()) video.flipHorizontal(0);
          video.mirror_x = false;
          emulation_stop();
        }
        // Aspetta fine DMA video prima di toccare il display
        video.waitDMA();

        // Mostra menu ROM Settings
        onRomSettingsFromMenu();
        romSettingsComboTimer = 0;
        // Aspetta che l'utente rilasci COIN+FIRE per evitare re-trigger
        while (true) {
          delay(50);
          input.updateBluetooth();
          unsigned char k = input.buttons_get_raw();
          if (!(k & BUTTON_COIN) || !(k & BUTTON_FIRE)) break;
        }
        return;
      }
    } else {
      romSettingsComboTimer = 0;
    }
  }

  // run video in main task. This will send signals to the emulation task in the background to synchronize video
  updateAudioVideo();

  // Update Bluetooth again at end of loop for more responsive input
  input.updateBluetooth();

  // Coin insert animation (non-blocking) - solo durante il gioco
  if (coinAnimPending) {
    coinAnimPending = false;
    if (!menu.machineIndexIsMenu()) {
      coinAnim_start();
    }
  }
  coinAnim_update();

  // Timeout volume/BT status - ripristina flyer dopo 2 secondi
  if (coinAnim_frame < 0 && volumeShowTime > 0 && (millis() - volumeShowTime > 2000)) {
    show_flyer(savedMenu);
    volumeShowTime = 0;
  }


#ifdef LED_PIN
  led.update(machines, menu.machineIndexPreselection(), menu.machineIndexSelected());
#endif
}

const int COIN_MOVE_FRAMES = 6;
const int COIN_TOTAL_FRAMES = 8;        // 6 movement + 1 insert + 1 "CREDIT!"

void coinAnim_drawFrame(int f) {
  const int CY = 32;
  const int CR = 10;
  const int coinX[] = {100, 83, 66, 49, 35, 22};

  flyer.clear();

  // ── PANNELLO SLOT ──
  flyer.drawRect(2, 6, 28, 52);
  flyer.drawRect(4, 8, 24, 48);
  flyer.fillRect(12, 20, 8, 24);
  flyer.setColor(BLACK);
  flyer.fillRect(14, 22, 4, 20);
  flyer.setColor(WHITE);

  if (f < COIN_MOVE_FRAMES) {
    int cx = coinX[f];
    // Mano
    flyer.fillCircle(cx + 18, CY, 12);
    flyer.fillRect(cx + 18, CY - 12, 22, 24);
    flyer.fillRect(cx + 5, CY - 16, 33, 7);
    flyer.fillCircle(cx + 5, CY - 13, 3);
    flyer.fillRect(cx + 5, CY + 10, 30, 7);
    flyer.fillCircle(cx + 5, CY + 13, 3);
    // Ritaglio presa
    flyer.setColor(BLACK);
    flyer.fillCircle(cx, CY, CR + 3);
    flyer.setColor(WHITE);
    // Moneta
    flyer.fillCircle(cx, CY, CR);
    flyer.setColor(BLACK);
    flyer.fillCircle(cx, CY, CR - 3);
    flyer.setColor(WHITE);
    flyer.drawCircle(cx, CY, CR - 3);
    flyer.drawLine(cx, CY - 5, cx, CY + 5);
    flyer.drawLine(cx - 3, CY - 2, cx + 1, CY - 2);
    flyer.drawLine(cx - 1, CY + 2, cx + 3, CY + 2);

  } else if (f == COIN_MOVE_FRAMES) {
    // Moneta nella fessura
    flyer.fillCircle(16, CY, 6);
    flyer.setColor(BLACK);
    flyer.fillCircle(16, CY, 3);
    flyer.setColor(WHITE);
    flyer.drawCircle(16, CY, 3);
    flyer.drawLine(8, CY - 10, 24, CY - 10);
    flyer.drawLine(8, CY + 10, 24, CY + 10);
    flyer.drawLine(16, CY - 14, 16, CY - 10);
    flyer.drawLine(16, CY + 10, 16, CY + 14);
    flyer.fillRect(50, CY - 14, 22, 7);
    flyer.fillRect(50, CY + 8, 20, 7);
    flyer.fillCircle(65, CY, 10);

  } else {
    // "CREDIT!"
    flyer.setPixel(16, CY);
    flyer.drawCircle(16, CY, 4);
    flyer.drawCircle(16, CY, 8);
    flyer.setFont(ArialMT_Plain_24);
    flyer.setTextAlignment(TEXT_ALIGN_CENTER);
    flyer.drawString(80, 20, "CREDIT!");
    flyer.setTextAlignment(TEXT_ALIGN_LEFT);
  }

  flyer.display();
}

void coinAnim_start() {
  savedMenu = menu.machineIndexPreselection() + 1;
  coinAnim_frame = 0;
  coinAnim_lastTime = millis();
  coinAnim_drawFrame(0);
}

void coinAnim_update() {
  if (coinAnim_frame < 0) return;

  unsigned long now = millis();
  // 80ms per frame di movimento, 120ms per inserimento, 500ms per "CREDIT!"
  unsigned long interval;
  if (coinAnim_frame < COIN_MOVE_FRAMES)
    interval = 80;
  else if (coinAnim_frame == COIN_MOVE_FRAMES)
    interval = 120;
  else
    interval = 500;

  if (now - coinAnim_lastTime >= interval) {
    coinAnim_frame++;
    if (coinAnim_frame >= COIN_TOTAL_FRAMES) {
      // Animazione finita, avvia timeout per ripristinare flyer
      coinAnim_frame = -1;
      volumeShowTime = millis();
      return;
    }
    coinAnim_lastTime = now;
    coinAnim_drawFrame(coinAnim_frame);
  }
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
      currentMachine = activeMachines[menu.machineIndexSelected()];
      audio.start(currentMachine);
      if (currentMachine->videoFlipY()) {
        video.flipVertical(1);
      } else if (currentMachine->videoFlipX()) {
        video.flipHorizontal(1);
      }
      
      #ifdef ENABLE_GYRUSS
      if (currentMachine->machineType() == MCH_GYRUSS) {
        video.mirror_x = true;
      }
      #endif
      // start new machine
      emulation_start();
    }
    currentMachine->prepare_frame();
  }



  if (doReset || menu.attract_gameTimeout()) {
    if (currentMachine->videoFlipY()) {
      video.flipVertical(0);
    } else if (currentMachine->videoFlipX()) {
      video.flipHorizontal(0);
    }
    video.mirror_x = false;
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


void show_flyer(unsigned char active_idx) {
  if (active_idx < 1 || active_idx > activeMachinesCount) return;
  signed char mtype = activeMachines[active_idx - 1]->machineType();

  const unsigned char *bits   = NULL;
  int                  width  = 0;
  int                  height = 0;
  bool                 invert = false;

  // Selezione logo in base al machineType
  switch (mtype) {
#ifdef ENABLE_GALAGA
    case MCH_GALAGA:    bits = Galaga_Logo_bits; break;
#endif
#ifdef ENABLE_PACMAN
    case MCH_PACMAN:    bits = Pacman_Logo_bits; break;
#endif
#ifdef ENABLE_DIGDUG
    case MCH_DIGDUG:    bits = Digdug_Logo_bits; break;
#endif
#ifdef ENABLE_1942
    case MCH_1942:      bits = A1942_Logo_bits; break;
#endif
#ifdef ENABLE_FROGGER
    case MCH_FROGGER:   bits = Frogger_Logo_bits; break;
#endif
#ifdef ENABLE_DKONG
    case MCH_DKONG:     bits = Dkong_Logo_bits; break;
#endif
#ifdef ENABLE_EYES
    case MCH_EYES:      bits = Eyes_Logo_bits; break;
#endif
#ifdef ENABLE_LIZARD
    case MCH_LIZARD:    bits = Lizard_Logo_bits; break;
#endif
#ifdef ENABLE_MRTNT
    case MCH_MRTNT:     bits = Tnt_Logo_bits; break;
#endif
#ifdef ENABLE_THEGLOB
    case MCH_THEGLOB:   bits = Glob_Logo_bits; break;
#endif
#ifdef ENABLE_CRUSH
    case MCH_CRUSH:     bits = Crush_Logo_bits; break;
#endif
#ifdef ENABLE_ANTEATER
    case MCH_ANTEATER:  bits = Ant_Logo_bits; break;
#endif
#ifdef ENABLE_MRDO
    case MCH_MRDO:      bits = MrDo_Logo_bits; break;
#endif
#ifdef ENABLE_BOMBJACK
    case MCH_BOMBJACK:  bits = BombJack_Logo_bits; break;
#endif
#ifdef ENABLE_MSPACMAN
    case MCH_MSPACMAN:  bits = MsPacman_Logo_bits; break;
#endif
#ifdef ENABLE_GALAXIAN
    case MCH_GALAXIAN:  bits = Galaxian_Logo_bits; break; 
#endif
#ifdef ENABLE_LADYBUG
    case MCH_LADYBUG:   bits = LadyBug_Logo_bits; break;
#endif
#ifdef ENABLE_GYRUSS
    case MCH_GYRUSS:    bits = Gyruss_Logo_bits; break; 
#endif
#ifdef ENABLE_TIMEPLT
    case MCH_TIMEPLT:   bits = TimePilot_Logo_bits; break;
#endif
#ifdef ENABLE_PENGO
    case MCH_PENGO:     bits = Pengo_Logo_bits; break;
#endif
#ifdef ENABLE_BAGMAN
    case MCH_BAGMAN:    bits = Bagman_Logo_bits; break;
#endif
#ifdef ENABLE_SPACE
    case MCH_SPACE:     bits = SpaceInvaders_Logo_bits; break;
#endif
#ifdef ENABLE_TUTANKHM
    case MCH_TUTANKHM:  bits = Tutankhm_Logo_bits; break;
#endif
  }

  if (bits == NULL)
    return;

  // Disegno comune
  flyer.clear();

  if (invert) {
    flyer.setColor(WHITE);
    flyer.fillRect(0, 0, 128, 64);
    flyer.setColor(BLACK);
  }

  flyer.drawXbm(0, 0, 128, 64, bits);

  if (invert) {
    flyer.setColor(WHITE);  // ripristina default
  }

  flyer.display();
}


void show_bt_status(bool enabled) {
  savedMenu = menu.machineIndexPreselection() + 1;
  flyer.clear();
  flyer.setColor(WHITE);
  oled_drawStringGFXCenter(flyer, 64, 18, enabled ? "BLU ON" : "BLU OFF", &FreeSans12pt7b);
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

  // Testo GRANDE sopra la barra (GFX font)
  char volText[8];
  sprintf(volText, "%d", maxVol - currentVol + 1);
  oled_drawStringGFX(flyer, 5, 2, "VOL", &FreeSans12pt7b);
  oled_drawStringGFX(flyer, 75, 2, volText, &FreeSans12pt7b);

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
