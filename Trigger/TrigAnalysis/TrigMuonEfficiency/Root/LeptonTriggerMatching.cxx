/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*=================================================================* 
 * Implementation of LeptonTriggerMatching
 *=================================================================*/
#include "TrigMuonEfficiency/LeptonTriggerMatching.h"
#include "TrigMuonEfficiency/ChainEntry.h"
#include "TrigMuonEfficiency/ElectronHypothesis.h"
#include "TrigMuonEfficiency/MuonHypothesis.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <TMath.h>
#include <cmath>



/*=================================================================* 
 * public methods
 *=================================================================*/
/*-----------------------------------------------------------------* 
 * for developers
 *-----------------------------------------------------------------*/
void
LeptonTriggerMatching::showSMKeys()
{
  std::map<int, std::map<std::string, int> >::const_iterator p;
  std::cout << "LeptonTriggerMatching\t" << "INFO\t" << "SMKeys:";
  for (p = m_MapOfChainIdMap.begin(); p != m_MapOfChainIdMap.end(); ++p) {
    std::cout << p->first << ", ";
  }
  std::cout << std::endl;
}



void
LeptonTriggerMatching::dumpChainIdMap(const int SMK)
{
  if (m_MapOfChainIdMap.find(SMK) == m_MapOfChainIdMap.end()) {
    std::cerr << "LeptonTriggerMatching\t" << "ERROR\t"
              << "ChainIdMap for SMK = " << SMK << " is not stored." << std::endl;
    return;
  }

  std::map<std::string, int> ChainIdMap = m_MapOfChainIdMap[SMK];
  std::map<std::string, int>::const_iterator p;
  std::cout << "LeptonTriggerMatching\t" << "INFO\t"
            << "dump ChainIdMap for SMKeys:" << SMK << std::endl;

  for (p = ChainIdMap.begin(); p != ChainIdMap.end(); ++p) {
    std::cout << p->first << "\t" << p->second << std::endl;
  }
}



void
LeptonTriggerMatching::createChainIdMapFile(const int SMK,
                                            const std::string& fileName)
{
  if (m_MapOfChainIdMap.find(SMK) == m_MapOfChainIdMap.end()) {
    std::cerr << "LeptonTriggerMatching\t" << "ERROR\t"
              << "ChainIdMap for SMK = " << SMK << " is not stored." << std::endl;
    return;
  }

  std::map<std::string, int> ChainIdMap = m_MapOfChainIdMap[SMK];
  std::map<std::string, int>::const_iterator p;

  std::ofstream ofs(fileName.c_str());
  ofs << "void LeptonTriggerMatching::createChainIdMapFromFile() {" << "\n";
  ofs << "  " << "std::map<std::string, int> ChainIdMap;" << "\n";
  for (p = ChainIdMap.begin(); p != ChainIdMap.end(); ++p) {
    ofs << "  " << "ChainIdMap.insert(std::pair<std::string, int>(\"" << p->first << "\", " << p->second << "));" << "\n";
  }
  ofs << "  " << "m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(-1, ChainIdMap));" << "\n";
  ofs << "}" << std::endl;
}



/*=================================================================* 
 * protected methods
 *=================================================================*/
double
LeptonTriggerMatching::matchedTrackDetail(EFmuon& efMuonId,
                                          const EFmuon& usedEfMuonId,
                                          const std::string& threshold,
                                          const double eta,
                                          const double phi,
                                          const std::vector<int>& container,
                                          const std::vector<int>& status)
{
  double dR_min = m_matchingDr;

  if (m_debug) std::cout << "DEBUG: # of muon container = " << container.size() << "\n";
  for (size_t iCont = 0; iCont < container.size(); ++iCont) {
    if (not status.at(iCont)) continue;

    if (efMuonId.isIso) {
      if (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainer == 0) throw "trig_RoI_EF_mu_TrigMuonEFIsolationContainer == 0";
      if (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus == 0) throw "trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus == 0";

      const std::vector<int>& vIsoContainer __attribute__((unused)) = (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainer)->at(efMuonId.roi_index);
      const std::vector<int>& vIsoStatus = (m_d3pd->trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus)->at(efMuonId.roi_index);

      if (not vIsoStatus.at(iCont)) continue;
    }

    const int container_index = container.at(iCont);
    const size_t container_size = (efMuonId.chain_type == CHAIN_MG) ? MUGIRL_CB(pt)->size() :
                                   ((efMuonId.chain_type == CHAIN_SA) ? MUONEF_SA(pt)->size() : MUONEF_CB(pt)->size());
    if (m_debug) {
      if (MUGIRL_CB(pt)) std::cout << "DEBUG: container_size (MG) = " << MUGIRL_CB(pt)->size() << "\n";
      if (MUONEF_SA(pt)) std::cout << "DEBUG: container_size (SA) = " << MUONEF_SA(pt)->size() << "\n";
      if (MUONEF_CB(pt)) std::cout << "DEBUG: container_size (CB) = " << MUONEF_CB(pt)->size() << "\n";
    }

    if ((container_index < 0) or (container_index >= (int) container_size)) {
      std::cerr << "ERROR: invalid container_index: container_size = " << container_size
                << " container_index = " << container_index << std::endl;
      throw "NotImplementedError";
    }

    const std::vector<float>& v_pt = (efMuonId.chain_type == CHAIN_MG) ? MUGIRL_CB(pt)->at(container_index) :
      ((efMuonId.chain_type == CHAIN_SA) ? MUONEF_SA(pt)->at(container_index) : MUONEF_CB(pt)->at(container_index));
    const std::vector<float>& v_eta = (efMuonId.chain_type == CHAIN_MG) ? MUGIRL_CB(eta)->at(container_index) :
      ((efMuonId.chain_type == CHAIN_SA) ? MUONEF_SA(eta)->at(container_index) : MUONEF_CB(eta)->at(container_index));
    const std::vector<float>& v_phi = (efMuonId.chain_type == CHAIN_MG) ? MUGIRL_CB(phi)->at(container_index) :
      ((efMuonId.chain_type == CHAIN_SA) ? MUONEF_SA(phi)->at(container_index) : MUONEF_CB(phi)->at(container_index));

    if (m_debug) std::cout << "DEBUG: # of tracks = " << v_pt.size() << "\n";

    for (size_t iTrk = 0; iTrk < v_pt.size(); ++iTrk) {
      if (m_debug) std::cout << "DEBUG: (pt, eta, phi) = ("
                             << v_pt.at(iTrk) << ", " << v_eta.at(iTrk) << ", " << v_phi.at(iTrk) << ")\n";

      if (usedEfMuonId.valid and
          isEqual(usedEfMuonId.pt, v_pt.at(iTrk)) and
          isEqual(usedEfMuonId.eta, v_eta.at(iTrk)) and
          isEqual(usedEfMuonId.phi, v_phi.at(iTrk))) continue;

      if (not MuonHypothesis::EF_isPassed(v_pt.at(iTrk), v_eta.at(iTrk), threshold)) continue;
      double dR_tmp = dR(eta, phi, v_eta.at(iTrk), v_phi.at(iTrk));
      if (not (dR_tmp < dR_min)) continue;
      dR_min = dR_tmp;

      efMuonId.valid = true;
      efMuonId.pt = v_pt.at(iTrk);
      efMuonId.eta = v_eta.at(iTrk);
      efMuonId.phi = v_phi.at(iTrk);
      efMuonId.indicies.first = container_index;
      efMuonId.indicies.second = iTrk;
    }
  }

  return dR_min;
}



double
LeptonTriggerMatching::matchedTrackDetail(EFelectron& efElectronId,
                                          const EFelectron& usedEfElectronId,
                                          const double eta,
                                          const double phi,
                                          const std::vector<int>& container,
                                          const std::vector<int>& status)
{
  double dR_min = m_matchingDr;

  for (size_t iCont = 0; iCont < container.size(); ++iCont) {
    if (not status.at(iCont)) continue;
    const int id = container.at(iCont);
    const double el_Et = efElectronId.useThreshold ? m_d3pd->trig_EF_el_Et->at(id) : 0.;
    const double el_eta = m_d3pd->trig_EF_el_eta->at(id);
    const double el_phi = m_d3pd->trig_EF_el_phi->at(id);

    if (usedEfElectronId.valid) {
      if (isEqual(usedEfElectronId.eta, el_eta) and
          isEqual(usedEfElectronId.phi, el_phi)) continue;
    }

    if (efElectronId.useThreshold and not ElectronHypothesis::EF_isPassed(el_Et, efElectronId.threshold)) continue;
    double dr = dR(eta, phi, el_eta, el_phi);
    if (not (dr < dR_min)) continue;
    dR_min = dr;

    efElectronId.valid = true;
    efElectronId.eta = el_eta;
    efElectronId.phi = el_phi;
  }

  return dR_min;
}



double
LeptonTriggerMatching::index_match(const double eta,
                                   const double phi,
                                   const std::string& chain,
                                   const std::string& threshold,
                                   EFmuon& efMuonId,
                                   const EFmuon& usedEfMuonId)
{
  efMuonId.valid = false;
  double dR_min = m_matchingDr;

  if (not m_isIndexMatchUsed) {
    m_isIndexMatchUsed = true;
    std::cout << "INFORMATION: index_match is used" << std::endl;
  }

  const std::vector<int>* status = 0;
  if (chain.compare("EF_mu24i_tight") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu24i_tight;

  } else if (chain.compare("EF_mu36_tight") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu36_tight;

  } else if (chain.compare("EF_mu18_tight_mu8_EFFS") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS;

  } else if (chain.compare("EF_2mu13") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_2mu13;

  } else if (chain.compare("EF_mu15") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu15;

  } else if (chain.compare("EF_mu8") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu8;

  } else if (chain.compare("EF_mu40_MSonly_barrel_tight") == 0) {
    status = m_d3pd->trig_EF_trigmuonef_EF_mu40_MSonly_barrel_tight;
  }

  if (not status) {
    std::cout << "WARNING: index_match: not supported: " << chain << std::endl;
    return false;
  }

  if (status->size() != MUONEF_CB(pt)->size()) {
    std::cout << "ERROR: index_match: length mismatch: " << std::endl;
    return false;
  }


  for (size_t ii = 0; ii < status->size(); ii++) {
    if (not status->at(ii)) continue;

    const std::vector<float>& v_pt = MUONEF_CB(pt)->at(ii);
    const std::vector<float>& v_eta = MUONEF_CB(eta)->at(ii);
    const std::vector<float>& v_phi = MUONEF_CB(phi)->at(ii);

    for (size_t iTrk = 0; iTrk < v_pt.size(); ++iTrk) {
      if (usedEfMuonId.valid and
          isEqual(usedEfMuonId.pt, v_pt.at(iTrk)) and
          isEqual(usedEfMuonId.eta, v_eta.at(iTrk)) and
          isEqual(usedEfMuonId.phi, v_phi.at(iTrk))) continue;

      if (not MuonHypothesis::EF_isPassed(v_pt.at(iTrk), v_eta.at(iTrk), threshold)) continue;
      double dR_tmp = dR(eta, phi, v_eta.at(iTrk), v_phi.at(iTrk));
      if (not (dR_tmp < dR_min)) continue;
      dR_min = dR_tmp;

      efMuonId.valid = true;
      efMuonId.pt = v_pt.at(iTrk);
      efMuonId.eta = v_eta.at(iTrk);
      efMuonId.phi = v_phi.at(iTrk);
      efMuonId.indicies.first = ii;
      efMuonId.indicies.second = iTrk;
    }
  }

  return dR_min;
}



bool
LeptonTriggerMatching::L1_isPassed(const int iRoI,
                                 const std::string& l1item)
{
  if (iRoI < 0) return false;
  return getL1pt(m_d3pd->trig_L1_mu_thrName->at(iRoI)) >= getL1pt(l1item);
}



int
LeptonTriggerMatching::getL1pt(const std::string& l1item)
{
  int rc = -1;

  std::vector<std::string> tokens;
  tokenize(l1item, tokens, "_");
  std::string pt;
  if (tokens.size() == 1) {
    pt = tokens.at(0);

  } else if ((tokens.size() == 2 ) and (tokens.at(0) == "L1")) {
    pt = tokens.at(1);

  } else {
    std::cerr << "LeptonTriggerMatching::getL1pt: cannot parse: " << l1item << std::endl;
    return rc;
  }

  const size_t index = pt.find("MU");
  if (index != std::string::npos) {
    std::istringstream iss(pt.substr(index + 2));
    iss >> rc;
  } else {
    std::cerr << "LeptonTriggerMatching::getL1pt: cannot parse: " << l1item << std::endl;
  }

  return rc;
}



void
LeptonTriggerMatching::readTrigConfTree(TTree* TrigConfTree,
                                        const std::vector<std::string>& skip)
{
  int smk=-9999;
  std::map<std::string, int>* hltmap=0;

  TrigConfTree->SetBranchAddress("SMK", &smk);
  TrigConfTree->SetBranchAddress("HLTNameMap", &hltmap);

  TrigConfTree->GetEntry(0);
  if (not ((smk != -9999) and hltmap)) {
    std::cerr << "LeptonTriggerMatching\t" << "ERROR\t"
              << "TrigConfTree does exist, but could not SetBranchAddress for SMK and HLTNameMap." << std::endl;
    return;
  }

  std::map<std::string, int>::const_iterator p;
  for (int iEntry = 0; iEntry < TrigConfTree->GetEntries(); ++iEntry) {
    TrigConfTree->GetEntry(iEntry);
    if (m_MapOfChainIdMap.count(smk)) continue;

    std::map<std::string, int> ChainIdMap;
    for (p = hltmap->begin(); p != hltmap->end(); ++p) {
      bool keep = false;
      for (std::vector<std::string>::const_iterator exp = skip.begin(); exp != skip.end(); exp++) {
        keep |= (p->first.find(*exp) == 0);
      }
      if (not keep) continue;

      int id = p->second;
      if (p->first.find("EF_") == 0) id += ChainEntry::kEFChainIdOffset;
      ChainIdMap.insert(std::pair<std::string, int>(p->first, id));
    }
    m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(smk, ChainIdMap));
  }
}



int
LeptonTriggerMatching::getChainIndex(const std::string& chain)
{
  if (not m_d3pd->isValid()) std::cerr << "LeptonTriggerMatching\tWARNING\tPlease check initialisation setting of TriggerNavigationVariables\n";

  int smk = static_cast<int>(*(m_d3pd->trig_DB_SMK));
  std::map<std::string, int> ChainIdMap = (m_MapOfChainIdMap.count(smk)) ? m_MapOfChainIdMap[smk] : m_MapOfChainIdMap[-1];

  if (ChainIdMap.find(chain) == ChainIdMap.end()) {
    if (m_NotFoundChain.find(chain) == m_NotFoundChain.end()) {
      std::cerr << "LeptonTriggerMatching\t" << "ERROR\t"
                << chain << " does not exist in ChainIdMap for SMK = " << smk
                << " (if SMK = -1, it means the Map is created from TrigMuonEfficiency/XxxxChainIdMap.h)" << std::endl;
      m_NotFoundChain.insert(chain);
    }
    return -1;
  }

  const short ChainId = static_cast<short>(ChainIdMap[chain]);
  if (m_debug) std::cout << "DEBUG: ChainId = " << ChainId << " for " << chain << " [SMK = " << smk << "]\n";
  for (int iChain = 0; iChain < *(m_d3pd->trig_Nav_n); ++iChain) {
    if (m_d3pd->trig_Nav_chain_ChainId->at(iChain) == ChainId) return iChain;
  }

  if (m_NotFoundChain.find(chain) == m_NotFoundChain.end()) {
    std::cerr << "LeptonTriggerMatching\t" << "ERROR\t"
              << chain << " does not exist in Flat Navigation Branch." << std::endl;
    m_NotFoundChain.insert(chain);
  }
  return -1;
}



void
LeptonTriggerMatching::tokenize(const std::string& str,
                              std::vector<std::string>& tokens,
                              const std::string& delimiters)
{
  tokens.clear();
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while ((std::string::npos != pos) or (std::string::npos != lastPos)) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}



bool
LeptonTriggerMatching::isEqual(const double x,
                             const double y)
{
  if (fabs(x - y) < std::numeric_limits<float>::epsilon()) return true;
  return false;
}



double
LeptonTriggerMatching::dR(const double eta1,
                        const double phi1,
                        const double eta2,
                        const double phi2)
{
  double deta = fabs(eta1 - eta2);
  double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath::Pi() - fabs(phi1 - phi2);
  return sqrt(deta*deta + dphi*dphi);
}
// eof
