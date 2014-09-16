/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <string>

#include "TrigL2MuonSA/MuCalStreamer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuCalStreamer::MuCalStreamer(const std::string& name, ISvcLocator* svc) 
  : HLT::FexAlgo(name, svc), 
    m_storeGate("StoreGateSvc", this->name())
{
  declareProperty("Timing", m_use_timer=false);

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuCalStreamer::~MuCalStreamer() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuCalStreamer::hltInitialize()
{

  msg() << MSG::DEBUG << "Initializing MuCalStreamer - package version " << PACKAGE_VERSION << endreq ;
  
  if (m_storeGate.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve service StoreGateSvc" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  m_timerSvc = 0;
  if (m_use_timer) {
    StatusCode sc = service( "TrigTimerSvc", m_timerSvc, true); 
    if( sc.isFailure() ) {
      msg() << MSG::ERROR << ": Unable to locate TrigTimer Service" << endreq;
    }
  
    if(m_timerSvc) {
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_DataPreparator"));
      m_timers[ITIMER_DATA_PREPARATOR]->propName("DataPreparator_Time");
            
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_PatternFinder"));
      m_timers[ITIMER_PATTERN_FINDER]->propName("PatternFinder_Time");
      
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_StationFitter"));
      m_timers[ITIMER_STATION_FITTER]->propName("StationFitter_Time");
      
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_TrackFitter"));
      m_timers[ITIMER_TRACK_FITTER]->propName("TrackFitter_Time");
    
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_TrackExtrapolator"));
      m_timers[ITIMER_TRACK_EXTRAPOLATOR]->propName("TrackExtrapolator_Time");
    
      m_timers.push_back(m_timerSvc->addItem("MuCalStreamer_TotalProcessing"));
      m_timers[ITIMER_TOTAL_PROCESSING]->propName("TotalProcessing_Time");
    }
  }

  // 
  
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuCalStreamer::hltBeginRun() {

   msg() << MSG::DEBUG << "hltBeginRun"<<  endreq;
   return HLT::OK;
}

HLT::ErrorCode MuCalStreamer::hltEndRun() {
   msg() << MSG::DEBUG << "hltEndRun"<<  endreq;
   return HLT::OK;
}

HLT::ErrorCode MuCalStreamer::hltFinalize() {
   msg() << MSG::DEBUG << "hltFinalize()" << endreq;
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuCalStreamer::hltExecute(const HLT::TriggerElement* inputTE, 
					  HLT::TriggerElement* outputTE)
{
  StatusCode sc = StatusCode::SUCCESS;

  // Initialize monitored variables;

  if (m_timerSvc) {
    for (unsigned int i_timer=0; i_timer<m_timers.size(); i_timer++) {
      m_timers[i_timer]->start();
      m_timers[i_timer]->pause();
    }
  }
  //  clearEvent();
  
  if (m_timerSvc) m_timers[ITIMER_TOTAL_PROCESSING]->resume();
  if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->resume();
  
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "hltExecute called" << endreq;
  


  //  m_timerSvc->print();

  return HLT::OK;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

