/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------
// This is a helper function that provides the suffix for ECF and 
// ECF ratio decorations.  No suffix is used for beta = 1.0 and
// other values are given a suffix of _BetaN where N is the value
// with decimal points removed.  This assumes beta values of [0.0,9.9]
// and that the maximum precision is 0.1.
// ----------------------------------------------------------------

#ifndef jetsubstructuremomenttools_ecfhelper
#define jetsubstructuremomenttools_ecfhelper

#include "JetSubStructureMomentTools/ECFHelper.h"

std::string GetBetaSuffix(float beta) {
  std::string suffix = "";

  // Create suffix for beta != 1.0
  // If 10*beta is used in the suffix
  // As long as beta < 10, this shouldn't cause any confusion
  if(fabs(beta-1.0) > 1.0e-5) {
    suffix = "_Beta" + std::to_string(int(beta*10));
  }

  return suffix;
}

#endif
