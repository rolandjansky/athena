/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <vector>

#include "MissingETPerformance/MissingETPerformance.h"

MissingETPerformance::MissingETPerformance(const std::string& name,
  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
			      m_data ( "MissingETData" ),
			      m_resolutionTool ( "ResolutionTool" ),
			      m_basicPlotsTool ( "BasicPlotsTool" ),
			      m_linearityTool ("LinearityTool" ),
			      m_etaRingsTool ( "EtaRingsTool" ),
                              m_trigMissingETTool ( "TrigMissingETTool" ),
                              m_trigVsOfflineMissingETTool ( "TrigVsOfflineMissingETTool" ),
			      m_fakeMissingETTool ( "FakeMissingETTool" ),
			      m_zMuMuTool ( "ZMuMuTool" ),
			      m_zeeTool ( "ZeeTool" ),
                              m_muonTool ( "MuonTool" ),
			      m_containerComparatorTool ( "ContainerComparatorTool" ),
			      m_pileUpTool ( "PileUpTool" ),
			      m_trigDec ("Trig::TrigDecisionTool/TrigDecisionTool"),
                              m_muonData ("MissingETMuonData" ),
			      m_MissingETScaleTool ("MissingETScaleTool" ),
			      m_eventSelector ( "MissingETEventSelector" ),
			      m_metCompositionTool ( "MissingETCompositionTool" )
{

  //whether to run each tool
  declareProperty("doBasicPlotsTool",              m_doBasicPlotsTool=true);
  declareProperty("doResolutionTool",              m_doResolution=true);
  declareProperty("doLinearityTool",               m_doLinearity=true);
  declareProperty("doTrigMissingETTool",           m_doTrigMissingETTool=true);
  declareProperty("doTrigVsOfflineMissingETTool",  m_doTrigVsOfflineMissingETTool=true);
  declareProperty("doEtaRingsTool",                m_doEtaRingsTool=false);
  declareProperty("doFakeMissingETTool",           m_doFakeMissingETTool=false);
  declareProperty("doZMuMuTool",                   m_doZMuMuTool=false);
  declareProperty("doZeeTool",                     m_doZeeTool=false);
  declareProperty("doMuonTool",                    m_doMuonTool=false);
  declareProperty("doContainerComparatorTool",     m_doContainerComparatorTool=false);
  declareProperty("doPileUpTool",                  m_doPileUpTool=false);
  declareProperty("doEventSelector",               m_doEventSelector=false);
  declareProperty("doMissingETScaleTool",          m_doMissingETScaleTool=false);
  declareProperty("doMissingETCompositionTool",    m_doMETCompositionTool=false);

  declareProperty("FolderName",                    m_folderName="");

  declareProperty("FilterOnTrigger", m_filterOnTrigger=false, "whether to require a trigger from the list to have fired");
  declareProperty("TriggerNames", m_trigger_names, "List of trigger names to accept");
  declareProperty("TrigDecisionTool", m_trigDec, "Tool to access the trigger decision");

  declareProperty("ResolutionTool",                      m_resolutionTool);
  declareProperty("BasicPlotsTool",                      m_basicPlotsTool);
  declareProperty("LinearityTool",                       m_linearityTool);
  declareProperty("EtaRingsTool",                        m_etaRingsTool);
  declareProperty("TrigMissingETTool",                   m_trigMissingETTool);
  declareProperty("TrigVsOfflineMissingETTool",          m_trigVsOfflineMissingETTool);
  declareProperty("FakeMissingETTool",                   m_fakeMissingETTool);
  declareProperty("ZMuMuTool",                           m_zMuMuTool);
  declareProperty("ZeeTool",                             m_zeeTool);
  declareProperty("MuonTool",                            m_muonTool);
  declareProperty("ContainerComparatorTool",             m_containerComparatorTool);
  declareProperty("PileUpTool",                          m_pileUpTool);
  declareProperty("MissingETScaleTool",                  m_MissingETScaleTool);
  declareProperty("MissingETEventSelector",              m_eventSelector);
  declareProperty("MissingETCompositionTool",            m_metCompositionTool);
}

MissingETPerformance::~MissingETPerformance() {}

StatusCode MissingETPerformance::CBNT_initializeBeforeEventLoop() {
  ATH_MSG_DEBUG( "Initializing MissingETPerformance (before eventloop)"  );

  if (m_filterOnTrigger) {
    ATH_CHECK( m_trigDec.retrieve() );
    ATH_MSG_INFO( "Get handle on TrigDecisionTool"  );
  }

  ATH_CHECK( m_data.retrieve() );
  ATH_CHECK( m_muonData.retrieve() );
  return StatusCode::SUCCESS;
} 

StatusCode MissingETPerformance::initialize() {return CBNT_initialize();}
StatusCode MissingETPerformance::finalize() {return CBNT_finalize();}
StatusCode MissingETPerformance::execute() {return CBNT_execute();}

StatusCode MissingETPerformance::CBNT_initialize() {

  CBNT_initializeBeforeEventLoop();

  ATH_MSG_DEBUG( "Initializing MissingETPerformance"  );

  // end foldername with slash
  if (!m_folderName.empty()) {
    if (m_folderName.rfind("/")!=m_folderName.size()-1)
      m_folderName = m_folderName + "/";
  }

  if (m_doBasicPlotsTool) {
    ATH_CHECK( m_basicPlotsTool.retrieve() );
    if (!m_folderName.empty()) { m_basicPlotsTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_basicPlotsTool->CBNT_initialize() );
  }//BasicPlotsTool

  if (m_doTrigMissingETTool) {
    ATH_CHECK( m_trigMissingETTool.retrieve() );
    if (!m_folderName.empty()) { m_trigMissingETTool->SetFolderName(m_folderName); }
    ATH_CHECK(m_trigMissingETTool->CBNT_initialize() );
  }//TrigMissingETTool

  if (m_doTrigVsOfflineMissingETTool) {
    ATH_CHECK( m_trigVsOfflineMissingETTool.retrieve() );
    if (!m_folderName.empty()) { m_trigVsOfflineMissingETTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_trigVsOfflineMissingETTool->CBNT_initialize() );
  }//TrigVsOfflineMissingETTool
 
  
  if (m_doResolution) {
    ATH_CHECK( m_resolutionTool.retrieve() );
    if (!m_folderName.empty()) { m_resolutionTool->SetFolderName(m_folderName); }
    ATH_CHECK(  m_resolutionTool->CBNT_initialize() );
  }//ResolutionTool

  if (m_doLinearity) {
    ATH_CHECK( m_linearityTool.retrieve() );
    if (!m_folderName.empty()) { m_linearityTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_linearityTool->CBNT_initialize() );
  }//LinearityTool

  if (m_doEtaRingsTool) {
    ATH_CHECK( m_etaRingsTool.retrieve() );
    if (!m_folderName.empty()) { m_etaRingsTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_etaRingsTool->CBNT_initialize() );
  }//EtaRingsTool

  if (m_doFakeMissingETTool) {
    ATH_CHECK(  m_fakeMissingETTool.retrieve() );
    if (!m_folderName.empty()) { m_fakeMissingETTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_fakeMissingETTool->CBNT_initialize() );
  }//FakeMissingETTool

  if (m_doMuonTool) {
    ATH_CHECK( m_muonTool.retrieve() );
    if (!m_folderName.empty()) { m_muonTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_muonTool->CBNT_initialize() );
  }//MuonTool

  if (m_doContainerComparatorTool) {
    ATH_CHECK( m_containerComparatorTool.retrieve() );
    ATH_CHECK( m_containerComparatorTool->CBNT_initialize() );
  }//ContainerComparatorTool

  if (m_doZMuMuTool) {
    ATH_CHECK( m_zMuMuTool.retrieve() );
    if (!m_folderName.empty()) { m_zMuMuTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_zMuMuTool->CBNT_initialize() );
  }//ZMuMuTool

  if (m_doZeeTool) {
    ATH_CHECK( m_zeeTool.retrieve() );
    if (!m_folderName.empty()) { m_zeeTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_zeeTool->CBNT_initialize() );
  }//ZeeTool
  
  if (m_doPileUpTool) {
    ATH_CHECK( m_pileUpTool.retrieve() );
    if (!m_folderName.empty()) { m_pileUpTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_pileUpTool->CBNT_initialize() );
  }//PileUpTool

   if (m_doMissingETScaleTool) {
     ATH_CHECK( m_MissingETScaleTool.retrieve() );
     ATH_CHECK( m_MissingETScaleTool->CBNT_initialize() );
   }//ScaleTool

   if (m_doMETCompositionTool) {
     ATH_CHECK( m_metCompositionTool.retrieve() );
    if (!m_folderName.empty()) { m_metCompositionTool->SetFolderName(m_folderName); }
    ATH_CHECK( m_metCompositionTool->CBNT_initialize() );
  }//MissingETCompositionTool

  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode MissingETPerformance::CBNT_finalize() {

  if (m_doResolution)
    ATH_CHECK( m_resolutionTool->CBNT_finalize() );

  if (m_doLinearity)
    ATH_CHECK( m_linearityTool->CBNT_finalize() );
  
  if (m_doPileUpTool)
    ATH_CHECK( m_pileUpTool->CBNT_finalize() );
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
StatusCode MissingETPerformance::CBNT_clear() {

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode MissingETPerformance::CBNT_execute() {
  
  ATH_MSG_DEBUG( "in execute()"  );

  //if requested, check whether event passed trigger
  if (m_filterOnTrigger && !triggerFired()) {
    //no trigger in the list given in jobOptions fired, don't process event
    return StatusCode::SUCCESS;
  }

  MissingETData *data = &(*m_data);
  ATH_CHECK( data->retrieveContainers() );

  //if requested, check whether event passed EventSelector
  if (m_doEventSelector) {
    ATH_CHECK( m_eventSelector->retrieveContainers() );
    m_eventSelector->passMissingETData(data);
    if (!m_eventSelector->isSelectedEvent()) {
      return StatusCode::SUCCESS;
    }//if passes EventSelector
  }//if doEventSelector

  if (m_doLinearity)
    ATH_CHECK( m_linearityTool->execute(data) );

  if (m_doBasicPlotsTool)
    ATH_CHECK( m_basicPlotsTool->execute(data) );

  if (m_doResolution)
    ATH_CHECK( m_resolutionTool->execute(data) );

  if (m_doTrigMissingETTool)
    ATH_CHECK( m_trigMissingETTool->execute(data) );

  if (m_doTrigVsOfflineMissingETTool)
    ATH_CHECK( m_trigVsOfflineMissingETTool->execute(data) );

  if (m_doEtaRingsTool)
    ATH_CHECK( m_etaRingsTool->execute(data) );

  MissingETMuonData *muondata = &(*m_muonData);
  ATH_CHECK( m_muonData->retrieveMuons() );

  if (m_doFakeMissingETTool)
    ATH_CHECK( m_fakeMissingETTool->execute(data, muondata) );

  if (m_doMuonTool)
    ATH_CHECK( m_muonTool->execute(muondata) );

  if (m_doContainerComparatorTool)
    ATH_CHECK( m_containerComparatorTool->execute(data) );

  if (m_doZMuMuTool)
    ATH_CHECK( m_zMuMuTool->execute(data, muondata) );

  if (m_doZeeTool)
    ATH_CHECK( m_zeeTool->execute(data) );
  
  if (m_doPileUpTool)
    ATH_CHECK( m_pileUpTool->execute(data) );

  if (m_doMissingETScaleTool)
    ATH_CHECK( m_MissingETScaleTool->execute(data) );

  if (m_doMETCompositionTool)
    ATH_CHECK( m_metCompositionTool->execute(data) );

  return StatusCode::SUCCESS;
}

bool MissingETPerformance::triggerFired() {

  //loop over trigger names
  for (const std::string& trigname : m_trigger_names) {
    if (m_trigDec->isPassed(trigname, TrigDefs::eventAccepted)) {
      //trigger passed, return true
      return true;
    }
  }
  //no trigger passed, return false
  return false;
}//end triggerFired()
