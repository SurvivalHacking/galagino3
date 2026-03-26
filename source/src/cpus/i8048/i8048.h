#ifndef _I8048_H_
#define _I8048_H_

#ifdef __cplusplus
extern "C" {
#endif

// Use i8048_bool instead of boolean to avoid conflict with Bluepad32
typedef char i8048_bool;

// true/false might already be defined by Arduino/C++
#ifndef true
  #define true 1
#endif
#ifndef false
  #define false 0
#endif

// Bits in PSW
#define CY_BIT  7
#define AC_BIT  6
#define F0_BIT  5
#define BS_BIT  4

#define REGISTER_BANK_0_BASE 0
#define REGISTER_BANK_1_BASE 24

struct i8048_state_S {
  // Interrupt pins and flipflops
  i8048_bool TF; // Timer Flag
  i8048_bool notINT;
  i8048_bool timerInterruptRequested;
  i8048_bool T0;
  i8048_bool T1;

  unsigned char T;
  unsigned char A;
  unsigned short PC;
  unsigned char PSW;
  i8048_bool DBF;
  i8048_bool F1;

  i8048_bool externalInterruptsEnabled;
  i8048_bool tcntInterruptsEnabled;
  i8048_bool counterRunning; // Whether counter is bound to T1 (STRT CNT)
  i8048_bool timerRunning; // Whether counter is bound to clock (STRT T)
  long cyclesUntilCount; // prescaler: Number of cycles until we need to increment the count (if counter is bound to clock)
  i8048_bool inInterrupt; // True if handling an interrupt. Reset by RETR

  unsigned char ram[128];

  int p2_state;
};

void i8048_reset(struct i8048_state_S *state);
void i8048_step(struct i8048_state_S *state);

unsigned char i8048_rom_read(struct i8048_state_S *state, unsigned short addr);

// ----- functions to be provided externally -----
void i8048_port_write(struct i8048_state_S *, unsigned char, unsigned char);
unsigned char i8048_port_read(struct i8048_state_S *, unsigned char);

unsigned char i8048_xdm_read(struct i8048_state_S *, unsigned char);
void i8048_xdm_write(struct i8048_state_S *, unsigned char, unsigned char);

#ifdef __cplusplus
}
#endif

#endif // _I8048_H_
