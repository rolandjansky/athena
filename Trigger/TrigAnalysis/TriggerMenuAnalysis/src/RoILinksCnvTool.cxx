/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoILinksCnvTool.cxx
*/
#include <algorithm>
#include <set>

#include "TriggerMenuAnalysis/RoILinksCnvTool.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "TriggerMenuAnalysis/ChainEntryContainer.h"
#include "TriggerMenuAnalysis/CombLinksContainer.h"
#include "TriggerMenuNtuple/MuonRoILinks.h"
#include "TriggerMenuAnalysis/IndexFinder.h"
#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "GaudiKernel/MsgStream.h"

#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"

#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"

#include "JetEvent/JetCollection.h"

#include <algorithm>
#include <functional>

using namespace std;

static const InterfaceID IID_RoILinksCnvTool("RoILinksCnvTool", 1, 0);

const InterfaceID& RoILinksCnvTool::interfaceID() {
  return IID_RoILinksCnvTool;
}


RoILinksCnvTool::RoILinksCnvTool(const std::string& type, 
				 const std::string& name, 
				 const IInterface* p) : 
  AthAlgTool(type, name, p), 
  m_trigConfSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_trigAccessTool("TrigAccessTool"), 
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"), 
  m_expertMethods(0)
{
  declareInterface<RoILinksCnvTool>(this);
  
  declareProperty("SaveInactiveCombination", m_saveInactiveCombination = true,
		  "Save also links to inactive objects in combinations");
  declareProperty("Chains_Muon", m_chains_Muon0, "L2/EF Muon chains");
  declareProperty("Chains_Electron", m_chains_Electron0);
  declareProperty("Chains_Tau", m_chains_Tau0);
  declareProperty("Chains_Jet", m_chains_Jet0);
  declareProperty("Chains_Bjet", m_chains_Bjet0);
  declareProperty("Chains_MET", m_chains_MET0);
  declareProperty("Chains_MinBias", m_chains_MinBias0);
  declareProperty("Chains_Bphysics", m_chains_Bphysics0);
  declareProperty("Chains_TileMu", m_chains_TileMu0);
  declareProperty("Chains_Any", m_chains_Any0);
  declareProperty("ChainsToSaveAllCombinations", m_chainsToSaveAllCombinations, 
		  "List of exceptional chains (regex) to save all combinations");
}

RoILinksCnvTool::~RoILinksCnvTool() {
}

StatusCode RoILinksCnvTool::initialize() {
  if (m_trigConfSvc.retrieve().isFailure()) {
    log() << MSG::WARNING << "Cannot retrieve TrigConfSvc" << endreq;
  }
  if (m_trigAccessTool.retrieve().isFailure()) {
    log() << MSG::WARNING << "Cannot retrieve TrigAccessTool" << endreq;
  }
  if (m_trigDecisionTool.retrieve().isFailure()) {
    log() << MSG::WARNING 
	  << "Cannot retrieve Trig::TrigDecisionTool" << endreq;
  }
  m_expertMethods = m_trigDecisionTool->ExperimentalAndExpertMethods();
  if (m_expertMethods) m_expertMethods->enable();

  CombLinksDef::buildKnownCombLinksDefs(&(*evtStore()), &log());

  return StatusCode::SUCCESS;
}

StatusCode RoILinksCnvTool::beginRun() {


  log() << MSG::INFO << "List of triggers to check" << endreq;

  vector<string>::const_iterator p2;
  vector<string> cg_chains;

//   log() << MSG::DEBUG << "Now checking all L2 triggers" << endreq;
//   const Trig::ChainGroup* chain_group = m_trigDecisionTool->getChainGroup("L2_.*");
//   cg_chains = chain_group->getListOfTriggers();
//   log() << MSG::DEBUG << "N triggers: " << cg_chains.size() << endreq;
//   for (p2=cg_chains.begin(); p2!=cg_chains.end(); ++p2) {
//     log() << MSG::DEBUG << "  " << (*p2) << endreq;
//   }
  
  findDefinedChains(m_chains_Muon0, m_chains_Muon);
  findDefinedChains(m_chains_Electron0, m_chains_Electron);
  findDefinedChains(m_chains_Tau0, m_chains_Tau);
  findDefinedChains(m_chains_Jet0, m_chains_Jet);
  findDefinedChains(m_chains_Bjet0, m_chains_Bjet);
  findDefinedChains(m_chains_MinBias0, m_chains_MinBias);
  findDefinedChains(m_chains_Bphysics0, m_chains_Bphysics);
  findDefinedChains(m_chains_TileMu0, m_chains_TileMu);
  findDefinedChains(m_chains_Any0, m_chains_Any);

  std::vector<std::string> allcomb;
  findDefinedChains(m_chainsToSaveAllCombinations, allcomb);
  m_chainsAllComb.clear();
  std::copy(allcomb.begin(), allcomb.end(), 
	    std::inserter(m_chainsAllComb, m_chainsAllComb.begin()) );

  printListOfTriggers(m_chains_Muon, "Muon");
  printListOfTriggers(m_chains_Electron, "Electron");
  printListOfTriggers(m_chains_Tau, "Tau");
  printListOfTriggers(m_chains_Jet, "Jet");
  printListOfTriggers(m_chains_Bjet, "Bjet");
  printListOfTriggers(m_chains_MinBias, "MinBias");
  printListOfTriggers(m_chains_Bphysics, "Bphysics");
  printListOfTriggers(m_chains_TileMu, "TileMu");
  printListOfTriggers(m_chains_Any, "Any");
  printListOfTriggers(allcomb, "Exceptional chains to save all combinations");

  return StatusCode::SUCCESS;
}

void RoILinksCnvTool::findDefinedChains(const vector<std::string>& chains0, 
					vector<std::string>& chains) {
  chains.clear();
  set<string> tmp;
  vector<string>::const_iterator p, p2;
  vector<string> cg_chains;
  
  for (p=chains0.begin(); p!=chains0.end(); ++p) {
    const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(*p);
    if (msgLvl(MSG::DEBUG) ) {
      log() << MSG::DEBUG << "Check chains matching regex " << (*p) << endreq;
    }
    if (cg) {
      cg_chains = cg->getListOfTriggers();
      for (p2=cg_chains.begin(); p2!=cg_chains.end(); ++p2) {
	if (tmp.find(*p2) == tmp.end()) {
	  tmp.insert(*p2);
	  if ( (*p2) != (*p)) {
	    if (msgLvl(MSG::DEBUG) ) {
	      log() << MSG::DEBUG << "Chain " << (*p2) << " found for regex "
		    << (*p) << endreq;
	    }
	  }
	}
      }
    }
  }
  std::copy(tmp.begin(), tmp.end(), back_inserter(chains));
}

void RoILinksCnvTool::printListOfTriggers(vector<string>& chains, 
					  const std::string& type) {
  int level = 0; // L2 -> 2, EF -> 3

  log() << MSG::INFO << "Chain type: " << type 
	<< " (size=" << chains.size() << ")" << endreq;
  std::vector<std::string> v2;
  std::vector<std::string>::const_iterator p;
  for (p=chains.begin(); p!=chains.end(); ++p) {
    level = findHltLevel(*p);
    if (level == 2 || level == 3) {
      log() << MSG::INFO << "  " << (*p) 
	    << "(level=" << level << ")" << endreq;
    } else {
      log() << MSG::WARNING << "Cannot find out if the chain " << (*p) 
	    << " is a L2 or EF chain. --> Ignore" << endreq;
    }
    if (find(v2.begin(), v2.end(), *p) == v2.end()) {
      v2.push_back(*p);
    } else {
      log() << MSG::WARNING << "Chain " << (*p) 
	    << " was defined more than once, removing duplicate"
	    << endreq;
    }
  }
  if (v2.size() < chains.size()) chains = v2;
}

StatusCode RoILinksCnvTool::finalize() {
  //  log() << MSG::INFO << "finalize" << endreq;
  return StatusCode::SUCCESS;
}

void RoILinksCnvTool::fill(std::vector<ChainEntry>& chain_entries, 
			   RoILinks& roi_links) {
  //   MsgStream log(msgSvc(), name());
  //   int level=0;
  //   ChainEntry::RoIType roi_type;
  //   std::vector<std::string>::const_iterator p;

  clear();

  fillChains(chain_entries, roi_links, 
	     "Muon", m_chains_Muon, 
	     ChainEntry::kRoIType_L2_mu, ChainEntry::kRoIType_EF_mu);
  fillChains(chain_entries, roi_links, 
	     "Electron", m_chains_Electron, 
	     ChainEntry::kRoIType_L2_e, ChainEntry::kRoIType_EF_e);
  fillChains(chain_entries, roi_links, 
	     "Tau", m_chains_Tau, 
	     ChainEntry::kRoIType_L2_tau, ChainEntry::kRoIType_EF_tau);
  fillChains(chain_entries, roi_links, 
	     "Jet", m_chains_Jet, 
	     ChainEntry::kRoIType_L2_j, ChainEntry::kRoIType_EF_j);
  fillChains(chain_entries, roi_links, 
	     "Bjet", m_chains_Bjet, 
	     ChainEntry::kRoIType_L2_b, ChainEntry::kRoIType_EF_b);
  fillChains(chain_entries, roi_links, 
	     "MET", m_chains_MET, 
	     ChainEntry::kRoIType_L2_xe, ChainEntry::kRoIType_EF_xe);
  fillChains(chain_entries, roi_links, 
	     "Bphysics", m_chains_Bphysics, 
	     ChainEntry::kRoIType_L2_DiMu, ChainEntry::kRoIType_EF_DiMu);
  fillChains(chain_entries, roi_links, 
	     "TileMu", m_chains_TileMu, 
	     ChainEntry::kRoIType_L2_TileMu, ChainEntry::kRoIType_EF_TileMu);

  for (unsigned int ic=0; ic<chain_entries.size(); ++ic) {
    ChainEntry::addChain(chain_entries[ic].getName());
//     int iic = static_cast<int>(ic);
//     if (ChainEntry::sChainIdMap.find(iic) == ChainEntry::sChainIdMap.end()) {
//       ChainEntry::sChainIdMap[iic] = 
//     }
  }

  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
    vector<CombLinks>::const_iterator p=roi_links.getCombLinks().begin();
    vector<CombLinks>::const_iterator p_end=roi_links.getCombLinks().end();
    for (; p!=p_end; ++p) {
      log() << MSG::DEBUG << "CombLinks: (valid=" << p->isValid() << ") " 
	    << (*p) << endreq;
    }
  }
}

int RoILinksCnvTool::fillChains(std::vector<ChainEntry>& chain_entries, 
				RoILinks& roi_links, 
				const std::string& slice_name, 
				const std::vector<std::string>& chain_names, 
				ChainEntry::RoIType l2type, 
				ChainEntry::RoIType eftype) {
  int level=0;
  ChainEntry::RoIType roi_type;
  std::vector<std::string>::const_iterator p;

  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
    log() << MSG::DEBUG << "N. of " << slice_name << " chains: " 
	  << chain_names.size() << endreq;
  }

  for (p=chain_names.begin(); p!=chain_names.end(); ++p) {
    level = findHltLevel(*p);
    if (level == 2) {
      roi_type = l2type;
    } else {
      roi_type = eftype;
    }
    processChain(*p, roi_type, chain_entries, roi_links);
  }
  return 0;
}

int RoILinksCnvTool::findHltLevel(const std::string& chain_name) const {
  int level = 0;
  MsgStream alog(msgSvc(), name());
  
  //   alog << MSG::DEBUG << "chain_name = " << chain_name 
  //        << " L2_ index = " << chain_name.find("L2_")
  //        << " EF_ index = " << chain_name.find("EF_")
  //        << endreq;
  if (chain_name.find("L2_") == 0) level = 2;
  else if (chain_name.find("EF_") == 0) level = 3;
  return level;
}



int RoILinksCnvTool::setMuonRoILinks(RoILinks& links, const Trig::Combination& comb) {
  typedef IndexFinder<MuonFeature, MuonFeatureContainer> IndexFinder1;
  typedef IndexFinder<TrigInDetTrackCollection, TrigInDetTrackCollection, TrigInDetTrack> IndexFinder2;
  typedef IndexFinder<CombinedMuonFeature, CombinedMuonFeatureContainer> IndexFinder3;

  IndexFinder1 indexFinder1("HLT", *evtStore(), log());
  IndexFinder2 indexFinder2("HLT_TrigSiTrack_Muon", *evtStore(), log());
  IndexFinder3 indexFinder3("HLT", *evtStore(), log());

  vector<Trig::Feature<MuonFeature> > feat1 = 
    comb.get<MuonFeature>("", TrigDefs::alsoDeactivateTEs);
  vector<Trig::Feature<TrigInDetTrackCollection> > feat2 = 
    comb.get<TrigInDetTrackCollection>("", TrigDefs::alsoDeactivateTEs);
  vector<Trig::Feature<CombinedMuonFeature> > feat3 = 
    comb.get<CombinedMuonFeature>("", TrigDefs::alsoDeactivateTEs);

  int index1=-1;
  std::vector<int> index2(0);
  int index3=-1;
  int te_status[3] = { 0, 0, 0 };
  bool isvalid = false;

  if (feat1.size() == 1) {
    index1 = indexFinder1.findIndex(feat1[0].cptr());
    if (feat1[0].te()->getActiveState()) te_status[0] = 1;
    isvalid = true;
  }
  if (feat2.size() == 1) {
    //    log() << MSG::DEBUG << "find indices of Tracks" << endreq;
    index2 = findIndexesForCont(feat2[0].cptr(), indexFinder2);
    if (feat2[0].te()->getActiveState()) te_status[1] = 1;
    isvalid = true;
  }
  if (feat3.size() == 1) {
    //    log() << MSG::DEBUG << "find index of CombinedMuonFeature" << endreq;
    index3 = indexFinder3.findIndex(feat3[0].cptr());
    if (feat3[0].te()->getActiveState()) te_status[2] = 1;
    isvalid = true;
  }

  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
    log() << MSG::DEBUG << "L2 muon links (" << isvalid << "): (" 
 	  << index1 << ", [ ";
    for (unsigned int i=0; i<index2.size(); ++i) {
      log() << index2[i] << ", ";
    }
    log() << "], " << index3 << ")" << endreq;
  }

  if (isvalid) {
    MuonRoILinks tmp;
    tmp.setMuonRoILinks(index1, index2, index3, 
			te_status[0], te_status[1], te_status[2]);
    links.addMuonRoILinks(tmp);
    return links.getMuonRoILinks().size() - 1;
  } else {
    return -1;
  }
}


int RoILinksCnvTool::setElectronRoILinks(RoILinks& links, const Trig::Combination& comb) {
  typedef IndexFinder<TrigEMCluster, TrigEMClusterContainer> IndexFinder1;
  typedef IndexFinder<TrigInDetTrackCollection, TrigInDetTrackCollection, TrigInDetTrack> IndexFinder2;
  typedef IndexFinder<TrigElectronContainer, TrigElectronContainer,TrigElectron> IndexFinder3;
  typedef IndexFinder<egammaContainer, egammaContainer, egamma> IndexFinder4;

  IndexFinder1 indexFinder1("HLT_TrigT2CaloEgamma", *evtStore(), log());
  IndexFinder2 indexFinder2("HLT_TrigSiTrack_eGamma", *evtStore(), log());
  IndexFinder3 indexFinder3("HLT_L2IDCaloFex", *evtStore(), log());
  IndexFinder4 indexFinder4("HLT_egamma_Electrons", *evtStore(), log());

  vector<Trig::Feature<TrigEMCluster> > feat1 = 
    comb.get<TrigEMCluster>("", TrigDefs::alsoDeactivateTEs);
  vector<Trig::Feature<TrigInDetTrackCollection> > feat2 = 
    comb.get<TrigInDetTrackCollection>("", TrigDefs::alsoDeactivateTEs);
  vector<Trig::Feature<TrigElectronContainer> > feat3 = 
    comb.get<TrigElectronContainer>("", TrigDefs::alsoDeactivateTEs);
  vector<Trig::Feature<egammaContainer> > feat4 = 
    comb.get<egammaContainer>("", TrigDefs::alsoDeactivateTEs);


  int index1=-1;
  std::vector<int> index2(0);
  std::vector<int> index3(0);
  std::vector<int> index4(0);
  //  int index3=-1;
  int te_status[4] = { 0, 0, 0, 0 };
  bool isvalid = false;

  if (feat1.size() == 1) {
    //    log() << MSG::DEBUG << "find index of TrigEMCluster" << endreq;
    index1 = indexFinder1.findIndex(feat1[0].cptr());
    if (feat1[0].te()->getActiveState()) te_status[0] = 1;
    isvalid = true;
  }
  if (feat2.size() == 1) {
    //    log() << MSG::DEBUG << "find indices of Tracks" << endreq;
    index2 = findIndexesForCont(feat2[0].cptr(), indexFinder2);
    if (feat2[0].te()->getActiveState()) te_status[1] = 1;
    isvalid = true;
  }
  if (feat3.size() == 1) {
    //    log() << MSG::DEBUG << "find indices of TrigElectron" << endreq;
    index3 = findIndexesForCont(feat3[0].cptr(), indexFinder3);
    if (feat3[0].te()->getActiveState()) te_status[2] = 1;
    isvalid = true;
  }
  if (feat4.size() == 1) {
    //    log() << MSG::DEBUG << "find indices of egamma" << endreq;
    index4 = findIndexesForCont(feat4[0].cptr(), indexFinder4);
    if (feat4[0].te()->getActiveState()) te_status[3] = 1;
    isvalid = true;
  }

  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {  
    log() << MSG::DEBUG << "L2 electron links: (" 
	  << index1 << ", [ ";
    for (unsigned int i=0; i<index2.size(); ++i) {
      log() << index2[i] << ", ";
    }
    log() << "], [";
    for (unsigned int i=0; i<index3.size(); ++i) {
      log() << index3[i] << ", ";
    }
    log() << " ], ";
    for (unsigned int i=0; i<index4.size(); ++i) {
      log() << index4[i] << ", ";
    }
    log() << MSG::DEBUG << "] )" << endreq;
  }

  if (isvalid) {
    links.addElectronRoILinks(index1, index2, index3, index4,
			      te_status[0], te_status[1], te_status[2], te_status[3]);
    return links.getElectronRoILinks().size() - 1;
  } else {
    return -1;
  }
}

void RoILinksCnvTool::record(std::vector<ChainEntry>& chain_entries, 
			     RoILinks& x) {
  if (!evtStore()->contains<ChainEntryContainer>("ChainEntryContainer")) {
    ChainEntryContainer* cec = new ChainEntryContainer();
    std::vector<ChainEntry>::const_iterator p;
    for (p=chain_entries.begin(); p!=chain_entries.end(); ++p) {
      cec->push_back(new ChainEntry(*p));
    }
    if (evtStore()->record(cec, "ChainEntryContainer").isFailure()) {
      if (msgLvl(MSG::DEBUG) ) {
	log() << MSG::DEBUG << "Cannot record ChainEntryContainer" << endreq;
      }
    }
  }

  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_mu), "L2_mu");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_mu), "EF_mu");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_TileMu), "L2_TileMu");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_TileMu), "EF_TileMu");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_e), "L2_e");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_e), "EF_e");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_tau), "L2_tau");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_tau), "EF_tau");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_j),"L2_j");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_j), "EF_j");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_L2_b),"L2_b");
  recordCombLinks(x.getCombLinks(ChainEntry::kRoIType_EF_b), "EF_b");
}

void RoILinksCnvTool::recordCombLinks(const std::vector<CombLinks>& x, 
				      const std::string& label) {
  if (!evtStore()->contains<CombLinksContainer>(label)) {
    CombLinksContainer* clc = new CombLinksContainer();
    std::vector<CombLinks>::const_iterator p;
    for (p=x.begin(); p!=x.end(); ++p) {
      clc->push_back(new CombLinks(*p));
    }
    if (evtStore()->record(clc, label).isFailure()) {
      if (msgLvl(MSG::DEBUG) ) {
	log() << MSG::DEBUG << "Cannot record CombLinksContainer with label: "
	      << label << endreq;
      }
    }
  }
}

MsgStream& RoILinksCnvTool::log() const {
  return msg();
}

void RoILinksCnvTool::clear() {
  m_RoILinks.clear();

  m_muonRoIVec.clear();
  m_electronRoIVec.clear();
}

int RoILinksCnvTool::processChain(const std::string& chain_name, 
				  ChainEntry::RoIType roi_type, 
				  std::vector<ChainEntry>& chain_entries, 
				  RoILinks& roi_links) {
  Trig::FeatureContainer fc = 
    m_trigDecisionTool->features(chain_name, TrigDefs::alsoDeactivateTEs);
  std::vector<Trig::Combination> combs = fc.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb;
  int chain_counter=-1;
  std::vector<int> roi_types;
  std::vector<int> roi_indices;
  int iroi=-1;

  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
    if (combs.size() > 0) {
      log() << MSG::DEBUG 
	    << "Number of combinations for " << chain_name 
	    << ": " << combs.size() << endreq;
    } else {
      log() << MSG::DEBUG
	    << "Number of combinations for " << chain_name 
	    << ": " << combs.size() << endreq;
    }
  }

  //int lastStep = -1;
  unsigned int tmp_step=0;

  const HLT::Chain* chainDetail = m_expertMethods->getChainDetails(chain_name);
  const TrigConf::HLTChain* configChain = 0;
  if (chainDetail) {
    configChain = chainDetail->getConfigChain();
    //lastStep = 
    chainDetail->getChainStep();
  }
  std::vector<const HLT::TriggerElement*>::const_iterator p_te1;
  std::vector<TrigConf::HLTTriggerElement*>::const_iterator p_te2;
  std::vector<TrigConf::HLTSignature*>::const_iterator p_sig;
  std::vector<TrigConf::HLTSignature*> dummylist;
  std::vector<TrigConf::HLTSignature*>& siglist = dummylist;

  if (configChain) {
    siglist = configChain->signatureList();
    chain_counter = configChain->chain_counter();
  }

  const TrigConf::HLTChainList* hltChains = m_trigConfSvc->chainList();

  for(TrigConf::HLTChainList::const_iterator chain = hltChains->begin(); chain != hltChains->end(); ++chain){
    if( (*chain)->chain_name()==chain_name ){
      chain_counter = (*chain)->chain_counter();
      break;
    }
  }

  //----- get L1 RoIs -----//
  const LVL1_ROI* coll=0;
  if (evtStore()->retrieve(coll, "LVL1_ROI").isFailure()) {
    if (msgLvl(MSG::DEBUG) ) {
      log() << MSG::DEBUG << "Cannot retrieve LVL1_ROI" << endreq;
    }
    return -1;
  }

  const LVL1_ROI::muons_type& muon_rois = coll->getMuonROIs();
  const LVL1_ROI::emtaus_type& emtau_rois = coll->getEmTauROIs();
  const LVL1_ROI::jets_type& jet_rois = coll->getJetROIs();

  //================================================================================//
  //    From here, create objects to write to the TTree  [ChainEntry, CombLinks]    //
  //================================================================================//
  ChainEntry tmp_chain_entry(chain_name, chain_counter);
  int duplicateChain=-1;
  for(size_t iChain=0;iChain<chain_entries.size();++iChain) {
    if( chain_entries[iChain].getName()==chain_name ) {
      duplicateChain=iChain;
      break;
    }
  }
  ChainEntry& chain_entry = (duplicateChain==-1) ? tmp_chain_entry : chain_entries[duplicateChain];

  //----- get L1_RoI name -----//
  const CombLinksDef* def = CombLinksDef::getCombLinksDef(roi_type);
  if( !def ) {
    if (msgLvl(MSG::DEBUG) ) {
      log() << MSG::DEBUG << "CombLinksDef for RoIType=" 
	    << roi_type << " is not defined" << endreq;
    }
    return -1;
  }
  std::string l1roi = "";
  const std::vector<std::string>& l1rois = def->getL1RoIs();
  std::vector<std::string>::const_iterator p_roi;
  if (def->getL1RoIs().size() > 0) l1roi = l1rois[0];

  int comb_index=0;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb, ++comb_index) {
    //----- count number of RoIs in Combination (multi-objects trigger?) -----//
    int nL1RoIinCombination = 0;
    const std::vector<const HLT::TriggerElement*>& tes = p_comb->tes();
    for( p_te1=tes.begin(); p_te1!=tes.end(); ++p_te1 ) {
      int i=-1;
      if ( l1roi == "Muon_ROI") {
	i = findIndexOfRoI<Muon_ROI>( *p_te1, muon_rois);
      } else if ( l1roi == "EmTau_ROI") {
    	i = findIndexOfRoI<EmTau_ROI>( *p_te1, emtau_rois);
      } else if ( l1roi == "Jet_ROI") {
	i = findIndexOfRoI<Jet_ROI>( *p_te1, jet_rois);
      }      
      if( i>=0 ) { ++nL1RoIinCombination; }
    }

    bool isSingleObjectChain = ( nL1RoIinCombination <= 1);
    //----- for multi-objects chain, only active Combinations are saved -----//
    if(!isSingleObjectChain) {
      bool skipcomb=true;
      if (m_saveInactiveCombination || p_comb->active() ||
	  m_chainsAllComb.find(chain_name) != m_chainsAllComb.end() ) {
	skipcomb = false;
      }
      if (skipcomb) continue;
      // if( !m_saveInactiveCombination && !p_comb->active() ) continue;
    }

    //----------  get Step ----------//
    tmp_step = 0;
    for (p_te1=tes.begin(); p_te1!=tes.end(); ++p_te1) {
      bool s = false;
	
      for (p_sig=siglist.begin(); p_sig!=siglist.end(); ++p_sig) {
	vector<TrigConf::HLTTriggerElement*>& tes2 = (*p_sig)->outputTEs();
	for (p_te2=tes2.begin(); p_te2!=tes2.end(); ++p_te2) {
	  if ( (*p_te1)->getId() == 
	       TrigConf::HLTUtils::string2hash( (*p_te2)->name(), "TE")) {
	    s = true;
	    break;
	  }
	}
	if (s) {
	  if ( (*p_sig)->signature_counter() > tmp_step) 
	    tmp_step = (*p_sig)->signature_counter();
	  break;
	}
      }
    }

    //----------  Fill CombLinks ----------//
    int te_index=0;
    for( p_te1=tes.begin(); p_te1!=tes.end(); ++p_te1, ++te_index) {
      // For chains with ComboAlgo or AllTEAlgo, there could be
      // multiple L1 RoI seeding this TE. 
      // Check all of them in turn and 
      // 
      for (p_roi=l1rois.begin(); p_roi!=l1rois.end(); ++p_roi) {
	// mapping from index in the vector to the corresponding RoI word
	std::map<int, long> roiIndexWord;
	std::map<int, long>::const_iterator p_roiIndex;
	findIndicesOfL1RoI(*p_roi, *p_te1, muon_rois, emtau_rois, jet_rois, 
			   roiIndexWord);
	if (roiIndexWord.size() > 0) {
	  // Index of given type of RoI
	  // Needed to distinguish electrons from Jpsi->ee
	  // (e5_tight or e4_etcut) seeding a ComboAlgo
	  // RoI = [0]: e4_etcut, [1]: e5_tight
 	  std::multiset<CombLinks, order_by_TELabel> rois_in_combination;
	  int roi_index=0;
	  for (p_roiIndex=roiIndexWord.begin(); p_roiIndex!=roiIndexWord.end(); 
	       ++p_roiIndex, ++roi_index) {
	    // [Comb][TE][RoI] = [][0][0] for single object chain
	    //                 = [][TE][0] for simple combined chain
	    //                 = [][TE][RoI] for chains with Combo/AllTE algorithms
	    CombLinks tmp(static_cast<int>(roi_type));
	    tmp.setLastStep(tmp_step);
	    tmp.setActiveState(p_comb->active());
	    tmp.setCombNumber(comb_index);
	    tmp.setTENumber(te_index);
	    
	    tmp.addIndex(*p_roi, FeatureIndex(p_roiIndex->first, true) );
	    if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
	      log() << MSG::DEBUG << "Filling CombLinks for this comb/TE/RoI"
		    << endreq;
	    }
	    def->fillCombLinks(tmp, *p_comb, &(*m_trigDecisionTool), 
			       p_roiIndex->second);

	    if (tmp.isValid()) {
	      rois_in_combination.insert(tmp);
	    }
	  }
 	  std::multiset<CombLinks, order_by_TELabel>::iterator ppp;

	  roi_index = 0;
	  for (ppp=rois_in_combination.begin(); ppp!=rois_in_combination.end(); 
	       ++ppp, ++roi_index) {
	    CombLinks ttt = *ppp;
	    ttt.setRoINumber(roi_index);
	    iroi = roi_links.addCombLinks(ttt);
	    chain_entry.addRoI(roi_type, iroi);
	    if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
	      log() << MSG::DEBUG << "Dump CombLinks" << endreq;
	      log() << MSG::DEBUG << (*ppp) << endreq;
	    }
	  }
	} else {
	  // Check if looking TE is not for Other Signature
	  if( hasOtherL1RoI(*p_roi, *p_te1, muon_rois, emtau_rois, jet_rois) ) {
	    continue;
	  }
	  // Cannot retrieve L1 RoI but try to collect indices anyway
	  CombLinks tmp(static_cast<int>(roi_type));
	  tmp.setLastStep(tmp_step);
	  tmp.setActiveState(p_comb->active());
	  tmp.setTENumber(te_index);
	  tmp.setRoINumber(-1); // unseeded. Not sure what to put here

	  def->fillCombLinks(tmp, *p_comb, &(*m_trigDecisionTool), -1);

	  if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
	    log() << MSG::DEBUG << tmp << endreq;
	  }

	  if (tmp.isValid()) {
	    iroi = roi_links.addCombLinks(tmp);
	    chain_entry.addRoI(roi_type, iroi);
	    if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
	      chain_entry.dump();
	    }
	  }
	}
      }
    }

  }


  if( duplicateChain==-1 ) {
    if (msgSvc()->outputLevel(name()) <= MSG::DEBUG) {
      chain_entry.dump();
    }
    chain_entries.push_back(chain_entry);
  }

  return 0;
}

int RoILinksCnvTool::findIndexOfL1RoI(const std::string& l1roi, 
				      const HLT::TriggerElement* te, 
				      const std::vector<Muon_ROI>& roilist_mu, 
				      const std::vector<EmTau_ROI>& roilist_emtau, 
				      const std::vector<Jet_ROI>& roilist_jet, 
				      long& roiword) {
  int index=-1;
  if (l1roi == "Muon_ROI") {
    index = findIndexOfRoI<Muon_ROI>(te, roilist_mu, roiword);
  } else if (l1roi == "EmTau_ROI") {
    index = findIndexOfRoI<EmTau_ROI>(te, roilist_emtau, roiword);
  } else if (l1roi == "Jet_ROI") {
    index = findIndexOfRoI<Jet_ROI>(te, roilist_jet, roiword);
  }
  return index;
}

void RoILinksCnvTool::findIndicesOfL1RoI(const std::string& l1roi, 
					 const HLT::TriggerElement* te, 
					 const std::vector<Muon_ROI>& roilist_mu, 
					 const std::vector<EmTau_ROI>& roilist_emtau, 
					 const std::vector<Jet_ROI>& roilist_jet, 
					 std::map<int, long>& roiIndexWord) {
  if (l1roi == "Muon_ROI") {
    findIndicesOfRoI<Muon_ROI>(te, roilist_mu, roiIndexWord);
  } else if (l1roi == "EmTau_ROI") {
    findIndicesOfRoI<EmTau_ROI>(te, roilist_emtau, roiIndexWord);
  } else if (l1roi == "Jet_ROI") {
    findIndicesOfRoI<Jet_ROI>(te, roilist_jet, roiIndexWord);
  }
}


bool RoILinksCnvTool::hasOtherL1RoI(const std::string& l1roi, 
				    const HLT::TriggerElement* te, 
				    const std::vector<Muon_ROI>& roilist_mu, 
				    const std::vector<EmTau_ROI>& roilist_emtau, 
				    const std::vector<Jet_ROI>& roilist_jet) {
  const int iMuon_ROI  = findIndexOfRoI<Muon_ROI>(te, roilist_mu);
  const int iEmTau_ROI = findIndexOfRoI<EmTau_ROI>(te, roilist_emtau);
  const int iJet_ROI   = findIndexOfRoI<Jet_ROI>(te, roilist_jet);

  if (l1roi == "Muon_ROI") {
    if(iEmTau_ROI>=0 || iJet_ROI>=0) return true;
  } else if (l1roi == "EmTau_ROI") {
    if(iMuon_ROI>=0 || iJet_ROI>=0) return true;
  } else if (l1roi == "Jet_ROI") {
    if(iMuon_ROI>=0 || iEmTau_ROI>=0) return true;
  }

  return false;
}
