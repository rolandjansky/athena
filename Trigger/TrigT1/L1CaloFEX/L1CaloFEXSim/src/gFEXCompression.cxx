/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXCompression - Energy encoder/decoder for gFEX
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************
#include "L1CaloFEXSim/gFEXCompression.h"

namespace LVL1 {

const int gFEXCompression::s_steps[] = {12800, 50, 25, 50, 100, 25600};
const int gFEXCompression::s_minET[] = {-101200, -50000, -12800, 12800, 51200, 200000};
const int gFEXCompression::s_minCode[] = {2, 6, 750, 1774, 2542, 4030};


unsigned int gFEXCompression::compress(int Et) {

  // Check for overflow
  if (Et >= s_maxET) return s_LArOverflow;

  // Find which range the ET value is in
  int range = -1;
  for (unsigned int i = 0; i < s_nRanges; i++) {
    if (Et < s_minET[i]) break;
    range = i;
  }

  // Calculate code
  unsigned int code = 0;

  if (range < 0) {
    // Below minimum value
    code = s_LArUnderflow;
  }
  else {
    // Lies inside one of the value ranges
    int steps = (Et - s_minET[range])/s_steps[range];
    code = s_minCode[range] + steps;
  }

  return code;
}

int gFEXCompression::expand(unsigned int code) {

  // Deal with special codes first:
  if (code == s_NoData)  return 0;
  if (code == s_LArInvalid || (code > s_LArReserved_min && code < s_LArReserved_max) || code > s_LArMaxCode) return s_error;
  if (code == s_LArOverflow ) return s_maxET;

  /** Now expand code into an ET value.
      Start by finding what range the code is in */
  int range = 0;
  for (unsigned int i = 0; i < s_nRanges-1; ++i) {
    if (code < (unsigned int)s_minCode[i+1]) break;
    range++;
  }
  /// Now expand the value
  int Et = s_minET[range] + (code-s_minCode[range])*s_steps[range];

  return Et;
}


unsigned int gFEXCompression::threshold(unsigned int code, int threshold) {

  /// Convert threshold into a compressed code
  unsigned int cut = gFEXCompression::compress(threshold);

  /// Code zero if < threshold
  if (code < cut) code = 0;

  return code;
}


unsigned int gFEXCompression::linearize(unsigned int code, int threshold) {

  /// Apply the threshold. Since eFEX ET is positive, minimum threshold is 0.
  if (threshold < 0) threshold = 0;
  code = gFEXCompression::threshold(code, threshold);

  /// Expand the ET value
  int Et = gFEXCompression::expand(code);

  // Check for overflow
  if (Et >= s_maxET) return s_gFEXOverflow;

  /// Convert to eFEX digit scale
  unsigned int gFexET = Et/s_gFEXstep;
  return gFexET;
}

} // end of namespace bracket
