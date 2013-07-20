/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigAccessTool.cxx
*/
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include "TriggerMenuAnalysis/L1RoIObjects.h"
#include "DataModel/ElementLinkVector.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecJetEtRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Conditions.h"
#include <iostream>

using namespace std;

static const InterfaceID IID_TrigAccessTool("TrigAccessTool", 1, 0);

const InterfaceID& TrigAccessTool::interfaceID() {
  return IID_TrigAccessTool;
}

TrigAccessTool::TrigAccessTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* p) :
  AlgTool(type, name , p), 
  mStoreGateSvc("StoreGateSvc", this->name()), 
  mTrigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool") {
  //  mNavigation("Navigation") {
  declareInterface<TrigAccessTool>(this);

  declareProperty("StoreGateSvc", mStoreGateSvc, "StoreGateSvc");
  declareProperty("TrigDecisionTool", mTrigDecisionTool, "TrigDecisionTool");
  //  declareProperty("Navigation", mNavigation, "HLT::Navigation");
}

TrigAccessTool::~TrigAccessTool() {
}

StatusCode TrigAccessTool::initialize() {
  MsgStream log(msgSvc(), name());

  if (mStoreGateSvc.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve service StoreGateSvc" << endreq;
  }
  if (mTrigDecisionTool.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve service TrigDecisionTool" << endreq;
  }
//   if (mNavigation.retrieve().isFailure()) {
//     log << MSG::WARNING << "Cannot retrieve service HLT::Navigation" << endreq;
//   }
  return StatusCode::SUCCESS;
}

StatusCode TrigAccessTool::finalize() {
  return StatusCode::SUCCESS;
}

bool TrigAccessTool::decodeHLTResults() {
  return true;
  decodeHLTResult("HLTResult_L2");
  decodeHLTResult("HLTResult_EF");
  return true;
}

HLTObjectsInRoI* TrigAccessTool::findRoI(int isig, 
					const HLT::TriggerElement* te, 
					std::vector<HLTObjectsInRoI*>& objs,
					HLTObjectsInRoI* (*func)(), int level) {
  HLT::NavigationCore* navitool = 0;

  MsgStream log(msgSvc(), name());
//   log << MSG::DEBUG << "navitool = " << navitool << endreq;
//   log << MSG::DEBUG << "te = " << te << endreq;
  log << MSG::DEBUG << "Look for an roi (already found " 
      << objs.size() << " objs)" << endreq;

  const std::vector<HLT::TriggerElement*> tmp_tes;
  const std::vector<HLT::TriggerElement*>::const_iterator p_te;
  HLTObjectsInRoI* x=0;

  std::vector<HLTObjectsInRoI*>::iterator p;
  for (p=objs.begin(); p!=objs.end(); ++p) {
    if (*p) {
      //      log << MSG::DEBUG << "roi ok" << endreq;

      HLTObjectsInRoI* y = func();
      y->setDataFromTE(isig, te, navitool, level);
      if (RoI_AseedsB(*p, te, navitool)) {
	if (y->runBySameFex(*p)) {
	  // roiA_a -> teA1_a -> teA2_a
	  // roiA_b -> teA1_b -> teA2_b
	  // This condition selects which RoI the object belong to when
	  // there are more than one RoI of the same TE name.
	  log << MSG::DEBUG << "SEEDED_BY && RUN_BY_SAME_FEX" << endreq;
	  x = *p;
	} else {
	  // A ->B1,B2->C1,C2 
	  // in case A->B1 and A->B2 use different algorithms
	}
      } else if ( (*p)->isInSameRoI(te, navitool) && y->runBySameFex(*p)) {
	// roi0 -> roiA -> teA -> teA
	//      |->roiB -> teB -> teB
	// This condition selects makes sure that no new RoI object is 
	// created for the physically same RoI. Note that the TE in the first 
	// step has no direct predecessor except for the RoI node.
	log << MSG::DEBUG << "IN_SAME_ROI && RUN_BY_SAME_FEX" << endreq;
	x = *p;
      }
      delete y;
    }
  }
  if (x==0) log << MSG::DEBUG << "NO_COMMON_ROI" << endreq;
  return x;
}

bool TrigAccessTool::TE_AseedsB(const HLT::TriggerElement* teA,
				const HLT::TriggerElement* teB, 
				HLT::NavigationCore* navitool) {
  bool status = true;
  std::list<HLT::TriggerElement*> tes;
  std::list<HLT::TriggerElement*> tes_to_check;
  const std::vector<HLT::TriggerElement*> temp = 
    navitool->getDirectPredecessors(teB);
  copy(temp.begin(), temp.end(), back_inserter(tes_to_check));
  tes = tes_to_check;

  std::list<HLT::TriggerElement*>::const_iterator p;
  std::vector<HLT::TriggerElement*>::const_iterator q;

  while (tes_to_check.size() > 0) {
    HLT::TriggerElement* te = tes_to_check.front();
    tes_to_check.pop_front();
    if (find(tes.begin(), tes.end(), te) == tes.end()) tes.push_back(te);
    const std::vector<HLT::TriggerElement*> tmp = 
      navitool->getDirectPredecessors(te);
    for (q=tmp.begin(); q!=tmp.end(); ++q) {
      if (find(tes.begin(), tes.end(), *q) == tes.end()) {
	tes_to_check.push_back(*q);
      }
    }
  }
  if (find(tes.begin(), tes.end(), teA) != tes.end()) status = true;
  return status;
}
 
bool TrigAccessTool::RoI_AseedsB(const HLTObjectsInRoI* roiA,
				   const HLT::TriggerElement* teB, 
				   HLT::NavigationCore* navitool) {
  bool status = true;

  const std::vector<HLT::TriggerElement*> tmp = 
    navitool->getDirectPredecessors(teB);
  std::vector<const HLT::TriggerElement*> tes = roiA->getTEs();

  std::vector<HLT::TriggerElement*>::const_iterator p;

  for(p=tmp.begin(); p!=tmp.end(); ++p) {
    if (find(tes.begin(), tes.end(), *p) == tes.end()) {
      status = false;
      break;
    }
  }
  return status;
}

bool TrigAccessTool::RoI_AseedsB(const HLTObjectsInRoI* roiA,
				   const HLT::TriggerElement* teB, 
				   Trig::TrigDecisionTool* /*tdt*/) {
  bool status = true;
  HLT::NavigationCore* navitool = 0;//tdt->navigation();
  //return const_cast<HLT::Navigation*>(tdt->cgm()->navigation());
  const std::vector<HLT::TriggerElement*> tmp = 
    navitool->getDirectPredecessors(teB);
  std::vector<const HLT::TriggerElement*> tes = roiA->getTEs();

  std::vector<HLT::TriggerElement*>::const_iterator p;

  for(p=tmp.begin(); p!=tmp.end(); ++p) {
    if (find(tes.begin(), tes.end(), *p) == tes.end()) {
      status = false;
      break;
    }
  }
  return status;
}


bool TrigAccessTool::getL1RoIObjects(L1RoIObjects& objs) {
  MsgStream log(msgSvc(), name());

  const LVL1_ROI* rois;
  StoreGateSvc* sgsvc = getStoreGateSvc();

  if (sgsvc->retrieve(rois).isFailure()) {
    log << MSG::WARNING << "Cannot find LVL1_ROI" << endreq;
  } else {
    log << MSG::WARNING << "found LVL1_ROI" << endreq;
    objs.setMuonRoIs(rois->getMuonROIs());
    objs.setEmTauRoIs(rois->getEmTauROIs());
    objs.setJetRoIs(rois->getJetROIs());
    objs.setJetEtRoIs(rois->getJetEtROIs());
    objs.setEnergyRoIs(rois->getEnergySumROIs());

  }

  return true;
}

bool TrigAccessTool::getChainObjects(const std::string& chain_name, 
				    std::vector<HLTObjectsInRoI*>& objs, 
				    HLTObjectsInRoI* (*func)()) {
  MsgStream log(msgSvc(), name());
  //  log.setLevel(MSG::DEBUG);
  log << MSG::DEBUG << "In getChainObjects: " << chain_name << endreq;
  //  return true;

  //  int level=2;
  const Trig::ChainGroup* cg=0;

  map<string, const Trig::ChainGroup*>::iterator p_cg = 
    mChainGroups.find(chain_name);
  if (p_cg == mChainGroups.end()) {
    cg = mTrigDecisionTool->getChainGroup(chain_name);
    mChainGroups[chain_name] = cg;
    log << MSG::DEBUG << "Creating chain group now" << endreq;
  } else {
    cg = p_cg->second;
  }

  log << MSG::DEBUG << "Print ChainGroup : " << cg->patterns() << endreq;
  log << MSG::DEBUG << "List of triggers: " << cg->getListOfTriggers() << endreq;
  log << MSG::DEBUG << "passed? : " <<  cg->isPassed() << endreq;

  Trig::FeatureContainer fc = cg->features(TrigDefs::alsoDeactivateTEs);
  std::vector<Trig::Combination> combs = fc.getCombinations();

  log << MSG::DEBUG << "Number of combinations from TDT: " << 
    combs.size() << endreq;
  std::vector<Trig::Combination>::const_iterator p_comb;
  std::vector<HLTObjectsInRoI*>::const_iterator p_roi;
  HLTObjectsInRoI* roi = 0;

  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    roi = func();
    roi->setDataFromCombination(*p_comb, chain_name, *mTrigDecisionTool);
    log << MSG::DEBUG << "dump HLTObjectsInRoI for the tmp" << endreq;
    roi->dump();
    bool foundit=false;
    log << MSG::DEBUG << "Number of existing RoIs to compare: " << objs.size() << endreq;
    for (p_roi=objs.begin(); p_roi!=objs.end(); ++p_roi) {
      log << MSG::DEBUG << "dump HLTObjectsInRoI for the comparing one" << endreq;
      (*p_roi)->dump();
      if ( (*p_roi)->runBySameFex(roi)) {
	(*p_roi)->setDataFromCombination(*p_comb, chain_name, *mTrigDecisionTool);
	delete roi;
	roi = *p_roi;
	foundit = true;
	log << MSG::DEBUG << "Found RoI run with the same Fex" << endreq;
	break;
      }
    }
    if (!foundit) {
      objs.push_back(roi);
    }
    roi->addAssociatedChain(chain_name);
  }
  log << MSG::DEBUG << "hltaccess done for " << chain_name << endreq;

  return true;
}

void TrigAccessTool::createChainGroup(const std::string& chain_name) {
  const Trig::ChainGroup* cg = mTrigDecisionTool->getChainGroup(chain_name);
  if (mChainGroups.find(chain_name) == mChainGroups.end()) {
    mChainGroups[chain_name] = cg;
  }
}

void TrigAccessTool::printL1Results() const {
  MsgStream log(msgSvc(), name());

//   std::vector<std::string> configured_chains = 
//     mTrigDecisionTool->getConfiguredChainNames();

//   std::vector<std::string>::const_iterator p;
//   for (p=configured_chains.begin(); p!=configured_chains.end(); ++p) {
//     bool result = mTrigDecisionTool->isPassed(*p);
//     log << MSG::INFO << "chain: " << (*p) << " : " << result << endreq;
//   }
  
}

