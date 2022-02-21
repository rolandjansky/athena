/**
 **     @file    TrigR3Mon.cxx
 **
 **     @brief   implementation of a AthAlgorithm based monitoring base class
 **
 **     @author  Mark Sutton (sutt@cern.ch)
 **     @date    Tue  8 Feb 2022 09:08:26 GMT
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/Filters.h"
#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/TagNProbe.h"

// #include "AthenaMonitoring/AthenaMonManager.h"
// #include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigInDetAnalysisExample/ChainString.h"
#include "TrigInDetAnalysisExample/TIDAHistogram.h"

#include "TrigR3Mon.h"


TrigR3Mon::TrigR3Mon( const std::string & name, ISvcLocator* pSvcLocator) 
  :  AthAlgorithm(name, pSvcLocator), 
     m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_roiInfo(false),
     m_buildNtuple(false),
     m_initialisePerRun(true),
     m_firstRun(true),
     m_keepAllEvents(false),
     m_fileopen(false),
     m_first(true),
     m_useHighestPT(false),
     m_vtxIndex(-1),
     m_runPurity(false),
     m_shifter(false),
     m_shifterChains(1),
     m_sliceTag(""),
     m_containTracks(false),
     m_legacy(true)
{
  msg(MSG::INFO) << "TrigR3Mon::TrigR3Mon() compiled: " << __DATE__ << " " << __TIME__ << endmsg;

  declareProperty( "SliceTag",   m_sliceTag = "" );

  declareProperty( "pTCut",   m_pTCut   = 0 );
  declareProperty( "etaCut",  m_etaCut  = 5 );
  declareProperty( "d0Cut",   m_d0Cut   = 1000 );
  declareProperty( "z0Cut",   m_z0Cut   = 2000 );
  declareProperty( "siHits",  m_siHits  = -1 );

  declareProperty( "trtHits",   m_trtHits   = -2 );
  declareProperty( "strawHits", m_strawHits = -2 );

  declareProperty( "tauEtCutOffline",   m_tauEtCutOffline = 0 );
  declareProperty( "doTauThreeProng",   m_doTauThreeProng = false);

  declareProperty( "pTCutOffline",      m_pTCutOffline      = 2000 );
  declareProperty( "etaCutOffline",     m_etaCutOffline     = 2.5 );
  declareProperty( "d0CutOffline",      m_d0CutOffline      = 1000 );
  declareProperty( "z0CutOffline",      m_z0CutOffline      = 2000 );
  declareProperty( "pixHitsOffline",    m_pixHitsOffline    =  2 ); // 1 <- old value ( 2 degrees of freedom = 1 cluster ) 
  declareProperty( "sctHitsOffline",    m_sctHitsOffline    =  6 ); // 6 <- old value ( 6 clusters = 3 spacepoints )
  declareProperty( "siHitsOffline",     m_siHitsOffline     =  8 );
  declareProperty( "blayerHitsOffline", m_blayerHitsOffline = -1 ); // no requirement - in case IBL is off

  declareProperty( "pixHolesOffline",   m_pixHolesOffline   =  20 ); // essentially no limit
  declareProperty( "sctHolesOffline",   m_sctHolesOffline   =  20 ); // essentially no limit
  declareProperty( "siHolesOffline",    m_siHolesOffline    =  2 );  // npix holes + nsi holes <= 2 ( not degrees of freedom ! )   

  declareProperty( "ContainTracks",     m_containTracks     = false );  // use only basic track containment

  declareProperty( "trtHitsOffline",    m_trtHitsOffline    = -2 );
  declareProperty( "strawHitsOffline",  m_strawHitsOffline  = -2 );

  declareProperty( "matchR",   m_matchR   = 0.1 );
  declareProperty( "matchPhi", m_matchPhi = 0.1 );

  declareProperty( "ntupleChainNames",  m_ntupleChainNames );
  declareProperty( "releaseMetaData",   m_releaseMetaData );

  declareProperty( "buildNtuple",   m_buildNtuple = false );
  declareProperty( "mcTruth",       m_mcTruth = false );

  declareProperty( "AnalysisConfig", m_analysis_config = "Ntuple");

  declareProperty( "SelectTruthPdgId", m_selectTruthPdgId = 0 );

  declareProperty( "InitialisePerRun", m_initialisePerRun = false );
  declareProperty( "KeepAllEvents",    m_keepAllEvents = false );
  declareProperty( "UseHighestPT",     m_useHighestPT = false );
  declareProperty( "VtxIndex",         m_vtxIndex = -1 );

  declareProperty( "RunPurity",        m_runPurity = false );
  declareProperty( "Shifter",          m_shifter = false );

  declareProperty( "ShifterChains",    m_shifterChains = 1 );

  declareProperty( "GenericFlag",      m_genericFlag = true );

  declareProperty( "Legacy",           m_legacy = true );
  
  msg(MSG::INFO) << "TrigR3Mon::TrigR3Mon() exiting " << gDirectory->GetName() << endmsg;


}



TrigR3Mon::~TrigR3Mon() {

  if ( m_fileopen ) for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();

  // msg(MSG::INFO) << "TrigR3Mon::~TrigR3Mon()" << endmsg;
  // for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  // for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];

}



StatusCode TrigR3Mon::initialize() {

  msg(MSG::DEBUG) << " ----- enter init() ----- " << endmsg;

  //  std::cout << "sliceTag: "     << m_sliceTag << std::endl; 

  msg(MSG::INFO) << "TrigR3Mon::init() " << gDirectory->GetName() << " " << m_sliceTag << endmsg;

  /// NB: Do NOT create the sequences here - leave it until the book() method, since
  ///     we need to be automatically determine which chains to process, and so need
  ///     the TrigDecisionTool which is niot configured until we have an iov

#if 0
  std::cout << "TrigR3Mon::name              = " << name()     << std::endl;
  std::cout << "TrigR3Mon::SliceTag          = " << m_sliceTag << std::endl;
  std::cout << "TrigR3Mon::AnalysisConfig    = " << m_analysis_config << std::endl;
  std::cout << "TrigR3Mon::Legacy            = " << m_legacy   << std::endl;
#endif

  ATH_CHECK(m_monTools.retrieve());

  msg(MSG::DEBUG) << " -----  exit init() ----- " << endmsg;

  return bookHistograms();

}


StatusCode TrigR3Mon::bookHistograms() {

  msg(MSG::DEBUG) << " ----- enter book() ----- " << endmsg;

  msg(MSG::INFO) << "TrigR3Mon::book() " << gDirectory->GetName() << endmsg;

  /// create sequences if need be ...
  
  // track filters
  // reference (offline) tracks...
  TrackFilter* filterRef = new Filter_Track( m_etaCutOffline,    m_d0CutOffline,   0, m_z0CutOffline,  m_pTCutOffline,
					     m_pixHitsOffline,   m_sctHitsOffline, m_siHitsOffline, m_blayerHitsOffline,  
					     m_strawHitsOffline, m_trtHitsOffline, 0, 
					     m_pixHolesOffline, m_sctHolesOffline, m_siHolesOffline );
  
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
  
  // "^[[91;1m"
  // "^[[m"
  
  msg(MSG::INFO) << "^[[91;1m" << name() << "\t:AnalysisConfig " << m_analysis_config << "^[[m" << endmsg;
  
  msg(MSG::DEBUG) << "configuring chains: " << m_ntupleChainNames.size() << "\tmonTools: " << m_monTools.size() << endmsg;
  
  /// keep counters of how many efid or ftf chains have been created
  /// for shifter histograms, only want m_shifterMaxChains of each
  int shifter_efid      = 0;
  int shifter_ftf       = 0;
  int shifter_efid_run1 = 0;
  
  std::string lastvtx = "";
  
  if (m_analysis_config == "Tier0") {
  
    std::vector<std::string> chains;
    
    chains.reserve( m_monTools.size() );
    
    /// in the tier 0 analysis the wildcard selection should always 
    /// return one and only one chain

    std::vector<ToolHandle<GenericMonitoringTool>*> monTools;
    monTools.reserve(m_monTools.size());

    ToolHandleArray<GenericMonitoringTool>::iterator toolitr = m_monTools.begin();;
    
    while ( toolitr!=m_monTools.end() ) {
         
      /// get chain
      ChainString chainName = (toolitr->name());
      
      msg(MSG::DEBUG) << "configuring chain: " << chainName.head() << "\t: " << chainName.tail() << endmsg;

      //      std::cout << "\tconfiguring chain: " << (*toolitr) << "\t" << chainName.head() << "\t: " << chainName.tail() << "\t:" << chainName.roi() << std::endl;
            
      if ( chainName.roi()!="" ) { 
	msg(MSG::DEBUG) << "trying chain: " << chainName.head() 
			<< "\ttracks: " << chainName.tail()
			<< "\troi: "    << chainName.roi()
			<< endmsg;
      }
      else { 
	msg(MSG::DEBUG) << "trying chain: " << chainName.head() 
			<< "\ttracks: " << chainName.tail()
			<< endmsg;
      }
      
      //	std::cout << "\ttrying chain: " << chainName.head() 
      //  	          << "\ttracks: " << chainName.tail()
      //		  << "\troi: "    << chainName.roi()
      //		  << std::endl;
      
      
      /// do offline type analyses first ...
            
      if ( chainName.head() == "" ) { 
	
	std::string selectChain = "";
	
	if ( chainName.tail()!="" )    selectChain += ":key="+chainName.tail();
	if ( chainName.element()!="" ) selectChain += ":te="+chainName.element();
	if ( chainName.roi()!="" )     selectChain += ":roi="+chainName.roi();
	if ( chainName.vtx()!="" )     selectChain += ":vtx="+chainName.vtx();
	
	if ( chainName.extra()!="" )   selectChain += ":ex="+chainName.extra();
	if ( !chainName.passed() )     selectChain += ";DTE";

	//	if ( chainName.postcount() )   selectChain += ":post:"+chainName.post();
	
	//     toolitr->setPath( m_sliceTag+"/"+chainName );

	if (std::find(chains.begin(), chains.end(), selectChain) == chains.end()) { // deduplicate
  	  chains.push_back( selectChain );
	  monTools.push_back( &(*toolitr) );
	}
	
      }
      else { 
	
	/// check for configured chains only ...
	
	if ( chainName.head().find("HLT_")==std::string::npos ) {
	  toolitr++;
	  continue;
	}
	
	/// get matching chains
	
	std::string selectChain = chainName.head();
	
	/// for the Run 3 python config based, shoud return one-and-only one chains per item

	msg(MSG::INFO) << "checking chain: " << chainName.head() << endmsg;

	
	if ( selectChain=="" ) { 
	  msg(MSG::WARNING) << "^[[91;1m" << "No chain matched\tchain input " << chainName.head() << "  :  " << chainName.tail() << "^[[m"<< endmsg;
	  toolitr++;
	  continue;
	}
	  
	std::string mchain = selectChain; //chainName.head();

	if ( chainName.tail()!="" )     mchain += "/"+chainName.tail();
	if ( chainName.extra()!="" )    mchain += "_"+chainName.extra();
	if ( chainName.roi()!="" )      mchain += "_"+chainName.roi();
	if ( chainName.vtx()!="" )      mchain += "_"+chainName.vtx();
	if ( chainName.element()!="" )  mchain += "_"+chainName.element();
	if ( !chainName.passed() )      mchain += "_DTE";
	
	if ( chainName.tail()!="" )    selectChain += ":key="+chainName.tail();
	if ( chainName.extra()!="" )   selectChain += ":extra="+chainName.extra();
	if ( chainName.roi()!="" )     selectChain += ":roi="+chainName.roi();
	if ( chainName.vtx()!="" )     selectChain += ":vtx="+chainName.vtx();
	if ( chainName.element()!="" ) selectChain += ":te="+chainName.element();
	if ( !chainName.passed() )     selectChain += ":DTE";
	  
	if ( chainName.postcount() )   selectChain += ":post:"+chainName.post();
	  
#if 0
	std::cout << "\nTrigR3Mon::chain specification: " << chainName << "\t" << chainName.raw() << std::endl;
	std::cout << "\tchain: " << chainName.head()    << std::endl;
	std::cout << "\tkey:   " << chainName.tail()    << std::endl;
	std::cout << "\troi:   " << chainName.roi()     << std::endl;
	std::cout << "\tvtx:   " << chainName.vtx()     << std::endl;
	std::cout << "\tte:    " << chainName.element() << std::endl;
	std::cout << "\textra: " << chainName.extra()   << std::endl;
#endif
	
	int shifterChains = m_shifterChains;
	if ( chainName.vtx()=="" ) shifterChains = ( m_shifterChains>1 ? 1 : m_shifterChains );
	
	if ( m_sliceTag.find("Shifter")!=std::string::npos || m_shifter ) { 
	  /// shifter histograms 
	  if ( chainName.tail().find("_FTF")!=std::string::npos ) { 
	    /// FTF chain
	    if (   shifter_ftf>=shifterChains || 
		   ( shifter_ftf<shifterChains && chainName.vtx()!="" && chainName.vtx()==lastvtx ) ) {  
	      msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChain << " excluded - Shifter chain already definied^[[m" << endmsg;
	      continue;
	    }
	    shifter_ftf++;
	    lastvtx = chainName.vtx();
	  }
	  else if ( chainName.tail().find("_IDTrig")!=std::string::npos || chainName.tail().find("CosmicsN_EFID")!=std::string::npos ) { 
	    /// EFID chain
	    shifter_efid++;
	    if ( shifter_efid>shifterChains ) {
	      msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChain << " excluded - Shifter chain already definied^[[m" << endmsg;
	      continue;
	    }
	  }
	  else if ( chainName.tail().find("_EFID")!=std::string::npos ) { 
	    /// EFID chain
	    shifter_efid_run1++;
	    if ( shifter_efid_run1>shifterChains ) {
	      msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChain << " excluded - Shifter chain already definied^[[m" << endmsg;
	      continue;
	    }
	  }
	}
	
	if (  std::find(chains.begin(), chains.end(), selectChain) == chains.end() ) { // deduplicate
	  chains.push_back( selectChain );
	  monTools.push_back( &(*toolitr) );
	}
	
	//  	  msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << "^[[m" << endmsg;
	
	//	  std::cout << "^[[91;1m" << "Matching chain " << selectChains[iselected] << "^[[m" << std::endl;;
	  
      }
     
      
      toolitr++;
    }
	
    m_chainNames = chains;
      
    //      std::cout << "TrigR3Mon::book() chains " << m_chainNames.size() << std::endl;
      
    // tag and probe object creation                                                                                                                                                                            
    std::vector<std::string>& allchains = m_ntupleChainNames;
    
    for ( size_t i=0 ; i<m_chainNames.size() ; i++ ) {

      ChainString probe = m_chainNames[i] ;

      TagNProbe* tnp = 0;

      if ( probe.extra().find("probe")!=std::string::npos ) {

	std::string probe_key = probe.extra().erase( probe.extra().find("probe"), 5 ) ;

	for ( size_t j=0 ; j<allchains.size() ; j++ ) {

	  ChainString tag = allchains[j];

	  // tag chain must be the same as probe chain but with te=0 and extra=*_tag                                                                                                                            
	  if ( tag.head() != probe.head() ) continue;
	  if ( tag.element() == probe.element() ) continue;
	  if ( tag.extra().find("tag")==std::string::npos ) continue;

	  std::string tag_key = tag.extra().erase( tag.extra().find("tag"), 3 ) ;

	  if ( tag_key != probe_key ) continue;

	  // if matching tag found then initialise tag and probe object and store tag and probe chains in there                                                                                               
	  tnp = new TagNProbe() ;
	  tnp->tag(tag) ;
	  tnp->probe(probe) ;
	  
	  ATH_MSG_DEBUG( "Tag and probe pair found: " + tag + " : " + probe );
	  
	  break ;
       
	}
      }


      /// can only iuse R3 navigation now

      if ( m_tdt->getNavigationFormat() != "TriggerElement" ) { 

	msg(MSG::INFO)  << "configure analysis: " << m_chainNames[i] << endmsg; 

	AnalysisConfigR3_Tier0* analysis = new AnalysisConfigR3_Tier0( m_sliceTag, // m_chainNames[i],
								       m_chainNames[i], "", "",
								       m_chainNames[i], "", "",
								       &m_roiInfo,
								       filterTest, filterRef,
								       dR_matcher,
								       new AnalysisR3_Tier0( m_chainNames[i], m_pTCut, m_etaCut, m_d0Cut, m_z0Cut ),
								       tnp );

	analysis->setRunPurity(m_runPurity);
	analysis->setShifter(m_shifter);
	analysis->containTracks(m_containTracks);

	analysis->set_monTool( monTools[i] );

	m_sequences.push_back( analysis );

      }


      std::string highestPT_str = "";
      std::string vtxindex_str  = "";

      if ( m_useHighestPT ) { 
	highestPT_str = ": using highest PT only";
	m_sequences.back()->setUseHighestPT(true);
      }
      else m_sequences.back()->setUseHighestPT(false); /// not needed now, but can't do any harm

      if ( !(m_vtxIndex<0) )  {
	vtxindex_str = ": searching for vertex index ";
	m_sequences.back()->setVtxIndex(m_vtxIndex);
      }
	
      msg(MSG::DEBUG)   << " ----- creating analysis " << m_sequences.back()->name() << " : " << m_chainNames[i] << highestPT_str << vtxindex_str << " -----" << endmsg;
	
      m_sequences.back()->releaseData(m_releaseMetaData);

      /// don't filter cosmic chains on Roi
      /// - could be done with a global job option, but then if configuring some cosmic chains,
      ///   and non cosmic chains, then all would be tarred with the same brush  
      if ( m_chainNames[i].find("cosmic")!=std::string::npos || 
	   m_chainNames[i].find("Cosmic")!=std::string::npos )  m_sequences.back()->setFilterOnRoi(false);

    }
  }
      
  

  //  std::cout << "TrigR3Mon: sequences: " << m_sequences.size() << std::endl;
  
  //  if ( m_sequences.size() == 0 ) std::exit(0);
  
  if ( !m_fileopen && ( m_initialisePerRun || m_firstRun ) ) {
    m_fileopen = true;
    
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) {
      msg(MSG::VERBOSE) << " ----- booking for analysis " << m_sequences[i]->name() << " -----" << endmsg;
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);
      m_sequences[i]->book();
    }
    m_firstRun = false;
  }
  
  msg(MSG::DEBUG) << " configured " << m_sequences.size() << " sequences" << endmsg;
  
  msg(MSG::DEBUG) << " ----- exit book() ----- " << endmsg;
  return StatusCode::SUCCESS;
  
}





StatusCode TrigR3Mon::execute() {

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- enter fill() ----- " << endmsg;
   }

  std::vector<std::string> selectChains  = m_tdt->getListOfTriggers( "HLT_e.*" );

  msg(MSG::DEBUG) << " TDT selected chains " << selectChains.size() << endmsg;
  
  int passed_count = 0;
  
  /// print out all the configured chains if need be
  static bool _first = true;
  for ( unsigned i=0 ; i<selectChains.size() ; i++ ) {
    if ( _first ) ATH_MSG_DEBUG( "\tchain " << selectChains[i] << " from TDT" );
  }

#if 1
  if ( _first ) { 
    for ( size_t i=selectChains.size() ; i-- ; ) {
      if ( i>5 ) i=5;
      msg(MSG::INFO) << "^[[91;1m" << "configured chain " << selectChains[i] << "^[[m" << endmsg;
    }
  }
#endif

  _first = false;

  for ( unsigned i=0 ; i<selectChains.size() ; i++ ) {
    if ( m_tdt->isPassed(selectChains[i]) ) {
      passed_count++;
      ATH_MSG_DEBUG( "chain " << selectChains[i] << "\tpass: " << m_tdt->isPassed(selectChains[i]) << "\tprescale: " << m_tdt->getPrescale(selectChains[i]) );
     }
  }
  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 
    m_sequences[i]->execute();
  }

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- exit fill() ----- " << endmsg;
  }

  return StatusCode::SUCCESS;
}





// bool newEventsBlock, bool newLumiBlock, bool newRun are protected varibales
// correctly set before this is called
StatusCode TrigR3Mon::finalize() {

  // StatusCode TrigR3Mon::procHistograms() {

  msg(MSG::DEBUG) << " ----- enter proc() ----- " << endmsg;
  if ( m_initialisePerRun ) {
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();
    m_fileopen = false;
  }
  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endmsg;



  return StatusCode::SUCCESS;
}








