/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_nsubjettinesshelper
#define jetsubstructuremomenttools_nsubjettinesshelper

#include "JetSubStructureMomentTools/NSubjettinessHelper.h"

std::string GetAlphaSuffix(float alpha) {
  std::string suffix = "";

  // Create suffix for alpha != 1.0
  // 10*alpha is used in the suffix
  // As long as alpha < 10, this shouldn't cause any confusion
  if(std::abs(alpha-1.0) > 1.0e-5) {
    suffix = "_Alpha" + std::to_string(int(alpha*10));
  }

  return suffix;
}

#endif
