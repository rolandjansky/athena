/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexEMRoIThresholdsTool.h"

namespace eFexEMRoIThresholdsToolParams {
  const char ContainerName[] = "L1_eEMRoI";
  const char ThresholdType[] = "eEM";
}

uint64_t eFexEMRoIThresholdsTool::getPattern(const xAOD::eFexEMRoI& roi,
                                             const RoIThresholdsTool::ThrVec& menuThresholds,
                                             const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // The logic needs to be implemented here, now just return dummy value with all bits fired

  // Get RoI properties (once, rather than for every threshold in the menu)
  unsigned int et    = roi.etTOB();
  unsigned int reta  = roi.RetaThresholds();
  unsigned int rhad  = roi.RhadThresholds();
  unsigned int wstot = roi.WstotThresholds();
  int ieta = roi.iEta();

  uint64_t thresholdMask = 0;
  // iterate through thresholds and see which ones are passed
  RoIThresholdsTool::ThrVec::const_iterator thr_Itr = menuThresholds.begin();
  for (; thr_Itr != menuThresholds.end(); ++thr_Itr) {
     std::shared_ptr<TrigConf::L1Threshold_eEM> thr = std::static_pointer_cast<TrigConf::L1Threshold_eEM>(*thr_Itr);
     // Test ET threshold and jet discriminant codes, set bit in threshold word if conditions met
     if (et > thr->thrValueCounts(ieta) && reta >= (unsigned int)thr->reta() && 
         rhad >= (unsigned int)thr->rhad() && wstot >= (unsigned int)thr->wstot()) 
        thresholdMask |= (1<<thr->mapping());
  }

  return thresholdMask;
}
