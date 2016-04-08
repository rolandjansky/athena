/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuNtComponent.cxx
*/
#include "TriggerMenuAnalysis/TrigMenuNtComponent.h"
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"

using namespace std;


TrigMenuNtComponent::TrigMenuNtComponent(NtupleAlgorithm* mainAlg, 
					 const NtComponent::NtComponentParameters& params) : 
  NtComponent::NtupleComponent(mainAlg, params), 
  m_trigDecisionTool(0), m_trigAccessTool(0), m_RoILinksCnvTool(0), 
  m_chainEntries(0), m_RoILinks(0) {
  TrigMenuNtupleAlg* mymainAlg = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);
  if (mymainAlg) {
    m_trigDecisionTool = mymainAlg->trigDecisionTool();
    m_trigAccessTool = mymainAlg->trigAccessTool();
    m_RoILinksCnvTool = mymainAlg->roILinksCnvTool();
  }
}

TrigMenuNtComponent::~TrigMenuNtComponent() {
}

StatusCode TrigMenuNtComponent::book() {
  m_chainEntries = new std::vector<ChainEntry>();
  m_RoILinks = new RoILinks();

  m_tree->Branch("ChainEntries", &m_chainEntries);
  m_tree->Branch("RoILinks", &m_RoILinks);

  return StatusCode::SUCCESS;
}

StatusCode TrigMenuNtComponent::fill() {
  ClearMember();

  fillChain("L2_mu4");
  fillChain("L2_mu6");
  fillChain("L2_mu10");
  fillChain("L2_mu20");

  fillChain("L2_e10_loose");
  fillChain("L2_e15_medium");
  fillChain("L2_e15i_medium");
  fillChain("L2_e20_loose");
  fillChain("L2_e20i_loose");
  fillChain("L2_e25i_loose");
  fillChain("L2_105_loose1");
  fillChain("L2_e22i_tight");
  fillChain("L2_e20_loose_passL2");
  fillChain("L2_e20_loose_passEF");
  fillChain("L2_e5_medium");
  fillChain("L2_e10_medium");
  fillChain("L2_e12_medium");
  fillChain("L2_e25_loose");
  fillChain("L2_em15_passHLT");
  fillChain("L2_em15i_passHLT");
  fillChain("L2_em20_passHLT");
  fillChain("L2_em20i_passHLT");
  fillChain("L2_em105_passHLT");
  fillChain("L2_e25i_medium");
  fillChain("L2_e5_medium1");
  fillChain("L2_e6_medium");
  fillChain("L2_e6_medium1");
  fillChain("L2_e10_loose_IdScan");
  fillChain("L2_e10_loose_SiTrk");
  fillChain("L2_e20_loose_SiTrk_robust");
  fillChain("L2_e20_loose_SiTrk");
  fillChain("L2_e5_medium_L2SW");
  fillChain("L2_e20_loose_IdScan");
  fillChain("L2_e20_loose_NoTrkCut");
  fillChain("L2_e10_NoCut");
  fillChain("L2_e12_medium");
  fillChain("L2_e25i_medium1");

  fillChain("EF_e10_loose");
  fillChain("EF_e15_medium");
  fillChain("EF_e15i_medium");
  fillChain("EF_e20_loose");
  fillChain("EF_e20i_loose");
  fillChain("EF_e25i_loose");
  fillChain("EF_105_loose1");
  fillChain("EF_e22i_tight");
  fillChain("EF_e20_loose_passL2");
  fillChain("EF_e20_loose_passEF");
  fillChain("EF_e5_medium");
  fillChain("EF_e10_medium");
  fillChain("EF_e12_medium");
  fillChain("EF_e25_loose");
  fillChain("EF_em15_passHLT");
  fillChain("EF_em15i_passHLT");
  fillChain("EF_em20_passHLT");
  fillChain("EF_em20i_passHLT");
  fillChain("EF_em105_passHLT");
  fillChain("EF_e25i_medium");
  fillChain("EF_e5_medium1");
  fillChain("EF_e6_medium");
  fillChain("EF_e6_medium1");
  fillChain("EF_e10_loose_IdScan");
  fillChain("EF_e10_loose_SiTrk");
  fillChain("EF_e20_loose_SiTrk_robust");
  fillChain("EF_e20_loose_SiTrk");
  fillChain("EF_e5_medium_L2SW");
  fillChain("EF_e20_loose_IdScan");
  fillChain("EF_e20_loose_NoTrkCut");
  fillChain("EF_e10_NoCut");
  fillChain("EF_e12_medium");
  fillChain("EF_e25i_medium1");

  m_RoILinksCnvTool->fill(*m_chainEntries, *m_RoILinks);

  return StatusCode::SUCCESS;
}


void TrigMenuNtComponent::fillChain(const std::string& chain_name){
  ChainEntry x1(chain_name);

  // Muon
  std::vector<int> MuonRoIIndex = fillL2Muon(chain_name);
  for(int i=0; i< (int)MuonRoIIndex.size(); i++){
    x1.addRoI(ChainEntry::kMuonRoIType, MuonRoIIndex[i]);
  }

  // Electron
  std::vector<int> ElectronRoIIndex = fillL2Electron(chain_name);
  for(int i=0; i< (int)ElectronRoIIndex.size(); i++){
    x1.addRoI(ChainEntry::kElectronRoIType, ElectronRoIIndex[i]);
  }

  m_chainEntries->push_back(x1);
}

std::vector<int> TrigMenuNtComponent::fillL2Muon(const std::string& chain_name) {
  Trig::FeatureContainer fc = 
    m_trigDecisionTool->features(chain_name, TrigDefs::alsoDeactivateTEs);

  int index=-1;
  std::vector<int> RoIIndex(0);
  std::vector<Trig::Combination> combs = fc.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb;
  (*m_msg) << MSG::DEBUG 
	   << "Number of combinations for " << chain_name 
	   << ": " << combs.size() << endreq;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    (*m_msg) << MSG::DEBUG << "Getting links" << endreq;
    if ( (index=m_RoILinksCnvTool->setMuonRoILinks(*m_RoILinks, *p_comb)) >= 0) {
      RoIIndex.push_back(index);
    }
  }
  return RoIIndex;
}

std::vector<int> TrigMenuNtComponent::fillL2Electron(const std::string& chain_name) {
  Trig::FeatureContainer fc = 
    m_trigDecisionTool->features(chain_name, TrigDefs::alsoDeactivateTEs);

  int index=-1;
  std::vector<int> RoIIndex(0);
  std::vector<Trig::Combination> combs = fc.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb;
  (*m_msg) << MSG::DEBUG 
	   << "Number of combinations for " << chain_name 
	   << ": " << combs.size() << endreq;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    (*m_msg) << MSG::DEBUG << "Getting links" << endreq;
    index=m_RoILinksCnvTool->setElectronRoILinks(*m_RoILinks,*p_comb);
    if (index >= 0) {
      RoIIndex.push_back(index);
    }
  }
  return RoIIndex;
}

void TrigMenuNtComponent::ClearMember(){
    m_chainEntries->clear();
    m_RoILinks->clear();
}
