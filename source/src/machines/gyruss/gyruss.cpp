#include "gyruss.h"
#include "esp_task_wdt.h"

// ============================================================
// M6809 callback bridge (C functions calling C++ methods)
// ============================================================

gyruss *g_gyruss_instance = nullptr;

extern "C" {
    uint8_t m6809_read(m6809_state *s, uint16_t addr) {
        return g_gyruss_instance->sub_read(addr);
    }
    void m6809_write(m6809_state *s, uint16_t addr, uint8_t val) {
        g_gyruss_instance->sub_write(addr, val);
    }
    uint8_t m6809_read_opcode(m6809_state *s, uint16_t addr) {
        return g_gyruss_instance->sub_read_opcode(addr);
    }
}

// ============================================================
// M6809 sub-CPU memory map
// ============================================================

uint8_t gyruss::sub_read(uint16_t addr) {
    if (addr == 0x0000) {
        return scanline_counter;
    }
    if (addr >= 0x4000 && addr <= 0x47FF) {
        return sub_ram[addr - 0x4000];
    }
    if (addr >= 0x6000 && addr <= 0x67FF) {
        return shared_ram[addr - 0x6000];
    }
    // ROM at 0xE000-0xFFFF only (no mirror, matching MAME)
    if (addr >= 0xE000) {
        return pgm_read_byte(&gyruss_rom_sub_raw[addr - 0xE000]);
    }
    return 0xFF;
}

void gyruss::sub_write(uint16_t addr, uint8_t val) {
    if (addr == 0x2000) {
        // MAME: gyruss_irq_clear_w — acknowledge IRQ (CLEAR_LINE)
        // IRQ mask is controlled by main CPU LS259 Q1 (0xC181), not here
        return;
    }
    if (addr >= 0x4000 && addr <= 0x47FF) {
        sub_ram[addr - 0x4000] = val;
        return;
    }
    if (addr >= 0x6000 && addr <= 0x67FF) {
        shared_ram[addr - 0x6000] = val;
        return;
    }
}

uint8_t gyruss::sub_read_opcode(uint16_t addr) {
    // Konami-1 decrypted opcodes at 0xE000-0xFFFF only (no mirror)
    if (addr >= 0xE000) {
        return pgm_read_byte(&gyruss_rom_sub_decrypt[addr - 0xE000]);
    }
    return sub_read(addr);
}

// ============================================================
// Audio Z80 dual-core task
// ============================================================

static void gyruss_audio_task_fn(void *param) {
    gyruss *g = (gyruss *)param;
    esp_task_wdt_delete(NULL);
    while (g->audio_running) {
        g->run_audio_batch(100);
        taskYIELD();
    }
    vTaskDelete(NULL);
}

void gyruss::run_audio_batch(int steps) {
    for (int i = 0; i < steps && audio_running; i++) {
        StepZ80(&cpu[1]);
        if (sound_irq_pending && (cpu[1].IFF & IFF_1)) {
            IntZ80(&cpu[1], INT_RST38);
            sound_irq_pending = 0;
        }
    }
    audio_cycle_approx += steps * 8;
}

void gyruss::start_audio_task() {
    if (audio_task_handle) return;
    audio_running = 1;
    int audio_core = (emu_core_id == 0) ? 1 : 0;
    xTaskCreatePinnedToCore(gyruss_audio_task_fn, "gyr_audio", 2048, this, 1, &audio_task_handle, audio_core);
}

void gyruss::stop_audio_task() {
    if (!audio_task_handle) return;
    audio_running = 0;
    vTaskDelay(20);  // let audio task exit
    audio_task_handle = NULL;
}

// ============================================================
// Init and Reset
// ============================================================

void gyruss::init(Input *input, unsigned short *framebuffer, sprite_S *spritebuffer, unsigned char *memorybuffer) {
    machineBase::init(input, framebuffer, spritebuffer, memorybuffer);
    g_gyruss_instance = this;
    audio_task_handle = NULL;
    audio_running = 0;
    emu_core_id = -1;
}

void gyruss::reset() {
    // Stop audio task if running (before resetting state)
    stop_audio_task();

    machineBase::reset();
    g_gyruss_instance = this;

    memset(sub_ram, 0, sizeof(sub_ram));
    memset(shared_ram, 0, sizeof(shared_ram));
    sub_irq_mask = 1;  // default enabled; main CPU can control via LS259 Q1 (0xC181)
    sound_latch = 0;
    sound_latch_pending = 0;
    sound_irq_pending = 0;
    flip_screen = 0;
    scanline_counter = 0;
    frame_odd = 0;
    memset(ay_address, 0, sizeof(ay_address));
    audio_cycle_approx = 0;
    emu_core_id = -1;

    // Reset M6809 sub-CPU
    m6809_reset(&sub_cpu);

    // Reset audio Z80 (still single-core at this point, audio task not started yet)
    current_cpu = 1;
    ResetZ80(&cpu[1]);
    current_cpu = 0;
}

// ============================================================
// Main Z80 memory map
// ============================================================

// Determine if current context is audio Z80 (works for both single-core and dual-core)
inline bool gyruss::is_audio_cpu() {
    if (audio_task_handle != NULL)
        return (xPortGetCoreID() != emu_core_id);  // dual-core: check which core
    return (current_cpu == 1);                       // single-core: use flag
}

unsigned char gyruss::opZ80(unsigned short Addr) {
    if (!is_audio_cpu()) {
        if (Addr < 0x6000)
            return pgm_read_byte(&gyruss_rom_main[Addr]);
        return rdZ80(Addr);
    } else {
        if (Addr < 0x4000)
            return pgm_read_byte(&gyruss_rom_audio[Addr]);
        return rdZ80(Addr);
    }
}

unsigned char gyruss::rdZ80(unsigned short Addr) {
    if (!is_audio_cpu()) {
        if (Addr < 0x6000)
            return pgm_read_byte(&gyruss_rom_main[Addr]);

        if (Addr >= 0x8000 && Addr <= 0x83FF)
            return memory[GYR_CRAM_OFF + (Addr & 0x3FF)];

        if (Addr >= 0x8400 && Addr <= 0x87FF)
            return memory[GYR_VRAM_OFF + (Addr & 0x3FF)];

        if (Addr >= 0x9000 && Addr <= 0x9FFF)
            return memory[GYR_WRAM_OFF + (Addr - 0x9000)];

        if (Addr >= 0xA000 && Addr <= 0xA7FF)
            return shared_ram[Addr - 0xA000];

        if (Addr == 0xC000) return GYRUSS_DSW1;

        if (Addr == 0xC080) {
            unsigned char keymask = input->buttons_get();
            unsigned char retval = 0xFF;
            if (keymask & BUTTON_COIN)  retval &= ~0x01;
            if (keymask & BUTTON_START) retval &= ~0x08;
            return retval;
        }

        if (Addr == 0xC0A0) {
            unsigned char keymask = input->buttons_get();
            unsigned char retval = 0xFF;
            if (keymask & BUTTON_LEFT)  retval &= ~0x01;
            if (keymask & BUTTON_RIGHT) retval &= ~0x02;
            if (keymask & BUTTON_UP)    retval &= ~0x04;
            if (keymask & BUTTON_DOWN)  retval &= ~0x08;
            if ((keymask & BUTTON_FIRE) && game_started) retval &= ~0x10;
            return retval;
        }

        if (Addr == 0xC0C0) return 0xFF;
        if (Addr == 0xC0E0) return GYRUSS_DSW0;
        if (Addr == 0xC100) return GYRUSS_DSW2;

        return 0xFF;
    }
    else {
        if (Addr < 0x4000)
            return pgm_read_byte(&gyruss_rom_audio[Addr]);

        if (Addr >= 0x6000 && Addr <= 0x63FF)
            return memory[GYR_ARAM_OFF + (Addr - 0x6000)];

        if (Addr == 0x8000) {
            return sound_latch;
        }

        return 0xFF;
    }
}

void gyruss::wrZ80(unsigned short Addr, unsigned char Value) {
    if (!is_audio_cpu()) {
        if (Addr >= 0x8000 && Addr <= 0x83FF) {
            memory[GYR_CRAM_OFF + (Addr & 0x3FF)] = Value;
            return;
        }

        if (Addr >= 0x8400 && Addr <= 0x87FF) {
            memory[GYR_VRAM_OFF + (Addr & 0x3FF)] = Value;
            if (!game_started && Value != 0) game_started = 1;
            return;
        }

        if (Addr >= 0x9000 && Addr <= 0x9FFF) {
            memory[GYR_WRAM_OFF + (Addr - 0x9000)] = Value;
            return;
        }

        if (Addr >= 0xA000 && Addr <= 0xA7FF) {
            shared_ram[Addr - 0xA000] = Value;
            return;
        }

        if (Addr == 0xC000) return;

        if (Addr == 0xC080) {
            // MAME: sh_irqtrigger_w — latch IRQ for audio Z80 (HOLD_LINE)
            sound_irq_pending = 1;
            return;
        }

        if (Addr == 0xC100) {
            sound_latch = Value;
            return;
        }

        // MAME: LS259 mainlatch at 0xC180-0xC187, each address sets one Q bit
        // Q0=NMI mask, Q2/Q3=coin counters, Q5=flip screen
        // Q1=sub CPU IRQ mask (kept always enabled, game init clears it otherwise)
        if (Addr >= 0xC180 && Addr <= 0xC187) {
            int bit = Addr & 7;
            int bval = Value & 1;
            switch (bit) {
                case 0: irq_enable[0] = bval; break;
                case 5: flip_screen = bval; break;
            }
            return;
        }

        return;
    }
    else {
        if (Addr >= 0x6000 && Addr <= 0x63FF) {
            memory[GYR_ARAM_OFF + (Addr - 0x6000)] = Value;
            return;
        }
    }
}

void gyruss::outZ80(unsigned short Port, unsigned char Value) {
    if (!is_audio_cpu()) return;

    uint8_t port_lo = Port & 0xFF;

    // MAME Gyruss audio IO: 5 AY chips, stride-4 ports
    // AY1: addr=0x00, read=0x01, write=0x02
    // AY2: addr=0x04, read=0x05, write=0x06
    // AY3: addr=0x08, read=0x09, write=0x0a
    // AY4: addr=0x0c, read=0x0d, write=0x0e
    // AY5: addr=0x10, read=0x11, write=0x12
    // 0x14: i8039 IRQ trigger (ignored)
    // 0x18: soundlatch2 (ignored, no 8039)

    if (port_lo > 0x12) return;

    int ay_chip = port_lo / 4;    // 0-4 = AY1-AY5
    int func = port_lo % 4;       // 0=addr, 1=read(ignored), 2=write

    if (func == 0) {
        // Address latch
        ay_address[ay_chip] = Value & 0x0F;
    } else if (func == 2) {
        // Data write — map AY1-3 to soundregs, drop AY4-5
        if (ay_chip < 3) {
            soundregs[(ay_chip * 16) + ay_address[ay_chip]] = Value;
        }
    }
}

unsigned char gyruss::inZ80(unsigned short Port) {
    if (!is_audio_cpu()) return 0xFF;

    uint8_t port_lo = Port & 0xFF;

    // AY data read ports: 0x01(AY1), 0x05(AY2), 0x09(AY3), 0x0d(AY4), 0x11(AY5)
    if (port_lo > 0x12) return 0xFF;
    int ay_chip = port_lo / 4;
    int func = port_lo % 4;

    if (func == 1 && ay_chip < 5) {
        int reg = ay_address[ay_chip];
        // AY3 register 14 (port A) returns a timer value (MAME: porta_r)
        if (ay_chip == 2 && reg == 14) {
            static const uint8_t timer_table[10] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x09, 0x0a, 0x0b, 0x0a, 0x0d
            };
            return timer_table[(audio_cycle_approx / 1024) % 10];
        }
        if (ay_chip < 3) {
            return soundregs[(ay_chip * 16) + reg];
        }
    }

    return 0xFF;
}

// ============================================================
// Frame execution
// ============================================================

void gyruss::run_frame(void) {
    // Start audio task on first frame (now we know which core we're on)
    if (emu_core_id < 0) {
        emu_core_id = xPortGetCoreID();
        start_audio_task();
    }

    // Optimized loop: 500 iterations (less overhead), batched steps
    // Main Z80: 20 * 500 = 10000 steps ≈ 40000 cycles (real: 51200, ~78%)
    // Sub M6809: 10 * 500 = 5000 steps ≈ 20000 cycles (real: 25600, ~78%)
    current_cpu = 0;
    scanline_counter = 0;
    int sc_acc = 0;
    for (int i = 0; i < 500; i++) {
        StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]);
        StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]);
        StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]);
        StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]); StepZ80(&cpu[0]);

        m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu);
        m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu); m6809_step(&sub_cpu);

        sc_acc += 256;
        if (sc_acc >= 500) { sc_acc -= 500; scanline_counter++; }
    }

    // Fire interrupts for next frame
    if (irq_enable[0]) {
        IntZ80(&cpu[0], INT_NMI);
    }

    if (sub_irq_mask) {
        m6809_irq(&sub_cpu);
    }

    // Audio Z80 runs continuously on separate core — no audio code here
    frame_odd ^= 1;
}

// ============================================================
// Sprite preparation
// ============================================================

void gyruss::prepare_frame(void) {
    // Snapshot CRAM+VRAM to prevent tearing in half-rate rendering
    // (emulation may modify memory[] between the two screen halves)
    memcpy(tile_snapshot, &memory[GYR_CRAM_OFF], 0x800);

    active_sprites = 0;

    // Sprite RAM at sub CPU address 0x4040-0x40FF (confirmed from MAME)
    unsigned char *sr = &sub_ram[0x40];

    for (int offs = 0xBC; offs >= 0 && active_sprites < 64; offs -= 4) {
        // MAME bitmap: drawgfx at (sr[offs], 241-sr[offs+3]), 8w×16h
        // ROT90 tile-derived mapping: frame_x = bitmap_y - 16, frame_y = bitmap_x + 16
        // Transposed: ROM row r → frame_x (16 wide), ROM col c → frame_y (8 tall)
        int bx = sr[offs];             // MAME bitmap_x
        int by = 241 - sr[offs + 3];   // MAME bitmap_y
        int gfx_bank = sr[offs + 1] & 1;
        int code = ((sr[offs + 2] & 0x20) << 2) | (sr[offs + 1] >> 1);
        int color = sr[offs + 2] & 0x0f;
        int hw_flip_x = (~sr[offs + 2] >> 6) & 1;  // MAME: ~bit6
        int hw_flip_y = (sr[offs + 2] >> 7) & 1;    // MAME: bit7

        sprite[active_sprites].x = by - 16;      // frame_x start (16 wide)
        sprite[active_sprites].y = bx + 16;      // frame_y start (8 tall)
        sprite[active_sprites].code = code;
        sprite[active_sprites].color = color;
        // ROT90 swaps flip axes: bitmap flip_y → frame flip_x (variant bit 0)
        //                        bitmap flip_x → frame flip_y (variant bit 1)
        sprite[active_sprites].flip_y = hw_flip_y;  // → variant bit 0 (row reversal = frame_x flip)
        sprite[active_sprites].flip_x = hw_flip_x;  // → variant bit 1 (pixel reversal = frame_y flip)
        sprite[active_sprites].color_block = gfx_bank;

        active_sprites++;
    }
}

// ============================================================
// Tile rendering
// ============================================================

void gyruss::blit_tile(short row, char col) {
    // Gyruss uses column-major VRAM with 90-degree CRT rotation
    // tile_reverse_col=1, tile_col_offset=0, tile_row_offset=0
    int vram_row = row - 2;
    int vram_col = 29 - col;  // reversed column order

    if (vram_row < 0 || vram_row >= 32 || vram_col < 0 || vram_col >= 32) return;

    unsigned short vram_addr = (31 - vram_col) * 32 + vram_row;
    unsigned char tile_code_raw = tile_snapshot[GYR_VRAM_OFF + vram_addr];
    unsigned char color_attr = tile_snapshot[GYR_CRAM_OFF + vram_addr];

    unsigned short tile_code = ((color_attr & 0x20) ? 256 : 0) + tile_code_raw;
    if (tile_code >= 512) tile_code = 0;

    unsigned char color_group = color_attr & 0x0F;
    unsigned char flip_x = (color_attr >> 6) & 1;
    unsigned char flip_y = (color_attr >> 7) & 1;

    const unsigned short *tile_data = gyruss_tilemap[tile_code];
    const unsigned short *colors = gyruss_char_colormap[color_group];

    // Preload 4 palette colors from flash to local RAM (4 reads vs 64)
    unsigned short lc[4];
    lc[0] = pgm_read_word(&colors[0]);
    lc[1] = pgm_read_word(&colors[1]);
    lc[2] = pgm_read_word(&colors[2]);
    lc[3] = pgm_read_word(&colors[3]);

    unsigned short *ptr = frame_buffer + 8 * col;

    for (int r = 0; r < 8; r++) {
        int src_r = flip_y ? (7 - r) : r;
        unsigned short pix = pgm_read_word(&tile_data[src_r]);

        if (flip_x) {
            for (int c = 0; c < 8; c++)
                ptr[c] = lc[(pix >> ((7 - c) * 2)) & 3];
        } else {
            for (int c = 0; c < 8; c++)
                ptr[c] = lc[(pix >> (c * 2)) & 3];
        }
        ptr += 224;
    }
}

// ============================================================
// Sprite rendering
// ============================================================

void gyruss::blit_sprite(short row, unsigned char s_idx) {
    struct sprite_S *s = &sprite[s_idx];

    int spr_start_y = s->y;       // frame_y start (8 tall after ROT90)
    int row_pixel_start = row * 8;

    // Transposed: 16 wide (frame_x) × 8 tall (frame_y) after ROT90
    if (spr_start_y >= row_pixel_start + 8 || spr_start_y + 8 <= row_pixel_start)
        return;

    int code = s->code + (s->color_block ? 256 : 0);
    if (code >= 512) code = 0;

    // ROT90 swaps flip axes: flip_y → variant bit 0, flip_x → variant bit 1
    int variant = 0;
    if (s->flip_y) variant |= 1;   // row reversal = frame_x flip
    if (s->flip_x) variant |= 2;   // pixel reversal = frame_y flip

    unsigned char color_group = s->color;
    const unsigned short *colors = gyruss_sprite_colormap[color_group];

    // Preload 16 sprite palette colors from flash to local RAM
    unsigned short lc[16];
    for (int i = 0; i < 16; i++) lc[i] = pgm_read_word(&colors[i]);

    // Pre-compute visible frame_y range within this row strip
    int c_start = (row_pixel_start > spr_start_y) ? (row_pixel_start - spr_start_y) : 0;
    int c_end = ((row_pixel_start + 8) < (spr_start_y + 8)) ? (row_pixel_start + 8 - spr_start_y) : 8;
    int y_base = spr_start_y - row_pixel_start;

    // ROM row r → frame_x offset (0..15), ROM col c → frame_y offset (0..7)
    for (int r = 0; r < 16; r++) {
        int screen_x = s->x + r;
        if (screen_x < 0 || screen_x >= 224) continue;

        unsigned long row_data = pgm_read_dword(&gyruss_sprites[variant][code][r]);

        for (int c = c_start; c < c_end; c++) {
            unsigned char px = (row_data >> (c * 4)) & 0x0F;
            if (px) {
                frame_buffer[(y_base + c) * 224 + screen_x] = lc[px];
            }
        }
    }
}

// ============================================================
// Row rendering
// ============================================================

void gyruss::render_row(short row) {
    if (row < 2 || row >= 34) return;

    for (char col = 0; col < 28; col++) {
        blit_tile(row, col);
    }

    for (int s = 0; s < active_sprites; s++) {
        blit_sprite(row, s);
    }

}

// ============================================================
// Logo
// ============================================================

const unsigned short *gyruss::logo(void) {
    return gyruss_logo;
}

#ifdef LED_PIN
void gyruss::menuLeds(CRGB *leds) {
    memcpy(leds, menu_leds, NUM_LEDS * sizeof(CRGB));
}
void gyruss::gameLeds(CRGB *leds) {
    memcpy(leds, menu_leds, NUM_LEDS * sizeof(CRGB));
}
#endif
