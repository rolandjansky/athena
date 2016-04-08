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
  AthAlgTool(type, name , p), 
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool") {
  //  mNavigation("Navigation") {
  declareInterface<TrigAccessTool>(this);

  declareProperty("TrigDecisionTool", m_trigDecisionTool, "TrigDecisionTool");
  //  declareProperty("Navigation", mNavigation, "HLT::Navigation");
}

TrigAccessTool::~TrigAccessTool() {
}

StatusCode TrigAccessTool::initialize() {
  if (m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_WARNING ("Cannot retrieve service TrigDecisionTool");
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
  // decodeHLTResult("HLTResult_L2");
  // decodeHLTResult("HLTResult_EF");
  // return true;
}

HLTObjectsInRoI* TrigAccessTool::findRoI(int isig, 
					const HLT::TriggerElement* te, 
					std::vector<HLTObjectsInRoI*>& objs,
					HLTObjectsInRoI* (*func)(), int level) {
  HLT::NavigationCore* navitool = 0;

//   log << MSG::DEBUG << "navitool = " << navitool << endreq;
//   log << MSG::DEBUG << "te = " << te << endreq;
  ATH_MSG_DEBUG ("Look for an roi (already found " 
                 << objs.size() << " objs)");

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
	  ATH_MSG_DEBUG ("SEEDED_BY && RUN_BY_SAME_FEX");
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
	ATH_MSG_DEBUG ("IN_SAME_ROI && RUN_BY_SAME_FEX");
	x = *p;
      }
      delete y;
    }
  }
  if (x==0) ATH_MSG_DEBUG ("NO_COMMON_ROI");
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
  const LVL1_ROI* rois;
  StoreGateSvc* sgsvc = getStoreGateSvc();

  if (sgsvc->retrieve(rois).isFailure()) {
    ATH_MSG_WARNING ("Cannot find LVL1_ROI");
  } else {
    ATH_MSG_WARNING ("found LVL1_ROI");
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
  //  log.setLevel(MSG::DEBUG);
  ATH_MSG_DEBUG ("In getChainObjects: " << chain_name);
  //  return true;

  //  int level=2;
  const Trig::ChainGroup* cg=0;

  map<string, const Trig::ChainGroup*>::iterator p_cg = 
    m_chainGroups.find(chain_name);
  if (p_cg == m_chainGroups.end()) {
    cg = m_trigDecisionTool->getChainGroup(chain_name);
    m_chainGroups[chain_name] = cg;
    ATH_MSG_DEBUG ("Creating chain group now");
  } else {
    cg = p_cg->second;
  }

  ATH_MSG_DEBUG ("Print ChainGroup : " << cg->patterns());
  ATH_MSG_DEBUG ("List of triggers: " << cg->getListOfTriggers());
  ATH_MSG_DEBUG ("passed? : " <<  cg->isPassed());

  Trig::FeatureContainer fc = cg->features(TrigDefs::alsoDeactivateTEs);
  std::vector<Trig::Combination> combs = fc.getCombinations();

  ATH_MSG_DEBUG ("Number of combinations from TDT: " << combs.size());
  std::vector<Trig::Combination>::const_iterator p_comb;
  std::vector<HLTObjectsInRoI*>::const_iterator p_roi;
  HLTObjectsInRoI* roi = 0;

  for (p_comb=combs.begin(); p_comb!=combs.end(); ++p_comb) {
    roi = func();
    roi->setDataFromCombination(*p_comb, chain_name, *m_trigDecisionTool);
    ATH_MSG_DEBUG ("dump HLTObjectsInRoI for the tmp");
    roi->dump();
    bool foundit=false;
    ATH_MSG_DEBUG ("Number of existing RoIs to compare: " << objs.size());
    for (p_roi=objs.begin(); p_roi!=objs.end(); ++p_roi) {
      ATH_MSG_DEBUG ("dump HLTObjectsInRoI for the comparing one");
      (*p_roi)->dump();
      if ( (*p_roi)->runBySameFex(roi)) {
	(*p_roi)->setDataFromCombination(*p_comb, chain_name, *m_trigDecisionTool);
	delete roi;
	roi = *p_roi;
	foundit = true;
	ATH_MSG_DEBUG ("Found RoI run with the same Fex");
	break;
      }
    }
    if (!foundit) {
      objs.push_back(roi);
    }
    roi->addAssociatedChain(chain_name);
  }
  ATH_MSG_DEBUG ("hltaccess done for " << chain_name);

  return true;
}

void TrigAccessTool::createChainGroup(const std::string& chain_name) {
  const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chain_name);
  if (m_chainGroups.find(chain_name) == m_chainGroups.end()) {
    m_chainGroups[chain_name] = cg;
  }
}

void TrigAccessTool::printL1Results() const {
//   std::vector<std::string> configured_chains = 
//     m_trigDecisionTool->getConfiguredChainNames();

//   std::vector<std::string>::const_iterator p;
//   for (p=configured_chains.begin(); p!=configured_chains.end(); ++p) {
//     bool result = m_trigDecisionTool->isPassed(*p);
//     log << MSG::INFO << "chain: " << (*p) << " : " << result << endreq;
//   }
  
}

