/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNEL2BYTES5_H 
#define TILEBYTESTREAM_TILERAWCHANNEL2BYTES5_H 
// This class converts the TileRawChannel object into bytes
// as it is defined at the commissioning 2007
// in one 32-bit word, 1 bit for the gain, 15 for the energy,
// 11 bits for time and 4+1 bits for quality factor and energy 
// trigger flag

/**
 *
 * @class TileRawChannel2Bytes5
 * @brief Converts the TileRawChannel object into bytes as it

 * Compressed Frag #5 In one 32-bit word<p>
*/

#include <vector>
#include <stdio.h>
#include <stdint.h>


// Amplitude encoding: 15 bits
// Value = amp*factor + offset
// amp = (Value - offset)/factor
// the offset depends on the gain
// the factor depends on the units
const int AMPLITUDE_OFFSET5[2] = { 512, 2048 };
const double AMPLITUDE_FACTOR5_LG[4] = { 16.0, 32.0,    32.0, 2.0/64.0 };
const double AMPLITUDE_FACTOR5_HG[4] = { 16.0, 32.0*64, 32.0*64.,  2.0 };

const int NOFWORDS_WEIGHTS_7S_1GAIN = 22;
const int NOFWORDS_WEIGHTS_7S = 2*NOFWORDS_WEIGHTS_7S_1GAIN;
const int RECALIB_OFFSET = 48*NOFWORDS_WEIGHTS_7S + 1;


class TileRawChannel;

class TileRawChannel2Bytes5
{
  public:

    enum FormatCode {
      code_amp5 = 0x20, //  G 0 1 .  . . . .
      code_raws = 0x40, //  G 1 0 .  . . . .
      code_dump = 0x60, //  G 1 1 .  . . . .
      code_ped4 = 0x10, //  0 0 0 1  . . . .
      code_ped5 = 0x90, //  1 0 0 1  . . . .
      code_rawf = 0x08, //  G 0 0 0  1 . . .
      code_amp6 = 0x04, //  G 0 0 0  0 1 . .
      code_full = 0x01, //  G 0 0 0  0 0 0 1
      code_null = 0x00  //  0 0 0 0  0 0 0 0
    };

    typedef struct {
      int code;
      int bad;
      int gain;
      int ene_bin;
      int time_bin;
      float ene;
      float time;
      int s[7];
    } TileChanData;

    uint8_t m_FormatLookup[256];

    TileRawChannel2Bytes5()
    {
      setFormatLookup();
    }

    /** Compression Format Code inlile functions. */
    inline static bool is_code_amp5(uint32_t code) { return ((code & 0x60) == code_amp5); }
    inline static bool is_code_raws(uint32_t code) { return ((code & 0x60) == code_raws); }
    inline static bool is_code_dump(uint32_t code) { return ((code & 0x60) == code_dump); }
    inline static bool is_code_ped4(uint32_t code) { return ((code & 0xF0) == code_ped4); }
    inline static bool is_code_ped5(uint32_t code) { return ((code & 0xF0) == code_ped5); }
    inline static bool is_code_rawf(uint32_t code) { return ((code & 0x78) == code_rawf); }
    inline static bool is_code_amp6(uint32_t code) { return ((code & 0x7C) == code_amp6); }
    inline static bool is_code_full(uint32_t code) { return ((code & 0x3F) == code_full); }
    inline static bool is_code_null(uint32_t code) { return ((code & 0xFF) == code_null); }

    inline static bool is_code_ped(uint32_t code) { return ((code & 0x70) == code_ped4); }
    inline static bool is_code_amp(uint32_t code) { return (is_code_amp5(code) || is_code_amp6(code)); }
    inline static bool is_code_amp_raw_dump(uint32_t code) { return (code & 0x6C); }

    /** Unpack Frag5 reco from 32-bit word w. */
    inline void unpack_reco(unsigned int w, unsigned int unit, int &fmt, int &gain, double &amp, double &time) const;
    inline bool unpack_reco_bin(unsigned int w, int &fmt, int &gain, int &amp, int &time) const;

    /** Get SumEt, SumEz, SumE value **/
    inline double getSum(uint32_t* ptr_frag, int n) const;
    
    inline double getSumEt(uint32_t* ptr_frag) const;
    inline double getSumEz(uint32_t* ptr_frag) const;
    inline double getSumE (uint32_t* ptr_frag) const;

    /** Sets verbose mode true or false. */
    inline void setVerbose(bool /*verbose*/) {  }  

    inline static int get_code(uint32_t reco)
    {
      int code = reco >> 24;

      if (is_code_null(code)) code = code_null; else
      if (is_code_ped4(code)) code = code_ped4; else
      if (is_code_ped5(code)) code = code_ped5; else
      if (is_code_amp5(code)) code = code_amp5; else
      if (is_code_amp6(code)) code = code_amp6; else
      if (is_code_raws(code)) code = code_raws; else
      if (is_code_rawf(code)) code = code_rawf; else
      if (is_code_full(code)) code = code_full; else
      if (is_code_dump(code)) code = code_dump; else {
        printf("\nTileRawChannel2Bytes5::get_code ERROR\n");
        code = -1;
      }
      return code;
    }

    inline static void print_code(uint32_t code)
    {
      if (code == code_ped4) printf("ped4"); else
      if (code == code_ped5) printf("ped5"); else
      if (code == code_amp5) printf("amp5"); else
      if (code == code_amp6) printf("amp6"); else
      if (code == code_raws) printf("raws"); else
      if (code == code_rawf) printf("rawf"); else
      if (code == code_full) printf("full"); else
      if (code == code_dump) printf("dump"); else
      if (code == code_null) printf("null"); else
        printf("ERR ");
    }

    inline static int get_size_code(uint32_t code)
    {
      int size = 0;
      if (is_code_null(code)) size = 32; else
      if (is_code_ped4(code)) size = 40; else
      if (is_code_ped5(code)) size = 48; else
      if (is_code_amp5(code)) size = 56; else
      if (is_code_amp6(code)) size = 64; else
      if (is_code_raws(code)) size = 72; else
      if (is_code_rawf(code)) size = 80; else
      if (is_code_full(code)) size = 80; else
      if (is_code_dump(code)) size = 88;

      return size/8;
    }

    inline int get_format(int code) const { return m_FormatLookup[code & 0xFF]; }
    inline static int get_quality(int bad, int format) { return (bad << 4) + format; }

    inline void setFormatLookup()
    {
      uint8_t fmt;
      for (int code = 0; code < 0xFF; code++) {
        if (is_code_null(code)) fmt = 0; else
        if (is_code_ped4(code)) fmt = 1; else
        if (is_code_ped5(code)) fmt = 2; else
        if (is_code_amp5(code)) fmt = 3; else
        if (is_code_amp6(code)) fmt = 4; else
        if (is_code_raws(code)) fmt = 5; else
        if (is_code_rawf(code)) fmt = 6; else
        if (is_code_full(code)) fmt = 7; else
        if (is_code_dump(code)) fmt = 8; else
          fmt = 0xFF;
        m_FormatLookup[code] = fmt;
      }
    }

    int amplitude(const uint32_t* ofw, int unit, int chan, int gain, int s[]) const;

    void unpack(const uint32_t* ofw, const uint32_t* ptr_frag, TileChanData* ChanData) const;
    bool check_raw(const uint32_t* feb, int of_energy[], TileChanData* ChanData) const;
    bool check_reco(const uint32_t* frag, int of_energy[]) const;
};

/// ped4 | ped5
#define if_Frag5_DO_PED(w, code, gain, amp, time)               \
if (TileRawChannel2Bytes5::is_code_ped(code)) {                 \
  gain = 1;                                                     \
  amp  = (w & 0x1FF) - 256;                                     \
  time = 0;                                                     \
}

/// amp5 | amp6 | raws | rawf | dump
#define if_Frag5_DO_AMP_RAW_DUMP(w, code, gain, amp, time)      \
if (TileRawChannel2Bytes5::is_code_amp_raw_dump(code)) {        \
  gain = (w >> 31);                                             \
  amp  = (w & 0x7FFF) - AMPLITUDE_OFFSET5[gain];                \
  if (TileRawChannel2Bytes5::is_code_amp(code)) {               \
    time = ((int32_t)(w << (32 - 15 - 6)) >> (32 - 6));         \
  } else {                                                      \
    time = 0;                                                   \
  }                                                             \
}

/// null
#define if_Frag5_DO_NULL(w, code, gain, amp, time)              \
if (TileRawChannel2Bytes5::is_code_null(code)) {                \
  int bit = w & 0x1;                                            \
  gain = bit;                                                   \
  amp  = 0;                                                     \
  time = 0;                                                     \
}

/// full
#define if_Frag5_DO_FULL(w, code, gain, amp, time)              \
if (TileRawChannel2Bytes5::is_code_full(code)) {                \
  gain = (w >> 31);                                             \
  int overflow = (w >> 23) & 0x1;                               \
  amp  = (overflow ? 0x7FFF : 0x0) - AMPLITUDE_OFFSET5[gain];   \
  time = 0;                                                     \
}

/// if_unpack_reco_bin
#define if_Frag5_unpack_reco_bin(w, code, gain, amp, time)      \
  if_Frag5_DO_PED(w, code, gain, amp, time) else                \
  if_Frag5_DO_AMP_RAW_DUMP(w, code, gain, amp, time) else       \
  if_Frag5_DO_NULL(w, code, gain, amp, time) else               \
  if_Frag5_DO_FULL(w, code, gain, amp, time)                    \

/// unpack_reco_bin
#define Frag5_unpack_reco_bin(w, code, gain, amp, time)         \
  if_Frag5_unpack_reco_bin(w, code, gain, amp, time);

/// unpack_bin2reco
#define Frag5_unpack_bin2reco(unit, gain, amp_bin, amp, time_bin, time) \
{                                                                       \
  time = time_bin/2.0;                                                  \
  amp  = (gain) ? amp_bin/AMPLITUDE_FACTOR5_HG[unit]                    \
                : amp_bin/AMPLITUDE_FACTOR5_LG[unit];                   \
}

/// unpack_bin2sum
#define Frag5_unpack_bin2sum(unit, amp_bin)                             \
  amp_bin/AMPLITUDE_FACTOR5_HG[unit]


// inline functions

inline bool TileRawChannel2Bytes5::unpack_reco_bin(unsigned int w, int &fmt, int &gain, int &amp, int &time) const
{
  uint32_t code = w >> 24;
  fmt = m_FormatLookup[code];

  if_Frag5_unpack_reco_bin(w, code, gain, amp, time)
  else {
    gain = 0; amp = 0; time = 0;
    printf("\nTileRawChannel2Bytes5::unpack_reco_bin.ERROR reco = %08X\n", w);
    return false;
  }
  return true;
}

inline void TileRawChannel2Bytes5::unpack_reco(unsigned int w, unsigned int unit,
                                               int &fmt, int &gain, double &amp, double &time) const
{
  int amp_bin, time_bin;
  if (unpack_reco_bin(w, fmt, gain, amp_bin, time_bin)) {
    Frag5_unpack_bin2reco(unit, gain, amp_bin, amp, time_bin, time);
  } else {
    amp = 0.0; time = 0.0;
  }
}

inline double TileRawChannel2Bytes5::getSum(uint32_t* ptr_frag, int n) const
{
  int unit = ptr_frag[2] >> (32 - 2);
  int amp_bin = (int) (ptr_frag[3 + 48 + n]); // Header + Reco + [SumEt, SumEz, SumE]
  return Frag5_unpack_bin2sum(unit, amp_bin);
}

inline double TileRawChannel2Bytes5::getSumEt(uint32_t* ptr_frag) const { return getSum(ptr_frag, 0); }
inline double TileRawChannel2Bytes5::getSumEz(uint32_t* ptr_frag) const { return getSum(ptr_frag, 1); }
inline double TileRawChannel2Bytes5::getSumE (uint32_t* ptr_frag) const { return getSum(ptr_frag, 2); }

#endif
