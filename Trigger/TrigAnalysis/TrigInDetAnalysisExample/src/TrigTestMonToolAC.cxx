//
//   @file    TrigTestMonToolAC.cxx         
//   
//            implementation of an IHLTMonToolAC 
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigTestMonToolAC.cxx, v0.0  Wed 28 Oct 2009 02:23:23 CET sutt $


#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"

#include "TrigInDetAnalysisExample/TrigTestMonToolAC.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Ntuple.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigInDetAnalysisUtils/Filters.h"


#include "TDirectory.h"

TrigTestMonToolAC::TrigTestMonToolAC(const std::string & type, const std::string & name, const IInterface* parent) 
  :  IHLTMonTool(type, name, parent),
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(0,0,0),
     m_buildNtuple(false), 
     m_initialisePerRun(true),
     m_firstRun(true),
     m_keepAllEvents(false),
     m_fileopen(false)
{
  msg(MSG::INFO) << "TrigTestMonToolAC::TrigTestMonToolAC() compiled: " << __DATE__ << " " << __TIME__ << endreq;

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
  declareProperty( "releaseMetaData",   m_releaseMetaData );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );
  declareProperty( "outputFileName", m_outputFileName = "TrkNtuple.root");
  declareProperty( "AnalysisConfig", m_analysis_config = "Ntuple");

  declareProperty( "GenericFlag", m_genericFlag = true );

  declareProperty( "InitialisePerRun", m_initialisePerRun = false );

  declareProperty( "SelectTruthPdgId", m_selectTruthPdgId = 0 );

  declareProperty( "KeepAllEvents", m_keepAllEvents = false );
  
  msg(MSG::INFO) << "TrigTestMonToolAC::TrigTestMonToolAC() " << gDirectory->GetName() << endreq;

  msg(MSG::INFO) << "TrigTestMonToolAC::TrigTestMonToolAC() returning: " << endreq;

}



TrigTestMonToolAC::~TrigTestMonToolAC() { 

  //  m_sequences[i]->finalize();
  if ( m_fileopen ) for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();

  msg(MSG::INFO) << "TrigTestMonToolAC::~TrigTestMonToolAC()" << endreq;
  for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];

}



StatusCode TrigTestMonToolAC::init() {

  msg(MSG::DEBUG) << " ----- enter init() ----- " << endreq;

  msg(MSG::INFO) << "TrigTestMonToolAC::init() " << gDirectory->GetName() << endreq;

  std::cout << "TrigTestMonToolAC::init() SUTT buildNtuple " << m_buildNtuple << std::endl;
  
  // roi width information
  m_roiInfo.etaHalfWidth(m_etaWidth);
  m_roiInfo.phiHalfWidth(m_phiWidth);
  m_roiInfo.zedHalfWidth(m_zedWidth);
  
  // track filters 
  // reference (offline) tracks...
  //  TrackFilter* filterRef = new Filter_Track( m_etaCutOffline, m_d0CutOffline, m_z0CutOffline, m_pTCutOffline,
  TrackFilter* filterRef = new Filter_Track( m_etaCutOffline, m_d0CutOffline, m_z0CutOffline, m_pTCutOffline,
					     1, 6, -1, -1,  -2, -2 );
  
  // test (trigger) tracks...
  //  TrackFilter* filterTest = new Filter_Track( m_etaCut, m_d0Cut, m_z0Cut, m_pTCut, -1, -1, -1, -1,  -2, -2 );
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

  if ( m_buildNtuple && m_analysis_config != "Tier0") { 
    m_sequences.push_back( new AnalysisConfig_Ntuple( &m_roiInfo, m_ntupleChainNames, 
						      m_outputFileName, m_tauEtCutOffline, m_selectTruthPdgId, 
						      m_keepAllEvents ) );
    m_sequences.back()->releaseData(m_releaseMetaData);
    if ( m_mcTruth ) m_sequences.back()->setMCTruth(m_mcTruth);
  }  
  
  if(m_analysis_config == "Tier0"){
    
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
  }
    
  msg(MSG::DEBUG) << " -----  exit init() ----- " << endreq;

  return StatusCode::SUCCESS; 
}



#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode TrigTestMonToolAC::book() { 
#else
  StatusCode TrigTestMonToolAC::book(bool newEventsBlock, bool newLumiBlock, bool newRun) { 
#endif

  msg(MSG::DEBUG) << " ----- enter book() ----- " << endreq;

  msg(MSG::INFO) << "TrigTestMonToolAC::book() " << gDirectory->GetName() << endreq;


  msg(MSG::DEBUG) << "TrigTestMonToolAC::book() SUTT buildNtuple " << m_buildNtuple 
		  << "\tNewEventBlock " << newEventsBlock 
		  << "\tNewLumiBlock "  << newLumiBlock 
		  << "\tNewRun "        << newRun  <<  std::endl;

  if ( !m_fileopen && newRun && ( m_initialisePerRun || m_firstRun ) ) { 
    m_fileopen = true;

    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
      msg(MSG::INFO) << " ----- booking for analysis " << m_sequences[i]->name() << " -----" << endreq;
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);
      m_sequences[i]->book();      
    }
    m_firstRun = false;
  }
  msg(MSG::DEBUG) << " ----- exit book() ----- " << endreq;
  return StatusCode::SUCCESS; 
}



StatusCode TrigTestMonToolAC::fill() { 
  msg(MSG::DEBUG) << " ----- enter fill() ----- " << endreq;  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->execute();
  msg(MSG::DEBUG) << " ----- exit fill() ----- " << endreq;
  return StatusCode::SUCCESS; 
}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestMonToolAC::proc() { 
#else
StatusCode TrigTestMonToolAC::proc(bool /* endOfEventsBlock*/, bool /* endOfLumiBlock*/, bool endOfRun) {
#endif

  msg(MSG::DEBUG) << " ----- enter proc() ----- " << endreq;
  if ( m_initialisePerRun && endOfRun) {
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();
    m_fileopen = false;
  }
  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endreq;
  return StatusCode::SUCCESS; 
}




