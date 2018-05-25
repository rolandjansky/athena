/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWPRDValAlg.h"

#include "MMDigitVariables.h"
#include "MMSimHitVariables.h"
#include "MMSDOVariables.h"
#include "MMRDOVariables.h"
#include "MMPRDVariables.h"

#include "sTGCDigitVariables.h"
#include "sTGCSimHitVariables.h"
#include "sTGCRDOVariables.h"
#include "sTGCSDOVariables.h"
#include "sTGCPRDVariables.h"

#include "CSCDigitVariables.h"

#include "MuEntryVariables.h"
#include "TruthVariables.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

NSWPRDValAlg::NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_TruthVar(nullptr),
    m_MuEntryVar(nullptr),
    m_sTgcSimHitVar(nullptr),
    m_sTgcRdoVar(nullptr),
    m_sTgcSdoVar(nullptr),
    m_sTgcFastSdoVar(nullptr),
    m_sTgcDigitVar(nullptr),
    m_sTgcPrdVar(nullptr),
    m_MmSimHitVar(nullptr),
    m_MmSdoVar(nullptr),
	 m_MmFastSdoVar(nullptr),
    m_MmDigitVar(nullptr),
    m_MmRdoVar(nullptr),
    m_MmPrdVar(nullptr),
    m_CscDigitVar(nullptr),
    m_thistSvc(nullptr),
    m_tree(nullptr),
    m_detManager(nullptr),
    m_MmIdHelper(nullptr),
    m_sTgcIdHelper(nullptr),
    m_CscIdHelper(nullptr),
    m_runNumber(0),
    m_eventNumber(0)
{
  declareProperty("Truth_ContainerName",            m_Truth_ContainerName="TruthEvent");
  declareProperty("MuonEntryLayer_ContainerName",   m_MuEntry_ContainerName="MuonEntryLayer");
  declareProperty("NSWsTGC_ContainerName",          m_NSWsTGC_ContainerName="sTGCSensitiveDetector");
  declareProperty("NSWsTGC_DigitContainerName",     m_NSWsTGC_DigitContainerName="sTGC_DIGITS");
  declareProperty("NSWsTGC_SDOContainerName",       m_NSWsTGC_SDOContainerName="sTGC_SDO");
  declareProperty("NSWsTGC_RDOContainerName",       m_NSWsTGC_RDOContainerName="sTGCRDO");
  declareProperty("NSWsTGC_PRDContainerName",       m_NSWsTGC_PRDContainerName="STGC_Measurements");
  declareProperty("NSWMM_ContainerName",            m_NSWMM_ContainerName="MicromegasSensitiveDetector");
  declareProperty("NSWMM_DigitContainerName",       m_NSWMM_DigitContainerName="MM_DIGITS");
  declareProperty("NSWMM_SDOContainerName",         m_NSWMM_SDOContainerName="MM_SDO");
  declareProperty("NSWMM_RDOContainerName",         m_NSWMM_RDOContainerName="MMRDO");
  declareProperty("NSWMM_PRDContainerName",         m_NSWMM_PRDContainerName="MM_Measurements");
  declareProperty("CSC_DigitContainerName",         m_CSC_DigitContainerName="CSC_DIGITS");

  declareProperty("doTruth",         m_doTruth=false);
  declareProperty("doMuEntry",       m_doMuEntry=false);
  declareProperty("doSTGCHit",       m_doSTGCHit=false);
  declareProperty("doSTGCDigit",     m_doSTGCDigit=false);
  declareProperty("doSTGCFastDigit", m_doSTGCFastDigit=false);
  declareProperty("doSTGCRDO",       m_doSTGCRDO=false);
  declareProperty("doSTGCPRD",       m_doSTGCPRD=false);
  declareProperty("doMMHit",         m_doMMHit=false);
  declareProperty("doMMDigit",       m_doMMDigit=false);
  declareProperty("doMMFastDigit",   m_doMMFastDigit=false);
  declareProperty("doMMRDO",         m_doMMRDO=false);
  declareProperty("doMMPRD",         m_doMMPRD=false);
  declareProperty("doCSCDigit",      m_doCSCDigit=false);
}

StatusCode NSWPRDValAlg::initialize() {

  ATH_MSG_INFO("initialize()");

  ATH_CHECK( service("THistSvc", m_thistSvc) );

  m_tree = new TTree("NSWHitsTree", "Ntuple of NSWHits");
  m_tree->Branch("runNumber", &m_runNumber, "runNumber/i");
  m_tree->Branch("eventNumber", &m_eventNumber, "eventNumber/i");

  ATH_CHECK( m_thistSvc->regTree("/NSWPRDValAlg/NSWHitsTree", m_tree) );

  ATH_CHECK( detStore()->retrieve( m_detManager ) );

  ATH_CHECK( detStore()->retrieve( m_MmIdHelper ) );

  ATH_CHECK( detStore()->retrieve( m_sTgcIdHelper ) );

  ATH_CHECK( detStore()->retrieve( m_CscIdHelper ) );

  if (m_doTruth){
     m_TruthVar = new TruthVariables(&(*(evtStore())), m_detManager,
                                                m_tree, m_Truth_ContainerName);
     CHECK( m_TruthVar->initializeVariables() );
  }

  if (m_doMuEntry){
     m_MuEntryVar = new MuEntryVariables(&(*(evtStore())), m_detManager,
                                                m_tree, m_MuEntry_ContainerName);
     ATH_CHECK( m_MuEntryVar->initializeVariables() );
  }

  if (m_doSTGCHit){
     m_sTgcSimHitVar = new sTGCSimHitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_ContainerName);
     ATH_CHECK( m_sTgcSimHitVar->initializeVariables() );
  }
  
  if (m_doSTGCDigit){
     m_sTgcDigitVar = new sTGCDigitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_DigitContainerName);
     ATH_CHECK( m_sTgcDigitVar->initializeVariables() );

  	  // Take SDO conainer
     m_sTgcSdoVar = new sTGCSDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_SDOContainerName);
     ATH_CHECK( m_sTgcSdoVar->initializeVariables() );
  }

  if (m_doSTGCFastDigit){
  	  // Take the "fast_SDO" instead of the SDOs from full sim
     m_sTgcFastSdoVar = new sTGCSDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, "sTGCfast_SDO");
     ATH_CHECK( m_sTgcFastSdoVar->initializeVariables() );

     // Fast digits = PRD
     m_doSTGCPRD = true;
  }

  if (m_doSTGCRDO){
     m_sTgcRdoVar = new sTGCRDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_RDOContainerName);
     ATH_CHECK( m_sTgcRdoVar->initializeVariables() );
  }

  if (m_doSTGCPRD){
     m_sTgcPrdVar = new sTGCPRDVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_PRDContainerName);
     ATH_CHECK( m_sTgcPrdVar->initializeVariables() );
  }

  if (m_doMMHit) {
     m_MmSimHitVar = new MMSimHitVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_ContainerName);
     ATH_CHECK( m_MmSimHitVar->initializeVariables() );
  }

  if (m_doMMDigit) {
     m_MmDigitVar = new MMDigitVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_DigitContainerName);
     ATH_CHECK( m_MmDigitVar->initializeVariables() );

     // Take SDO conainer
     m_MmSdoVar = new MMSDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_SDOContainerName);
     ATH_CHECK( m_MmSdoVar->initializeVariables() );
  }

  if (m_doMMFastDigit){
  	  // Take the "fast_SDO" instead of the SDOs from full sim
     m_MmFastSdoVar = new MMSDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, "MMfast_SDO");
     ATH_CHECK( m_MmFastSdoVar->initializeVariables() );

     // Fast digits = PRD
     m_doMMPRD = true;
  }

  if (m_doMMRDO) {

    m_MmRdoVar = new MMRDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_RDOContainerName);
    ATH_CHECK( m_MmRdoVar->initializeVariables() );
  }

 if (m_doMMPRD){
     m_MmPrdVar = new MMPRDVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_PRDContainerName);
     ATH_CHECK( m_MmPrdVar->initializeVariables() );
  }

  if (m_doCSCDigit){
     m_CscDigitVar = new CSCDigitVariables(&(*(evtStore())), m_detManager,
                                                m_CscIdHelper, m_tree, m_CSC_DigitContainerName);
     ATH_CHECK( m_CscDigitVar->initializeVariables() );
  }

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::finalize()
{
  ATH_MSG_INFO("finalize()");

  if (m_TruthVar) { delete m_TruthVar; m_TruthVar=0;}
  if (m_doMuEntry) { delete m_MuEntryVar; m_MuEntryVar=0;}
  if (m_sTgcSimHitVar) { delete m_sTgcSimHitVar; m_sTgcSimHitVar=0;}
  if (m_sTgcRdoVar) { delete m_sTgcRdoVar; m_sTgcRdoVar=0;}
  if (m_sTgcSdoVar) { delete m_sTgcSdoVar; m_sTgcSdoVar=0;}
  if (m_sTgcFastSdoVar) { delete m_sTgcSdoVar; m_sTgcSdoVar=0;}
  if (m_sTgcDigitVar) { delete m_sTgcDigitVar; m_sTgcDigitVar=0;}
  if (m_sTgcPrdVar) { delete m_sTgcPrdVar; m_sTgcPrdVar=0;}
  if (m_MmSimHitVar) { delete m_MmSimHitVar; m_MmSimHitVar=0;}
  if (m_MmSdoVar) { delete m_MmSdoVar; m_MmSdoVar=0;}
  if (m_MmFastSdoVar) { delete m_MmSdoVar; m_MmSdoVar=0;}
  if (m_MmDigitVar) { delete m_MmDigitVar; m_MmDigitVar=0;}
  if (m_MmRdoVar) { delete m_MmRdoVar; m_MmRdoVar=0;}
  if (m_MmPrdVar) { delete m_MmPrdVar; m_MmPrdVar=0;}
  if (m_CscDigitVar) { delete m_CscDigitVar; m_CscDigitVar=0;}

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::execute()
{
  ATH_MSG_INFO("execute()");

  // Event information
  const EventInfo* pevt(0);
  if( evtStore()->retrieve(pevt).isSuccess() ) {
    m_runNumber = pevt->event_ID()->run_number();
    m_eventNumber = pevt->event_ID()->event_number();
    ATH_MSG_DEBUG("Now processing event number:" << m_eventNumber << ", run number:" << m_runNumber);
  } else {
    ATH_MSG_WARNING("Could not retrieve event info!");
    m_runNumber = -1;
    m_eventNumber = -1;
  }

  if (m_doTruth) ATH_CHECK( m_TruthVar->fillVariables() );

  if (m_doMuEntry) ATH_CHECK( m_MuEntryVar->fillVariables() );

  if (m_doSTGCHit) ATH_CHECK( m_sTgcSimHitVar->fillVariables() );

  if (m_doSTGCDigit) { ATH_CHECK( m_sTgcDigitVar->fillVariables() ); ATH_CHECK( m_sTgcSdoVar->fillVariables() ); }

  if (m_doSTGCFastDigit) ATH_CHECK( m_sTgcFastSdoVar->fillVariables() );

  if (m_doSTGCRDO) ATH_CHECK( m_sTgcRdoVar->fillVariables() );

  if (m_doSTGCPRD) ATH_CHECK( m_sTgcPrdVar->fillVariables() );

  if (m_doMMHit) ATH_CHECK( m_MmSimHitVar->fillVariables() );

  if (m_doMMDigit) { ATH_CHECK( m_MmDigitVar->fillVariables() ); ATH_CHECK( m_MmSdoVar->fillVariables() ); }

  if (m_doMMFastDigit) ATH_CHECK( m_MmFastSdoVar->fillVariables() );

  if (m_doMMRDO) ATH_CHECK( m_MmRdoVar->fillVariables() );

  if (m_doMMPRD) ATH_CHECK( m_MmPrdVar->fillVariables() );

  if (m_doCSCDigit) ATH_CHECK( m_CscDigitVar->fillVariables() );

  m_tree->Fill();

  return StatusCode::SUCCESS;
}

