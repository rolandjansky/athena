/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_ecfhelper
#define jetsubstructuremomenttools_ecfhelper

#include "JetSubStructureMomentTools/ECFHelper.h"

std::string GetBetaSuffix(float beta) {
  std::string suffix = "";

  // Create suffix for beta != 1.0
  // 10*beta is used in the suffix
  // As long as beta < 10, this shouldn't cause any confusion
  if(std::abs(beta-1.0) > 1.0e-5) {
    suffix = "_Beta" + std::to_string(int(beta*10));
  }

  return suffix;
}

#endif
