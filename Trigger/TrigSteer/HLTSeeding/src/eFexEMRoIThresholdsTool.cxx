/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexEMRoIThresholdsTool.h"

uint64_t eFexEMRoIThresholdsTool::getPattern(const xAOD::eFexEMRoI& roi,
                                             const RoIThresholdsTool::ThrVec& menuThresholds,
                                             const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // Get RoI properties (once, rather than for every threshold in the menu)
  unsigned int et    = roi.etTOB();
  unsigned int reta  = roi.RetaThresholds();
  unsigned int rhad  = roi.RhadThresholds();
  unsigned int wstot = roi.WstotThresholds();
  int ieta = roi.iEta();

  uint64_t thresholdMask = 0;
  // Iterate through thresholds and see which ones are passed
  for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds) {
    std::shared_ptr<TrigConf::L1Threshold_eEM> thr = std::static_pointer_cast<TrigConf::L1Threshold_eEM>(thrBase);
    // Test ET threshold and jet discriminant codes, set bit in threshold word if conditions met
    if (et > thr->thrValueCounts(ieta) && reta >= (unsigned int)thr->reta() &&
        rhad >= (unsigned int)thr->rhad() && wstot >= (unsigned int)thr->wstot()) {
      thresholdMask |= (1<<thr->mapping());
    }
  }

  return thresholdMask;
}
