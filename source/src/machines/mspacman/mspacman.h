#ifndef MSPACMAN_H
#define MSPACMAN_H

#ifndef ENABLE_MSPACMAN
#error "ENABLE_MSPACMAN mancante nel config.h!"
#endif

// pacman.h includes pacman_cmap.h, pacman_wavetable.h, pacman_dipswitches.h,
// tileaddr.h and machineBase.h — include it first to avoid duplicate symbols.
#include "../pacman/pacman.h"
#include "mspacman_pacrom.h"
#include "mspacman_auxrom.h"
#include "mspacman_tilemap.h"
#include "mspacman_spritemap.h"
#include "mspacman_logo.h"

class mspacman : public pacman
{
public:
  mspacman() { }
  ~mspacman() { }

  signed char machineType() override { return MCH_MSPACMAN; }
  void reset() override;
  unsigned char rdZ80(unsigned short Addr) override;
  void wrZ80(unsigned short Addr, unsigned char Value) override;
  unsigned char opZ80(unsigned short Addr) override;
  const unsigned short *logo(void) override;

protected:
  const unsigned short *tileRom(unsigned short addr) override;
  const unsigned long  *spriteRom(unsigned char flags, unsigned char code) override;

private:
  bool decode; // true = Ms. Pac-Man mode, false = original Pac-Man mode
};

#endif
