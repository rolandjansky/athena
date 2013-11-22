/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*=================================================================* 
 * Implementation of MuonTriggerMatching
 *=================================================================*/
#include "TrigMuonEfficiency/ChainEntry.h"
#include "TrigMuonEfficiency/MuonTriggerMatching.h"
#include "TrigMuonEfficiency/MuonHypothesis.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <TMath.h>
#include <cmath>


/*=================================================================* 
 * public methods
 *=================================================================*/
bool
MuonTriggerMatching::match(const double eta,
                           const double phi,
                           const std::string& chain,
                           const std::string& l1item)
{
  EFmuon efmuon, dummy;
  if (m_index) {
    matchedTrackDetail_index(efmuon, dummy, eta, phi, chain, l1item);
  } else {
    matchedTrackDetail(efmuon, dummy, eta, phi, chain, l1item);
  }
  return efmuon.valid;
}


std::pair<int, int>
MuonTriggerMatching::matchedTrack(const double eta,
                                  const double phi,
                                  const std::string& chain,
                                  const std::string& l1item)
{
  EFmuon efmuon, dummy;
  if (m_index) {
    matchedTrackDetail_index(efmuon, dummy, eta, phi, chain, l1item);
  } else {
    matchedTrackDetail(efmuon, dummy, eta, phi, chain, l1item);
  }
  return efmuon.indicies;
}


bool
MuonTriggerMatching::matchDimuon(const TLorentzVector& muon1,
                                 const TLorentzVector& muon2,
                                 const std::string& chain,
                                 std::pair<bool, bool>& result1,
                                 std::pair<bool, bool>& result2)
{
  DimuonChainInfo chainInfo(chain);
  if (not decodeDimuonChain(chainInfo)) {
    std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
              << "Failed to decode chain(" << chain
              << "matchDimuon can accept only chains named EF_2muXX_YY or EF_muXX_muYY_ZZ." << std::endl;
    return false;
  }

  std::pair<bool, bool> rc12, rc21;
  rc12 = matchDimuon(muon1, muon2, chainInfo);
  if (chainInfo.isSymmetric) {
    rc21.first = rc12.second; rc21.second = rc12.first;
  } else {
    rc21 = matchDimuon(muon2, muon1, chainInfo);
  }

  result1.first = rc12.first; result1.second = rc21.second;
  result2.first = rc21.first; result2.second = rc12.second;
  return true;
}



/*=================================================================* 
 * private methods
 *=================================================================*/
bool
MuonTriggerMatching::checkSeed_index(const double eta,
                                     const double phi,
                                     const std::string& chain)
{
  const std::vector<int>* status = 0;
  if (chain.compare("EF_mu18_tight") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu18_tight;
  }

  if (not status) {
    std::cout << "WARNING: checkSeed_index: not supported: " << chain << std::endl;
    return false;
  }

  if (not m_d3pd->trig_EF_trigmuonef_track_MuonType) {
    std::cout << "ERROR: full-scan trigger matching requested but trig_EF_trigmuonef_track_MuonType == 0" << std::endl;
    return false;
  }

  double efdr = 0.05;

  bool found = false;
  for (size_t ii = 0; ii < status->size(); ii++) {
    if (not status->at(ii)) continue;

    const std::vector<float>& v_pt = MUONEF_CB(pt)->at(ii);
    const std::vector<float>& v_eta = MUONEF_CB(eta)->at(ii);
    const std::vector<float>& v_phi = MUONEF_CB(phi)->at(ii);
    const std::vector<int>& v_type = m_d3pd->trig_EF_trigmuonef_track_MuonType->at(ii);

    for (size_t iTrk = 0; iTrk < v_pt.size(); ++iTrk) {
      if (v_type.at(iTrk) != 1) continue; // !MuonTypeEFMuon
      if (not MuonHypothesis::EF_isPassed(v_pt.at(iTrk), v_eta.at(iTrk), chain)) continue;
      double dr = dR(eta, phi, v_eta.at(iTrk), v_phi.at(iTrk));
      if (not (dr < efdr)) continue;
      efdr = dr;
      found = true;
    }
  }
  return found;
}



bool
MuonTriggerMatching::checkSeedL2(const double eta,
                                 const double phi,
                                 const std::string& chain,
                                 double l2dr,
                                 double l1dr)
{
  const int chainIndex = getChainIndex(chain);
  if (chainIndex < 0) return false;

  const std::vector<int>& RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(chainIndex);
  const std::vector<int>& RoIType = m_d3pd->trig_Nav_chain_RoIType->at(chainIndex);

  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    if (RoIType.at(iRoI) != ChainEntry::kRoIType_L2_mu) continue;
    if (not m_d3pd->trig_RoI_L2_mu_CombinedMuonFeatureStatus->at(RoIIndex.at(iRoI))) continue;

    double minDr = l2dr;
    const int muComb = m_d3pd->trig_RoI_L2_mu_CombinedMuonFeature->at(RoIIndex.at(iRoI));
    double trigEta = m_d3pd->trig_L2_combmuonfeature_eta->at(muComb);
    double trigPhi = m_d3pd->trig_L2_combmuonfeature_phi->at(muComb);

    // L2 ID tracks can be truncated after trigger decision,
    // as a consequence (eta,phi) of CombinedMuonFeature become (0,0)
    // We use MuonFeature in this case.
    if (isEqual(trigEta, 0.) and isEqual(trigPhi, 0.)) {
      const int muFast = m_d3pd->trig_RoI_L2_mu_MuonFeature->at(RoIIndex.at(iRoI));
      trigEta = m_d3pd->trig_L2_muonfeature_eta->at(muFast);
      trigPhi = m_d3pd->trig_L2_muonfeature_phi->at(muFast);
    }

    // pathlogical case, use L1 for matching
    if (isEqual(trigEta, 0.) and isEqual(trigPhi, 0.)) {
      const int muRoI = m_d3pd->trig_RoI_L2_mu_Muon_ROI->at(RoIIndex.at(iRoI));
      trigEta = m_d3pd->trig_L1_mu_eta->at(muRoI);
      trigPhi = m_d3pd->trig_L1_mu_phi->at(muRoI);
      minDr = l1dr;
    }

    if (not (dR(eta, phi, trigEta, trigPhi) < minDr)) continue;
    return true;
  }

  return false;
}



bool
MuonTriggerMatching::checkSeedEF(const double eta,
                                 const double phi,
                                 const std::string& chain)
{
  double efdr = 0.05;
  const int chainIndex = getChainIndex(chain);
  if (chainIndex < 0) {
    return checkSeed_index(eta, phi, chain);
  }

  const std::vector<int>& RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(chainIndex);
  const std::vector<int>& RoIType = m_d3pd->trig_Nav_chain_RoIType->at(chainIndex);

  if ((chain.find("_MSonly") != std::string::npos) or
      (chain.find("_MG") != std::string::npos) or
      (chain.find("i_") != std::string::npos)) {
    std::cout << "MuonTriggerMatching::checkSeedEF: " << chain << "\n";
    throw "NotImplementedError";
  }

  if (not m_d3pd->trig_EF_trigmuonef_track_MuonType) {
    std::cout << "ERROR: full-scan trigger matching requested but trig_EF_trigmuonef_track_MuonType == 0" << std::endl;
    return false;
  }


  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    if (RoIType.at(iRoI) != ChainEntry::kRoIType_EF_mu) continue;
    const int roi_index = RoIIndex.at(iRoI);

    const std::vector<int>& EFmuonCont = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainer->at(roi_index);
    const std::vector<int>& status = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus->at(roi_index);

    for (size_t iCont = 0; iCont < EFmuonCont.size(); ++iCont) {
      if (not status.at(iCont)) continue;

      const int container_index = EFmuonCont.at(iCont);
      const std::vector<float>& v_pt = MUONEF_CB(pt)->at(container_index);
      const std::vector<float>& v_eta = MUONEF_CB(eta)->at(container_index);
      const std::vector<float>& v_phi = MUONEF_CB(phi)->at(container_index);
      const std::vector<int>& v_type = m_d3pd->trig_EF_trigmuonef_track_MuonType->at(container_index);

      for (size_t iTrk = 0; iTrk < v_pt.size(); ++iTrk) {
        if (v_type.at(iTrk) != 1) continue; // !MuonTypeEFMuon
        if (not MuonHypothesis::EF_isPassed(v_pt.at(iTrk), v_eta.at(iTrk), chain)) continue;
        double dr = dR(eta, phi, v_eta.at(iTrk), v_phi.at(iTrk));
        if (not (dr < efdr)) continue;
        efdr = dr;
        return true;
      }
    }
  }

  return false;
}


double
MuonTriggerMatching::matchedTrackDetail(EFmuon& efMuonId,
                                        const EFmuon& usedEfMuonId,
                                        const double eta,
                                        const double phi,
                                        const std::string& chainForThisMuon,
                                        const std::string& l1item,
                                        const std::string& chainForEventTrigger)
{
  if (m_debug) std::cout << "DEBUG: matchedTrackDetail: (eta, phi) = (" << eta << ", " << phi << ") "
                         << chainForThisMuon << " " << l1item << " " << chainForEventTrigger << "\n";

  efMuonId.indicies.first = -1;
  efMuonId.indicies.second = -1;
  efMuonId.iso_index = -1;
  efMuonId.valid = false;
  efMuonId.isIso = false;
  efMuonId.chain_type = CHAIN_CB;
  double dR_min = m_matchingDr;

  const std::string eventTrigger = chainForEventTrigger.empty() ? chainForThisMuon : chainForEventTrigger;
  const int chainIndex = getChainIndex(eventTrigger);
  if (chainIndex < 0) {
    return index_match(eta, phi, eventTrigger, chainForThisMuon, efMuonId, usedEfMuonId);
  }

  if (not m_isNavigationMatchUsed) {
    m_isNavigationMatchUsed = true;
    std::cout << "INFORMATION: navigation is used" << std::endl;
  }

  const std::vector<int>& RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(chainIndex);
  const std::vector<int>& RoIType = m_d3pd->trig_Nav_chain_RoIType->at(chainIndex);
  
  if (chainForThisMuon.find("_MSonly") != std::string::npos) efMuonId.chain_type = CHAIN_SA;
  if (chainForThisMuon.find("_MG") != std::string::npos) efMuonId.chain_type = CHAIN_MG;
  if (chainForThisMuon.find("i_") != std::string::npos) efMuonId.isIso = true;

  if (m_debug) std::cout << "DEBUG: chain_type = " << efMuonId.chain_type << " (SA = " << CHAIN_SA << "  MG = " << CHAIN_MG << ")\n";
  if (m_debug) std::cout << "DEBUG: " << (efMuonId.isIso ? "" : "not ") << "Isolated Trigger\n";

  if (efMuonId.isIso) {
    if (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainer == 0) {
      std::cerr << "ERROR: matching with isolated muon trigger required but trig_RoI_EF_mu_TrigMuonEFIsolationContainer == 0" << std::endl;
      return dR_min;
    }
    if (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus == 0) {
      std::cerr << "ERROR: matching with isolated muon trigger required but trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus == 0" << std::endl;
      return dR_min;
    }
  }


  if (m_debug) std::cout << "DEBUG: # of RoI = " << RoIIndex.size() << "\n";

  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    if (RoIType.at(iRoI) != ChainEntry::kRoIType_EF_mu) continue;
    efMuonId.roi_index = RoIIndex.at(iRoI);

    if (not l1item.empty()) {
      if (not L1_isPassed(m_d3pd->trig_RoI_EF_mu_Muon_ROI->at(efMuonId.roi_index), l1item)) continue;
    }

    const std::vector<int>& container = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainer->at(efMuonId.roi_index);
    const std::vector<int>& status = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus->at(efMuonId.roi_index);

    double dr = LeptonTriggerMatching::matchedTrackDetail(efMuonId, usedEfMuonId, chainForThisMuon, eta, phi, container, status);
    if (dr < dR_min) dR_min = dr;
  }

  return dR_min;
}



double
MuonTriggerMatching::matchedTrackDetail_index(EFmuon& efMuonId,
                                              const EFmuon& usedEfMuonId,
                                              const double eta,
                                              const double phi,
                                              const std::string& chainForThisMuon,
                                              const std::string& /*l1item*/,
                                              const std::string& chainForEventTrigger)
{
  efMuonId.indicies.first = -1;
  efMuonId.indicies.second = -1;
  efMuonId.iso_index = -1;
  efMuonId.valid = false;
  efMuonId.isIso = false;

  const std::string eventTrigger = chainForEventTrigger.empty() ? chainForThisMuon : chainForEventTrigger;
  return index_match(eta, phi, eventTrigger, chainForThisMuon, efMuonId, usedEfMuonId);
}



std::pair<bool, bool>
MuonTriggerMatching::matchDimuon(const TLorentzVector& muon1,
                                 const TLorentzVector& muon2,
                                 const DimuonChainInfo& chainInfo)
{
  EFmuon trkId1, trkId2, dummy;
  const double dr1 = m_index ? matchedTrackDetail_index(trkId1, dummy, muon1.Eta(), muon1.Phi(), chainInfo.thresholds.first, "", chainInfo.chain) :
                                     matchedTrackDetail(trkId1, dummy, muon1.Eta(), muon1.Phi(), chainInfo.thresholds.first, "", chainInfo.chain);
  if (chainInfo.isEFFS and trkId1.valid) {
    if (chainInfo.is2011) {
      trkId1.valid = checkSeedL2(muon1.Eta(), muon1.Phi(), chainInfo.l2seed);
    } else {
      trkId1.valid = m_index ? checkSeed_index(trkId1.eta, trkId1.phi, chainInfo.efseed) :
                                   checkSeedEF(trkId1.eta, trkId1.phi, chainInfo.efseed);
    }
  }
  const double dr2 = m_index ? matchedTrackDetail_index(trkId2, dummy, muon2.Eta(), muon2.Phi(), chainInfo.thresholds.second, "", chainInfo.chain) :
                                     matchedTrackDetail(trkId2, dummy, muon2.Eta(), muon2.Phi(), chainInfo.thresholds.second, "", chainInfo.chain);

  if (trkId1.valid and trkId2.valid and
      isEqual(trkId1.pt, trkId2.pt) and
      isEqual(trkId1.eta, trkId2.eta) and
      isEqual(trkId1.phi, trkId2.phi)) {
    if (dr1 > dr2) {
      if (m_index) {
        matchedTrackDetail_index(trkId1, trkId2, muon1.Eta(), muon1.Phi(), chainInfo.thresholds.first, "", chainInfo.chain);
      } else {
        matchedTrackDetail(trkId1, trkId2, muon1.Eta(), muon1.Phi(), chainInfo.thresholds.first, "", chainInfo.chain);
      }
      if (chainInfo.isEFFS and trkId1.valid) {
        if (chainInfo.is2011) {
          trkId1.valid = checkSeedL2(muon1.Eta(), muon1.Phi(), chainInfo.l2seed);
        } else {
          trkId1.valid = m_index ? checkSeed_index(trkId1.eta, trkId1.phi, chainInfo.efseed) :
                                       checkSeedEF(trkId1.eta, trkId1.phi, chainInfo.efseed);
        }
      }

    } else {
      if (m_index) {
        matchedTrackDetail_index(trkId2, trkId1, muon2.Eta(), muon2.Phi(), chainInfo.thresholds.second, "", chainInfo.chain);

      } else {
        matchedTrackDetail(trkId2, trkId1, muon2.Eta(), muon2.Phi(), chainInfo.thresholds.second, "", chainInfo.chain);
      }
    }
  }
  return std::pair<bool, bool>(trkId1.valid, trkId2.valid);
}



bool
MuonTriggerMatching::decodeDimuonChain(DimuonChainInfo& chainInfo)
{
  chainInfo.isValid = false;

  std::map<std::string, DimuonChainInfo >::const_iterator p = m_DimuonChainMap.find(chainInfo.chain);
  if (p != m_DimuonChainMap.end()) {
    chainInfo = p->second;
    return chainInfo.isValid;
  }

  std::vector<std::string> tokens;
  tokenize(chainInfo.chain, tokens, "_");
  if (tokens.size() < 2) return false;
  if (tokens[0] != "EF") return false;
  chainInfo.isSymmetric = (tokens[1].substr(0, 3) == "2mu");

  if (chainInfo.isSymmetric) {
    std::string threshold = std::string("EF_" + tokens[1].substr(1));
    chainInfo.thresholds.first = threshold;
    chainInfo.thresholds.second = threshold;
    chainInfo.isValid = true;
    if (tokens.size() == 3) chainInfo.tightness = tokens[2];

  } else {
    if (tokens.size() < 4) return false;

    if (tokens[3] == "EFFS") {  // 2011
      if ((tokens[1].substr(0,2) != "mu") or (tokens[2].substr(0,2) != "mu")) return false;
      std::string threshold1 = std::string("EF_" + tokens[1]);
      std::string threshold2 = std::string("EF_" + tokens[2]);
      chainInfo.isEFFS = true;
      chainInfo.thresholds.first = threshold1;
      chainInfo.thresholds.second = threshold2;
      if (tokens.size() == 5) chainInfo.tightness = tokens[4];
      chainInfo.l2seed = "L2_" + tokens[1] + (chainInfo.tightness.empty() ? "" : "_" + chainInfo.tightness);
      chainInfo.is2011 = true;
      chainInfo.isValid = true; 

    } else if (tokens[4] == "EFFS") { // 2012
      if ((tokens[1].substr(0,2) != "mu") or (tokens[3].substr(0,2) != "mu")) return false;
      std::string threshold1 = std::string("EF_" + tokens[1]);
      std::string threshold2 = std::string("EF_" + tokens[3]);
      chainInfo.isEFFS = true;
      chainInfo.thresholds.first = threshold1;
      chainInfo.thresholds.second = threshold2;
      if (tokens.size() == 5) chainInfo.tightness = tokens[2];
      chainInfo.efseed = "EF_" + tokens[1] + (chainInfo.tightness.empty() ? "" : "_" + chainInfo.tightness);
      chainInfo.is2011 = false;
      chainInfo.isValid = true; 

    } else {
      return false;
    }
  }

  m_DimuonChainMap[chainInfo.chain] = chainInfo;

  return chainInfo.isValid;
}
#include "TrigMuonEfficiency/MuonChainIdMap.h"
// eof
