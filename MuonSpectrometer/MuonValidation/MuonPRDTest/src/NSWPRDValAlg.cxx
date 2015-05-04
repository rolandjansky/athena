/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWPRDValAlg.h"

#include "MMDigitVariables.h"
#include "MMSimHitVariables.h"
#include "MMFastDigitVariables.h"

#include "sTGCDigitVariables.h"
#include "sTGCSimHitVariables.h"
#include "sTGCFastDigitVariables.h"

#include "CSCDigitVariables.h"

#include "MuEntryVariables.h"
#include "TruthVariables.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "xAODEventInfo/EventInfo.h"

NSWPRDValAlg::NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_TruthVar(nullptr),
    m_MuEntryVar(nullptr),
    m_sTgcSimHitVar(nullptr),
    m_sTgcFastDigitVar(nullptr),
    m_sTgcDigitVar(nullptr),
    m_MmSimHitVar(nullptr),
    m_MmFastDigitVar(nullptr),
    m_MmDigitVar(nullptr),
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
  declareProperty("Truth_ContainerName", m_Truth_ContainerName="TruthEvent");
  declareProperty("MuonEntryLayer_ContainerName", m_MuEntry_ContainerName="MuonEntryLayer");
  declareProperty("NSWsTGC_ContainerName", m_NSWsTGC_ContainerName="sTGCSensitiveDetector");
  declareProperty("NSWsTGC_FastDigitContainerName", m_NSWsTGC_FastDigitContainerName="STGC_Measurements");
  declareProperty("NSWsTGC_DigitContainerName", m_NSWsTGC_DigitContainerName="");
  declareProperty("NSWMM_ContainerName", m_NSWMM_ContainerName="MicromegasSensitiveDetector");
  declareProperty("NSWMM_FastDigitContainerName", m_NSWMM_FastDigitContainerName="MM_Measurements");
  declareProperty("NSWMM_DigitContainerName", m_NSWMM_DigitContainerName="MM_DIGITS");
  declareProperty("CSC_DigitContainerName", m_CSC_DigitContainerName="CSC_DIGITS");

  declareProperty("doTruth",         m_doTruth=false);
  declareProperty("doMuEntry",       m_doMuEntry=false);
  declareProperty("doSTGCHit",       m_doSTGCHit=false);
  declareProperty("doSTGCFastDigit", m_doSTGCFastDigit=false);
  declareProperty("doSTGCDigit",     m_doSTGCDigit=false);
  declareProperty("doMMHit",         m_doMMHit=false);
  declareProperty("doMMFastDigit",   m_doMMFastDigit=false);
  declareProperty("doMMDigit",       m_doMMDigit=false);
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

  if (m_doSTGCFastDigit){
     m_sTgcFastDigitVar = new sTGCFastDigitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_FastDigitContainerName);
     ATH_CHECK( m_sTgcFastDigitVar->initializeVariables() );
  }

  if (m_doSTGCDigit){
     m_sTgcDigitVar = new sTGCDigitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_DigitContainerName);
     ATH_CHECK( m_sTgcDigitVar->initializeVariables() );
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
  }

  if (m_doMMFastDigit){
     m_MmFastDigitVar = new MMFastDigitVariables(&(*(evtStore())), m_detManager,
                                                   m_MmIdHelper, m_tree, m_NSWMM_FastDigitContainerName);
     ATH_CHECK( m_MmFastDigitVar->initializeVariables() );
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
  if (m_sTgcFastDigitVar) { delete m_sTgcFastDigitVar; m_sTgcFastDigitVar=0;}
  if (m_sTgcDigitVar) { delete m_sTgcDigitVar; m_sTgcDigitVar=0;}
  if (m_MmSimHitVar) { delete m_MmSimHitVar; m_MmSimHitVar=0;}
  if (m_MmFastDigitVar) { delete m_MmFastDigitVar; m_MmFastDigitVar=0;}
  if (m_MmDigitVar) { delete m_MmDigitVar; m_MmDigitVar=0;}
  if (m_CscDigitVar) { delete m_CscDigitVar; m_CscDigitVar=0;}

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::execute()
{
  ATH_MSG_INFO("execute()");

  // Event information
  const xAOD::EventInfo *pevt = nullptr;
  if( ! ( evtStore()->retrieve(pevt).isSuccess() ) ) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    m_runNumber = -999999;
    m_eventNumber = -999999;
  } else {
    m_runNumber = pevt->runNumber();
    m_eventNumber = pevt->eventNumber();
  }

  if (m_doTruth) ATH_CHECK( m_TruthVar->fillVariables() );

  if (m_doMuEntry) ATH_CHECK( m_MuEntryVar->fillVariables() );

  if (m_doSTGCHit) ATH_CHECK( m_sTgcSimHitVar->fillVariables() );

  if (m_doSTGCFastDigit) ATH_CHECK( m_sTgcDigitVar->fillVariables() );

  if (m_doSTGCDigit) ATH_CHECK( m_sTgcDigitVar->fillVariables() );

  if (m_doMMHit) ATH_CHECK( m_MmSimHitVar->fillVariables() );

  if (m_doMMFastDigit) ATH_CHECK( m_MmFastDigitVar->fillVariables() );

  if (m_doMMDigit) ATH_CHECK( m_MmDigitVar->fillVariables() );

  if (m_doCSCDigit) ATH_CHECK( m_CscDigitVar->fillVariables() );

  m_tree->Fill();

  return StatusCode::SUCCESS;
}

