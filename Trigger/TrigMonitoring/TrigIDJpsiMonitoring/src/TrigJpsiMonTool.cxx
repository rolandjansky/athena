//
//   @file    TrigJpsiMonTool.cxx         
//   
//            implementation of an IHLTMonToolAC 
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigTestMonToolAC.cxx, v0.0  Wed 28 Oct 2009 02:23:23 CET sutt $
/** @author Mark Sutton (sutt@cern.ch) **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/
/** @author Christian Gutschow (chris.g@cern.ch) **/

#include "TrigInDetAnalysisUtils/Filter_Track.h"

#include "TrigIDJpsiMonitoring/TrigJpsiMonTool.h"
#include "TrigIDJpsiMonitoring/AnalysisConfig_Jpsi.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

using namespace TrigInDetAnalysis;

TrigJpsiMonTool::TrigJpsiMonTool(const std::string & type, const std::string & name, const IInterface* parent) 
  :  IHLTMonTool(type, name, parent),
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(false),
     m_buildNtuple(false),
     m_countBook(true)
{
  //  msg(MSG::DEBUG) << "TrigJpsiMonTool::TrigJpsiMonTool() compiled: " << __DATE__ << " " << __TIME__ << endmsg;

  declareProperty( "pTCut",   m_pTCut   = 0 );
  declareProperty( "etaCut",  m_etaCut  = 5 );
  declareProperty( "d0Cut",   m_d0Cut   = 1000 );
  declareProperty( "z0Cut",   m_z0Cut   = 2000 );
  declareProperty( "siHits",  m_siHits  = -1 );

  declareProperty( "trtHits",   m_trtHits   = -2 );
  declareProperty( "strawHits", m_strawHits = -2 );

  declareProperty( "pTCutOffline",   m_pTCutOffline   = 4000 );
  declareProperty( "etaCutOffline",  m_etaCutOffline  = 2.5 );
  declareProperty( "d0CutOffline",   m_d0CutOffline   = 100 );
  declareProperty( "z0CutOffline",   m_z0CutOffline   = 200 );
  declareProperty( "siHitsOffline",  m_siHitsOffline  = -1 );
  declareProperty( "pixHitsOffline", m_pixHitsOffline  = 1 );
  declareProperty( "sctHitsOffline", m_sctHitsOffline  = 6 );

  declareProperty( "trtHitsOffline",   m_trtHitsOffline   = -2 );
  declareProperty( "strawHitsOffline", m_strawHitsOffline = -2 );

  declareProperty( "phiWidth", m_phiWidth = 0.2 );
  declareProperty( "etaWidth", m_etaWidth = 0.2 ); 
  declareProperty( "zedWidth", m_zedWidth = 168 );

  declareProperty( "matchR",   m_matchR   = 0.2 );
  declareProperty( "matchPhi", m_matchPhi = 999 );

  //  declareProperty( "Chain0", m_chain0="EF_2mu4T_Jpsimumu_IDTrkNoCut" );
  //declareProperty( "Chain0", m_chain0="EF_e5_tight1_e5_NoCut" );
  declareProperty( "Chain0", m_chain1="EF_e24vh_tight1_e15_NoCut_Zee" );
  //declareProperty( "Chain1", m_chain1="EF_e5_tight1_e9_etcut_Jpsi" );
  declareProperty( "Chain1", m_chain0="EF_e5_tight1_e4_etcut_Jpsi" );

  /*declareProperty( "IDSCAN", m_collection0="TrigIDSCAN_Muon" );
  declareProperty( "SiTrack", m_collection1="TrigSiTrack_Muon" );
  //declareProperty( "TRTSF", m_collection2="TRTSegmentFinder" );
  //declareProperty( "EF", m_collection3="InDetTrigParticleCreation_Muon_EFID" );
  //declareProperty( "EFTRT", m_collection4="InDetTrigParticleCreationTRTOnly_Muon_EFID" );
  declareProperty( "StratA", m_collection5="TrigL2SiTrackFinder_MuonA" );
  declareProperty( "StratB", m_collection6="TrigL2SiTrackFinder_MuonB" );
  declareProperty( "StratC", m_collection7="TrigL2SiTrackFinder_MuonC" ); */

  //declareProperty( "IDSCAN", m_collection0="TrigIDSCAN_eGamma" );
  //declareProperty( "SiTrack", m_collection1="TrigSiTrack_eGamma" );
  //declareProperty( "TRTSF", m_collection2="TrigTRTSegFinder_eGamma" );
  declareProperty( "EF", m_collection0="InDetTrigParticleCreation_Electron_EFID" );
  //declareProperty( "EFTRT", m_collection4="InDetTrigParticleCreationTRTOnly_Electron_EFID" );
  declareProperty( "StratA", m_collection1="TrigL2SiTrackFinder_eGammaA" );
  declareProperty( "StratB", m_collection2="TrigL2SiTrackFinder_eGammaB" );
  declareProperty( "StratC", m_collection3="TrigL2SiTrackFinder_eGammaC" );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );

}



TrigJpsiMonTool::~TrigJpsiMonTool() { 
  for( unsigned int i = m_filters.size()     ; i-- ; )  delete m_filters[i];
  for( unsigned int i = m_associators.size() ; i-- ; )  delete m_associators[i];
  for( unsigned int i = m_sequences.size()   ; i-- ; )  delete m_sequences[i];
}



StatusCode TrigJpsiMonTool::init() {

  //  msg(MSG::DEBUG) << " ----- enter init() ----- " << endmsg;

  // roi width information
  //  m_roiInfo.etaHalfWidth(m_etaWidth);
  //  m_roiInfo.phiHalfWidth(m_phiWidth);
  //  m_roiInfo.zedHalfWidth(m_zedWidth);
  
  // track filters 
  // reference (EF) tracks...
  TrackFilter* filterRef = new Filter_Track( m_etaCutOffline, m_d0CutOffline, m_z0CutOffline, m_pTCutOffline,
					     -1, -1, -1, -1,  -2, -2 );
  
  // test (trigger) tracks...
  TrackFilter* filterTest = new Filter_Track( m_etaCut, m_d0Cut, m_z0Cut, m_pTCut, -1, -1, -1, -1,  -2, -2 );
  //TrackFilter* filterTest = new Filter_AcceptAll();
  
  // keep track of the filters so they can be cleaned up at the end
  m_filters.push_back(filterRef);
  m_filters.push_back(filterTest);

  // track associators 
  TrackAssociator*  dR_matcher = new   Associator_BestDeltaRMatcher(  "EBdeltaR",    m_matchR ); // this needs to be set correctly
  
  // keep track of the filters so they can be cleaned up at the end
  m_associators.push_back(dR_matcher);
  
    if( !m_chain0.empty() ) m_chainNames.push_back( m_chain0 );
    if( !m_chain1.empty() ) m_chainNames.push_back( m_chain1 );
    if( !m_chain2.empty() ) m_chainNames.push_back( m_chain2 );
    if( !m_chain3.empty() ) m_chainNames.push_back( m_chain3 );

    if( !m_collection0.empty() ) m_collectionNames.push_back( m_collection0 );
    if( !m_collection1.empty() ) m_collectionNames.push_back( m_collection1 );
    if( !m_collection2.empty() ) m_collectionNames.push_back( m_collection2 );
    if( !m_collection3.empty() ) m_collectionNames.push_back( m_collection3 );
    if( !m_collection4.empty() ) m_collectionNames.push_back( m_collection4 );    
    if( !m_collection5.empty() ) m_collectionNames.push_back( m_collection5 );
    if( !m_collection6.empty() ) m_collectionNames.push_back( m_collection6 );
    if( !m_collection7.empty() ) m_collectionNames.push_back( m_collection7 );    
  
   for( unsigned i=0 ; i<m_chainNames.size() ; i++ ){
      for( unsigned j=0; j<m_collectionNames.size(); j++ ){
	
	m_sequences.push_back( new AnalysisConfig_Jpsi( m_chainNames[i],  
							m_chainNames[i], "", m_collectionNames[j],
							m_chainNames[i], "", "TrackParticleCandidate",
							&m_roiInfo,
							filterTest, filterRef, 
							dR_matcher,
							new JpsiAnalysis(m_chainNames[i]) ) );

      }    
    }

    m_countBook = true;

    //    msg(MSG::DEBUG) << " -----  exit init() ----- " << endmsg;
    
    return StatusCode::SUCCESS; 
}



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigJpsiMonTool::book() { 
#else
StatusCode TrigJpsiMonTool::book(bool /*newEventsBlock*/, bool /*newLumiBlock*/, bool /*newRun*/) {
#endif
  //  msg(MSG::DEBUG) << " ----- enter book() ----- " << endmsg;
  if( m_countBook ){
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->book();      
    }
  }
  m_countBook=false;
  //  msg(MSG::DEBUG) << " ----- exit book() ----- " << endmsg;
  return StatusCode::SUCCESS; 
}

StatusCode TrigJpsiMonTool::fill() { 
  //  msg(MSG::DEBUG) << " ----- enter fill() ----- " << endmsg;  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->execute();
  //  msg(MSG::DEBUG) << " ----- exit fill() ----- " << endmsg;
  return StatusCode::SUCCESS; 
}

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigJpsiMonTool::proc() {
  bool endOfRun = endOfRunFlag();
#else
StatusCode TrigJpsiMonTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun) {
#endif
  //  msg(MSG::DEBUG) << " ----- enter proc() ----- " << endmsg;
  if(endOfRun) {
    for( unsigned i=0 ; i<m_sequences.size() ; i++ ) { m_sequences[i]->finalize(); }
  }
  //  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endmsg;
  return StatusCode::SUCCESS; 
}




