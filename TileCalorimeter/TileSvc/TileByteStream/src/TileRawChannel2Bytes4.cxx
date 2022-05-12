/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileRawChannel2Bytes4.h" 
#include "TileEvent/TileFastRawChannel.h" 
#include <algorithm> 
#include <cmath>

int TileRawChannel2Bytes4::getBytes(const TileFastRawChannel& rc,
                                    unsigned int unit,
                                    int gain,
                                    std::vector<unsigned int>& v) const
{
  // pack raw data into one 32-bits integer
  v.push_back(getWord(rc, unit, gain));

  return 1; // one int added to vector
}

unsigned int TileRawChannel2Bytes4::getWord(const TileFastRawChannel& rc,
                                            unsigned int unit,
                                            int gain) const
{
  // pack raw data into one 32-bits integer
  float tmp = rc.amplitude();
  if (unit != 0 && gain == 1) tmp = tmp * 64.0F;
  unsigned int a = std::max(0, std::min(AMPLITUDE_RANGE4
      , (int) std::lround(tmp * AMPLITUDE_FACTOR4[unit] + AMPLITUDE_OFFSET4[gain])));

  unsigned int t = std::max(0,std::min(TIME_RANGE4
      , (int) std::lround(rc.time() * TIME_FACTOR4 + TIME_OFFSET4)));

  unsigned int q = std::max(0,
      std::min(QUALITY_RANGE4_NOFLAG,
          (int) std::lround(rc.quality() * QUALITY_FACTOR4 + QUALITY_OFFSET4)));

  unsigned int w = (a << AMPLITUDE_SHIFT4) | (t << TIME_SHIFT4) | (q << QUALITY_SHIFT4);

  if (gain) w |= GAIN_BIT4;

  return w;
}

