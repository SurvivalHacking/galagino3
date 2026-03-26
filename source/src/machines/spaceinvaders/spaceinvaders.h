#ifndef SPACEINVADERS_H
#define SPACEINVADERS_H

#ifndef ENABLE_SPACE
#error "ENABLE_SPACE mancante nel config.h!"
#endif

#include <pgmspace.h>
#include "spaceinvaders_logo.h"
#include "spaceinvaders_rom.h"
#include "spaceinvaders_dipswitches.h"
#include "../machineBase.h"

class spaceinvaders : public machineBase
{
public:
    spaceinvaders() { }
    ~spaceinvaders() { }

    signed char machineType() override { return MCH_SPACE; }
    unsigned char rdZ80(unsigned short Addr) override;
    void wrZ80(unsigned short Addr, unsigned char Value) override;
    void outZ80(unsigned short Port, unsigned char Value) override;
    unsigned char opZ80(unsigned short Addr) override;
    unsigned char inZ80(unsigned short Port) override;

    void run_frame(void) override;
    void prepare_frame(void) override;
    void render_row(short row) override;
    const unsigned short *logo(void) override;
    void reset() override;

#ifdef LED_PIN
    void menuLeds(CRGB *leds) override;
    void gameLeds(CRGB *leds) override;
#endif

protected:
    // Space Invaders uses bitmap framebuffer, no tiles/sprites
    void blit_tile(short row, char col) override { }
    void blit_sprite(short row, unsigned char s) override { }

private:
    // Hardware shift register
    uint16_t shift_data;
    uint8_t shift_amount;
    uint8_t last_coin = 0;  // edge detection for coin insert sound

    // Color overlay lookup (RGB565 byte-swapped)
    unsigned short get_pixel_color(int y);

#ifdef LED_PIN
    const CRGB menu_leds[7] = { LED_GREEN, LED_WHITE, LED_GREEN, LED_WHITE, LED_GREEN, LED_WHITE, LED_GREEN };
#endif
};

#endif
