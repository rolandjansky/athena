/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*=================================================================* 
 * Implementation of ElectronTriggerMatching
 *=================================================================*/
#include "TrigMuonEfficiency/ChainEntry.h"
#include "TrigMuonEfficiency/ElectronTriggerMatching.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <TMath.h>
#include <cmath>

/*=================================================================* 
 * public methods
 *=================================================================*/
bool
ElectronTriggerMatching::match(const double eta,
                               const double phi,
                               const std::string& chain)
{
  EFelectron efelectron, dummy;
  matchedTrackDetail(efelectron, dummy, eta, phi, chain);
  return efelectron.valid;
}



bool
ElectronTriggerMatching::matchDielectron(const TLorentzVector& electron1,
                                         const TLorentzVector& electron2,
                                         const std::string& chain,
                                         bool& result1,
                                         bool& result2)
{
  DielectronChainInfo chainInfo(chain);
  if (not decodeDielectronChain(chainInfo)) {
    std::cerr << "ElectronTriggerMatching\t" << "ERROR\t"
              << "Failed to decode chain(" << chain
              << "matchDielectron can accept only chains named EF_2eXX_YY." << std::endl;
              return false;
  }

  if (not chainInfo.isSymmetric) {
    std::cerr << "ElectronTriggerMatching\t" << "ERROR\t"
              << "use matchDielectron with different signature" << std::endl;
              return false;
  }

  std::pair<bool, bool> rc;
  rc = matchDielectron(electron1, electron2, chainInfo);

  result1 = rc.first;
  result2 = rc.second;

  return true;
}



bool
ElectronTriggerMatching::matchDielectron(const TLorentzVector& electron1,
                                         const TLorentzVector& electron2,
                                         const std::string& chain,
                                         std::pair<bool, bool>& result1,
                                         std::pair<bool, bool>& result2)
{
  DielectronChainInfo chainInfo(chain);
  if (not decodeDielectronChain(chainInfo)) {
    std::cerr << "ElectronTriggerMatching\t" << "ERROR\t"
              << "Failed to decode chain(" << chain
              << "matchDielectron can accept only chains named EF_2eXX[_YY] or EF_eWW_XX_eYY_ZZ." << std::endl;
    return false;
  }

  std::pair<bool, bool> rc12, rc21;
  rc12 = matchDielectron(electron1, electron2, chainInfo);
  if (chainInfo.isSymmetric) {
    rc21.first = rc12.second; rc21.second = rc12.first;
  } else {
    rc21 = matchDielectron(electron2, electron1, chainInfo);
  }

  result1.first = rc12.first; result1.second = rc21.second;
  result2.first = rc21.first; result2.second = rc12.second;

  return true;
}



bool
ElectronTriggerMatching::matchElectronMuon(const TLorentzVector& electron,
                                           const TLorentzVector& muon,
                                           const std::string& chain)
{

  std::string muonChain;
  if (not decodeElectronMuonChain(chain, muonChain)) {
    std::cerr << "ElectronTriggerMatching\t" << "ERROR\t"
              << "Failed to decode chain(" << chain
              << "matchElectronMuon can accept only chains named EF_eXX_muYY." << std::endl;
              return false;
  }

  EFelectron efElectron, efElectronDummy;
  EFmuon efMuon, efMuonDummy;
  double dR_electron = m_matchingDr;
  double dR_muon = m_matchingDr;

  const int chainIndex = getChainIndex(chain);
  if (chainIndex < 0) return false;

  const std::vector<int> RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(chainIndex);
  const std::vector<int> RoIType = m_d3pd->trig_Nav_chain_RoIType->at(chainIndex);

  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    if (RoIType.at(iRoI) == ChainEntry::kRoIType_EF_e) {
      const int index = RoIIndex.at(iRoI);
      const std::vector<int>& container = m_d3pd->trig_RoI_EF_e_egammaContainer_egamma_Electrons->at(index);
      const std::vector<int>& status = m_d3pd->trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus->at(index);

      double dr = LeptonTriggerMatching::matchedTrackDetail(efElectron, efElectronDummy, electron.Eta(), electron.Phi(), container, status);
      if (dr < dR_electron) dR_electron = dr;

    } else if (RoIType.at(iRoI) == ChainEntry::kRoIType_EF_mu) {
      efMuon.roi_index = RoIIndex.at(iRoI);
      const std::vector<int>& container = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainer->at(efMuon.roi_index);
      const std::vector<int>& status = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus->at(efMuon.roi_index);

      double dr = LeptonTriggerMatching::matchedTrackDetail(efMuon, efMuonDummy, muonChain, muon.Eta(), muon.Phi(), container, status);
      if (dr < dR_muon) dR_muon = dr;
    }
  }

  if ((dR_electron < m_matchingDr) and (dR_muon < m_matchingDr)) return true;

  return false; 
}



/*=================================================================* 
 * private methods
 *=================================================================*/
double
ElectronTriggerMatching::matchedTrackDetail(EFelectron& efElectronId,
                                            const EFelectron& usedEfElectronId,
                                            const double eta,
                                            const double phi,
                                            const std::string& chainForEventTrigger,
                                            const std::string& threshold)
{
  efElectronId.valid = false;
  double dR_min = m_matchingDr;

  const int chainIndex = getChainIndex(chainForEventTrigger);
  if (chainIndex < 0) return false;

  const std::vector<int> RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(chainIndex);
  const std::vector<int> RoIType = m_d3pd->trig_Nav_chain_RoIType->at(chainIndex);

  efElectronId.threshold = threshold;
  efElectronId.useThreshold = (not threshold.empty());
  if (efElectronId.useThreshold and m_d3pd->trig_EF_el_Et == 0) {
    std::cout << "ERROR: threshold cut requested but trig_EF_el_Et == 0" << std::endl;
    throw "NotImplementedError";
  }

  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    if (RoIType.at(iRoI) != ChainEntry::kRoIType_EF_e) continue;

    efElectronId.roi_index = RoIIndex.at(iRoI);
    if (usedEfElectronId.valid) {
      if (isEqual(usedEfElectronId.roi_index, efElectronId.roi_index)) continue;
    }

    const std::vector<int> container = m_d3pd->trig_RoI_EF_e_egammaContainer_egamma_Electrons->at(efElectronId.roi_index);
    const std::vector<int> status = m_d3pd->trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus->at(efElectronId.roi_index);

    double dr = LeptonTriggerMatching::matchedTrackDetail(efElectronId, usedEfElectronId, eta, phi, container, status);
    if (dr < dR_min) dR_min = dr;
  }

  return dR_min;
}



std::pair<bool, bool>
ElectronTriggerMatching::matchDielectron(const TLorentzVector& electron1,
                                         const TLorentzVector& electron2,
                                         const DielectronChainInfo& chainInfo)
{
  EFelectron efElectron1, efElectron2, dummy;
  const double dr1 = matchedTrackDetail(efElectron1, dummy, electron1.Eta(), electron1.Phi(), chainInfo.chain, chainInfo.isSymmetric ? "" : chainInfo.thresholds.first);
  const double dr2 = matchedTrackDetail(efElectron2, dummy, electron2.Eta(), electron2.Phi(), chainInfo.chain, chainInfo.isSymmetric ? "" : chainInfo.thresholds.second);

  if (efElectron1.valid and efElectron2.valid and
      isEqual(efElectron1.eta, efElectron2.eta) and
      isEqual(efElectron1.phi, efElectron2.phi)) {

    if (dr1 > dr2) {
      matchedTrackDetail(efElectron1, efElectron2, electron1.Eta(), electron1.Phi(), chainInfo.chain, chainInfo.isSymmetric ? "" : chainInfo.thresholds.first);

    } else {
      matchedTrackDetail(efElectron2, efElectron1, electron2.Eta(), electron2.Phi(), chainInfo.chain, chainInfo.isSymmetric ? "" : chainInfo.thresholds.second);
    }
  }

  return std::pair<bool, bool>(efElectron1.valid, efElectron2.valid);
}



bool
ElectronTriggerMatching::decodeDielectronChain(DielectronChainInfo& chainInfo)
{

  chainInfo.isValid = false;

  std::map<std::string, DielectronChainInfo>::const_iterator p = m_DielectronChainMap.find(chainInfo.chain);
  if (p != m_DielectronChainMap.end()) {
    chainInfo = p->second;
    return chainInfo.isValid;
  }


  std::vector<std::string> tokens;
  tokenize(chainInfo.chain, tokens, "_");

  if (tokens.size() < 2) return false;
  if (tokens[0] != "EF") return false;

  chainInfo.isSymmetric = (tokens[1].substr(0, 2) == "2e");

  if (chainInfo.isSymmetric) {
    chainInfo.thresholds.first = "EF_" + tokens[1].substr(1);
    chainInfo.thresholds.second = chainInfo.thresholds.first;
    if (tokens.size() == 3) chainInfo.tightness = tokens[2];
    chainInfo.isValid = true;

  } else {
    if (tokens.size() != 5) return false;
    if ((tokens[1].substr(0,1) != "e") or (tokens[3].substr(0,1) != "e")) return false;
    chainInfo.thresholds.first = "EF_" + tokens[1] + "_" + tokens[2];
    chainInfo.thresholds.second = "EF_" + tokens[3] + "_" + tokens[4];
    chainInfo.isValid = true;
  }

  m_DielectronChainMap[chainInfo.chain] = chainInfo;

  return chainInfo.isValid;
}



bool
ElectronTriggerMatching::decodeElectronMuonChain(const std::string& chain,
                                                 std::string& threshold)
{
  std::vector<std::string> tokens;
  tokenize(chain, tokens, "_");

  if (tokens.size() != 4) return false;
  if (tokens[0] != "EF") return false;
  if (not (tokens[1].substr(0, 1) == "e")) return false;

  int index = -1;
  for (size_t ii = 2; ii < tokens.size(); ii++) {
    if (tokens[ii].substr(0,2) == "mu") {
      index = ii;
      break;
    }
  }
  if (index < 0) return false;

  threshold = "EF_" + tokens[index];

  return true;
}

#include "TrigMuonEfficiency/ElectronChainIdMap.h"
// eof
