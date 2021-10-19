/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexTauRoIThresholdsTool.h"

namespace eFexTauRoIThresholdsToolParams {
  const char ContainerName[] = "L1_eTauRoI";
  const char ThresholdType[] = "eTAU";
}

uint64_t eFexTauRoIThresholdsTool::getPattern(const xAOD::eFexTauRoI& /*roi*/,
                                              const RoIThresholdsTool::ThrVec& /*menuThresholds*/,
                                              const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // THIS IS CURRENTLY ONLY A PLACEHOLDER
  return 0xffffffffffffffff;
}
