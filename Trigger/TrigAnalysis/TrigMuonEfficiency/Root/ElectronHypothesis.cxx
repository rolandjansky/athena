/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Takashi Matsushita
 */

#include "TrigMuonEfficiency/ElectronThresholds.h"
#include <iostream>
#include <cmath>



bool
ElectronHypothesis::EF_isPassed(const float et,
                                const std::string& chain)
{
  const double threshold = getThresholds(chain);
  if (not threshold) {
    std::cerr << "ElectronHypothesis\t" << "ERROR\t"
              << "Cannot get thresholds for chain " << chain
              << ". Please check ElectronThresholds.h(or Electronhypothesis::getThresholds)" << std::endl;
    return false;
  }
  return EF_isPassed(et, threshold);
}


bool
ElectronHypothesis::EF_isPassed(const float et,
                                const double threshold)
{
  std::cout << "DEBUG: EF_isPassed: " << (threshold <= et) << " " << threshold << " " << et << "\n";
  return threshold <= et;
}


double
ElectronHypothesis::getThresholds(const std::string& chain)
{
  EF_thresholdMap::const_iterator cit;
  const EF_thresholdMap *thresholds = &ElectronHypoThresholds::ef_el_map;

  if (chain.find("EF_e7") != std::string::npos) {
    if (thresholds->count("7GeV")) return thresholds->find("7GeV")->second;

  } else if (chain.find("EF_e12") != std::string::npos) {
    if (thresholds->count("12GeV")) return thresholds->find("12GeV")->second;

  } else if (chain.find("EF_e24") != std::string::npos) {
    if (thresholds->count("24GeV")) return thresholds->find("24GeV")->second;

  } else {
    std::cerr << "ElectronHypothesis\t" << "ERROR\t"
              << "Cannot get thresholds. " << chain << " is not defined in getThresholds()" << std::endl;
  }

  return 0.;
}
// eof
