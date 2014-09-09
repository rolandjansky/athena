/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    TIDAMonTool.cxx         
//   
//            implementation of an IHLTMonToolAC 
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TIDAMonTool.cxx, v0.0  Wed 28 Oct 2009 02:23:23 CET sutt $


#include "TrigInDetAnalysis/Filter_AcceptAll.h"

//#include "TrigInDetAnalysisUtils/AnalysisConfig_Signature.h"
#include "TrigInDetAnalysisUtils/Filter_Track.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigIDtrkMonitoring/TIDAMonTool.h"

#include "TDirectory.h"


TIDAMonTool::TIDAMonTool(const std::string & type, const std::string & name, const IInterface* parent) 
  :  IHLTMonTool(type, name, parent),
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(0,0,0),
     m_buildNtuple(false),m_first_time(true)
{
  ATH_MSG_INFO("TIDAMonTool::TIDAMonTool() compiled: " << __DATE__ << " " << __TIME__);

  declareProperty( "pTCut",   m_pTCut   = 0 );
  declareProperty( "etaCut",  m_etaCut  = 5 );
  declareProperty( "d0Cut",   m_d0Cut   = 1000 );
  declareProperty( "z0Cut",   m_z0Cut   = 2000 );
  declareProperty( "siHits",  m_siHits  = -1 );

  declareProperty( "trtHits",   m_trtHits   = -2 );
  declareProperty( "strawHits", m_strawHits = -2 );

  declareProperty( "tauEtCutOffline",   m_tauEtCutOffline   = 0 );
  declareProperty( "doTauThreeProng",   m_doTauThreeProng = false);

  declareProperty( "pTCutOffline",   m_pTCutOffline   = 500 );
  declareProperty( "etaCutOffline",  m_etaCutOffline  = 2.5 );
  declareProperty( "d0CutOffline",   m_d0CutOffline   = 1000 );
  declareProperty( "z0CutOffline",   m_z0CutOffline   = 200 );
  declareProperty( "siHitsOffline",  m_siHitsOffline  = -1 );
  declareProperty( "pixHitsOffline", m_pixHitsOffline  = 1 );
  declareProperty( "sctHitsOffline", m_sctHitsOffline  = 6 );

  declareProperty( "trtHitsOffline",   m_trtHitsOffline   = -2 );
  declareProperty( "strawHitsOffline", m_strawHitsOffline = -2 );


  declareProperty( "phiWidth", m_phiWidth = 0.1 );
  declareProperty( "etaWidth", m_etaWidth = 0.1 );
  declareProperty( "zedWidth", m_zedWidth = 168 );

  declareProperty( "matchR",   m_matchR   = 0.1 );
  declareProperty( "matchPhi", m_matchPhi = 0.1 );

  declareProperty( "chainNames",        m_chainNames );
  declareProperty( "ntupleChainNames",  m_ntupleChainNames );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );
  declareProperty( "outputFileName", m_outputFileName = "TrkNtuple.root");
  declareProperty( "AnalysisConfig", m_analysis_config = "Ntuple");

  declareProperty( "GenericFlag", m_genericFlag = true );
  
  ATH_MSG_DEBUG("TIDAMonTool::TIDAMonTool() " << gDirectory->GetName());
  ATH_MSG_DEBUG("TIDAMonTool::TIDAMonTool() returning: ");

}



TIDAMonTool::~TIDAMonTool() { 
  ATH_MSG_INFO("TIDAMonTool::~TIDAMonTool()");
  for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];
}



StatusCode TIDAMonTool::init() {

  ATH_MSG_DEBUG(" ----- enter init() ----- ");
  ATH_MSG_DEBUG("TIDAMonTool::init() " << gDirectory->GetName());

  // roi width information
  m_roiInfo.etaHalfWidth(m_etaWidth);
  m_roiInfo.phiHalfWidth(m_phiWidth);
  m_roiInfo.zedHalfWidth(m_zedWidth);
  
  // track filters 
  // reference (offline) tracks...
  TrackFilter* filterRef = new Filter_Track( m_etaCutOffline, m_d0CutOffline, m_z0CutOffline, m_pTCutOffline,
					     1, 6, -1, -1,  -2, -2 );
  
  // test (trigger) tracks...
  TrackFilter* filterTest = new Filter_AcceptAll();
  
  // test (trt trigger) track selector 
  TrackFilter* filterTest_TRT = new Filter_Track( m_etaCut, m_d0Cut, m_z0Cut, m_pTCut,
						  -1, -1, -1, -1, m_strawHits, -2 );
  
  // keep track of the filters so they can be cleaned up at the end
  m_filters.push_back(filterRef);
  m_filters.push_back(filterTest);
  m_filters.push_back(filterTest_TRT);
  
  // track associators 
  TrackAssociator*  dR_matcher = new   Associator_BestDeltaRMatcher(  "EBdeltaR",    m_matchR ); // this needs to be set correctly
  TrackAssociator* phi_matcher = new Associator_BestDeltaPhiMatcher( "EBdeltaPhi", m_matchPhi ); // this needs to be set correctly
  
  
  // keep track of the filters so they can be cleaned up at the end
  m_associators.push_back(dR_matcher);
  m_associators.push_back(phi_matcher);

    for( unsigned i=0 ; i<m_ntupleChainNames.size() ; i++ ){

      m_sequences.push_back( new AnalysisConfig_Tier0(m_ntupleChainNames[i], 
						      m_ntupleChainNames[i], "", "",
						      m_ntupleChainNames[i], "", "",
						      &m_roiInfo,
						      filterTest, filterRef, 
						      dR_matcher,
						      new Analysis_Tier0(m_ntupleChainNames.at(i), 1000., 2.5, 1.5, 1.5 ) ) );

      if(m_ntupleChainNames.at(i).find("L2_e")!=std::string::npos ||
	 m_ntupleChainNames.at(i).find("EF_e")!=std::string::npos)
	
	m_sequences.push_back( new AnalysisConfig_Tier0(m_ntupleChainNames[i],
							m_ntupleChainNames[i], "mediumPP", "",
							m_ntupleChainNames[i], "", "",
							&m_roiInfo,
							filterTest, filterRef,
							dR_matcher,
							new Analysis_Tier0(m_ntupleChainNames.at(i), 1000., 2.5, 1.5, 1.5 ) ) );
						      
    }
    
    ATH_MSG_VERBOSE(" -----  exit init() ----- ");
    return StatusCode::SUCCESS; 
}


#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TIDAMonTool::book() { 
#else
StatusCode TIDAMonTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) { 
#endif

  ATH_MSG_VERBOSE(" ----- enter book() ----- ");
  
  ATH_MSG_DEBUG("TIDAMonTool::book() " << gDirectory->GetName());
    
  ATH_MSG_DEBUG("TIDAMonTool::book() SUTT buildNtuple " << m_buildNtuple
        << "\tNewEventBlock " << newEventsBlock 
		    << "\tNewLumiBlock "  << newLumiBlock 
		    << "\tNewRun "        << newRun);
  
  if ( newRun && m_first_time) { 
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
      ATH_MSG_INFO(" ----- booking for analysis " << m_sequences[i]->name() << " -----");
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);
      m_sequences[i]->book();      
    }
  }
  m_first_time = false;
  ATH_MSG_VERBOSE(" ----- exit book() ----- ");
  
  return StatusCode::SUCCESS; 
}

StatusCode TIDAMonTool::fill() { 
  ATH_MSG_VERBOSE(" ----- enter fill() ----- ");
  
  for( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->execute();
  
  ATH_MSG_VERBOSE(" ----- exit fill() ----- ");
  
  return StatusCode::SUCCESS; 
}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TIDAMonTool::proc() { 
#else
StatusCode TIDAMonTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun) {
#endif

  ATH_MSG_VERBOSE(" ----- enter proc() ----- ");
  
  
  if(endOfRun) {
    for( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();
  }
  
  ATH_MSG_VERBOSE(" ====== exit proc() ====== ");

  return StatusCode::SUCCESS; 
}


