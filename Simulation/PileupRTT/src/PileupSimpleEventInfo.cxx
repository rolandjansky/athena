/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "PileupRTT/PileupSimpleEventInfo.h"

using namespace PileupRTT;

//<<<<<< CONSTRUCTOR
 
PileupSimpleEventInfo::PileupSimpleEventInfo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  
  m_iEvent(0),

  m_runNumber(0),
  m_eventNumber(0),
  m_eventTime(0),
  m_lumiBlock(0),
  m_bCID(0),
  m_lVL1ID(0),
  m_iEventNumber(0),
  m_eventWeight(0),
  m_statusElement(0),
  m_lvl1TriggerType(0),
  m_lvl1TriggerInfo(0),
  m_lvl2TriggerInfo(0),
  m_evtFilterInfo(0),
  m_streamTagName(0),
  m_streamTagType(0),

  m_theTree(0),
  m_thistSvc(0) {}

StatusCode PileupSimpleEventInfo::initialize(){
  
  // get the tool service
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR("Unable to retrieve ToolSvc");
    return StatusCode::FAILURE;
  }

  m_iEvent =0;

  //declare more variables
  
  ATH_MSG_INFO("declare more variables");

  m_lvl1TriggerInfo = new std::vector<unsigned int>;
  m_lvl2TriggerInfo = new std::vector<unsigned int>;
  m_evtFilterInfo = new std::vector<unsigned int>;

  m_streamTagName = new std::vector<std::string>;
  m_streamTagType = new std::vector<std::string>;

  if ((service("THistSvc", m_thistSvc)).isFailure()){
    ATH_MSG_FATAL("THistSvc service not found");
    return StatusCode::FAILURE;
  }

  m_theTree = new TTree("EventInfoTree","EventInfoTree");

  m_theTree->Branch("Run",    &m_runNumber, "Run/I");    // run number
  m_theTree->Branch("Event",  &m_eventNumber, "Event/I");  // event number
  m_theTree->Branch("Time",   &m_eventTime, "Time/I");   // time stamp
  m_theTree->Branch("LumiBlock", &m_lumiBlock, "LumiBlock/I"); // luminosity block number
  m_theTree->Branch("BCID", &m_bCID, "BCID/I"); // bunch crossing ID
  m_theTree->Branch("LVL1ID", &m_lVL1ID, "LVL1ID/I"); // trigger LVL1 ID
  m_theTree->Branch("Weight", &m_eventWeight, "Weight/D"); // weight
  m_theTree->Branch("IEvent", &m_iEventNumber, "IEvent/I"); // sequential event number
  m_theTree->Branch("StatusElement",  &m_statusElement, "StatusElement/I");
  m_theTree->Branch("LVL1TriggerType",  &m_lvl1TriggerType, "LVL1TriggerType/I");
  m_theTree->Branch("LVL1TriggerInfo",m_lvl1TriggerInfo);
  m_theTree->Branch("LVL2TriggerInfo",m_lvl2TriggerInfo);
  m_theTree->Branch("EventFilterInfo",m_evtFilterInfo);
  m_theTree->Branch("StreamTagName",m_streamTagName);
  m_theTree->Branch("StreamTagType",m_streamTagType);

  if (StatusCode::SUCCESS == m_thistSvc->regTree("/AANT/EventInfoTree",m_theTree)) {
    ATH_MSG_INFO("Booked module ntuple");
  } else {
    ATH_MSG_ERROR("Unable to register module ntuple");
  }

  if (sc.isFailure()){
    ATH_MSG_ERROR(" Could not add variable to ntuple ");
  }
  
  return StatusCode::SUCCESS;
}


//<<<<<< METHOD execute

StatusCode PileupSimpleEventInfo::execute(){

  //get EventInfo for run and event number
 
  const EventInfo* eventInfo;
  //StatusCode sc = evtStore()->retrieve(eventInfo, m_eventInfo);
  StatusCode sc = StoreGate::pointer("StoreGateSvc")->retrieve(eventInfo);
  
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve event info");
    }
  
  const EventID* myEventID=eventInfo->event_ID();
  
  m_runNumber=myEventID->run_number();
  m_eventNumber=myEventID->event_number();
  ++m_iEvent;
  m_eventTime= myEventID->time_stamp() ; 
  m_lumiBlock=myEventID->lumi_block() ;
  m_bCID=myEventID->bunch_crossing_id() ;
  m_iEventNumber=m_iEvent;
  
  const EventType* myEventType=eventInfo->event_type();
  if (myEventType!=0) {
    m_eventWeight=myEventType->mc_event_weight();
  }else
    {
      m_eventWeight=-999;
    }
  
  const TriggerInfo* myTriggerInfo=eventInfo->trigger_info();
  if (myTriggerInfo!=0) {
    m_lVL1ID=myTriggerInfo->extendedLevel1ID();
    m_statusElement=myTriggerInfo->statusElement();
    m_lvl1TriggerType=myTriggerInfo->level1TriggerType();
    
    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt=myTriggerInfo->level1TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt_e=myTriggerInfo->level1TriggerInfo().end();
    for (;lvl1TrigIt!=lvl1TrigIt_e;lvl1TrigIt++)
      m_lvl1TriggerInfo->push_back(*lvl1TrigIt);
    
    
    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt=myTriggerInfo->level2TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt_e=myTriggerInfo->level2TriggerInfo().end();
    for (;lvl2TrigIt!=lvl2TrigIt_e;lvl2TrigIt++)
      m_lvl2TriggerInfo->push_back(*lvl2TrigIt);
    
    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt=myTriggerInfo->eventFilterInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt_e=myTriggerInfo->eventFilterInfo().end();
    for (;evtFilterIt!=evtFilterIt_e;evtFilterIt++)
      m_evtFilterInfo->push_back(*evtFilterIt);
    
    
    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt=myTriggerInfo->streamTags().begin();
    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt_e=myTriggerInfo->streamTags().end();
    for (;streamInfoIt!=streamInfoIt_e;streamInfoIt++) { 
      m_streamTagName->push_back(streamInfoIt->name());
      m_streamTagType->push_back(streamInfoIt->type());
    }
    
  }else
    {
      m_lVL1ID=0;
      m_statusElement=0;
      m_lvl1TriggerType=0;
    }
  
  m_theTree->Fill();

  return StatusCode::SUCCESS;
}


//<<<<<< METHOD finalize

StatusCode PileupSimpleEventInfo::finalize() {
  
  m_theTree->Print(); 

  m_runNumber=0;
  m_eventNumber=0;
  m_eventTime=0;
  m_lumiBlock=0;
  m_bCID=0;
  m_lVL1ID=0;
  m_iEventNumber=0;
  m_eventWeight=0;
  m_statusElement=0;
  m_lvl1TriggerType=0;
  m_lvl1TriggerInfo->clear();
  m_lvl2TriggerInfo->clear();
  m_evtFilterInfo->clear();
  m_streamTagName->clear();
  m_streamTagType->clear();

  return StatusCode::SUCCESS;
}
