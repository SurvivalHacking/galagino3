#ifndef TUTANKHM_H
#define TUTANKHM_H

#include "../machineBase.h"
#include "../../cpus/m6809/m6809.h"
#include "tutankhm_rom.h"
#include "tutankhm_bank_rom.h"
#include "tutankhm_snd_rom.h"
#include "tutankhm_dipswitches.h"
#include "tutankhm_logo.h"

// Tutankham (Konami/Stern 1982)
//   Main CPU: MC6809E @ 1.5 MHz
//   Sound: timeplt_audio (Z80 @ 1.789 MHz + 2x AY-3-8910)
//   Video: 256x256 bitmap, 4bpp, ROT90
//
// Memory layout in shared memory[] buffer:
//   0x0000-0x07FF: Work RAM (2KB, maps to CPU 0x8800-0x8FFF)
//   0x0800-0x080F: Palette RAM (16 bytes, maps to CPU 0x8000-0x800F)
//   Total: 0x0810 = 2064 bytes (fits in RAMSIZE 9344)
//
// Video RAM (32KB) is allocated separately in PSRAM.

#define TUT_WORKRAM   0x0000
#define TUT_PALETTE   0x0800

// M6809 runs at ~1.5 MHz, ~25000 cycles/frame at 60Hz
// Using step count similar to other games
#define TUT_STEPS_PER_FRAME  600

class tutankhm : public machineBase
{
public:
    tutankhm() { videoram = nullptr; }
    ~tutankhm() { if(videoram) free(videoram); }

    void init(Input *input, unsigned short *framebuffer, sprite_S *spritebuffer, unsigned char *memorybuffer) override;
    void reset() override;

    signed char machineType() override { return MCH_TUTANKHM; }

    // Tutankham doesn't use Z80 for main CPU, but the base class has rdZ80/wrZ80
    // These are used by the sound CPU (Z80)
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

    // M6809 main CPU memory access (called from C callbacks)
    uint8_t main_read(uint16_t addr);
    void main_write(uint16_t addr, uint8_t val);

private:
    // M6809 main CPU
    m6809_state main_cpu;

    // Video RAM (32KB bitmap, allocated from PSRAM)
    uint8_t *videoram;

    // Palette cache (16 entries, pre-computed RGB565 byte-swapped)
    unsigned short palette_rgb565[16];

    // Control registers
    unsigned char irq_enable;
    unsigned char irq_toggle;   // fires IRQ every other frame
    unsigned char scroll_reg;   // scroll register at 0x8100
    unsigned char soundlatch;
    unsigned char sound_mute;

    // Sound CPU state (timeplt_audio clone)
    unsigned char snd_ram[1024];
    unsigned char snd_irq_pending;
    unsigned char snd_irq_last;
    unsigned char ay_addr[2];
    unsigned char ay_regs[2][16];
    unsigned long snd_icnt;

    // ROM bank select
    unsigned char bank_select;

    // Flash bomb: EXTRA held 2 seconds on PCF triggers flash
    unsigned long extra_hold_start;
    bool extra_flash_active;

    // Gunshot sound effect
    unsigned char shot_timer;       // frames remaining for shot sound
    bool last_fire_state;

    // Helper: convert palette byte to RGB565 (byte-swapped for SPI)
    unsigned short palette_to_rgb565(uint8_t val);

#ifdef LED_PIN
    const CRGB menu_leds[7] = { LED_YELLOW, LED_WHITE, LED_YELLOW, LED_WHITE, LED_YELLOW, LED_WHITE, LED_YELLOW };
#endif
};

// Global pointer for M6809 callbacks
extern tutankhm *g_tutankhm_instance;

#endif
