#ifndef LADYBUG_H
#define LADYBUG_H

#ifndef ENABLE_LADYBUG
#error "ENABLE_LADYBUG mancante nel config.h!"
#endif

#include "ladybug_rom.h"
#include "ladybug_dipswitches.h"
#include "ladybug_logo.h"
#include "ladybug_tilemap.h"
#include "ladybug_spritemap.h"
#include "ladybug_cmap.h"
// NOTE: Lady Bug does NOT use shared tileaddr.h - has custom tilemap scan
#include "../machineBase.h"

class ladybug : public machineBase
{
public:
	ladybug() {
		pRom1 = ladybug_rom_cpu1;
	}
	~ladybug() { }

	signed char machineType() override { return MCH_LADYBUG; }
	signed char videoFlipX() override { return 1; }
	bool hasNamcoAudio() override { return false; }

	unsigned char rdZ80(unsigned short Addr) override;
	void wrZ80(unsigned short Addr, unsigned char Value) override;
	unsigned char opZ80(unsigned short Addr) override;

	void run_frame(void) override;
	void prepare_frame(void) override;
	void render_row(short row) override;
	const unsigned short *logo(void) override;

#ifdef LED_PIN
	void menuLeds(CRGB *leds) override;
	void gameLeds(CRGB *leds) override;
#endif

protected:
	void blit_tile(short row, char col) override;
	void blit_sprite(short row, unsigned char s) override;

	const unsigned char *pRom1;    // 24KB CPU ROM

	// Video state
	unsigned char flipScreen = 0;

	// Vblank state (toggled in run_frame for polling via IN1 bits 6-7)
	unsigned char vblankActive = 0;

	// Coin NMI tracking
	unsigned char coinPrev = 0;

	// Frame counter for game_started detection
	unsigned short frameCount = 0;

private:
	void SN76489_Write_2chip(int chip, unsigned char data);
	int sn_last_register[2] = {0, 0};

#ifdef LED_PIN
	const CRGB menu_leds[7] = { LED_RED, LED_RED, LED_BLACK, LED_RED, LED_BLACK, LED_RED, LED_RED };
#endif
};

#endif
