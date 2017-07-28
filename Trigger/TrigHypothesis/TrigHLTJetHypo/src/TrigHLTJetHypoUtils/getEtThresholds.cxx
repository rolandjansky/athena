/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/getEtThresholds.h"
#include <algorithm>


std::vector<double> getEtThresholds(const std::vector<double>& dEtas,
                                    const std::vector<double>& etThresholds){
  // Emulate the python code setup by TM
  for (auto dEta : dEtas){
    if (dEta > 0.) {
      auto etThreshold = 
        std::min(40000., 
                 *std::min_element(etThresholds.cbegin(), 
                                   etThresholds.cend()));

      // The TM python code chooses the Alg class corresponding
      // to the etThreshold. This class sets the threshold.
      // It is not clear what happened if the class did not exist - 
      // probably a crash. Here we let any threshold be  used (ie
      // no crash on an unknown threshold)
      return std::vector<double> {etThreshold, etThreshold};
    }
  }

  // For invariant mass hypos without dEta cuts
  return std::vector<double> {0., 0.};
}
