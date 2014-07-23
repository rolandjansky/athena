/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
SUSYPlotTool.cxx: Base tool to make plots from EventView. See SUSYPlotTool.h.

Modified from SusyPlot.
***********************************************************************/

#include "SUSYPhysValMonitoring/rel17_SUSYPlotTool.h"

// #include "EventInfo/EventInfo.h"
// #include "EventInfo/EventID.h"
// #include "EventInfo/EventType.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include <xAODEventInfo/xAODEventInfo/EventInfo.h>

//////////////////////////////////////////////////////////////////////
SUSYPlotTool::SUSYPlotTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent )
  : AthAlgTool( type, name, parent)
  , m_pPAT(0)
  , m_thistSvc(0)
{

  // Get information from parent SUSYPlot algorithm  
  // CONFUSES GENCONF

  m_foldername= "/hist/susy/";
  const INamedInterface* theAlg = dynamic_cast< const INamedInterface* >(parent);
  if (theAlg != 0) 
    m_foldername += theAlg->name() + "/";

  // Tool properties
  declareProperty("McEventName", m_McEventName = "GEN_AOD");

}

//////////////////////////////////////////////////////////////////////
SUSYPlotTool::~SUSYPlotTool() {}

//////////////////////////////////////////////////////////////////////
StatusCode SUSYPlotTool::initialize() {

  msg(MSG::INFO) <<"Initializing SUSYPlotTool " <<name() <<endreq;

  StatusCode sc;

  // create the tool service
  IToolSvc* myTools;
  sc=service("ToolSvc",myTools);
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Tool Service not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Get an AnalysisTools pointer
  IAlgTool* analysisTools(0);
  sc = myTools->retrieveTool("AnalysisTools", analysisTools);
  if ( sc.isFailure() ){
    msg(MSG::ERROR) << "Cannot find AnalysisTools" << endreq;
    return StatusCode::FAILURE;
  }
  m_pPAT=dynamic_cast< IAnalysisTools* >(analysisTools);
  if( ! m_pPAT ) {
    msg(MSG::ERROR) << "Cannot create m_pPAT" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Created AnalysisTools pointer m_pPAT"
       << endreq;

  // Get Root histogram service
  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure() || !m_thistSvc) {
    msg(MSG::ERROR) 
         << "Unable to retrieve pointer to THistSvc"
         << endreq;
    return sc;
  }

  msg(MSG::INFO) <<"m_thistSvc = " <<m_thistSvc <<endreq;

  msg(MSG::INFO) <<"McEventName = " <<m_McEventName <<endreq;

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////
StatusCode SUSYPlotTool::finalize() {
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// This should contain common code; the rest is in takeAction().


StatusCode SUSYPlotTool::doIt(bool dot, bool dt, ToolHandle<Trig::TrigDecisionTool> tdt) {

  // Trigger
  m_doTrigger = dot;
  if( m_doTrigger ) m_trigDec = tdt;

  // Run/event number and weight

  StatusCode sc;
//   const EventInfo* eventInfo;
//   StatusCode sc = evtStore()->retrieve( eventInfo );
//   if( sc.isFailure()  ||  !eventInfo ) {
//      msg(MSG::ERROR) << "No EventInfo found in TDS" << endreq;
//      return StatusCode::SUCCESS;
//   }
// 
//   m_runNumber = eventInfo->event_ID()->run_number();
//   m_eventNumber = eventInfo->event_ID()->event_number();
//   m_eventWeight=1;  //for data

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >( "EventInfo" );
  if (!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve event info with key: " "EventInfo");
    return StatusCode::FAILURE;
  }
  m_runNumber   = eventInfo->runNumber();
  m_eventNumber = eventInfo->eventNumber();
  m_eventWeight = 1;
  
  m_doTruth=dt;
  const McEventCollection* mcCollptr = 0;
  
  if( m_doTruth ) {
//     m_eventWeight=eventInfo->event_type()->mc_event_weight();
    m_eventWeight = eventInfo->mcEventWeight();
    sc = evtStore()->retrieve(mcCollptr, m_McEventName);
    if( sc.isFailure() ) {
      msg(MSG::ERROR) << "Could not retrieve McEventCollection "
		    << m_McEventName << endreq;
      return sc;
    }
    if( mcCollptr->size() == 0 ) {
      msg(MSG::ERROR) << "McEventCollection has size "
		    << mcCollptr->size() << endreq;
      return StatusCode::SUCCESS;
    }
    const HepMC::GenEvent* firstevt = (*mcCollptr->begin());
    const HepMC::WeightContainer& wtCont = firstevt->weights();
    if( wtCont.size() >= 1 ) m_eventWeight = wtCont[0];
    if( wtCont.size() >= 2 ) m_eventWeight = wtCont[1];
    if( m_eventWeight == 0 ) {
      msg(MSG::ERROR) << "Event weight = " << m_eventWeight << " " 
		    << wtCont.size() << endreq;
      m_eventWeight = 1;
    }
  }//end if m_doTruth
    
    
  if( m_doTrigger ) {
    m_trigger = m_trigDec->isPassed("L1_.*,L2_.*,EF_.*");
  } else {
    m_trigger = false;
  }


  // Now do the rest...

  sc = this->takeAction();
  return sc;

}
