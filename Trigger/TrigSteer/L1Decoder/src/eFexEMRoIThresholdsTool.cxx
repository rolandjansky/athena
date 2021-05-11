/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexEMRoIThresholdsTool.h"

namespace eFexEMRoIThresholdsToolParams {
  const char ContainerName[] = "L1_eEMRoI";
  const char ThresholdType[] = "eEM";
}

uint64_t eFexEMRoIThresholdsTool::getPattern(const xAOD::eFexEMRoI& /*roi*/,
                                             const RoIThresholdsTool::ThrVec& /*menuThresholds*/,
                                             const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // The logic needs to be implemented here, now just return dummy value with all bits fired
  return 0xFFFFFFFFFFFFFFFF;
}
