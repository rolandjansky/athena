/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNEL2BYTES2_H 
#define TILEBYTESTREAM_TILERAWCHANNEL2BYTES2_H 
// This class converts the TileRawChannel object into bytes
// as it is defined at the testbeam
// in one 32-bits word 16 bits are used for energy,
// 12 bits for time and 4 bits for quality

/**
 *
 * @class TileRawChannel2Bytes2
 * @brief Converts the TileRawChannel object into bytes
 as it is defined at the testbeam
 
 * In one 32-bit word:
 \li 16 bits are used for the energy (non calibrated energy)
 \li 12 bits are used for the phase (ns)
 \li 4 bits are used for the quality factor <p>
 */

#include <vector>

// Gain encoding: one most significant bit
const unsigned int GAIN_RANGE2 = 0x1;
const int GAIN_SHIFT2 = 31;
const unsigned int GAIN_BIT2 = (GAIN_RANGE2 << GAIN_SHIFT2);

// Amplitude encoding: 15 bits
// // Value = (amp + 64) * 32, shifted by 16 bits
// const double AMPLITUDE_OFFSET2 = 64.0;
// const double AMPLITUDE_FACTOR2 = 32.0;
// Value = (amp + 1024) * 16, shifted by 16 bits
const float AMPLITUDE_OFFSET2 = 1024.0F;
const float AMPLITUDE_FACTOR2 = 16.0F;
const int AMPLITUDE_RANGE2 = 0x7FFF;
const int AMPLITUDE_SHIFT2 = 16;
const float AMPLITUDE_MINVALUE = -800.0F;

// Time encoding: 12 bits
// Value = (time + 128) * 16, shifted by 4 bits
const float TIME_OFFSET2 = 128.0F;
const float TIME_FACTOR2 = 16.0F;
const int TIME_RANGE2 = 0xFFF;
const int TIME_SHIFT2 = 4;

// Quality encoding: 4 bits
// Value = q, not shifted
const float QUALITY_OFFSET2 = 0.0F;
const float QUALITY_FACTOR2 = 1.0F;
const int QUALITY_RANGE2 = 0xF;
const int QUALITY_SHIFT2 = 0;

class TileFastRawChannel;

class TileRawChannel2Bytes2 {
  public:
    TileRawChannel2Bytes2()
        : m_verbose(false) {
    }

    /** Adds an entry to the vector<int> v for this TileRawChannel.
     The entry is the single 32-bit word generated using TileRawChannel2Bytes2::getWord() method.<p>
     Returns 1 which is the number of entries added to the vector. */
    int getBytes(const TileFastRawChannel* rc, int gain, std::vector<unsigned int>& v);
    /** Returns a single 32-bit word which encodes the TileRawChannel
     information (gain,amplitude,phase and quality) for a single read-out channel. */
    unsigned int getWord(const TileFastRawChannel* rc, int gain);

    /** Returns the gain unpacked from the single 32-bit word w. */
    inline int gain(unsigned int w) const;
    /** Returns the non calibrated amplitude unpacked from the single 32-bit word w. */
    inline float amplitude(unsigned int w) const;
    /** Returns the phase of the pulse in ns, unpacked from the single 32-bit word w. */
    inline float time(unsigned int w) const;
    /** Returns the quality factor unpacked from the single 32-bit word w. */
    inline float quality(unsigned int w) const;

    /** Sets verbose mode true or false. */
    inline void setVerbose(bool verbose) {
      m_verbose = verbose;
    }

  private:
    /** Verbose flag. */
    bool m_verbose;
};

// inline functions

inline int TileRawChannel2Bytes2::gain(unsigned int w) const {
  int g = (w >> GAIN_SHIFT2) & GAIN_RANGE2;
  return g;
}

inline float TileRawChannel2Bytes2::amplitude(unsigned int w) const {
  float a = ((w >> AMPLITUDE_SHIFT2) & AMPLITUDE_RANGE2) * (1. / AMPLITUDE_FACTOR2) - AMPLITUDE_OFFSET2;
  if (a < AMPLITUDE_MINVALUE) a += 2 * AMPLITUDE_OFFSET2;
  return a;
}

inline float TileRawChannel2Bytes2::time(unsigned int w) const {
  float t = ((w >> TIME_SHIFT2) & TIME_RANGE2) * (1./TIME_FACTOR2) - TIME_OFFSET2;
  return t;
}

inline float TileRawChannel2Bytes2::quality(unsigned int w) const {
  float q = ((w >> QUALITY_SHIFT2) & QUALITY_RANGE2) * (1./QUALITY_FACTOR2) - QUALITY_OFFSET2;
  return q;
}

#endif 
