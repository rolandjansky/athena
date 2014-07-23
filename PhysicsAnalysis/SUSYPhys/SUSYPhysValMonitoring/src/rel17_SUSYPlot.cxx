/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
SUSYPlot.cxx: Algorithm to make plots. See SUSYPlot.h.

Modified from SusyPlot.
***********************************************************************/

#include "SUSYPhysValMonitoring/rel17_SUSYPlot.h"

#include "SUSYPhysValMonitoring/rel17_ISUSYPlotTool.h"

#include "GaudiKernel/IToolSvc.h"

//////////////////////////////////////////////////////////////////////
SUSYPlot::SUSYPlot(const std::string& name,  ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) 
    , m_trigDecAlg("Trig::TrigDecisionTool/TrigDecisionTool")
    , m_histTools(this)
    , m_selectTools(this)
{

  declareProperty("CollectionKey", m_collectionKey);
  declareProperty("EventTruthKey", m_eventTruthKey);
  declareProperty("SelectToolKey", m_selectTools);
  declareProperty("HistToolKeys", m_histTools);
  declareProperty("DoTrigger", m_doTrigAlg=true);
  declareProperty("DoTruth", m_doTruthAlg=true);
  declareProperty("TrigDecisionTool",m_trigDecAlg,"Public TrigDecisionTool");
}

//////////////////////////////////////////////////////////////////////
SUSYPlot::~SUSYPlot() {}

//////////////////////////////////////////////////////////////////////
StatusCode SUSYPlot::initialize() {

  msg(MSG::INFO) <<"Initializing SUSYPlot " <<name() <<endreq;

  // create the tool service
  IToolSvc* myTools;
  StatusCode sc=service("ToolSvc", myTools);
  if ( sc.isFailure() ) {
    msg(MSG::FATAL) << "Tool Service not found" << endreq;
    return StatusCode::FAILURE;
  }
 
 // Get trigger tool
  if( m_doTrigAlg ) {
    sc = m_trigDecAlg.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) <<"Could not retrieve TrigDecisionTool!" <<endreq;
      return sc;
    }
    msg(MSG::INFO) <<"Created TriggerDecisionTool m_trigDecAlg" <<&(*m_trigDecAlg)<<endreq;  
  }

  //check that the hist tools exist
  sc = m_histTools.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Error retrieving histTools "<<endreq;
  } 

  if( m_histTools.size() == 0 ) {
    msg(MSG::ERROR) 
         << "No available ISUSYTool's found" << endreq;
    return StatusCode::FAILURE;
  }

  //check that the selectorTool exists if it has been specified
  sc =  m_selectTools.retrieve();
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Error retrieving selectTool"<<endreq;
  }

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////
StatusCode SUSYPlot::finalize() {

  msg(MSG::INFO) << "Finalizing SUSYPlot " << name() << endreq;

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////
StatusCode SUSYPlot::execute() {

  msg(MSG::DEBUG) <<"Executing SUSYPlot " <<name() <<endreq;

  // First call the selector tool if one is given 

  ToolHandleArray< ISUSYPlotTool >::iterator selIt = m_selectTools.begin();
  for(; selIt != m_selectTools.end(); ++selIt) {
    StatusCode sc = (*selIt)->takeAction();
    if( sc == StatusCode::FAILURE) {
      msg(MSG::ERROR) << "Select tool "<<(*selIt)<<" failed."<<endreq;
      return StatusCode::SUCCESS; // exit from event
    }
  }
  // Now call as many plotters as we have
  // Introduce doIt() in base class containing common code; it calls
  // takeAction() which does the rest.

  ToolHandleArray< ISUSYPlotTool >::iterator it = m_histTools.begin();

  for(; it != m_histTools.end(); ++it) {
    msg(MSG::DEBUG) <<"Executing tool for " <<name() <<endreq;
    StatusCode sc = (*it)->doIt(m_doTrigAlg, m_doTruthAlg, m_trigDecAlg);
    if (sc.isFailure())
      msg(MSG::ERROR) << "Tool " << (*it) <<" failed to doIt. " << endreq;
  }

  if(m_doTrigAlg){
   // Access trigger
   bool passl1 = m_trigDecAlg->isPassed("L1_.*",TrigDefs::eventAccepted);
   std::string l1em3 = "L1_EM3";
   bool passem3 = m_trigDecAlg->isPassed(l1em3,TrigDefs::eventAccepted);
   msg(MSG::DEBUG) <<"TRIGCHECK " <<passl1 <<" " <<passem3 <<endreq;
  }

  return StatusCode::SUCCESS;

}
