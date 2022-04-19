/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "gFexLRJetRoIThresholdsTool.h"

uint64_t gFexLRJetRoIThresholdsTool::getPattern(const xAOD::gFexJetRoI& roi,
                                                const RoIThresholdsTool::ThrVec& menuThresholds,
                                                const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  float et = roi.et();
  uint64_t thresholdMask = 0;

  for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds) {
    std::shared_ptr<TrigConf::L1Threshold_gLJ> thr = std::static_pointer_cast<TrigConf::L1Threshold_gLJ>(thrBase);
    
    if (et > thr->thrValueMeV()) {
      thresholdMask |= (1<<thr->mapping());
    }
    
  }
  
  return thresholdMask;
}
