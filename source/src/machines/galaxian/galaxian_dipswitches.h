#ifndef _galaxian_dipswitches_h_
#define _galaxian_dipswitches_h_

// Galaxian DIP switches (galmidw ROM set)
// IN2 bits 0-1: Bonus life
#define GALAXIAN_DIP_BONUS_NONE   0b00000000
#define GALAXIAN_DIP_BONUS_4000   0b00000001
#define GALAXIAN_DIP_BONUS_5000   0b00000010
#define GALAXIAN_DIP_BONUS_7000   0b00000011

// IN2 bit 2: Lives (galmidw: bit2=0 → 3 lives, bit2=1 → 5 lives)
#define GALAXIAN_DIP_LIVES_3      0b00000000
#define GALAXIAN_DIP_LIVES_5      0b00000100

// IN2 bit 6: Coinage
#define GALAXIAN_DIP_COIN_1C1P    0b00000000
#define GALAXIAN_DIP_COIN_2C1P    0b01000000

// IN1 bit 6: Cabinet (MAME: 0x40=Upright, 0x00=Cocktail)
#define GALAXIAN_DIP_UPRIGHT      0b01000000
#define GALAXIAN_DIP_COCKTAIL     0b00000000

#define GALAXIAN_DIP_IN1  (GALAXIAN_DIP_UPRIGHT)
#define GALAXIAN_DIP_IN2  (GALAXIAN_DIP_BONUS_7000 | GALAXIAN_DIP_LIVES_5 | GALAXIAN_DIP_COIN_1C1P)  // 5 lives, bonus at 7000, 1 coin/play

#endif
