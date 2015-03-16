/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// EventPropertyNtupleTool.cxx
//   Source file for class EventPropertyNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////


#include "TTree.h"
//Gaudi
// #include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/PropertyMgr.h"
#include "TrkValTools/EventPropertyNtupleTool.h"
#include "CommissionEvent/ComTime.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

// constructor
Trk::EventPropertyNtupleTool::EventPropertyNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_doTriggerInfo(true),
  m_doTrtPhase(false),
  m_comTimeName("TRT_Phase"),
  m_runNumber(0),
  m_eventNumber(0),
  m_tdc_phase(0),
  m_trigLvl1Type(0),
  m_trigLvl1TAV(0),
  m_lumiBlock(0),
  m_timeStamp(0),
  m_bCID     (0)

{
  declareInterface<IEventPropertyNtupleTool>(this);
  declareProperty("FillTrtPhase",m_doTrtPhase);
  declareProperty("FillTrigger",m_doTriggerInfo);
}

// destructor
Trk::EventPropertyNtupleTool::~EventPropertyNtupleTool() {}

StatusCode Trk::EventPropertyNtupleTool::initialize() {
  m_trigLvl1TAV = new std::vector<unsigned int>();
  ATH_MSG_DEBUG ("initialise successful in "<<name());
  return AthAlgTool::initialize();
}

StatusCode Trk::EventPropertyNtupleTool::finalize() {
  delete m_trigLvl1TAV;
  ATH_MSG_DEBUG ("finalize successfull in "<<name());
  return StatusCode::SUCCESS;
}


StatusCode Trk::EventPropertyNtupleTool::addNtupleItems( TTree* tree, const std::string ) const {
  if (!tree) return StatusCode::FAILURE;
  //-----------------
  // add items
  tree->Branch("RunNumber",   &m_runNumber,   "RunNumber/I");
  tree->Branch("EventNumber", &m_eventNumber, "EventNumber/I");
  tree->Branch("LumiBlockN",  &m_lumiBlock,   "LumiBlockN/I");
  tree->Branch("TimeStamp",   &m_timeStamp,   "TimeStamp/I");
  tree->Branch("BCID",        &m_bCID,        "BCID/I");
  if (m_doTrtPhase) {
    tree->Branch("TRT_Phase", &m_tdc_phase,   "TRT_Phase/F"); 
  }
  if (m_doTriggerInfo) {
    tree->Branch("Trig_LVL1StreamType", &m_trigLvl1Type,"Trig_LVL1StreamType/I");
    tree->Branch("Trig_LVL1InfoTAV",    &m_trigLvl1TAV);
  }
  return StatusCode::SUCCESS;
}

StatusCode Trk::EventPropertyNtupleTool::fillEventData( ) const {
  const EventInfo* eventInfo;
  if ((evtStore()->retrieve(eventInfo)).isFailure()) {
    msg(MSG::WARNING) << "Could not retrieve event info" << endreq;
    m_runNumber   = -999;
    m_eventNumber = -999;
    m_timeStamp   = -999;
    m_lumiBlock   = -999;
    m_bCID        = -999;
    return StatusCode::SUCCESS;
  } else {
    EventID* myEventID=eventInfo->event_ID();
    m_runNumber   = myEventID->run_number();
    m_eventNumber = myEventID->event_number();
    m_timeStamp   = myEventID->time_stamp() ; 
    m_lumiBlock   = myEventID->lumi_block() ;
    m_bCID        = myEventID->bunch_crossing_id() ;
  }

  if (m_doTrtPhase) {
    const ComTime* comtime=0;
  
    if (evtStore()->contains<ComTime>(m_comTimeName)) {
      StatusCode sc = evtStore()->retrieve(comtime,m_comTimeName);
      if (sc.isFailure() || !comtime) {
        ATH_MSG_WARNING ("Could not find the ComTime object with name "<< m_comTimeName);
      } else {
        m_tdc_phase = comtime->getTime();
      }
    } else {
      ATH_MSG_WARNING ("Could not find the ComTime object with name "<< m_comTimeName);
      return StatusCode::FAILURE;
    }
  }

  if (m_doTriggerInfo) {
    const TriggerInfo* myTriggerInfo=eventInfo->trigger_info();
    if (myTriggerInfo!=0) {
      m_trigLvl1Type = myTriggerInfo->level1TriggerType();
      std::vector<unsigned int> l1trigger = myTriggerInfo->level1TriggerInfo();
      if (l1trigger.size() == 24) { // size is 0 if trigger not simulated.
        m_trigLvl1TAV->resize(8);
        for (int i=0; i<8; ++i) {
          m_trigLvl1TAV->at(i) = l1trigger.at(i+16);
        }
      }
    } else m_trigLvl1Type = -1000;
  }

  return StatusCode::SUCCESS;

  // remember calling algo is the one to record the content on the tree.
}

StatusCode Trk::EventPropertyNtupleTool::resetVariables( ) const {
  m_eventNumber = 0;
  m_runNumber   = 0;
  m_tdc_phase   = 0.0;
  m_lumiBlock   = 0;
  m_tdc_phase   = 0;
  m_bCID        = 0;
  if (m_doTriggerInfo) {
    m_trigLvl1Type  = 0;
    m_trigLvl1TAV->clear();
  }
  return StatusCode::SUCCESS;
}

