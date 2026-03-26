#include "emulation/audio.h"
#include "machines/spaceinvaders/spaceinvaders_samples.h"

void Audio::init() {
  // 24 kHz @ 16 bit = 48000 bytes/sec = 800 bytes per 60hz game frame =
  // 1600 bytes per 30hz screen update = ~177 bytes every four tile rows
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 24000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
#ifdef SND_DIFF
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#elif defined(SND_LEFT_CHANNEL) // For devices using the left channel (e.g. CYD)
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
#else
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
#endif
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = 64,   // 64 samples
    // APLL usage is broken in ESP-IDF 4.4.5
#ifdef WORKAROUND_I2S_APLL_PROBLEM
    .use_apll = false
#else
    .use_apll = true
#endif
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

#ifdef SND_DIFF
  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#elif defined(SND_LEFT_CHANNEL) // For devices using the left channel (e.g. CYD)
  i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN); 
#else
  i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
#endif  

  generateSinusWave(256, sinusWaveBuffer, sizeof(sinusWaveBuffer)  / 2 );
}

void Audio::start(machineBase *machineBase) {
  currentMachine = machineBase;
  machineType = currentMachine->machineType();
    
#ifndef WORKAROUND_I2S_APLL_PROBLEM
  // The audio CPU of donkey kong runs at 6Mhz...
  signed char machineType = currentMachine->machineType();
#ifdef ENABLE_DKONG  
  i2s_set_sample_rates(I2S_NUM_0, machineType == MCH_DKONG ? 11765 : 24000);
#else
  i2s_set_sample_rates(I2S_NUM_0, 24000);
#endif
#endif
}

void Audio::volumeUpDown(bool up, bool down) {
  if (up && !volumeUpLast) {
    if (volumeSetting > 1)
      volumeSetting--;
  }
  volumeUpLast = up;

  if (down && !volumeDownLast) {
    if (volumeSetting < 30)
      volumeSetting++;
  }
  volumeDownLast = down;
}

void Audio::transmit() {
  // (try to) transmit as much audio data as possible. Since we
  // write data in exact the size of the DMA buffers we can be sure
  // that either all or nothing is actually being written
  size_t bytesOut = 0;
  do {
    // copy data in i2s dma buffer if possible
    i2s_write(I2S_NUM_0, snd_buffer, sizeof(snd_buffer), &bytesOut, 0);

    // render the next audio chunk if data has actually been sent
    if(bytesOut) {      
      if (currentMachine->hasNamcoAudio())
        namco_render_buffer();
#if defined(ENABLE_MRDO) || defined(ENABLE_LADYBUG)
      else if(machineType == MCH_MRDO || machineType == MCH_LADYBUG)
        sn76489_render_buffer();
#endif
#ifdef ENABLE_BAGMAN
      else if(machineType == MCH_BAGMAN)
        discrete_render_buffer();
#endif
#ifdef ENABLE_GALAXIAN
      else if(machineType == MCH_GALAXIAN)
        galaxian_render_buffer();
#endif
#ifdef ENABLE_SPACE
      else if(machineType == MCH_SPACE)
        spaceinvaders_render_buffer();
#endif
      else
        ay_render_buffer();
    }
  } while(bytesOut);
}

void Audio::ay_render_buffer(void) {
  char AY = 2; 
  char AY_INC = 8;
  char AY_VOL = 5;

#ifdef ENABLE_FROGGER
  if (machineType == MCH_FROGGER) { AY = 1; AY_INC = 9; AY_VOL = 11; }
#endif
#ifdef ENABLE_ANTEATER
  if (machineType == MCH_ANTEATER) { AY = 2; AY_INC = 9; AY_VOL = 5; }
#endif
#ifdef ENABLE_TIMEPLT
  if (machineType == MCH_TIMEPLT) { AY = 2; AY_INC = 9; AY_VOL = 5; }
#endif
#ifdef ENABLE_BOMBJACK
  if (machineType == MCH_BOMBJACK) { AY = 3; AY_INC = 8; AY_VOL = 10; }
#endif
#ifdef ENABLE_GYRUSS
  if (machineType == MCH_GYRUSS) { AY = 5; AY_INC = 9; AY_VOL = 3; }
#endif
#ifdef ENABLE_TUTANKHM
  if (machineType == MCH_TUTANKHM) { AY = 2; AY_INC = 7; AY_VOL = 5; }
#endif


  for(char ay = 0; ay < AY; ay++) {
    int ay_off = 16 * ay;
    for(char c = 0; c < 3; c++) {
      ay_period[ay][c] = currentMachine->soundregs[ay_off + 2 * c] + 256 * (currentMachine->soundregs[ay_off + 2 * c + 1] & 15);
      ay_enable[ay][c] = (((currentMachine->soundregs[ay_off + 7] >> c) & 1) | ((currentMachine->soundregs[ay_off + 7] >> (c + 2)) & 2)) ^ 3;
      ay_volume[ay][c] = currentMachine->soundregs[ay_off + 8 + c] & 0x0f;
    }
    ay_period[ay][3] = currentMachine->soundregs[ay_off + 6] & 0x1f;

#ifdef ENABLE_BOMBJACK
    if (machineType == MCH_BOMBJACK) {
      ay_envelope_period[ay] = currentMachine->soundregs[ay_off + 11] + 256 * currentMachine->soundregs[ay_off + 12];
      uint8_t new_shape = currentMachine->soundregs[ay_off + 13];
      if (new_shape != ay_envelope_shape[ay]) {
        ay_envelope_shape[ay] = new_shape & 0x0F;
        ay_envelope_counter[ay] = 0;
        ay_envelope_step[ay] = (ay_envelope_shape[ay] < 4 || (ay_envelope_shape[ay] >= 8 && ay_envelope_shape[ay] < 12)) ? 0 : 15;
        ay_envelope_holding[ay] = 0;
      }
    }
#endif
  }

  for(int i = 0; i < 64; i++) {
    short value = 0;

#ifdef ENABLE_DKONG
    if(machineType == MCH_DKONG) {
      dkong *dkongMachine = (dkong*)currentMachine;
      if(dkongMachine->dkong_audio_rptr != dkongMachine->dkong_audio_wptr) {
#ifdef WORKAROUND_I2S_APLL_PROBLEM
        value = dkongMachine->dkong_audio_transfer_buffer[dkongMachine->dkong_audio_rptr][(dkongMachine->dkong_obuf_toggle ? 32 : 0) + (i / 2)];
#else
        value = dkongMachine->dkong_audio_transfer_buffer[dkongMachine->dkong_audio_rptr][i];
#endif
      }
      for(char j = 0; j < 3; j++) {
        if(dkongMachine->dkong_sample_cnt[j]) {
#ifdef WORKAROUND_I2S_APLL_PROBLEM
          value += *dkongMachine->dkong_sample_ptr[j] >> (2 - j); 
          if(i & 1) { dkongMachine->dkong_sample_ptr[j]++; dkongMachine->dkong_sample_cnt[j]--; }
#else
          value += *dkongMachine->dkong_sample_ptr[j]++ >> (2 - j); 
          dkongMachine->dkong_sample_cnt[j]--;
#endif
        }
      }
#ifdef WORKAROUND_I2S_APLL_PROBLEM
      if (i == 63) {
        if(dkongMachine->dkong_obuf_toggle) dkongMachine->dkong_audio_rptr = (dkongMachine->dkong_audio_rptr + 1) & DKONG_AUDIO_QUEUE_MASK;
        dkongMachine->dkong_obuf_toggle = !dkongMachine->dkong_obuf_toggle;
      }
#endif
    } else 
#endif
    if(
#ifdef ENABLE_FROGGER
       machineType == MCH_FROGGER || 
#endif
#ifdef ENABLE_1942
       machineType == MCH_1942 || 
#endif
#ifdef ENABLE_ANTEATER
       machineType == MCH_ANTEATER || 
#endif
#ifdef ENABLE_TIMEPLT
       machineType == MCH_TIMEPLT || 
#endif
#ifdef ENABLE_GYRUSS
       machineType == MCH_GYRUSS ||
#endif
#ifdef ENABLE_TUTANKHM
       machineType == MCH_TUTANKHM ||
#endif
       false
    ) {    
      for(char ay = 0; ay < AY; ay++) {
        if(ay_period[ay][3]) {
          audio_cnt[ay][3] += AY_INC;
          if(audio_cnt[ay][3] > ay_period[ay][3]) {
            audio_cnt[ay][3] -= ay_period[ay][3];
            ay_noise_rng[ay] ^= (((ay_noise_rng[ay] & 1) ^ ((ay_noise_rng[ay] >> 3) & 1)) << 17);
            ay_noise_rng[ay] >>= 1;
          }
        }
        for(char c = 0; c < 3; c++) {
          if(ay_period[ay][c] && ay_volume[ay][c] && ay_enable[ay][c]) {
            short bit = 1;
            if(ay_enable[ay][c] & 1) bit &= (audio_toggle[ay][c] > 0) ? 1 : 0;
            if(ay_enable[ay][c] & 2) bit &= (ay_noise_rng[ay] & 1) ? 1 : 0;
            if(bit == 0) bit = -1;
            value += AY_VOL * bit * ay_volume[ay][c];
            audio_cnt[ay][c] += AY_INC;
            if(audio_cnt[ay][c] > ay_period[ay][c]) {
              audio_cnt[ay][c] -= ay_period[ay][c];
              audio_toggle[ay][c] = -audio_toggle[ay][c];
            }
          }
        }
      }
    }
#ifdef ENABLE_BOMBJACK
    else if (machineType == MCH_BOMBJACK) {
      for(char ay = 0; ay < AY; ay++) {
        if (!ay_envelope_holding[ay] && ay_envelope_period[ay] > 0) {
          ay_envelope_counter[ay] += AY_INC;
          if (ay_envelope_counter[ay] >= ay_envelope_period[ay]) {
            ay_envelope_counter[ay] -= ay_envelope_period[ay];
            if (ay_envelope_shape[ay] < 8) {
              ay_envelope_step[ay]++;
              if (ay_envelope_step[ay] > 15) {
                ay_envelope_step[ay] = (ay_envelope_shape[ay] & 1) ? 0 : 15; 
                if (ay_envelope_shape[ay] & 2) ay_envelope_holding[ay] = 1; 
              }
            } else {
              ay_envelope_step[ay]--;
              if (ay_envelope_step[ay] < 0) {
                ay_envelope_step[ay] = (ay_envelope_shape[ay] & 1) ? 15 : 0; 
                if (ay_envelope_shape[ay] & 2) ay_envelope_holding[ay] = 1; 
              }
            }
          }
        }
        if(ay_period[ay][3]) {
          audio_cnt[ay][3] += AY_INC;
          if(audio_cnt[ay][3] > ay_period[ay][3]) {
            audio_cnt[ay][3] -= ay_period[ay][3];
            uint32_t b = (((ay_noise_rng[ay] >> 0) ^ (ay_noise_rng[ay] >> 3)) & 1);
            ay_noise_rng[ay] = (ay_noise_rng[ay] >> 1) | (b << 16);
          }
        }
        for(char c = 0; c < 3; c++) {
          if(ay_period[ay][c] && ay_enable[ay][c]) {
            int cur_v = (ay_volume[ay][c] & 0x10) ? ay_envelope_step[ay] : (ay_volume[ay][c] & 0x0F);
            if (cur_v > 0) {
              short bit = 1;
              if(ay_enable[ay][c] & 1) bit &= (audio_toggle[ay][c] > 0) ? 1:0;
              if(ay_enable[ay][c] & 2) bit &= (ay_noise_rng[ay] & 1) ? 1:0;
              if(bit == 0) bit = -1;
              value += AY_VOL * bit * cur_v;
            }
            audio_cnt[ay][c] += AY_INC;
            if(audio_cnt[ay][c] > ay_period[ay][c]) {
              audio_cnt[ay][c] -= ay_period[ay][c];
              audio_toggle[ay][c] = -audio_toggle[ay][c];
            }
          }
        }
      }
      value = value / 3;
    }
#endif
    valueToBuffer(i, value);
  }
}

void Audio::sn76489_render_buffer(void) {
    int sn_inc = 11;  // SN_CLOCK / SAMPLE_RATE

    static uint32_t noise_lfsr[2] = {0x4000, 0x4000};

    // Volumi con hold
    int vol[2][4];
    for (int chip = 0; chip < 2; chip++) {
        for (int c = 0; c < 4; c++) {
            if (currentMachine->sn_hold[chip][c] > 0) {
                vol[chip][c] = currentMachine->sn_min_volume[chip][c];
                currentMachine->sn_hold[chip][c]--;
                if (currentMachine->sn_hold[chip][c] == 0)
                    currentMachine->sn_min_volume[chip][c] = currentMachine->sn_volume[chip][c];
            } else {
                vol[chip][c] = currentMachine->sn_volume[chip][c];
                currentMachine->sn_min_volume[chip][c] = currentMachine->sn_volume[chip][c];
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        short sample = 0;

        for (int chip = 0; chip < 2; chip++) {
            for (int c = 0; c < 4; c++) {
                int period = currentMachine->sn_period[chip][c];

                if (vol[chip][c] < 15 && period > 0) {
                    sn_counter[chip][c] -= sn_inc;

                    while (sn_counter[chip][c] <= 0) {
                        if (c == 3) {  // Noise channel
                            uint32_t feedback = (noise_lfsr[chip] & 0x01) ^ ((noise_lfsr[chip] & 0x02) >> 1);
                            noise_lfsr[chip] = (noise_lfsr[chip] >> 1) | (feedback << 14);
                            sn_toggle[chip][c] = (noise_lfsr[chip] & 0x01) ? 1 : -1;
                            sn_counter[chip][c] += (period << 3);  // Rallenta noise
                        } else {  // Tone
                            sn_counter[chip][c] += period;
                            sn_toggle[chip][c] = -sn_toggle[chip][c];
                        }
                    }
                    sample += sn_toggle[chip][c] * (15 - vol[chip][c]) * 6;
                }
            }
        }
        valueToBuffer(i, sample);
    }
}


void Audio::namco_render_buffer(void) {
  // parse all three wsg channels
  for(char ch = 0; ch < 3; ch++) {  
    snd_wave[ch] = currentMachine->waveRom(currentMachine->soundregs[ch * 5 + 0x05] & 0x07);
    snd_freq[ch] = (ch == 0) ? currentMachine->soundregs[0x10] : 0; //5050-5054, 5056-5059, 505b-505e
    snd_freq[ch] += currentMachine->soundregs[ch * 5 + 0x11] << 4;
    snd_freq[ch] += currentMachine->soundregs[ch * 5 + 0x12] << 8;
    snd_freq[ch] += currentMachine->soundregs[ch * 5 + 0x13] << 12;
    snd_freq[ch] += currentMachine->soundregs[ch * 5 + 0x14] << 16;        
    snd_volume[ch] = currentMachine->soundregs[ch * 5 + 0x15]; //5055, 505a, 505f
  }

  // render first buffer contents
  for(int i = 0; i < 64; i++) {
    short value = 0;

    // add up to three wave signals
    if(snd_volume[0]) value += snd_volume[0] * snd_wave[0][(snd_cnt[0] >> 13) & 0x1f];
    if(snd_volume[1]) value += snd_volume[1] * snd_wave[1][(snd_cnt[1] >> 13) & 0x1f];
    if(snd_volume[2]) value += snd_volume[2] * snd_wave[2][(snd_cnt[2] >> 13) & 0x1f];

    snd_cnt[0] += snd_freq[0];
    snd_cnt[1] += snd_freq[1];
    snd_cnt[2] += snd_freq[2];
    
    if(machineType == MCH_GALAGA) {
      //galaga *galagaMachine = dynamic_cast<galaga*>(currentMachine);
      galaga *galagaMachine = (galaga*)currentMachine;

      if(galagaMachine->snd_boom_cnt) {
        value += *galagaMachine->snd_boom_ptr * 3;
        
        if(galagaMachine->snd_boom_cnt & 1) 
          galagaMachine->snd_boom_ptr++;
        
        galagaMachine->snd_boom_cnt--;
      }
    }
    valueToBuffer(i, value);
  }
}

void Audio::spaceinvaders_render_buffer(void) {
  // Space Invaders discrete audio (based on MAME mw8080bw_a.cpp)
  //
  // soundregs[0] = port 3: UFO(0) Shot(1) Explosion(2) InvaderDie(3) ExtPlay(4)
  // soundregs[1] = port 5: Fleet1(0) Fleet2(1) Fleet3(2) Fleet4(3) UFOhit(4)

  uint8_t p3 = currentMachine->soundregs[0];
  uint8_t p5 = currentMachine->soundregs[1];

  // Fleet: pick highest active bit → tone frequency (Hz)
  // Original hardware: 555 timer ~33-55Hz, doubled for small speaker audibility
//  const int fleet_freq[4] = { 66, 110, 80, 74 };
  const int fleet_freq[4] = { 37, 55, 48, 41};
  int fleet_f = 0;
  for(int b = 3; b >= 0; b--) {
    if(p5 & (1 << b)) { fleet_f = fleet_freq[b]; break; }
  }

  for(int i = 0; i < 64; i++) {
    short value = 0;

    // ── Advance noise LFSR: 17-bit, taps 4+16, clock 7515 Hz ──
    si_noise_clock += 7515;
    while(si_noise_clock >= 24000) {
      si_noise_clock -= 24000;
      int bit = ((si_noise_rng >> 4) ^ (si_noise_rng >> 16)) & 1;
      si_noise_rng = ((si_noise_rng << 1) | bit) & 0x1FFFF;
      si_noise_out = (si_noise_rng >> 12) & 1;
    }

    // ── UFO: SN76477 – SLF triangle ~5.3Hz modulates VCO 1220-3700Hz ──
    if(p3 & 0x01) {
      // SLF triangle: full cycle = 24000/5.3 ≈ 4528 samples
      si_ufo_sweep = (si_ufo_sweep + 1) % 4528;
      int slf_pos = (si_ufo_sweep < 2264) ? si_ufo_sweep : (4528 - si_ufo_sweep);
      int vco_freq = 1220 + (int)((long)slf_pos * 2480 / 2264);
      // VCO square wave: counter += freq, toggle at 12000 (= 24kHz/2)
      si_ufo_cnt += vco_freq;
      if(si_ufo_cnt >= 12000) {
        si_ufo_cnt -= 12000;
        si_ufo_toggle = -si_ufo_toggle;
      }
      value += si_ufo_toggle * 60;
    } else {
      si_ufo_sweep = 0;
    }

    // ── SHOT: original sample playback (12kHz samples, play each twice for 24kHz) ──
    if(p3 & 0x02) {
      if(!si_shot_playing) { si_shot_playing = 1; si_shot_pos = 0; si_shot_toggle = 0; }
      if((si_shot_pos >> 1) < si_sample_shot_LEN) {
        value += (signed char)pgm_read_byte(&si_sample_shot[si_shot_pos >> 1]) * 3;
        si_shot_pos++;
      }
    } else {
      si_shot_playing = 0;
    }

    // ── COIN INSERT: metallic clink (triggered via soundregs[2]) ──
    if(currentMachine->soundregs[2] && si_coin_timer == 0) {
      si_coin_timer = 360;  // ~15ms
      si_coin_env = 120;
      currentMachine->soundregs[2] = 0;
    }
    if(si_coin_timer > 0) {
      // Primary metallic tone: 4500Hz
      si_coin_cnt += 4500;
      if(si_coin_cnt >= 12000) {
        si_coin_cnt -= 12000;
        si_coin_toggle = -si_coin_toggle;
      }
      // Overtone for metallic ring: 9500Hz
      si_coin_cnt2 += 9500;
      if(si_coin_cnt2 >= 12000) {
        si_coin_cnt2 -= 12000;
        si_coin_toggle2 = -si_coin_toggle2;
      }
      value += (si_coin_toggle * si_coin_env + si_coin_toggle2 * (si_coin_env / 2)) / 2;
      si_coin_timer--;
      if((si_coin_timer % 12) == 0 && si_coin_env > 5) si_coin_env--;
    }

    // ── EXPLOSION: noise burst with slow decay (RC ~2.7s) ──
    if(p3 & 0x04) {
      if(si_explo_env == 0) si_explo_env = 120;  // init on trigger
      int noise = si_noise_out ? 1 : -1;
      value += noise * si_explo_env;
      // Slow decay: decrease envelope every ~50 samples (~2ms)
      si_explo_cnt++;
      if(si_explo_cnt >= 50) {
        si_explo_cnt = 0;
        if(si_explo_env > 15) si_explo_env--;
      }
    } else {
      si_explo_env = 0;
      si_explo_cnt = 0;
    }

    // ── INVADER DIE: original sample playback (12kHz samples, play each twice for 24kHz) ──
    if(p3 & 0x08) {
      if(!si_invhit_playing) { si_invhit_playing = 1; si_invhit_pos = 0; }
      if((si_invhit_pos >> 1) < si_sample_invhit_LEN) {
        value += (signed char)pgm_read_byte(&si_sample_invhit[si_invhit_pos >> 1]) * 3;
        si_invhit_pos++;
      }
    } else {
      si_invhit_playing = 0;
    }

    // ── FLEET MOVEMENT: low bass tone while any fleet bit set ──
    if(fleet_f > 0) {
      si_fleet_cnt += fleet_f;
      if(si_fleet_cnt >= 12000) {
        si_fleet_cnt -= 12000;
        si_fleet_toggle = -si_fleet_toggle;
      }
      value += si_fleet_toggle * 90;
    }

    // ── UFO HIT: descending warble tone ~2000Hz with ~15Hz modulation ──
    if(p5 & 0x10) {
      if(si_ufohit_freq == 0) si_ufohit_freq = 2000;  // init on trigger
      // Warble modulation at ~15Hz: amplitude ±200Hz
      si_ufohit_warble = (si_ufohit_warble + 1) % 1600;  // 24000/15 = 1600
      int warble_pos = (si_ufohit_warble < 800) ?
        (int)si_ufohit_warble : (int)(1600 - si_ufohit_warble);
      int mod_freq = si_ufohit_freq + (warble_pos * 400 / 800 - 200);
      if(mod_freq < 100) mod_freq = 100;
      si_ufohit_cnt += mod_freq;
      if(si_ufohit_cnt >= 12000) {
        si_ufohit_cnt -= 12000;
        si_ufohit_toggle = -si_ufohit_toggle;
      }
      value += si_ufohit_toggle * 80;
      // Descend (~2000→300 over ~1.5s = 36000 samples)
      if(si_ufohit_freq > 300) si_ufohit_freq--;
    } else {
      si_ufohit_freq = 0;
      si_ufohit_warble = 0;
    }

    // Clamp
    if(value > 500) value = 500;
    if(value < -500) value = -500;

    valueToBuffer(i, value);
  }
}


void Audio::galaxian_render_buffer(void) {
  // Galaxian discrete sound hardware emulation (MAME galaxian_a.cpp)
  // SOUND_CLOCK = 18.432MHz/6/2 = 1.536MHz
  //
  // soundregs[0]    = VCO pitch (8-bit, written at 0x7800)
  // soundregs[1-4]  = LFO DAC bits (4-bit, 0x6004-0x6007)
  // soundregs[8-10] = FS1/FS2/FS3 background tone enables (0x6800-0x6802)
  // soundregs[11]   = HIT noise enable (0x6803)
  // soundregs[12]   = (unused, offset 4 not wired)
  // soundregs[13]   = FIRE shoot enable (0x6805, offset 5)
  // soundregs[14]   = VOL1 (0x6806, offset 6)
  // soundregs[15]   = VOL2 (0x6807, offset 7)
  // NOTE: No BGEN register — VCO is always active when pitch is audible

  int vco_pitch = currentMachine->soundregs[0];

  // VOL1/VOL2 control VCO output volume via resistor network
  int vol1 = currentMachine->soundregs[14];  // offset 6
  int vol2 = currentMachine->soundregs[15];  // offset 7
  int vco_vol = (vol1 || vol2) ? (20 + vol1 * 20 + vol2 * 20) : 25;

  // VCO half-period: freq = 1.536MHz / (16*(256-pitch))
  // At 24kHz: half_period = (256-pitch) / 8
  int half_period = (256 - vco_pitch) / 8;

  // Detect pitch sweeps (credit sound): VCO plays through R34 base path
  // when pitch is actively changing even without VOL1/VOL2
  static int gal_last_pitch = 0xFF;
  static int gal_pitch_active = 0;
  if(vco_pitch != gal_last_pitch) {
    gal_pitch_active = 500;  // sustain ~20ms (just over 1 frame)
    gal_last_pitch = vco_pitch;
  }
  if(gal_pitch_active > 0) gal_pitch_active--;

  // VCO plays when: VOL is on (normal sounds) OR pitch is sweeping (credit sound)
  bool vco_on = (half_period > 1) && (vol1 || vol2 || gal_pitch_active > 0);

  // FS1/FS2/FS3: 555 timer tones (frequencies from RC values)
  // FS1 ~130Hz, FS2 ~170Hz, FS3 ~230Hz
  // Half-periods at 24kHz sample rate
  static const int fs_period[3] = {92, 71, 52};

  for(int i = 0; i < 64; i++) {
    short value = 0;

    // === VCO tone ===
    if(vco_on) {
      gal_tone_cnt++;
      if(gal_tone_cnt >= (unsigned long)half_period) {
        gal_tone_cnt = 0;
        gal_tone_toggle = -gal_tone_toggle;
      }
      value += gal_tone_toggle * vco_vol;
    }
/*

    // === FS1, FS2, FS3: background march tones (independent volume) ===
    for(int fs = 0; fs < 3; fs++) {
      if(currentMachine->soundregs[8 + fs]) {
        gal_fs_cnt[fs]++;
        if(gal_fs_cnt[fs] >= (unsigned long)fs_period[fs]) {
          gal_fs_cnt[fs] = 0;
          gal_fs_toggle[fs] = -gal_fs_toggle[fs];
        }
        value += gal_fs_toggle[fs] * 25;
      }
    }
*/
    // === HIT: explosion noise (LFSR, bandpass ~470Hz) ===
    if(currentMachine->soundregs[11]) {
      uint32_t b = ((gal_noise_rng >> 0) ^ (gal_noise_rng >> 3)) & 1;
      gal_noise_rng = (gal_noise_rng >> 1) | (b << 16);
      value += ((gal_noise_rng & 1) ? 90 : -90);
    }

    // === FIRE: shooting sound (offset 5, 555 astable ~2.7kHz + noise) ===
    if(currentMachine->soundregs[13]) {
      gal_fire_cnt++;
      if(gal_fire_cnt >= 9) {
        gal_fire_cnt = 0;
        uint32_t b = ((gal_fire_rng >> 0) ^ (gal_fire_rng >> 3)) & 1;
        gal_fire_rng = (gal_fire_rng >> 1) | (b << 16);
      }
      value += ((gal_fire_rng & 1) ? 70 : -70);
    }

    valueToBuffer(i, value);
  }
}




void Audio::generateSinusWave(int32_t amplitude, short* buffer, uint16_t length) { 
  for (int i=0; i<length; ++i) {
    buffer[i] = int32_t(float(amplitude) * sin(2.0 * PI * (1.0 / length) * i));
  }
}

void Audio::discrete_render_buffer() {
  unsigned short duration = currentMachine->soundregs[0] + (currentMachine->soundregs[1] << 8);

  if (duration > 0)
    duration--;

  currentMachine->soundregs[0] = duration & 0x00ff;
  currentMachine->soundregs[1] = (duration & 0xff00) > 8;

  float frequency;
  switch (currentMachine->soundregs[2]) {
    case 0x3: frequency = A5_3; break;
    case 0x4: frequency = C6_4; break;
    case 0x5: frequency = F5_5; break;
    case 0x6: frequency = G5_6; break;
    case 0x7: frequency = E6_7; break;
    case 0x8: frequency = B6_8; break;
    case 0xE: frequency = D6_E; break;
    case 0xF: frequency = B5_F; break;
    case 0xB: frequency = XX_B; break;
  }

  unsigned short pause = currentMachine->soundregs[3];
  if (pause > 0)
    currentMachine->soundregs[3]--;

  float delta = 0;
  if (duration != 0 && pause == 0)
    delta = (frequency * (sizeof(sinusWaveBuffer) / 2)) / float(24000);
  
  for(int i = 0; i < 64; i++) {
    uint16_t pos = uint32_t(((i + 1) * delta) + positionLast) % (sizeof(sinusWaveBuffer) / 2);
    short value = sinusWaveBuffer[pos];

    if (i == 63)
      positionLast = pos;

    valueToBuffer(i, value);
  }
}

void Audio::valueToBuffer(int index, short value) {
    // value is now in the range of +/- 512, so expand to +/- 15 bit
    value = value * 64;

#ifdef SND_DIFF
    // generate differential output
    snd_buffer[2 * index]   = 0x8000 + (volume / volumeSetting);    // positive signal on GPIO26
    snd_buffer[2 * index + 1] = 0x8000 - (volume / volumeSetting);    // negatve signal on GPIO25 
#else
    // work-around weird byte order bug, see 
    // https://github.com/espressif/arduino-esp32/issues/8467#issuecomment-1656616015
    snd_buffer[index ^ 1]   = 0x8000 + (value / volumeSetting); 
#endif
}