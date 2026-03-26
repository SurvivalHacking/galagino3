#ifndef _timeplt_dipswitches_h_
#define _timeplt_dipswitches_h_

// Time Pilot DIP switches (MAME defaults are active-high)

// DSW1 (read at 0xC360) - Coinage
// bits 0-3: Coin A (0x0F = 1 coin 1 play)
// bits 4-7: Coin B (0xF0 = 1 coin 1 play)
// 0x00 = FREE PLAY!
#define TIMEPLT_DSW1  0xFF  // 1 coin 1 play both slots

// DSW2 (read at 0xC200)
// bits 0-1: Lives (0x03=3, 0x02=4, 0x01=5, 0x00=255)
// bit 2: Cabinet (0x00=Upright, 0x04=Cocktail)
// bit 3: Bonus (0x08=10K/50K, 0x00=20K/60K)
// bits 4-6: Difficulty (0x40=normal)
// bit 7: Demo sounds (0x00=ON, 0x80=OFF)
#define TIMEPLT_DSW2  0x49  // 5 lives, upright, bonus 10K/50K, normal, demo sounds ON

// IN0 at 0xC300: coins, starts
// IN1 at 0xC320: P1 controls
// IN2 at 0xC340: P2 controls

#endif
