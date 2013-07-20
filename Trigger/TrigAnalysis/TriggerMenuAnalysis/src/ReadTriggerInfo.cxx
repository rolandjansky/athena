/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ReadTriggerInfo.cxx
*/
#include "TriggerMenuAnalysis/ReadTriggerInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "TriggerMenuAnalysis/TileMuObjectsInRoI.h"
#include "TriggerMenuAnalysis/ElectronObjectsInRoI.h"
#include "TriggerMenuAnalysis/JetObjectsInRoI.h"
#include "TriggerMenuAnalysis/TauObjectsInRoI.h"
#include "TriggerMenuAnalysis/MissingETObjectsInRoI.h"
#include "TriggerMenuAnalysis/L1RoIObjects.h"
#include "TrigSteeringEvent/TriggerDecision.h"
#include "TrigMuonEvent/MuonFeature.h"
#include <vector>

using namespace std;


ReadTriggerInfo::ReadTriggerInfo(const std::string& name, 
				 ISvcLocator* svcloc) :
  Algorithm(name, svcloc), 
  mStoreGateSvc("StoreGateSvc", this->name()),
  mTrigAccessTool("TrigAccessTool"), 
  //  mTrigMenuHists("TrigMenuHists"), 
  mEvent(0) {
  declareProperty("TrigAccessTool", mTrigAccessTool, "TrigAccessTool");
  //  declareProperty("TrigMenuHists", mTrigMenuHists, "TrigMenuHists");

  declareProperty("MuonChains", mMuonChains, "List of muon chain names");
  declareProperty("TileMuChains", mTileMuChains, "List of tileMu chain names");
  declareProperty("ElectronChains", mElectronChains,
                  "List of electron chain names");
  declareProperty("TauChains", mTauChains, "List of tau chain names");
  declareProperty("JetChains", mJetChains, "List of jet chain names");
  declareProperty("MetChains", mMetChains, "List of MET chain names");
}

ReadTriggerInfo::~ReadTriggerInfo() {
}

StatusCode ReadTriggerInfo::initialize() {
  MsgStream log(msgSvc(), name());

  if (mTrigAccessTool.retrieve().isFailure()) {
    log << MSG::WARNING << "Cannot retrieve TrigAccessTool" << endreq;
  }
//   if (mTrigMenuHists.retrieve().isFailure()) {
//     log << MSG::WARNING << "Cannot retrieve TrigMenuHists" << endreq;
//   }

  mEvent = TrigMenuEvent::getInstance();

  return StatusCode::SUCCESS;
}

StatusCode ReadTriggerInfo::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "ReadTriggerInfo::execute()" << endreq;

  mEvent->clear();

  std::vector<HLTObjectsInRoI*>& mu_objs = mEvent->getMuonChainObjects();
  std::vector<HLTObjectsInRoI*>& e_objs = mEvent->getElectronChainObjects();
  std::vector<HLTObjectsInRoI*>& j_objs = mEvent->getJetChainObjects();
  std::vector<HLTObjectsInRoI*>& tau_objs = mEvent->getTauChainObjects();
  std::vector<HLTObjectsInRoI*>& met_objs = mEvent->getMissingETChainObjects();
  std::vector<HLTObjectsInRoI*>& tilemu_objs = mEvent->getTileMuChainObjects();

  std::vector<std::string>::const_iterator s1;

  log << MSG::DEBUG << "Get chain objects in RoI" << endreq;
  for (s1=mMuonChains.begin(); s1!=mMuonChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<MuonObjectsInRoI>(*s1, mu_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << mu_objs.size() << endreq;
  }
  for (s1=mElectronChains.begin(); s1!=mElectronChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<ElectronObjectsInRoI>(*s1, e_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << e_objs.size() << endreq;
  }
  for (s1=mTauChains.begin(); s1!=mTauChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<TauObjectsInRoI>(*s1, tau_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << tau_objs.size() << endreq;
  }
  for (s1=mJetChains.begin(); s1!=mJetChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<JetObjectsInRoI>(*s1, j_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << j_objs.size() << endreq;
  }
  for (s1=mMetChains.begin(); s1!=mMetChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<MissingETObjectsInRoI>(*s1, met_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << met_objs.size() << endreq;
  }
  for (s1=mTileMuChains.begin(); s1!=mTileMuChains.end(); ++s1) {
    mTrigAccessTool->getChainObjects<TileMuObjectsInRoI>(*s1, tilemu_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << tilemu_objs.size() << endreq;
  }

  if (mu_objs.size() > 0) {
    MuonObjectsInRoI* muroi = dynamic_cast<MuonObjectsInRoI*>(mu_objs[0]);
    if (muroi) {
      const MuonFeature* x = muroi->getMuonFeature();
      if (x) {
	log << MSG::DEBUG << "muFast pt: " << x->pt() << endreq;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ReadTriggerInfo::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}

