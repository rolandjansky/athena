/**
 **     @file    TrigTestMonToolAC.cxx
 **
 **     @brief   implementation of an IHLTMonToolAC 
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:23:23 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"

#include "TrigInDetAnalysisExample/TrigTestMonToolAC.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Ntuple.h"
#include "TrigInDetAnalysisExample/ChainString.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigInDetAnalysisUtils/Filters.h"


#include "TDirectory.h"

TrigTestMonToolAC::TrigTestMonToolAC(const std::string & type, const std::string & name, const IInterface* parent) 
  :  IHLTMonTool(type, name, parent),
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(false),
     m_buildNtuple(false), 
     m_initialisePerRun(true),
     m_firstRun(true),
     m_keepAllEvents(false),
     m_fileopen(false),
     m_requireDecision(false),
     m_containTracks(false)
{
  msg(MSG::WARNING) << "TrigTestMonToolAC::TrigTestMonToolAC() compiled: " << __DATE__ << " " << __TIME__ << endmsg;

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

  declareProperty( "ContainTracks", m_containTracks = false );

  declareProperty( "ntupleChainNames",  m_ntupleChainNames );
  declareProperty( "releaseMetaData",   m_releaseMetaData );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );
  declareProperty( "outputFileName", m_outputFileName = "TrkNtuple.root");
  declareProperty( "AnalysisConfig", m_analysis_config = "Ntuple");

  declareProperty( "GenericFlag", m_genericFlag = true );

  declareProperty( "InitialisePerRun", m_initialisePerRun = false );

  declareProperty( "RequireDecision", m_requireDecision = false );

  declareProperty( "SelectTruthPdgId", m_selectTruthPdgId = 0 );

  declareProperty( "KeepAllEvents", m_keepAllEvents = false );
  
  msg(MSG::INFO) << "TrigTestMonToolAC::TrigTestMonToolAC() " << gDirectory->GetName() << endmsg;

  msg(MSG::INFO) << "TrigTestMonToolAC::TrigTestMonToolAC() returning: " << endmsg;

}



TrigTestMonToolAC::~TrigTestMonToolAC() { 

  //  m_sequences[i]->finalize();
  if ( m_fileopen ) for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();

  msg(MSG::INFO) << "TrigTestMonToolAC::~TrigTestMonToolAC()" << endmsg;
  for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];

}



StatusCode TrigTestMonToolAC::init() {

  msg(MSG::DEBUG) << " ----- enter init() ----- " << endmsg;

  msg(MSG::INFO) << "TrigTestMonToolAC::init() " << gDirectory->GetName() << endmsg;

  //  std::cout << "TrigTestMonToolAC::init() SUTT buildNtuple " << m_buildNtuple << std::endl;
  
  // roi width information
  //  m_roiInfo.etaHalfWidth(m_etaWidth);
  //  m_roiInfo.phiHalfWidth(m_phiWidth);
  //  m_roiInfo.zedHalfWidth(m_zedWidth);
  
  if ( m_buildNtuple ) { 
    m_sequences.push_back( new AnalysisConfig_Ntuple( &m_roiInfo, m_ntupleChainNames, 
						      m_outputFileName, m_tauEtCutOffline, m_selectTruthPdgId, 
						      m_keepAllEvents ) );
    m_sequences.back()->releaseData(m_releaseMetaData);
    if ( m_requireDecision ) m_sequences.back()->setRequireDecision(true);
    if ( m_mcTruth ) m_sequences.back()->setMCTruth(m_mcTruth);
  }  
  
  /// NB: Do NOT create the sequences here - leave it until the book() method, since
  ///     we need to be automatically determine which chains to process, and so need
  ///     the TrigDecisionTool which is niot configured until we have an iov

  msg(MSG::DEBUG) << " -----  exit init() ----- " << endmsg;

  return StatusCode::SUCCESS; 
}



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestMonToolAC::book() { 
#else
StatusCode TrigTestMonToolAC::book(bool newEventsBlock, bool newLumiBlock, bool newRun) { 
#endif

  msg(MSG::INFO) << " ----- enter book() (athena) ----- " << endmsg;

  msg(MSG::INFO) << "TrigTestMonToolAC::book() " << gDirectory->GetName() << endmsg;

#ifdef  ManagedMonitorToolBase_Uses_API_201704
  bool newEventsBlock = newEventsBlockFlag();
  bool newLumiBlock   = newLumiBlockFlag();
  bool newRun         = newRunFlag();
#endif

  msg(MSG::DEBUG) << "TrigTestMonToolAC::book() SUTT buildNtuple " << m_buildNtuple 
		  << "\tNewEventBlock " << newEventsBlock 
		  << "\tNewLumiBlock "  << newLumiBlock 
		  << "\tNewRun "        << newRun  <<  std::endl;


  /// create sequences if need be ...

  static bool _first = true;

  if ( _first ) { 

    _first = false;

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

    msg(MSG::WARNING) << "[91;1m" << "m_analysis_config " << m_analysis_config << "[m" << endmsg;
   
    if ( m_analysis_config=="Tier0" || !m_buildNtuple ) {

      msg(MSG::INFO) << "[91;1m" << "setting up tier 0 analyses " << endmsg;

      std::vector<std::string> chains;
      chains.reserve( m_ntupleChainNames.size() );
      
      /// handle wildcard chain selection - but only the first time

      std::vector<std::string>::iterator chainitr = m_ntupleChainNames.begin();
      
      
      while ( chainitr!=m_ntupleChainNames.end() ) {
	
	/// get chain

	ChainString chainName = (*chainitr);
	
	/// get matching chains ...
	if ( chainName.head().find("HLT_")==std::string::npos && 
	     chainName.head().find("EF_")==std::string::npos  && 
	     chainName.head().find("L2_")==std::string::npos ) { 
	  chainitr++;
	  continue;
	}

	std::vector<std::string> selectChains  = m_tdt->getListOfTriggers( chainName.head() );
	
	//  "[91;1m" 
	//  "[m" 

	if ( selectChains.size()==0 ) msg(MSG::WARNING) << "[91;1m"<< "No chains matched for requested input " << chainName <<   "[m" << endmsg;
	
	for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {
	  
	  if ( chainName.tail()!="" )    selectChains[iselected] += ":"+chainName.tail();
	  if ( chainName.extra()!="" )   selectChains[iselected] += ":"+chainName.extra();
	  if ( chainName.element()!="" ) selectChains[iselected] += ":"+chainName.element(); 
	  if ( chainName.roi()!="" )     selectChains[iselected] += ":"+chainName.roi();
	  if ( !chainName.passed() )     selectChains[iselected] += ";DTE";
	  
	  /// replace wildcard with actual matching chains ...

	  chains.push_back( selectChains[iselected] );
	  
	  msg(MSG::INFO) << "[91;1m"  << "Matching chain " << selectChains[iselected] << "[m" << endmsg;
	  
	}
	
	chainitr++;
      }
      
      m_chainNames = chains;
      
      msg(MSG::INFO) << "[91;1m" << "Matching chains " << m_chainNames.size() << " init() [m" << endmsg;
      
      for ( unsigned i=0 ; i<m_chainNames.size() ; i++ ){
	
	msg(MSG::INFO) << "[91;1m"  << "booking a Tier0 chain " << m_chainNames[i] << " [m" << endmsg;
	
	m_sequences.push_back( new AnalysisConfig_Tier0(m_chainNames[i], 
							m_chainNames[i], "", "",
							m_chainNames[i], "", "",
							&m_roiInfo,
							filterTest, filterRef, 
							dR_matcher,
							new Analysis_Tier0(m_chainNames.at(i), 1000., 2.5, 1.5, 1.5 ) ) );
	
	/// add the meta data
	
	m_sequences.back()->releaseData(m_releaseMetaData);	
	
	/// should the tracks be completely contained ? 

	dynamic_cast<AnalysisConfig_Tier0*>(m_sequences.back())->containTracks( m_containTracks );	


      }
      
    }
  }   
  
  msg(MSG::DEBUG) << "[91;1m  ----- File open? -----  [m" << endmsg;
 
  if ( !m_fileopen && newRun && ( m_initialisePerRun || m_firstRun ) ) { 
    m_fileopen = true;
    
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
      msg(MSG::INFO) << "[91;1m ----- booking for analysis " << m_sequences[i]->name() << " ----- [m" << endmsg;
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);
      m_sequences[i]->book();      
      msg(MSG::INFO) << "[91;1m ----- booked for analysis " << m_sequences[i]->name() << " ----- [m" << endmsg;
    }
    m_firstRun = false;
  }
  msg(MSG::DEBUG) << " ----- exit book() ----- " << endmsg;
  return StatusCode::SUCCESS; 
}
  





StatusCode TrigTestMonToolAC::fill() { 
  msg(MSG::INFO) << " ----- enter fill() (athena) ----- " << endmsg;  


  msg(MSG::INFO) << "chains: " << m_chainNames.size() << endmsg;

  for ( unsigned i=0 ; i<m_chainNames.size() ; i++ ) { 
    ChainString s = m_chainNames[i];
    //std::vector<std::string> triggers = m_tdt->getListOfTriggers( s.head() );
    msg(MSG::INFO) << "Trigger output " << s.head() << " " << s.tail() << "\tdecision " << m_tdt->isPassed( s.head() ) << endmsg;
  }
  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->execute();

  msg(MSG::INFO) << " ----- exit fill() ----- " << endmsg;
  return StatusCode::SUCCESS; 
}





#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestMonToolAC::proc() { 
#else
StatusCode TrigTestMonToolAC::proc(bool /* endOfEventsBlock*/, bool /* endOfLumiBlock*/, bool endOfRun) {
#endif

#ifdef  ManagedMonitorToolBase_Uses_API_201704
  bool endOfRun       = endOfRunFlag();
#endif

  msg(MSG::DEBUG) << " ----- enter proc() ----- " << endmsg;

  msg(MSG::DEBUG) << " ----- initialisePerRun: " <<  m_initialisePerRun << "\tendOfRun: " << endOfRun << "  -----" << endmsg;
 
  if ( m_initialisePerRun && endOfRun) {
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
      msg(MSG::INFO) << "[91;1m ----- finalize for analysis " << m_sequences[i]->name() << " ----- [m" << endmsg;
      m_sequences[i]->finalize();
    }
    m_fileopen = false;
  }
  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endmsg;
  return StatusCode::SUCCESS; 
}




