# Galagino 3.2 a special LVL ESP32-2432S024 porting with enanched joystick and flyer oled display, with bluethooth support for external controller.
Galagino V3.2 software can be uploaded on V2.0 hardware without any modification.

Video tutorial for build a new Galagino V3.0:  https://youtu.be/Nz3LRrY3Ukw 

To build a joystick follow Galagino V2.0 video: https://youtu.be/YmvyNwJLqJM

ported from speckholier platformio to Arduino IDE - with 8 additional games: https://github.com/speckhoiler/galagino
This repo is a port of Till Harbaum's awesome [Galaga emulator](https://github.com/harbaum/galagino) ported to platformio.
This port is NOT by the original author, so please do not bother him with issues.

![IMG_6985_(00-00-00-00)](https://github.com/user-attachments/assets/6cb5540a-4477-43ef-997c-9424cbe6df4d)

## Hardware

* Joystick 4 posizioni/4 way joystick type15: https://s.click.aliexpress.com/e/_c3Qgeoth
* PCF8574P: https://s.click.aliexpress.com/e/_oDPPZgo
* Modulo LVGL 2.4" ESP32-2432S024: https://s.click.aliexpress.com/e/_DECY96V
* Connettori JST 1.25MM, 10Cm, 2P/Connector 2P: https://s.click.aliexpress.com/e/_DBzkKsz
* Connettori JST 1.25MM, 10Cm, 4P/Connector 4P: https://s.click.aliexpress.com/e/_DF72N8Z
* Batteria al Litio 3.7V 2000mA/lythium battery: https://s.click.aliexpress.com/e/_c3S7Fg07 oppure https://amzn.to/4b0yBZs
* Altoparlante 3070/Speaker: https://s.click.aliexpress.com/e/_c4NQslWX oppure https://amzn.to/48bumpJ
* Display OLED SD1306 0.91" : https://s.click.aliexpress.com/e/_c4DUrL47 
* Pulsanti 6x6x10 type 4 /PushButton: https://s.click.aliexpress.com/e/_c4eKUiJH
* Resistenze SMD 4.7K 0805/Resistors: https://s.click.aliexpress.com/e/_DdHzrWd
* USB-C da pannello 6P 5A/USB panel mount 6P 5A: https://s.click.aliexpress.com/e/_c3BmvvEj
* Mini controller BT: https://s.click.aliexpress.com/e/_c2JYkcQF

---

UPDATE !!!!
Recently, some modules have been shipped that are clones of those used in the project and have a different display driver as well as different connections on the connectors.
If your module is one of these (ESP32-024)—and you’ll notice right away because when you upload the code, the screen will be rotated 90°—then you must use the latest version of the code, 3.2A, and follow the new wiring diagram 3.0A. You’ll also need to modify the definition for your display type in the config.h file.

Comment out the line:     // #define CHEAP_YELLOW_DISPLAY_CONF

And uncomment the line:   #define CHEAP_YELLOW_DISPLAY_CONF_ALT  

---

<img width="195" height="259" alt="tutankham" src="https://github.com/user-attachments/assets/95170047-8a0e-475a-87dd-19cfe15cdc7c" />
<img width="210" height="240" alt="timeplt" src="https://github.com/user-attachments/assets/f26e6cde-2d99-4c92-a8b2-14a14e1cc538" />
<img width="224" height="288" alt="theglob" src="https://github.com/user-attachments/assets/043d37cb-a3e7-42e2-aa3f-f124fb39538f" />
<img width="224" height="288" alt="pengo" src="https://github.com/user-attachments/assets/74120ad5-5ef3-4a2d-b4ff-8ae1740492a6" />
<img width="198" height="255" alt="mspacman" src="https://github.com/user-attachments/assets/55845ea5-3b6c-4a97-b712-09089c671e4f" />
<img width="224" height="288" alt="mrtnt" src="https://github.com/user-attachments/assets/d81095c8-101c-4a40-8379-cca3effc9e60" />
<img width="224" height="288" alt="mrdo" src="https://github.com/user-attachments/assets/c0f6e7f0-0a77-461b-b977-f97415cb8122" />
<img width="224" height="288" alt="lizwiz" src="https://github.com/user-attachments/assets/2ef2a2ef-d4b7-4691-b404-0d58fb9034ca" />
<img width="192" height="240" alt="LadyBug" src="https://github.com/user-attachments/assets/9fed1c3f-b4c7-4eb1-bb6a-f3dc6971add2" />
<img width="210" height="240" alt="invaders" src="https://github.com/user-attachments/assets/c242bd38-97d8-4b76-80a8-8d6e2cbe9e19" />
<img width="210" height="240" alt="gyrrus" src="https://github.com/user-attachments/assets/58d9f40c-2246-459f-a186-5a5f17726392" />
<img width="224" height="288" alt="frogger" src="https://github.com/user-attachments/assets/141aebbd-7346-46a8-af7c-d5d17bb1dc0b" />
<img width="224" height="288" alt="eyes" src="https://github.com/user-attachments/assets/a1858b34-95c9-45f2-b813-3dea62a7edc3" />
<img width="224" height="288" alt="digdug" src="https://github.com/user-attachments/assets/69952234-dfb1-4b60-907f-9a6b70a37843" />
<img width="224" height="288" alt="crush" src="https://github.com/user-attachments/assets/316773ed-8551-417e-b092-bdbc2bb89d36" />
<img width="224" height="288" alt="bombjack" src="https://github.com/user-attachments/assets/a0a4b67b-56fe-4b06-bb82-a44291ee4608" />
<img width="224" height="288" alt="bagman" src="https://github.com/user-attachments/assets/ab861795-0ada-4118-8f82-5f636cac7557" />
<img width="224" height="288" alt="anteater" src="https://github.com/user-attachments/assets/e7914917-bd36-4252-a196-c7b0d45e3da3" />
<img width="224" height="288" alt="1942" src="https://github.com/user-attachments/assets/f09ff10f-0bfe-402b-bdc8-2f2bb9e23b82" />
<img width="224" height="288" alt="Galaxian" src="https://github.com/user-attachments/assets/25bfec15-d028-4772-9e1f-c38b1c751271" />
<img width="224" height="288" alt="Pacman" src="https://github.com/user-attachments/assets/218fafdf-3750-4bad-af19-9a825435d816" />
<img width="224" height="288" alt="Galaga" src="https://github.com/user-attachments/assets/5f3d89f8-5f08-4e20-940e-c5f2a63ce4ba" />
<img width="224" height="288" alt="DonkeyKong" src="https://github.com/user-attachments/assets/2c92ff66-bffa-435f-abea-f5aa5ad31b80" />

## Software

Like in the original from Till Harbaum's Galaga emulator, download these files:

* The [Galagino specific code](source/) contained in this repository
* A [Z80 software emulation](https://fms.komkon.org/EMUL8/Z80-081707.zip) by [Marat Fayzullin](https://fms.komkon.org/)
* The original ROM files
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
    * [BombJack](https://www.google.com/search?q=bombjack.zip+arcade+rom)
    * [MrDo](https://www.google.com/search?q=mrdo.zip+arcade+rom)
    * [Lady Bug](https://www.google.com/search?q=ladybug.zip+arcade+rom)
    * [Gyrrus](https://www.google.com/search?q=gyruss.zip+arcade+rom)
    * [Time Pilot](https://www.google.com/search?q=timeplt.zip+arcade+rom)
    * [Galaxian](https://www.google.com/search?q=glaxian.zip+arcade+rom)
    * [MsPacman](https://www.google.com/search?q=mspacman.zip+arcade+rom)
    * [SpaceInvaders](https://www.google.com/search?q=invaders.zip+arcade+rom)
    * [Thuthankam](https://www.google.com/search?q=tutankhm.zip+arcade+rom) 

Due to memory limitations, not all games can be enabled. Edit the config.h file to select which games to enable. At least two must always be disabled; otherwise, they won’t fit in the ESP32’s memory.
By default, Gyruss and Tutankham are disabled; they are still emulated but run too slowly to be playable.

Galagino uses code that is not freely available and thus not included in this repository. Preparing the firmware thus consists of a few additional steps:

* If you do not have Python installed, download it from here. [Python 3.13.0](https://www.python.org/downloads/release/python-3130)
* The ROM ZIP files have to be placed in the [romszip directory](romszip/), together with the ZIP file containing the Z80 emulator.
* A set of [python scripts](romconv/) is then being used to convert and patch the ROM data and emulator code and to include the resulting code into the galagino machines directory. For all games, just use conv__all.bat.

The [ROM conversion](./romconv) create a whole bunch of additional files in the [source directory](./source). Please check the README in the [romconv](./romconv) directory for further instructions.
Please ensure that the stripts run without errors!

* Open src.INO with Arduino IDE
* Add this url in your arduino ide board source preferences: https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json 
* then add a ESP32 bluepad board
* select board "ESP 32 DEV module" family:esp32_bluepad32
* Use huge partition
* Enable PSRAM


## Controls

* Joystick UP/DOWN to select game
* Any key to enter in the selected game.
* COIN key to add coins
* START key to start game or flight loop in 1945 game
* FIRE key to of a fire in game the uses fire/action
* COIN + UP/DOWN to setup volume
* COIN pressed for 10 second to enable/disable BT
* START for 5 seconds to exit game and return to selection menu
* FIRE during boot to access configuration menu

## Attract mode

In Attract mode, the machine cycles through all games if you do not touch the joystick. The games end after 5 minutes.

## Revisions
V3.0 1/2/2026  
 * Conversion from speckhoiler VsCode version to Arduino Ide by Paolo Sambinello
 * Adding a Bluethoot Controller support, by Paolo Sambinello
 * Adding a Bluethooot enabler/disabler and increase lives to 5 by Marco Prunca
 * Adding a COIN animation by Marco Prunca
 
V3.1 11/3/2026
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
 
V3.2 26/4/2026 
 * Added QR code by Paolo Sambinello
 * Added Configuration Menu by Paolo Sambinello
 * Fixed insert coin animation by Marco Prunca
 * Added Galaxian game by Marco Prunca (minor audio problems)
 * Fixed Time Pilot game by Marco Prunca (minor glitch on clouds graphichs)
 * Added Tutankham game by Marco Prunca (not playable at full speed)

V3.2A 21/06/2026
 * Added support for alternate Cheap Yellow Display ESP32-024, need to move Joystick Connector to upper right connector integrating +3v3 wire. See new schematic 3.0A

