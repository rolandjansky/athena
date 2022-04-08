/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "jFexTauRoIThresholdsTool.h"

uint64_t jFexTauRoIThresholdsTool::getPattern(const xAOD::jFexTauRoI& roi,
                                              const RoIThresholdsTool::ThrVec& menuThresholds,
                                              const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
    
    // Get RoI properties (once, rather than for every threshold in the menu)
    unsigned int et     = roi.et();
    unsigned int iso    = roi.iso();
    int          ieta   = roi.globalEta();
    uint64_t     thresholdMask = 0;
    
    
    // Iterate through thresholds and see which ones are passed
    for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds) {
        std::shared_ptr<TrigConf::L1Threshold_jTAU> thr = std::static_pointer_cast<TrigConf::L1Threshold_jTAU>(thrBase);
        
        // Checking et and isolation thresholds
        if (et > thr->thrValueMeV(ieta) && iso >= static_cast<unsigned int>(thr->isolation()) ) {
            thresholdMask |= (1<<thr->mapping());
        }
        
        ATH_MSG_DEBUG("jFEX Taus HLT seeding for ("<< thr->name() <<"): et=" << et << " > "<<thr->thrValueMeV(ieta) << " and iso="<<iso << " >= "<<static_cast<unsigned int>(thr->isolation()));
    }
    return thresholdMask;    

}
