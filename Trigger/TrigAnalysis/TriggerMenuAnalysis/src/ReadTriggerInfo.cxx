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
  AthAlgorithm(name, svcloc), 
  m_trigAccessTool("TrigAccessTool"), 
  //  mTrigMenuHists("TrigMenuHists"), 
  m_event(0) {
  declareProperty("TrigAccessTool", m_trigAccessTool, "TrigAccessTool");
  //  declareProperty("TrigMenuHists", mTrigMenuHists, "TrigMenuHists");

  declareProperty("MuonChains", m_muonChains, "List of muon chain names");
  declareProperty("TileMuChains", m_tileMuChains, "List of tileMu chain names");
  declareProperty("ElectronChains", m_electronChains,
                  "List of electron chain names");
  declareProperty("TauChains", m_tauChains, "List of tau chain names");
  declareProperty("JetChains", m_jetChains, "List of jet chain names");
  declareProperty("MetChains", m_metChains, "List of MET chain names");
}

ReadTriggerInfo::~ReadTriggerInfo() {
}

StatusCode ReadTriggerInfo::initialize() {
  if (m_trigAccessTool.retrieve().isFailure()) {
    ATH_MSG_WARNING ("Cannot retrieve TrigAccessTool");
  }
//   if (mTrigMenuHists.retrieve().isFailure()) {
//     log << MSG::WARNING << "Cannot retrieve TrigMenuHists" << endreq;
//   }

  m_event = TrigMenuEvent::getInstance();

  return StatusCode::SUCCESS;
}

StatusCode ReadTriggerInfo::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "ReadTriggerInfo::execute()" << endreq;

  m_event->clear();

  std::vector<HLTObjectsInRoI*>& mu_objs = m_event->getMuonChainObjects();
  std::vector<HLTObjectsInRoI*>& e_objs = m_event->getElectronChainObjects();
  std::vector<HLTObjectsInRoI*>& j_objs = m_event->getJetChainObjects();
  std::vector<HLTObjectsInRoI*>& tau_objs = m_event->getTauChainObjects();
  std::vector<HLTObjectsInRoI*>& met_objs = m_event->getMissingETChainObjects();
  std::vector<HLTObjectsInRoI*>& tilemu_objs = m_event->getTileMuChainObjects();

  std::vector<std::string>::const_iterator s1;

  log << MSG::DEBUG << "Get chain objects in RoI" << endreq;
  for (s1=m_muonChains.begin(); s1!=m_muonChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<MuonObjectsInRoI>(*s1, mu_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << mu_objs.size() << endreq;
  }
  for (s1=m_electronChains.begin(); s1!=m_electronChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<ElectronObjectsInRoI>(*s1, e_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << e_objs.size() << endreq;
  }
  for (s1=m_tauChains.begin(); s1!=m_tauChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<TauObjectsInRoI>(*s1, tau_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << tau_objs.size() << endreq;
  }
  for (s1=m_jetChains.begin(); s1!=m_jetChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<JetObjectsInRoI>(*s1, j_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << j_objs.size() << endreq;
  }
  for (s1=m_metChains.begin(); s1!=m_metChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<MissingETObjectsInRoI>(*s1, met_objs);
    log << "Checking RoI size after chain: " << (*s1) 
	<< " objs.size: " << met_objs.size() << endreq;
  }
  for (s1=m_tileMuChains.begin(); s1!=m_tileMuChains.end(); ++s1) {
    m_trigAccessTool->getChainObjects<TileMuObjectsInRoI>(*s1, tilemu_objs);
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

