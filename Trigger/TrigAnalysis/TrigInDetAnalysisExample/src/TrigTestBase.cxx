//
//   @file    TrigTestBase.cxx
//   @author  Benjamin Sowden (benjamin.sowden@cern.ch)
//   @date    Oct 2014
//
//            implementation of a IHLTMonTool monitoring base class
//


#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"

#include "TrigInDetAnalysisExample/TrigTestBase.h"
#include "TrigInDetAnalysisExample/ChainString.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigInDetAnalysisUtils/Filters.h"


#include "TDirectory.h"

TrigTestBase::TrigTestBase(const std::string & type, const std::string & name, const IInterface* parent)
  :  IHLTMonTool(type, name, parent),
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(0,0,0),
     m_buildNtuple(false),
     m_initialisePerRun(true),
     m_firstRun(true),
     m_keepAllEvents(false),
     m_fileopen(false),
     m_sliceTag("")
{
  msg(MSG::INFO) << "TrigTestBase::TrigTestBase() compiled: " << __DATE__ << " " << __TIME__ << endreq;

  declareProperty( "SliceTag",   m_sliceTag = "TIDAMonTool/" );

  declareProperty( "pTCut",   m_pTCut   = 0 );
  declareProperty( "etaCut",  m_etaCut  = 5 );
  declareProperty( "d0Cut",   m_d0Cut   = 1000 );
  declareProperty( "z0Cut",   m_z0Cut   = 2000 );
  declareProperty( "siHits",  m_siHits  = -1 );

  declareProperty( "trtHits",   m_trtHits   = -2 );
  declareProperty( "strawHits", m_strawHits = -2 );

  declareProperty( "tauEtCutOffline",   m_tauEtCutOffline   = 0 );
  declareProperty( "doTauThreeProng",   m_doTauThreeProng = false);

  declareProperty( "pTCutOffline",      m_pTCutOffline      = 2000 );
  declareProperty( "etaCutOffline",     m_etaCutOffline     = 2.5 );
  declareProperty( "d0CutOffline",      m_d0CutOffline      = 1000 );
  declareProperty( "z0CutOffline",      m_z0CutOffline      = 2000 );
  declareProperty( "pixHitsOffline",    m_pixHitsOffline    = -1 ); // 1 <- old value
  declareProperty( "sctHitsOffline",    m_sctHitsOffline    = -1 ); // 6 <- old value
  declareProperty( "siHitsOffline",     m_siHitsOffline     = -1 );
  declareProperty( "blayerHitsOffline", m_blayerHitsOffline = -1 );
  declareProperty( "strawHitsOffline",  m_strawHitsOffline  = -2 );
  declareProperty( "trtHitsOffline",    m_trtHitsOffline    = -2 );

  declareProperty( "phiWidth", m_phiWidth = 0.1 );
  declareProperty( "etaWidth", m_etaWidth = 0.1 );
  declareProperty( "zedWidth", m_zedWidth = 168 );

  declareProperty( "matchR",   m_matchR   = 0.1 );
  declareProperty( "matchPhi", m_matchPhi = 0.1 );

  // declareProperty( "chainNames",        m_chainNames );
  declareProperty( "ntupleChainNames",  m_ntupleChainNames );
  declareProperty( "releaseMetaData",   m_releaseMetaData );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );

  // declareProperty( "outputFileName", m_outputFileName = "TrkNtuple.root");
  declareProperty( "AnalysisConfig", m_analysis_config = "Ntuple");

  declareProperty( "GenericFlag", m_genericFlag = true );

  declareProperty( "InitialisePerRun", m_initialisePerRun = false );

  declareProperty( "SelectTruthPdgId", m_selectTruthPdgId = 0 );

  declareProperty( "KeepAllEvents", m_keepAllEvents = false );

  msg(MSG::INFO) << "TrigTestBase::TrigTestBase() exiting " << gDirectory->GetName() << endreq;

  //  msg(MSG::INFO) << "TrigTestBase::TrigTestBase() returning: " << endreq;

}



TrigTestBase::~TrigTestBase() {

  //  m_sequences[i]->finalize();
  if ( m_fileopen ) for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();

  // msg(MSG::INFO) << "TrigTestBase::~TrigTestBase()" << endreq;
  // for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  // for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];

}



StatusCode TrigTestBase::init() {

  msg(MSG::DEBUG) << " ----- enter init() ----- " << endreq;

  msg(MSG::INFO) << "TrigTestBase::init() " << gDirectory->GetName() << endreq;

  // roi width information
  m_roiInfo.etaHalfWidth(m_etaWidth);
  m_roiInfo.phiHalfWidth(m_phiWidth);
  m_roiInfo.zedHalfWidth(m_zedWidth);

  /// NB: Do NOT create the sequences here - leave it until the book() method, since
  ///     we need to be automatically determine which chains to process, and so need
  ///     the TrigDecisionTool which is niot configured until we have an iov

  //  ATH_CHECK(ManagedMonitorToolBase::initialize());

  //  ATH_CHECK(IHLTMonTool::init());

  msg(MSG::DEBUG) << " -----  exit init() ----- " << endreq;

  return StatusCode::SUCCESS;
}


// bool newEventsBlock, bool newLumiBlock, bool newRun are protected varibales
// correctly set before this is called
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestBase::book() {
#else
StatusCode TrigTestBase::book(bool newEventsBlock, bool newLumiBlock, bool newRun)  {
#endif
  //StatusCode TrigTestBase::bookHistograms() {

  msg(MSG::DEBUG) << " ----- enter book() ----- " << endreq;

  msg(MSG::INFO) << "TrigTestBase::book() " << gDirectory->GetName() << endreq;


  msg(MSG::DEBUG) << "TrigTestBase::book() SUTT buildNtuple " << m_buildNtuple
      << "\tNewEventBlock " << newEventsBlock
      << "\tNewLumiBlock "  << newLumiBlock
      << "\tNewRun "        << newRun  <<  std::endl;


  /// create sequences if need be ...

  static bool _first = true;

  if ( _first ) {

    _first = false;

    // track filters
    // reference (offline) tracks...
    TrackFilter* filterRef = new Filter_Track( m_etaCutOffline,    m_d0CutOffline,   m_z0CutOffline,  m_pTCutOffline,
					       m_pixHitsOffline,   m_sctHitsOffline, m_siHitsOffline, m_blayerHitsOffline,  
					       m_strawHitsOffline, m_trtHitsOffline);

    // test (trigger) tracks...
    //  TrackFilter* filterTest = new Filter_Track( m_etaCut, m_d0Cut, m_z0Cut, m_pTCut, -1, -1, -1, -1,  -2, -2 );
    TrackFilter* filterTest = new Filter_AcceptAll();


    // keep track of the filters so they can be cleaned up at the end
    m_filters.push_back(filterRef);
    m_filters.push_back(filterTest);
    // m_filters.push_back(filterTest_TRT);


    // track associators
    TrackAssociator*  dR_matcher = new Associator_BestDeltaRMatcher("EBdeltaR", m_matchR); // this needs to be set correctly

    // keep track of the associators so they can be cleaned up at the end
    m_associators.push_back(dR_matcher);

    /// should only create the associators that we want to pass in - should control which 
    /// we want with a flag, so that we can configure which matcher we want to use and also 
    /// with what matching criteria
 
    //    TrackAssociator* phi_matcher = new Associator_BestDeltaPhiMatcher("EBdeltaPhi", m_matchPhi); // this needs to be set correctly
    //    m_associators.push_back(phi_matcher);

    msg(MSG::INFO) << "[91;1m" << "m_analysis_config " << m_analysis_config << "[m" << endreq;


    // if (m_analysis_config == "Tier0") {
    {
      std::vector<std::string> chains;
      chains.reserve( m_ntupleChainNames.size() );

      /// handle wildcard chain selection - but only the first time
      std::vector<std::string>::iterator chainitr = m_ntupleChainNames.begin();

      while ( chainitr!=m_ntupleChainNames.end() ) {
        /// get chain
        ChainString chainName = (*chainitr);

        /// check for wildcard ...
        // if ( chainName.head().find("*")!=std::string::npos ) {

          // std::cout << "wildcard chains: " << chainName << std::endl;

          /// delete from vector
          // m_ntupleChainNames.erase(chainitr);

          /// get matching chains
          std::vector<std::string> selectChains  = m_tdt->getListOfTriggers( chainName.head() );

          // std::cout << "selected chains " << selectChains.size() << std::endl;

          if ( selectChains.size()==0 ) msg(MSG::WARNING) << "[91;1m" << "No chains matched for requested input " << chainName << "[m" << endreq;

          for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {

            if ( chainName.tail()!="" )    selectChains[iselected] += ":"+chainName.tail();
            if ( chainName.extra()!="" )   selectChains[iselected] += ":"+chainName.extra();
            if ( chainName.element()!="" ) selectChains[iselected] += ":"+chainName.element();
            if ( !chainName.passed() )     selectChains[iselected] += ";DTE";

            /// replace wildcard with actual matching chains ...
            chains.push_back( selectChains[iselected] );

            msg(MSG::INFO) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << "^[[m" << endreq;

          }
        // } else {
          // chains.push_back( *chainitr );
        // }

        ++chainitr;
      }

      m_chainNames = chains;

      for (unsigned i=0; i<m_chainNames.size(); ++i){
        m_sequences.push_back( new AnalysisConfig_Tier0( m_sliceTag, // m_chainNames[i],
							m_chainNames[i], "", "",
							m_chainNames[i], "", "",
							&m_roiInfo,
							filterTest, filterRef,
							dR_matcher,
							new Analysis_Tier0(m_chainNames.at(i), m_pTCut, m_etaCut, m_d0Cut, m_z0Cut ) ) );
	
        m_sequences.back()->releaseData(m_releaseMetaData);
        msg(MSG::INFO) << " ----- creating for analysis " << m_sequences.back()->name() << " -----" << endreq;

      }
    }

  }

  if ( !m_fileopen && newRun && ( m_initialisePerRun || m_firstRun ) ) {
    m_fileopen = true;

    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) {
      msg(MSG::VERBOSE) << " ----- booking for analysis " << m_sequences[i]->name() << " -----" << endreq;
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);
      m_sequences[i]->book();
    }
    m_firstRun = false;
  }

  msg(MSG::DEBUG) << " ----- exit book() ----- " << endreq;
  return StatusCode::SUCCESS;

}





StatusCode TrigTestBase::fill() {

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- enter fill() ----- " << endreq;
  }

  std::vector<std::string> selectChains  = m_tdt->getListOfTriggers( "HLT_.*" );

  int passed_count = 0;

  for ( unsigned i=0 ; i<selectChains.size() ; i++ ) {
    if ( m_tdt->isPassed(selectChains[i]) ) {
      passed_count++;
      ATH_MSG_DEBUG( "chain " << selectChains[i] << "\tpass: " << m_tdt->isPassed(selectChains[i]) << "\tprescale: " << m_tdt->getPrescale(selectChains[i]) );
    }
  }
  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->execute();

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- exit fill() ----- " << endreq;
  }

  return StatusCode::SUCCESS;
}





// bool newEventsBlock, bool newLumiBlock, bool newRun are protected varibales
// correctly set before this is called
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestBase::proc() {
#else
StatusCode TrigTestBase::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun) {
#endif
  // StatusCode TrigTestBase::procHistograms() {


  msg(MSG::INFO) << " ----- enter proc() ----- " << endreq;
  if ( m_initialisePerRun && endOfRun) {
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();
    m_fileopen = false;
  }
  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endreq;
  return StatusCode::SUCCESS;
}








