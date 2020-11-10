/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNEL2BYTES4_H 
#define TILEBYTESTREAM_TILERAWCHANNEL2BYTES4_H 
// This class converts the TileRawChannel object into bytes
// as it is defined at the commissioning 2007
// in one 32-bit word, 1 bit for the gain, 15 for the energy,
// 11 bits for time and 4+1 bits for quality factor and energy 
// trigger flag

/**
 *
 * @class TileRawChannel2Bytes4
 * @brief Converts the TileRawChannel object into bytes as it
 is defined at the commissioning 2007. Updated for M6.

 * In one 32-bit word:
 \li 1 bit is for the gain (0=low gain , 1=high gain),
 \li 15 bits are for the energy (which can be calibrated to different units),
 \li 11 bits are for the phase (ns): values between -64 to 63 ns with 1/16ns precision  and
 \li 4+1 bits for the quality factor and trigger flag bit. Quality factor
 larger than 15 indicates a problem in the corresponding channel and should not
 be considered for the trigger. <p>
 */

#include <vector>

// Gain encoding: one most significant bit
const unsigned int GAIN_RANGE4 = 0x1;
const int GAIN_SHIFT4 = 31;
const unsigned int GAIN_BIT4 = (GAIN_RANGE4 << GAIN_SHIFT4);

// Amplitude encoding: 15 bits
// Value = amp*factor + offset
// amp = (Value - offset)/factor
// the offset depends on the gain
// the factor depends on the units
const float AMPLITUDE_OFFSET4[2] = { 512.0F, 2048.0F };
const float AMPLITUDE_FACTOR4[4] = { 16.0F, 32.0F, 32.0F, 1.0F / 32.0F };
const int AMPLITUDE_RANGE4 = 0x7FFF;
const int AMPLITUDE_SHIFT4 = 16;

// Time encoding: 11 bits
// Value = time*factor + offset
// time = (Value - offset)/factor
const float TIME_OFFSET4 = 1024.0F;
const float TIME_FACTOR4 = 16.0F;
const int TIME_RANGE4 = 0x7FF;
const int TIME_SHIFT4 = 5;

// Quality encoding: 4+1 bits
// Value = Qfactor*factor + offset
// Qfactor = (Value - offset)/factor
const float QUALITY_OFFSET4 = 0.0F;
const float QUALITY_FACTOR4 = 1.0F;
const int QUALITY_RANGE4 = 0x1F;
const int QUALITY_RANGE4_NOFLAG = 0xF;
const int QUALITY_RANGE4_FLAG = 0x10;
const float QUALITY_THRESHOLD = 15.999F; // just a number between 0xF and 0x10
const int QUALITY_SHIFT4 = 0;

class TileFastRawChannel;

class TileRawChannel2Bytes4 {
  public:
    TileRawChannel2Bytes4()
    {
    }

    /** Adds an entry to the vector<int> v for this TileRawChannel.
     The entry is the single 32-bit word generated using TileRawChannel2Bytes4::getWord() method.<p>
     Returns 1 which is the number of entries added to the vector. */
    int getBytes(const TileFastRawChannel& rc, unsigned int unit,
                 int gain, std::vector<unsigned int>& v) const;
    /** Returns a single 32-bit word which encodes the TileRawChannel information
     (gain,amplitude,phase and quality) for a single read-out channel. */
    unsigned int getWord(const TileFastRawChannel& rc, unsigned int unit, int gain) const;

    /** Returns the gain unpacked from the single 32-bit word w. */
    inline int gain(unsigned int w) const;
    /** Returns the amplitude in the corresponding units, unpacked from the single 32-bit word w. */
    inline float amplitude(unsigned int w, unsigned int unit) const;
    /** Returns the phase of the pulse in ns, unpacked from the single 32-bit word w. */
    inline float time(unsigned int w) const;
    /** Returns the quality factor unpacked from the single 32-bit word w. */
    inline float quality(unsigned int w) const;

    /** Sets verbose mode true or false. */
    inline void setVerbose(bool /*verbose*/) {
    }
};

// inline functions

inline int TileRawChannel2Bytes4::gain(unsigned int w) const {
  int g = (w >> GAIN_SHIFT4) & GAIN_RANGE4;
  return g;
}

inline float TileRawChannel2Bytes4::amplitude(unsigned int w, unsigned int unit) const {
  int g = (w >> GAIN_SHIFT4) & GAIN_RANGE4;
  float a = (((w >> AMPLITUDE_SHIFT4) & AMPLITUDE_RANGE4) - AMPLITUDE_OFFSET4[g])
      / AMPLITUDE_FACTOR4[unit];
  if (unit != 0 && g == 1) a = a / 64.0F;
  return a;
}

inline float TileRawChannel2Bytes4::time(unsigned int w) const {
  float t = (((w >> TIME_SHIFT4) & TIME_RANGE4) - TIME_OFFSET4) * (1./TIME_FACTOR4);
  return t;
}

inline float TileRawChannel2Bytes4::quality(unsigned int w) const {
  float q = (((w >> QUALITY_SHIFT4) & QUALITY_RANGE4) - QUALITY_OFFSET4) * (1./QUALITY_FACTOR4);
  return q;
}

#endif 
