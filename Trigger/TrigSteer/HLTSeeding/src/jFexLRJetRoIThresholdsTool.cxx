/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "jFexLRJetRoIThresholdsTool.h"

uint64_t jFexLRJetRoIThresholdsTool::getPattern(const xAOD::jFexLRJetRoI& /*roi*/,
                                                const RoIThresholdsTool::ThrVec& /*menuThresholds*/,
                                                const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // THIS IS CURRENTLY ONLY A PLACEHOLDER
  return 0xffffffffffffffff;
}
