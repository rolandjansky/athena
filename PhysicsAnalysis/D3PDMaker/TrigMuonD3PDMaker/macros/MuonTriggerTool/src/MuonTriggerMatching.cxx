/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*=================================================================* 
 * Implementation of MuonTriggerMatching
 *=================================================================*/
#include "../MuonTriggerTool/MuonTriggerMatching.h"
#include "../MuonTriggerTool/MuonHypothesis.h"
#include "ChainIdMap.hxx"

#include <iostream>
#include <fstream>
#include <sstream>
#include <TMath.h>

#define _CB_BR_(x) trig_EF_trigmuonef_track_CB_##x
#define _SA_BR_(x) trig_EF_trigmuonef_track_SA_##x
#define _MG_BR_(x) trig_EF_trigmugirl_track_CB_##x



/*=================================================================* 
 * public methods
 *=================================================================*/
bool
MuonTriggerMatching::match(const double eta,
                           const double phi,
                           const std::string& chain,
                           const std::string& l1item)
{
  // get Index to access Navigation Branch (check SMKey and use correct HLTNameMap)
  const int ChainIndex = getChainIndex(chain);
  if (ChainIndex < 0) return false;
  const std::vector<int> RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(ChainIndex);
  const std::vector<int> RoIType = m_d3pd->trig_Nav_chain_RoIType->at(ChainIndex);
  
  unsigned int chain_type = CB;
  if (chain.find("_MSonly") != std::string::npos) chain_type = SA;
  if (chain.find("_MG") != std::string::npos) chain_type = MG;

  // Loop Over physical RoI associated to given EF chain
  for (size_t iRoI = 0; iRoI < RoIIndex.size(); ++iRoI) {
    // check RoIType
    if (RoIType[iRoI] != ChainEntry::kRoIType_EF_mu) continue;


    // require L1 thresholds if required
    if (not l1item.empty()) {
      if (not L1_isPassed(m_d3pd->trig_RoI_EF_mu_Muon_ROI->at(RoIIndex[iRoI]), l1item)) continue;
    }

    const std::vector<int> EFmuonCont = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainer->at(RoIIndex[iRoI]);
    // Loop Over Containers in the RoI (EF muon container can exist 2 or more in a RoI)
    for (size_t iCont = 0; iCont < EFmuonCont.size(); ++iCont) {
      std::vector<float> v_pt = (chain_type==MG) ? m_d3pd->_MG_BR_(pt)->at(EFmuonCont[iCont]) :          //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(pt)->at(EFmuonCont[iCont]) : m_d3pd->_CB_BR_(pt)->at(EFmuonCont[iCont]) );  //TrigMuonEF
      std::vector<float> v_eta = (chain_type==MG) ? m_d3pd->_MG_BR_(eta)->at(EFmuonCont[iCont]) :         //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(eta)->at(EFmuonCont[iCont]) : m_d3pd->_CB_BR_(eta)->at(EFmuonCont[iCont]) ); //TrigMuonEF
      std::vector<float> v_phi = (chain_type==MG) ? m_d3pd->_MG_BR_(phi)->at(EFmuonCont[iCont]) :         //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(phi)->at(EFmuonCont[iCont]) : m_d3pd->_CB_BR_(phi)->at(EFmuonCont[iCont]) ); //TrigMuonEF

      // Loop Over Tracks (TrigMuonEFInfo coud contain 2 or more tracks i)
      for (size_t iTrk = 0; iTrk < v_pt.size(); ++iTrk) {
        if (not MuonHypothesis::EF_isPassed(v_pt[iTrk], v_eta[iTrk], chain)) continue;
        if (not (dR(eta, phi, v_eta[iTrk], v_phi[iTrk]) < m_matchingDr)) continue;
        return true;
      }
    }
  }

  return false;
}


std::pair<int, int> 
MuonTriggerMatching::matchedTrack(const double eta,
                                  const double phi,
                                  const std::string& chain,
                                  const std::string& l1item)
{
  std::pair<int, int> best_matched_index(-1, -1);
  double dR_min=m_matchingDr;

  // get Index to access Navigation Branch (check SMKey and use correct HLTNameMap)
  const int ChainIndex = getChainIndex(chain);
  if(ChainIndex<0) return best_matched_index;
  const std::vector<int> RoIIndex = m_d3pd->trig_Nav_chain_RoIIndex->at(ChainIndex);
  const std::vector<int> RoIType  = m_d3pd->trig_Nav_chain_RoIType->at(ChainIndex);
  
  enum chain_type {
    CB = 0,
    SA = 1,
    MG = 2
  };

  unsigned int chain_type = CB;
  if( chain.find("_MSonly")!=std::string::npos ) chain_type = SA;
  if( chain.find("_MG")!=std::string::npos ) chain_type = MG;

  // Loop Over physical RoI associated to given EF chain
  for(size_t iRoI=0; iRoI<RoIIndex.size(); ++iRoI) {
    // check RoIType
    if(RoIType[iRoI]!=ChainEntry::kRoIType_EF_mu) continue;


    // require L1 thresholds if required
    if(l1item!="") {
      if( !L1_isPassed( m_d3pd->trig_RoI_EF_mu_Muon_ROI->at(RoIIndex[iRoI]), l1item) ) continue;
    }


    const std::vector<int> EFmuonCont = m_d3pd->trig_RoI_EF_mu_TrigMuonEFInfoContainer->at(RoIIndex[iRoI]);
    // Loop Over Containers in the RoI (EF muon container can exist 2 or more in a RoI)
    for(size_t iCont=0; iCont<EFmuonCont.size(); ++iCont) {
      std::vector<float> v_pt  = (chain_type==MG) ? m_d3pd->_MG_BR_(pt)->at(EFmuonCont[iCont]) :          //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(pt)->at(EFmuonCont[iCont])  : m_d3pd->_CB_BR_(pt)->at(EFmuonCont[iCont]) );  //TrigMuonEF
      std::vector<float> v_eta = (chain_type==MG) ? m_d3pd->_MG_BR_(eta)->at(EFmuonCont[iCont]) :         //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(eta)->at(EFmuonCont[iCont]) : m_d3pd->_CB_BR_(eta)->at(EFmuonCont[iCont]) ); //TrigMuonEF
      std::vector<float> v_phi = (chain_type==MG) ? m_d3pd->_MG_BR_(phi)->at(EFmuonCont[iCont]) :         //MuGirl
        ( (chain_type==SA) ? m_d3pd->_SA_BR_(phi)->at(EFmuonCont[iCont]) : m_d3pd->_CB_BR_(phi)->at(EFmuonCont[iCont]) ); //TrigMuonEF

      // Loop Over Tracks (TrigMuonEFInfo can contain 2 or more tracks)
      for(size_t iTrk=0; iTrk<v_pt.size(); ++iTrk) {
        if( !MuonHypothesis::EF_isPassed(v_pt[iTrk], v_eta[iTrk], chain) ) continue;
        double dR_tmp = dR(eta, phi, v_eta[iTrk], v_phi[iTrk]) ;
        if( dR_tmp < dR_min ) continue;
        dR_min = dR_tmp;
        best_matched_index.first = EFmuonCont[iCont];
        best_matched_index.first = iTrk;
      }
    }
  }
  return best_matched_index;
}



/*-----------------------------------------------------------------* 
 * for developers
 *-----------------------------------------------------------------*/
void
MuonTriggerMatching::showSMKeys()
{
  std::map<int, std::map<std::string, int> >::const_iterator p;
  std::cout << "MuonTriggerMatching\t" << "INFO\t" << "SMKeys:";
  for (p = m_MapOfChainIdMap.begin(); p != m_MapOfChainIdMap.end(); ++p) {
    std::cout << p->first << ", ";
  }
  std::cout << std::endl;
}


void
MuonTriggerMatching::dumpChainIdMap(const int SMK)
{
  if (m_MapOfChainIdMap.find(SMK) == m_MapOfChainIdMap.end()) {
    std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
              << "ChainIdMap for SMK = " << SMK << " is not stored." << std::endl;
    return;
  }

  std::map<std::string, int> ChainIdMap = m_MapOfChainIdMap[SMK];
  std::map<std::string, int>::const_iterator p;
  std::cout << "MuonTriggerMatching\t" << "INFO\t"
            << "dump ChainIdMap for SMKeys:" << SMK << std::endl;

  for (p = ChainIdMap.begin(); p != ChainIdMap.end(); ++p) {
    std::cout << p->first << "\t" << p->second << std::endl;
  }
}


void
MuonTriggerMatching::createChainIdMapFile(const int SMK)
{
  if (m_MapOfChainIdMap.find(SMK) == m_MapOfChainIdMap.end()) {
    std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
              << "ChainIdMap for SMK=" << SMK << " is not stored." << std::endl;
    return;
  }

  std::map<std::string, int> ChainIdMap = m_MapOfChainIdMap[SMK];
  std::map<std::string, int>::const_iterator p;

  std::ofstream ofs("ChainIdMap.hxx");
  ofs << "void MuonTriggerMatching::createChainIdMapFromFile() {" << "\n";
  ofs << "  " << "std::map<std::string, int> ChainIdMap;" << "\n";
  for (p = ChainIdMap.begin(); p != ChainIdMap.end(); ++p) {
    ofs << "  " << "ChainIdMap.insert( std::pair<std::string, int>(\"" << p->first << "\", " << p->second << ") );" << "\n";
  }
  ofs << "  " << "m_MapOfChainIdMap.insert( std::pair<int, std::map<std::string, int> >(-1, ChainIdMap) );" << "\n";
  ofs << "}" << std::endl;
}



/*=================================================================* 
 * private methods
 *=================================================================*/
int
MuonTriggerMatching::getChainIndex(const std::string& chain)
{
  int smk = static_cast<int>(m_d3pd->trig_DB_SMK);
  std::map<std::string, int> ChainIdMap = (m_MapOfChainIdMap.count(smk)) ? m_MapOfChainIdMap[smk] : m_MapOfChainIdMap[-1];

  if (ChainIdMap.find(chain) == ChainIdMap.end()) {
    if (m_NotFoundChain.find(chain) == m_NotFoundChain.end()) {
      std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
                << chain << " does not exist in ChainIdMap for SMK=" << smk
                << " (if SMK=-1, it means the Map is created from src/ChainIdMap.hxx)" << std::endl;
      m_NotFoundChain.insert(chain);
    }
    return -1;
  }

  const short ChainId = static_cast<short>(ChainIdMap[chain]);
  for (int iChain = 0; iChain < m_d3pd->trig_Nav_n; ++iChain) {
    if (m_d3pd->trig_Nav_chain_ChainId->at(iChain) == ChainId) return iChain;
  }

  if (m_NotFoundChain.find(chain) == m_NotFoundChain.end()) {
    std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
              << chain << " does not exist in Flat Navigation Branch." << std::endl;
    m_NotFoundChain.insert(chain);
  }
  return -1;
}


void
MuonTriggerMatching::readTrigConfTree(TTree* TrigConfTree)
{
  int smk=-9999;
  std::map<std::string, int>* hltmap=0;

  if (TrigConfTree) {
    TrigConfTree->SetBranchAddress("SMK", &smk);
    TrigConfTree->SetBranchAddress("HLTNameMap", &hltmap);

    TrigConfTree->GetEntry(0);
    if (not ((smk !=-9999) and hltmap)) {
      std::cerr << "MuonTriggerMatching\t" << "ERROR\t"
                << "TrigConfTree does exist, but could not SetBranchAddress for SMK and HLTNameMap." << std::endl;
      return;
    }

    std::map<std::string, int>::const_iterator p;
    for (int iEntry = 0; iEntry < TrigConfTree->GetEntries(); ++iEntry) {
      TrigConfTree->GetEntry(iEntry);
      if (m_MapOfChainIdMap.count(smk)) continue;

      std::map<std::string, int> ChainIdMap;
      for (p = hltmap->begin(); p != hltmap->end(); ++p) {
        if ((p->first.find("EF_mu") != 0) and (p->first.find("EF_2mu") != 0)) continue;

        int id = p->second;
        if (p->first.find("EF_") == 0) id += ChainEntry::kEFChainIdOffset;
        ChainIdMap.insert(std::pair<std::string, int>(p->first, id));
      }
      m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(smk, ChainIdMap));
    }

  } else {
    std::cerr << "MuonTriggerMatching\t" << "WARNING\t" << "TrigConfTTree is null\n";;
    std::cerr << "MuonTriggerMatching\t" << "       \t" << "Create ChainIdMap from src/ChainIdMap.hxx.\n";
    std::cerr << "MuonTriggerMatching\t" << "       \t"
              << "It is strongly recommended to use TrigConfTree to ensure correct TriggerConfiguration." << std::endl;
    createChainIdMapFromFile();
  }
}



bool
MuonTriggerMatching::L1_isPassed(const int iRoI,
                                 const std::string& l1item)
{
  if (iRoI < 0) return false;
  return getL1pt(m_d3pd->trig_L1_mu_thrName->at(iRoI)) >= getL1pt(l1item);
}



void
MuonTriggerMatching::tokenize(const std::string& str,
                              std::vector<std::string>& tokens,
                              const std::string& delimiters) {
  tokens.clear();
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while ((std::string::npos != pos) or (std::string::npos != lastPos)) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}



int
MuonTriggerMatching::getL1pt(const std::string& l1item)
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
    std::cerr << "MuonTriggerMatching::getL1pt: cannot parse: " << l1item << std::endl;
    return rc;
  }

  std::istringstream iss(pt.substr(pt.find("MU")+2));
  iss >> rc;
  return rc;
}



double
MuonTriggerMatching::dR(const double eta1,
                        const double phi1,
                        const double eta2,
                        const double phi2)
{
  double deta = fabs(eta1 - eta2);
  double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2.*TMath::Pi() - fabs(phi1 - phi2);
  return sqrt(deta*deta + dphi*dphi);
}



//=============
// Dummy Class
//=============
namespace Dummy {
  class D3PDEvent {
  public:
    unsigned int trig_DB_SMK;
    int trig_Nav_n;
    std::vector<short>   *trig_Nav_chain_ChainId;
    std::vector<std::vector<int> > *trig_Nav_chain_RoIType;
    std::vector<std::vector<int> > *trig_Nav_chain_RoIIndex;
    std::vector<std::string>  *trig_L1_mu_thrName;
    std::vector<int>     *trig_RoI_EF_mu_Muon_ROI;
    std::vector<std::vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_pt;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_eta;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_SA_phi;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_pt;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_eta;
    std::vector<std::vector<float> > *trig_EF_trigmuonef_track_CB_phi;
    std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_pt;
    std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_eta;
    std::vector<std::vector<float> > *trig_EF_trigmugirl_track_CB_phi;
  };
}
// eof
