/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNEL2BYTES_H 
#define TILEBYTESTREAM_TILERAWCHANNEL2BYTES_H 
// This class converts the TileRawChannel object into bytes
// 

/**
 *
 * @class TileRawChannel2Bytes
 * @brief Converts the TileRawChannel object into bytes

 * Amplitude, phase and quality factor are encoded in 16-bit words:
 \li 14 bits for the amplitude plus 1 bit for the sign and 1 bit for gain,
 \li 15 bits for the phase plus 1 bit for the sign and
 \li 15 bits for the quality plus 1 bit for the sign. <p>
 */
#include <vector>

// Amplitude encoding: gain bit + sign bit + Value(14bits) 
const int GAIN_BIT = 0x8000;
const int AMPLITUDE_SIGN = 0x4000;
const int AMPLITUDE_RANGE = 0x3FFF;

// Time encoding: sign bit + Value(15bits)
const int TIME_SIGN = 0x8000;
const int TIME_RANGE = 0x7FFF;

// Quality encoding: sign bit + Value(15bits) 
const int QUALITY_SIGN = 0x8000;
const int QUALITY_RANGE = 0x7FFF;

// Conversion factors from double to int values
const float AMPLITUDE_FACTOR = 16.0F;
const float TIME_FACTOR = 256.0F;
const float QUALITY_FACTOR = 32768.0F;

class TileFastRawChannel;

class TileRawChannel2Bytes {
  public:
    TileRawChannel2Bytes()
        : m_verbose(false) {
    }

    /** Pack TileRawChannel information (gain, amplitude, phase and quality) in 3 16-bit words.
     Fills in the vector<short> the three entries for the corresponding read-out channel.<p>
     Returns number of words added to the vector, 3 for amplitude + phase + quality factor. */
    int getBytes(const TileFastRawChannel* rc, int gain, std::vector<short>& v);

    // unpack short to double
    /** Returns the gain unpacked from the single 16-bit word w. */
    inline int gain(short i) const;
    /** Returns the non calibrated amplitude unpacked from a single 16-bit word w. */
    inline float amplitude(short i) const;
    /** Returns the phase of the pulse, unpacked from a single 16-bit word w. */
    inline float time(short i) const;
    /** Returns the quality factor unpacked from a single 16-bit word w. */
    inline float quality(short i) const;

    /** Sets verbose mode true or false. */
    inline void setVerbose(bool verbose) {
      m_verbose = verbose;
    }

  private:

    /** Returns AMPLITUDE_RANGE .*/
    inline int amplitude_range() {
      return AMPLITUDE_RANGE;
    }
    /** Returns TIME_RANGE . */
    inline int time_range() {
      return TIME_RANGE;
    }
    /** Retuns QUALITY_RANGE .*/
    inline int quality_range() {
      return QUALITY_RANGE;
    }

    /** Verbose flag. */
    bool m_verbose;
};

// inline functions

inline int TileRawChannel2Bytes::gain(short s) const {
  // s >>= AMPLITUDE_SHIFT;
  int g = (s & GAIN_BIT) ? 1 : 0;
  return g;
}

inline float TileRawChannel2Bytes::amplitude(short s) const {
  // s >>= AMPLITUDE_SHIFT;
  float a = (s & AMPLITUDE_RANGE) * (1./AMPLITUDE_FACTOR);
  if (s & AMPLITUDE_SIGN) a = -a;
  return a;
}

inline float TileRawChannel2Bytes::time(short s) const {
  // s >>= TIME_SHIFT;
  float t = (s & TIME_RANGE) * (1./TIME_FACTOR);
  if (s & TIME_SIGN) t = -t;
  return t;
}

inline float TileRawChannel2Bytes::quality(short s) const {
  // s >>= QUALITY_SHIFT;
  float q = (s & QUALITY_RANGE) * (1./QUALITY_FACTOR);
  if (s & QUALITY_SIGN) q = -q;
  return q;
}

#endif 
