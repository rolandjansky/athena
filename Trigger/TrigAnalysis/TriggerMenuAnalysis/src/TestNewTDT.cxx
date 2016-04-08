/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TestNewTDT.cxx
*/
#include <set>
#include <algorithm>
#include <functional>
#include "TriggerMenuAnalysis/TestNewTDT.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/TDTUtilities.h"

// #define private public
// #include "SGTools/CLIDRegistry.h"
// #undef private

using namespace std;


TestNewTDT::TestNewTDT(const std::string& name, ISvcLocator* svcloc) : 
  AthAlgorithm(name, svcloc), 
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"), 
  m_classIDSvc("ClassIDSvc/ClassIDSvc", this->name()), 
  m_chainGroupNames(), m_chainGroups(), m_chainCombMap() {
  declareProperty("ChainGroupNames", m_chainGroupNames, 
		  "List of chain groups to investigate");
  m_chainGroup_L2 = 0;
  m_chainGroup_EF = 0;
}

TestNewTDT::~TestNewTDT() {
}

StatusCode TestNewTDT::initialize() {
  MsgStream log(msgSvc(), name());

  if (m_trigDecisionTool.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve TrigDecisionTool" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_classIDSvc.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve ClassIDSvc" << endreq;
    return StatusCode::FAILURE;
  }
  std::vector<std::string>::const_iterator p;
  for (p=m_chainGroupNames.begin(); p!=m_chainGroupNames.end(); ++p) {
    m_chainGroups.push_back(m_trigDecisionTool->getChainGroup(*p));
  }
  m_chainGroup_L2 = m_trigDecisionTool->getChainGroup("L2_.*");
  m_chainGroup_EF = m_trigDecisionTool->getChainGroup("EF_.*");

  return StatusCode::SUCCESS;
}

StatusCode TestNewTDT::execute() {
  MsgStream log(msgSvc(), name());

  std::vector<std::string> l2chains = m_chainGroup_L2->getListOfTriggers();
  std::vector<std::string> efchains = m_chainGroup_EF->getListOfTriggers();

  log << MSG::INFO << "In execute: N L2 chains " << l2chains.size() 
      << ", N EF chains " << efchains.size() << endreq;

//   std::set<std::string> chainList;
//   chainList.insert("L2_mu6_MSonly");
//   chainList.insert("L2_mu10_MSonly");

  std::vector<std::string>::const_iterator p_chain;
  for (p_chain=l2chains.begin(); p_chain!=l2chains.end(); ++p_chain) {
    if (p_chain->find("mu4_L1J") != string::npos) {
      inspectChain(*p_chain);
    }
  }
  for (p_chain=efchains.begin(); p_chain!=efchains.end(); ++p_chain) {
    if (p_chain->find("mu4_L1J") != string::npos) {
      inspectChain(*p_chain);
    }
  }

  const TrigMuonEFInfoContainer* tmeic=0;
  if (sgSvc()->retrieve(tmeic, "HLT_MuonEFInfo").isFailure()) {
    log << MSG::DEBUG << "Cannot retrieve HLT_MuonEFInfo" << endreq;
  } else {
    log << MSG::DEBUG << "TrigMuonEFInfoContainer size: " << tmeic->size() 
	<< endreq;
  }

  for_each(m_chainGroups.begin(), m_chainGroups.end(), 
	   bind1st(mem_fun(&TestNewTDT::printTDT), this));

  return StatusCode::SUCCESS;
}

StatusCode TestNewTDT::finalize() {
  MsgStream log(msgSvc(), name());
  std::map<std::string, std::multiset<std::string> >::const_iterator p;
  for (p=m_chainCombMap.begin(); p!=m_chainCombMap.end(); ++p) {
    log << MSG::INFO << "Features in chain: " << p->first << endreq;
    std::set<std::string>::const_iterator s;
    for (s=p->second.begin(); s!=p->second.end(); ++s) {
      log << MSG::INFO << "  Comb: " << (*s) << endreq;
    }
  }
  return StatusCode::SUCCESS;
}

void TestNewTDT::printTDT(const Trig::ChainGroup* cg) {
  MsgStream log(msgSvc(), name());

  if (cg == 0) {
    log << MSG::DEBUG << "Chain group is null" << endreq;
    return;
  }

  log << MSG::INFO << "Print ChainGroup : " << cg->patterns() << endreq;
  log << MSG::INFO << "List of triggers: " << cg->getListOfTriggers() << endreq;
  log << MSG::INFO << "passed? : " <<  cg->isPassed() << endreq;

  log << MSG::INFO << "Print TDT structure for ChainGroup: " << cg << endreq;
  Trig::FeatureContainer fc = cg->features();
  log << MSG::INFO << "Got feature container" << endreq;
  std::vector<Trig::Combination> combs = fc.getCombinations();
  
  log << MSG::INFO << "Number of combinations: " << combs.size() << endreq;

  std::vector<Trig::Combination>::iterator p_comb;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    const std::vector<const HLT::TriggerElement*>& tes = p_comb->tes();
    log << MSG::INFO << "Checking combination: N_TE=" << tes.size() << endreq;
    for (unsigned int i=0; i<tes.size(); ++i) {
      //log << MSG::INFO << "TE[" << i << "] = " << p_comb->getTEName(*tes[i]) << endreq;
      log << MSG::INFO << "TE[" << i << "] = " << Trig::getTEName(*tes[i]) << endreq;
    }
    std::vector<Trig::Feature<MuonFeature> > muon_features = p_comb->get<MuonFeature>();
    muon_features = p_comb->get<MuonFeature>( (const std::string&)"", (unsigned int)TrigDefs::alsoDeactivateTEs);
    log << MSG::INFO << "Got vector of MuonFeature feature: " 
	<< muon_features.size() << endreq;
  }
  log << MSG::INFO << "Done printTDT" << endreq;
}

void TestNewTDT::inspectChain(const std::string& chain_name) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Inspecting chain: " << chain_name << endreq;
  const Trig::FeatureContainer fc = 
    m_trigDecisionTool->features(chain_name, TrigDefs::alsoDeactivateTEs);
  std::vector<Trig::Combination> combs = fc.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb; 
  log << MSG::DEBUG << "  N combinations: " << combs.size() << endreq;
  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    inspectCombination(*p_comb, chain_name);
  }
}

void TestNewTDT::inspectCombination(const Trig::Combination& comb, 
				    const std::string& chain_name) {
  MsgStream log(msgSvc(), name());
  std::multiset<int> clids;
  std::string clid_string;

  const std::vector<const HLT::TriggerElement*>& tes = comb.tes();
  std::vector<const HLT::TriggerElement*>::const_iterator p_te;

  for (p_te=tes.begin(); p_te!=tes.end(); ++p_te) {
//     const vector<HLT::TriggerElement::FeatureAccessHelper>& fs = 
//       (*p_te)->getFeatureAccessHelpers();
    const vector<HLT::TriggerElement::FeatureAccessHelper>& fs = 
      (*p_te)->getPreviousFeatures();
    vector<HLT::TriggerElement::FeatureAccessHelper>::const_iterator p;
    int i=0;
    for (p=fs.begin(); p!=fs.end(); ++p, ++i) {
      CLID clid = p->getCLID();
      std::string clsname="???";
      m_classIDSvc->getTypeNameOfID(clid, clsname).ignore();
//       CLIDRegistry::const_iterator q;
//       for (q=CLIDRegistry::begin(); q!=CLIDRegistry::end(); ++q) {
// 	if (boost::get<0>(*q) == clid) {
// 	  clsname = boost::get<1>(*q);
// 	  break;
// 	}
//       }

      const HLT::TriggerElement::ObjectIndex& index = p->getIndex();
	
      log << MSG::DEBUG << "Feature [" << i << "] " << clsname 
	  << "(clid=" << clid 
	  << ") index.subType=" << index.subTypeIndex() 
	  << " index.begin=" << index.objectsBegin()
	  << " index.end=" << index.objectsEnd()
	  << endreq;

      clids.insert(clid);
    }
  }

  ostringstream os;
  multiset<int>::const_iterator tt;
  for (tt=clids.begin(); tt!=clids.end(); ++tt) {
    os << (*tt) << " ";
  }
  clid_string = os.str();

  std::map<std::string, std::multiset<std::string> >::iterator p;
  if ( (p=m_chainCombMap.find(chain_name)) == m_chainCombMap.end()) {
    std::multiset<std::string> tmptmp;
    tmptmp.insert(clid_string);
    m_chainCombMap[chain_name] = tmptmp;
  } else {
    if (p->second.find(clid_string) == p->second.end()) {
      p->second.insert(clid_string);
    }
  }
}
