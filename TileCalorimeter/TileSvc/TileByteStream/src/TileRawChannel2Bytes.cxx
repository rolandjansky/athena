/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileRawChannel2Bytes.h" 
#include "TileEvent/TileFastRawChannel.h"
#include <algorithm> 
#include <cmath>

int TileRawChannel2Bytes::getBytes(const TileFastRawChannel* rc, int gain, std::vector<short>& v) {
  // pack raw data into 16bit integers

  int a = lround(rc->amplitude() * AMPLITUDE_FACTOR);
  int t = lround(rc->time() * TIME_FACTOR);
  int q = lround(rc->quality() * QUALITY_FACTOR);

  short w1 = std::min(abs(a), amplitude_range());
  if (a < 0) w1 |= AMPLITUDE_SIGN;
  if (gain) w1 |= GAIN_BIT;
  // w1 <<= AMPLITUDE_SHIFT;

  short w2 = std::min(abs(t), time_range());
  if (t < 0) w2 |= TIME_SIGN;
  // w2 <<= TIME_SHIFT;

  short w3 = std::min(abs(q), quality_range());
  if (q < 0) w3 |= QUALITY_SIGN;
  // w3 <<= QUALITY_SHIFT;

  v.push_back(w1);
  v.push_back(w2);
  v.push_back(w3);

  return 3;
}

