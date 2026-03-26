#ifndef TIMEPLT_H
#define TIMEPLT_H

#include <pgmspace.h>
#include "timeplt_logo.h"
#include "timeplt_rom.h"
#include "timeplt_snd_rom.h"
#include "timeplt_dipswitches.h"
#include "timeplt_tilemap.h"
#include "timeplt_spritemap.h"
#include "../tileaddr.h"
#include "../machineBase.h"

class timeplt : public machineBase
{
public:
	timeplt() { }
	~timeplt() { }

	signed char machineType() override { return MCH_TIMEPLT; }
	void reset() override {
		machineBase::reset();
		nmi_enable = 0;
		video_enable = 0;
		flip_screen = 0;
		soundlatch = 0;
		scanline_counter = 0;
		frame_progress = 0;
		snd_irq_pending = 0;
		snd_irq_last = 0;
		ay_timer_counter = 0;
		snd_icnt = 0;
		memset(snd_ram, 0, sizeof(snd_ram));
		memset(ay_addr, 0, sizeof(ay_addr));
		memset(ay_regs, 0, sizeof(ay_regs));
	}
	unsigned char rdZ80(unsigned short Addr) override;
	void wrZ80(unsigned short Addr, unsigned char Value) override;
	void outZ80(unsigned short Port, unsigned char Value) override;
	unsigned char opZ80(unsigned short Addr) override;
	unsigned char inZ80(unsigned short Port) override;

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
	void blit_tile_cat(short row, char col, signed char cat_filter);
	void blit_sprite(short row, unsigned char s) override;
	void extract_sprites(const unsigned char *bank0, const unsigned char *bank1);

private:
	unsigned char nmi_enable;
	unsigned char video_enable;
	unsigned char flip_screen;
	unsigned char soundlatch;
	unsigned char scanline_counter;
	int frame_progress;  // current iteration within run_frame (0..INST_PER_FRAME-1)

	// Sound CPU state
	Z80 snd_cpu;
	unsigned char snd_ram[1024];
	unsigned char snd_irq_pending;
	unsigned char snd_irq_last;    // previous Q2 state for edge detection

	// AY-3-8910 registers
	unsigned char ay_addr[2];
	unsigned char ay_regs[2][16];
	unsigned char ay_timer_counter;
	unsigned long snd_icnt;

#ifdef LED_PIN
	const CRGB menu_leds[7] = { LED_CYAN, LED_WHITE, LED_CYAN, LED_WHITE, LED_CYAN, LED_WHITE, LED_CYAN };
#endif
};

#endif
