/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Toyonobu Okuyama
 * @author Takashi Matsushita
 */

#include "TrigMuonEfficiency/MuonThresholds.h"
#include <iostream>
#include <cmath>


MuonHypothesis::MuonHypothesis() {
}


MuonHypothesis::~MuonHypothesis() {
}


bool
MuonHypothesis::EF_isPassed(const float pt,
                            const float eta,
                            const std::string& chain)
{
  const double *threshold = getThresholds(chain);
  if (not threshold) {
    std::cerr << "MuonHypothesis\t" << "ERROR\t"
              << "Cannot get thresholds for chain " << chain
              << ". Please check MuonThresholds.h(or Muonhypothesis::getThresholds)" << std::endl;
    return false;
  }
  return EF_isPassed(pt*0.001, eta, threshold);
}


bool
MuonHypothesis::EF_isPassed(const float pt,
                            const float eta,
                            const double* threshold)
{
  const size_t hypo_binsize = MuonHypoThresholds::hypo_binsize;
  const float* hypo_etaregion = MuonHypoThresholds::hypo_etaregion;
  
  size_t etabin = 0;
  for (; etabin < hypo_binsize; etabin++) {
    if (fabs(eta) < hypo_etaregion[etabin]) break;
  }

  if (etabin == hypo_binsize) {
    std::cerr << "MuonHypothesis\t" << "Invalid Argument\t"
              << "Input eta for EF_isPassed is larger than 9.9!! (eta = " << eta << ")" << std::endl;
    return false;
  }
  return threshold[etabin] <= pt;
}


const double*
MuonHypothesis::getThresholds(const std::string& chain)
{
  EF_thresholdMap::const_iterator cit;
  const EF_thresholdMap *thresholds = (chain.find("_MSonly") != std::string::npos) 
    ? &MuonHypoThresholds::ef_sa_map : &MuonHypoThresholds::ef_cb_map; 

  if (chain.find("EF_mu18") != std::string::npos) {
    if (thresholds->count("18GeV")) return thresholds->find("18GeV")->second;

  } else if (chain.find("EF_mu40") != std::string::npos) {
    if (chain.find("EF_mu40_MSonly_barrel") != std::string::npos) {
      if (thresholds->count("40GeV_barrelOnly")) return thresholds->find("40GeV_barrelOnly")->second;

    } else {
      if (thresholds->count("40GeV")) return thresholds->find("40GeV")->second;
    }

  } else if (chain.find("EF_mu50_MSonly_barrel") != std::string::npos) {
    if (thresholds->count("50GeV_barrelOnly")) return thresholds->find("50GeV_barrelOnly")->second;

  } else if (chain.find("EF_mu36") != std::string::npos) {
    if (thresholds->count("36GeV")) return thresholds->find("36GeV")->second;

  } else if (chain.find("EF_mu24") != std::string::npos) {
    if (thresholds->count("24GeV")) return thresholds->find("24GeV")->second;

  } else if (chain.find("EF_mu15") != std::string::npos) {
    if (thresholds->count("15GeV")) return thresholds->find("15GeV")->second;

  } else if (chain.find("EF_mu13") != std::string::npos) {
    if (thresholds->count("13GeV")) return thresholds->find("13GeV")->second;

  } else if (chain.find("EF_mu10") != std::string::npos) {
    if (thresholds->count("10GeV")) return thresholds->find("10GeV")->second;

  } else if (chain.find("EF_mu8") != std::string::npos) {
    if (thresholds->count("8GeV")) return thresholds->find("8GeV")->second;

  } else if (chain.find("EF_mu6") != std::string::npos) {
    if (thresholds->count("6GeV")) return thresholds->find("6GeV")->second;

  } else if (chain.find("EF_mu4") != std::string::npos) {
    if (thresholds->count("4GeV")) return thresholds->find("4GeV")->second;

  } else {
    std::cerr << "MuonHypothesis\t" << "ERROR\t"
              << "Cannot get thresholds. " << chain << " is not defined in getThresholds()" << std::endl;
  }

  return 0;
}
// eof
