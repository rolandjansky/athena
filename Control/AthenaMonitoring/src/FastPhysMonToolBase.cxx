/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastPhysMonToolBase.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "TTree.h"

FastPhysMonToolBase::FastPhysMonToolBase( const std::string & type, const std::string & name,
					  const IInterface* parent ):
  ManagedMonitorToolBase(type,name,parent),
  m_eventNumber(0),m_runNumber(0),m_LBNumber(0),m_mcEventWeight(0.),m_booked(false)
{}

FastPhysMonToolBase::~FastPhysMonToolBase() {}

StatusCode FastPhysMonToolBase::initialize() {

  ATH_MSG_DEBUG("Intializing FastPhysMonToolBase " << this->name());
  //Register Incident-handler
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",this->name());
  if (incSvc.retrieve().isFailure()) {
    msg(MSG::ERROR) << "Can't retrieve IIncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }
 
  incSvc->addListener(this,"BeginEvent");
  

  return ManagedMonitorToolBase::initialize();
}
  


void FastPhysMonToolBase::handle(const Incident&) {
  
  ATH_MSG_DEBUG("In incident handler...");
  const EventInfo* eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    msg(MSG::ERROR) << "Can't retrieve EventInfo object" << endmsg;
    return;
  }

  m_eventNumber=eventInfo->event_ID()->event_number();
  m_runNumber=eventInfo->event_ID()->run_number();
  m_LBNumber=eventInfo->event_ID()->lumi_block();
  m_mcEventWeight=eventInfo->event_type()->mc_event_weight();

  return;
}


TTree* FastPhysMonToolBase::regFPMTree(const std::string& treeName, const std::string treeTitle, const std::string& systemName, 
				       const std::string& chain, const std::string& merge) {

  TTree* tree=new TTree(treeName.c_str(),treeTitle.c_str());
  StatusCode sc=this->regFPMTree(tree,systemName,chain,merge);
  if (sc.isFailure()) {
    delete tree;
    tree=NULL;
  }
  return tree;
}

StatusCode  FastPhysMonToolBase::regFPMTree(TTree* tree,const std::string& systemName,const std::string& chain, const std::string& merge) {

  ATH_MSG_DEBUG("Registering FPM Tree with name " << tree->GetName() << " with system name " << systemName);

  if (ManagedMonitorToolBase::regTree(tree,systemName,all, ManagedMonitorToolBase::ATTRIB_UNMANAGED, chain, merge).isFailure()) {
    msg(MSG::ERROR) << "Failed to register Tree with name " << tree->GetName() << " with system name " << systemName << endmsg;
    return StatusCode::FAILURE;
  }

  tree->Branch("EventNumber",&m_eventNumber);
  tree->Branch("RunNumber",&m_runNumber);
  tree->Branch("LumiBlockN",&m_LBNumber);
  tree->Branch("MCEventWeight",&m_mcEventWeight);

  return StatusCode::SUCCESS;
}

StatusCode  FastPhysMonToolBase::bookHistograms() {
  if (m_booked) return StatusCode::SUCCESS;
  
  
  StatusCode sc=this->bookNtuple();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "bookNtuple method failed!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_booked=true;
  return StatusCode::SUCCESS;
}



StatusCode  FastPhysMonToolBase::bookNtuple() {
  msg(MSG::ERROR) << "No bookNtuple() method implemented for this FastPhysMonTool!" << endmsg;
  return StatusCode::FAILURE;
}
