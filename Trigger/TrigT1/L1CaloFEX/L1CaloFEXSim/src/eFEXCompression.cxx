/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          eFEXCompression.cxx  -  description
                             -------------------
    begin                : 07-02-2019
    email                : Alan.Watson@cern.ch antonio.jacques.costa@cern.ch
 ***************************************************************************/

#include "L1CaloFEXSim/eFEXCompression.h"

namespace LVL1 {

const int eFEXCompression::s_steps[] = {25, 50, 100, 200, 400, 102400};
const int eFEXCompression::s_minET[] = {-750, 1600, 6400, 25600, 102400, 200000};
const int eFEXCompression::s_minCode[] = {2, 96, 192, 384, 768, 1012};

unsigned int eFEXCompression::Compress(int Et) {

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

int eFEXCompression::Expand(unsigned int code) {

  // Deal with special codes first:
  if (code == s_NoData)  return 0;
  if (code == s_LArInvalid || code == s_LArReserved || code > s_LArMaxCode) return s_error; 
  if (code == s_LArOverflow || code == s_LArSaturated) return s_maxET;

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


unsigned int eFEXCompression::Threshold(unsigned int code, int threshold) {

  /// Convert threshold into a compressed code
  unsigned int cut = eFEXCompression::Compress(threshold);

  /// Zero code if < threshold
  if (code < cut) code = 0;

  return code;
}


unsigned int eFEXCompression::Linearize(unsigned int code, int threshold) {

  /// Apply the threshold. Since eFEX ET is positive, minimum threshold is 0.
  if (threshold < 0) threshold = 0;
  code = eFEXCompression::Threshold(code, threshold);

  /// Expand the ET value
  int Et = eFEXCompression::Expand(code);

  // Check for overflow
  if (Et >= s_maxET) return s_eFEXOverflow;

  /// Convert to eFEX digit scale
  unsigned int eFexET = Et/s_eFEXstep;
  return eFexET;
}

} // end of namespace bracket
