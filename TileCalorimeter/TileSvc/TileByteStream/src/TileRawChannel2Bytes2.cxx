/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileRawChannel2Bytes2.h" 
#include "TileEvent/TileFastRawChannel.h" 
#include <algorithm> 
#include <cmath>

int TileRawChannel2Bytes2::getBytes(const TileFastRawChannel* rc, int gain,
    std::vector<unsigned int>& v) {
  // pack raw data into one 32-bits integer
  v.push_back(getWord(rc, gain));

  return 1; // one int added to vector
}

unsigned int TileRawChannel2Bytes2::getWord(const TileFastRawChannel* rc, int gain) {
  // pack raw data into one 32-bits integer

  unsigned int a = std::max(0, std::min(AMPLITUDE_RANGE2
      , (int) lround((rc->amplitude() + AMPLITUDE_OFFSET2) * AMPLITUDE_FACTOR2)));

  unsigned int t = std::max(0,
      std::min(TIME_RANGE2, (int) lround((rc->time() + TIME_OFFSET2) * TIME_FACTOR2)));

  unsigned int q = std::max(0, std::min(QUALITY_RANGE2
      , (int) lround((rc->quality() + QUALITY_OFFSET2) * QUALITY_FACTOR2)));

  unsigned int w = (a << AMPLITUDE_SHIFT2) | (t << TIME_SHIFT2) | (q << QUALITY_SHIFT2);

  if (gain) w |= GAIN_BIT2;

  return w;
}

