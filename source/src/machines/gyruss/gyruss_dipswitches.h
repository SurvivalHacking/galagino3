// Gyruss DIP Switch configuration
// Based on MAME driver settings

// DSW0 (read at 0xC0E0)
// Bits 0-3: Coin A  (0x0F = 1 coin/1 credit)
// Bits 4-7: Coin B  (0xF0 = 1 coin/1 credit)
#define GYRUSS_DSW0  0xFF

// DSW1 (read at 0xC000)
// Bits 0-1: Lives (00=3, 01=4, 10=5, 11=255)
// Bit 2: Cabinet (0=upright, 1=cocktail)
// Bit 3: Bonus life (0=30K/90K, 1=40K/110K) combined with DSW2 bit
// Bit 4-5: Difficulty (00=easy, 01=normal, 10=hard, 11=hardest)
// Bit 6: Demo sounds (0=off, 1=on)
// Bit 7: Freeze (0=off, 1=on)
#define GYRUSS_DSW1  0x69  // 5 lives, upright, 30K bonus, normal diff, demo sounds on

// DSW2 (read at 0xC100)
// Bit 0: Bonus life extra (combined with DSW1 bit 3)
// Other bits: not used in gyrussk
#define GYRUSS_DSW2  0xFE
