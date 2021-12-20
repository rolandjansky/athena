/**
 **     @file    TrigTestBase.cxx
 **
 **     @brief   implementation of a IHLTMonTool monitoring base class
 **
 **     @author  Benjamin Sowden (benjamin.sowden@cern.ch)
 **     @date    Oct 2014
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

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
  msg(MSG::INFO) << "TrigTestBase::TrigTestBase() compiled: " << __DATE__ << " " << __TIME__ << endmsg;

  declareProperty( "SliceTag",   m_sliceTag = "TIDAMonTool/" );

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
  
  msg(MSG::INFO) << "TrigTestBase::TrigTestBase() exiting " << gDirectory->GetName() << endmsg;


}



TrigTestBase::~TrigTestBase() {

  if ( m_fileopen ) for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();

  // msg(MSG::INFO) << "TrigTestBase::~TrigTestBase()" << endmsg;
  // for ( unsigned i=m_filters.size()     ; i-- ; ) delete m_filters[i];
  // for ( unsigned i=m_associators.size() ; i-- ; ) delete m_associators[i];

}



StatusCode TrigTestBase::init() {

  msg(MSG::DEBUG) << " ----- enter init() ----- " << endmsg;

  //  m_sliceTag = m_sliceTagArse;
  
  //  std::cout << "sliceTag: "     << m_sliceTag << std::endl; 
  //  std::cout << "sliceTagArse: " << m_sliceTagArse << std::endl; 

  msg(MSG::INFO) << "TrigTestBase::init() " << gDirectory->GetName() << " " << m_sliceTag << endmsg;
  //  std::cout << "TrigTestBase::init() " << gDirectory->GetName() << std::endl;

  /// NB: Do NOT create the sequences here - leave it until the book() method, since
  ///     we need to be automatically determine which chains to process, and so need
  ///     the TrigDecisionTool which is niot configured until we have an iov

  //  ATH_CHECK(ManagedMonitorToolBase::initialize());

  //  ATH_CHECK(IHLTMonTool::init());

#if 0
  std::cout << "TrigTestBase::name              = " << name()     << std::endl;
  std::cout << "TrigTestBase::SliceTag          = " << m_sliceTag << std::endl;
  std::cout << "TrigTestBAse::AnalysisConfig    = " << m_analysis_config << std::endl;
  std::cout << "TrigTestBase::Legacy            = " << m_legacy   << std::endl;
#endif

  msg(MSG::DEBUG) << " -----  exit init() ----- " << endmsg;

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

  msg(MSG::DEBUG) << " ----- enter book() ----- " << endmsg;

  msg(MSG::INFO) << "TrigTestBase::book() " << gDirectory->GetName() << endmsg;

  //  MMTB_DEPRECATED(duff)



#ifdef ManagedMonitorToolBase_Uses_API_201704
  bool newEventsBlock = newEventsBlockFlag();
  bool newLumiBlock   = newLumiBlockFlag();
  bool newRun         = newRunFlag();
#endif


  msg(MSG::DEBUG) << "TrigTestBase::book() buildNtuple " << m_buildNtuple
      << "\tNewEventBlock " << newEventsBlock
      << "\tNewLumiBlock "  << newLumiBlock
      << "\tNewRun "        << newRun  <<  endmsg;


  /// create sequences if need be ...

  if ( m_first ) {

    m_first = false;

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

    msg(MSG::INFO) << "^[[91;1m" << "AnalysisConfig " << m_analysis_config << "^[[m" << endmsg;

    msg(MSG::DEBUG) << "configuring chains: " << m_ntupleChainNames.size() << endmsg;

    /// keep counters of how many efid or ftf chains have been created
    /// for shifter histograms, only want m_shifterMaxChains of each
    int shifter_efid      = 0;
    int shifter_ftf       = 0;
    int shifter_efid_run1 = 0;

    std::string lastvtx = "";

    // if (m_analysis_config == "Tier0") {
    {
      std::vector<std::string> chains;
      // std::vector<ChainString> chains;
      chains.reserve( m_ntupleChainNames.size() );

      /// handle wildcard chain selection - but only the first time
      std::vector<std::string>::iterator chainitr = m_ntupleChainNames.begin();


      while ( chainitr!=m_ntupleChainNames.end() ) {
	
	/// get chain
        ChainString chainName = (*chainitr);

	msg(MSG::DEBUG) << "configuring chain: " << chainName.head() << "\t: " << chainName.tail() << endmsg;

	//	std::cout << "\tconfiguring chain: " << (*chainitr) << "\t" << chainName.head() << "\t: " << chainName.tail() << "\t:" << chainName.roi() << std::endl;


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
	  if ( chainName.vtx()!="" )     selectChain += ":vtx="+chainName.vtx();
	  if ( chainName.postcount() )   selectChain += ":post:"+chainName.post();
	  
	  if ( chainName.extra()!="" )   continue;
	  if ( chainName.element()!="" ) continue;
	  if ( chainName.roi()!="" )     continue;
	  //            if ( !chainName.passed() )     continue;
	  
    if (std::find(chains.begin(), chains.end(), selectChain) == chains.end()) { // deduplicate
  	  chains.push_back( selectChain );
    }

	}
	else { 
	  
	  /// check for configured chains only ...

	  if ( chainName.head().find("HLT_")==std::string::npos && 
	       chainName.head().find("EF_")==std::string::npos  && 
	       chainName.head().find("L2_")==std::string::npos ) { 
	    chainitr++;
	    continue;
	  }
	  
	  /// get matching chains

	  std::vector<std::string> selectChains;
	  selectChains.clear();

	  if ( chainName.head()!="" ) selectChains = m_tdt->getListOfTriggers( chainName.head() );
	  
	  if ( selectChains.size()==0 ) { 
	    msg(MSG::DEBUG) << "^[[91;1m" << "No chains matched\tchain input " << chainName.head() << "  :  " << chainName.tail() << "^[[m"<< endmsg;
	  }
	  
	  //	  std::cout << "^[[91;1m" << "\tChain count matched\tchain input " << chainName.head() << "  :  " << chainName.tail() << "^[[m"<< std::endl;

	  for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {
	    if ( chainName.tail()!="" )    selectChains[iselected] += ":key="+chainName.tail();
            if ( chainName.extra()!="" )   selectChains[iselected] += ":extra="+chainName.extra();
            if ( chainName.element()!="" ) selectChains[iselected] += ":te="+chainName.element();
	    if ( chainName.roi()!="" )     selectChains[iselected] += ":roi="+chainName.roi();
	    if ( chainName.vtx()!="" )     selectChains[iselected] += ":vtx="+chainName.vtx();
            if ( !chainName.passed() )     selectChains[iselected] += ":DTE"; 
	    //            if ( !chainName.passed() )     selectChains[iselected] += ";DTE";

	    if ( chainName.postcount() )     selectChains[iselected] += ":post:"+chainName.post();
	    
#if 0
	    std::cout << "\nTrigTestBase::chain specification: " << chainName << "\t" << chainName.raw() << std::endl;
	    std::cout << "\tchain: " << chainName.head()    << std::endl;
	    std::cout << "\tkey:   " << chainName.tail()    << std::endl;
	    std::cout << "\troi:   " << chainName.roi()     << std::endl;
	    std::cout << "\tvtx:   " << chainName.vtx()     << std::endl;
	    std::cout << "\tte:    " << chainName.element() << std::endl;
#endif

	    int shifterChains = m_shifterChains;
	    if ( chainName.vtx()=="" ) shifterChains = ( m_shifterChains>1 ? 1 : m_shifterChains );
	    
	    if ( m_sliceTag.find("Shifter")!=std::string::npos || m_shifter ) { 
	      /// shifter histograms 
	      if ( chainName.tail().find("_FTF")!=std::string::npos ) { 
		/// FTF chain
		if (   shifter_ftf>=shifterChains || 
		     ( shifter_ftf<shifterChains && chainName.vtx()!="" && chainName.vtx()==lastvtx ) ) {  
		  msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << " excluded - Shifter chain already definied^[[m" << endmsg;
		  continue;
		}
		shifter_ftf++;
		lastvtx = chainName.vtx();
	      }
	      else if ( chainName.tail().find("_IDTrig")!=std::string::npos || chainName.tail().find("CosmicsN_EFID")!=std::string::npos ) { 
		/// EFID chain
		shifter_efid++;
		if ( shifter_efid>shifterChains ) {
		  msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << " excluded - Shifter chain already definied^[[m" << endmsg;
		  continue;
		}
	      }
	      else if ( chainName.tail().find("_EFID")!=std::string::npos ) { 
		/// EFID chain
		shifter_efid_run1++;
		if ( shifter_efid_run1>shifterChains ) {
		  msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << " excluded - Shifter chain already definied^[[m" << endmsg;
		  continue;
		}
	      }
	    }
	    
            /// replace wildcard with actual matching chains ...
	    if (std::find(chains.begin(), chains.end(), selectChains[iselected]) == chains.end()) { 
	      chains.push_back( selectChains[iselected] );
	    }

            msg(MSG::DEBUG) << "^[[91;1m" << "Matching chain " << selectChains[iselected] << "^[[m" << endmsg;

	    //	    std::cout << "^[[91;1m" << "Matching chain " << selectChains[iselected] << "^[[m" << std::endl;;

	  }
	}	 
 
	++chainitr;
      }
	
      m_chainNames = chains;

      // reserving m_chainNames size for m_TnP_tools vector
      m_TnP_tools.reserve( m_chainNames.size() ) ;
      
      // tag and probe object creation
      for (unsigned i=0; i<m_chainNames.size(); ++i) {

	m_TnP_tools[i] = 0 ;
	ChainString probeChainName = m_chainNames[i] ;

	// probe can be the .head() so convert m_chainNames to a ChainString and search the .extra() specifically
	if ( probeChainName.extra().find("_probe")!=std::string::npos ) {
	  for ( unsigned j = 0; j<m_chainNames.size(); ++j) {
	    ChainString tagChainName = m_chainNames[j] ;
	    if (tagChainName == probeChainName) continue ; // skips if the same chain

	    // need to compare just the 'el1' part of of .extra() so create variables without '_probe/_tag' part
	    std::string probe_extra_key = probeChainName.extra().erase((size_t)(probeChainName.extra().find("_probe")), 6) ;
	    std::string tag_extra_key = "" ;
	    
	    if ( tagChainName.extra().find("_tag")==std::string::npos ) { continue ; } // skip if not a tag chain
	    tag_extra_key = tagChainName.extra().erase((size_t)(tagChainName.extra().find("_tag")), 4) ;

	    // tag chain must be the same as probe chain but with te=0 and extra=*_tag
	    if ( tagChainName.head() == probeChainName.head() 
		 && tagChainName.tail() == probeChainName.tail() 
		 && tagChainName.roi() == probeChainName.roi() 
		 && tagChainName.element() != probeChainName.element() 
		 && tagChainName.extra() != probeChainName.extra() 
		 && tag_extra_key == probe_extra_key ) {
	      // if matching tag found then initialise tag and probe object and store tag and probe chains in there
	      m_TnP_tools[i] = new TagNProbe() ;
	      m_TnP_tools[i]->tag(tagChainName) ;
	      m_TnP_tools[i]->probe(probeChainName) ;
	      ATH_MSG_DEBUG( "Tag and probe pair found" ); 
	      break ;
	    }
	  }
	}
	
	if ( m_tdt->getNavigationFormat() == "TriggerElement" ) {

	  AnalysisConfig_Tier0* analysis = new AnalysisConfig_Tier0( m_sliceTag, // m_chainNames[i],
								     m_chainNames[i], "", "",
								     m_chainNames[i], "", "",
								     &m_roiInfo,
								     filterTest, filterRef,
								     dR_matcher,
								     new Analysis_Tier0( m_chainNames[i], m_pTCut, m_etaCut, m_d0Cut, m_z0Cut ) );

	  analysis->setRunPurity(m_runPurity);
	  analysis->setShifter(m_shifter);
	  analysis->containTracks(m_containTracks);
	  	  
	  m_sequences.push_back( analysis );

	}
	else {
	  
	  // skip AnalysisConfigMT_Tier0 instance if it's a tag chain as we're not interested in this for tag and probe analysis
	  ChainString chainName( m_chainNames[i] ) ;
	  if ( chainName.extra().find("_tag")!=std::string::npos && m_TnP_tools[i] == 0 ) {
	    continue ;
	  }
	  
	  AnalysisConfigMT_Tier0* analysis = new AnalysisConfigMT_Tier0( m_sliceTag, // m_chainNames[i],
									 m_chainNames[i], "", "",
									 m_chainNames[i], "", "",
									 &m_roiInfo,
									 filterTest, filterRef,
									 dR_matcher,
									 new Analysis_Tier0( m_chainNames[i], m_pTCut, m_etaCut, m_d0Cut, m_z0Cut ),
									 m_TnP_tools[i]
									 );

	  analysis->setRunPurity(m_runPurity);
	  analysis->setShifter(m_shifter);
	  analysis->containTracks(m_containTracks);

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
      
  }

  //  std::cout << "TrigTestBase: sequences: " << m_sequences.size() << std::endl;

  //  if ( m_sequences.size() == 0 ) std::exit(0);

  if ( !m_fileopen && newRun && ( m_initialisePerRun || m_firstRun ) ) {
    m_fileopen = true;
    
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) {
      msg(MSG::VERBOSE) << " ----- booking for analysis " << m_sequences[i]->name() << " -----" << endmsg;
      m_sequences[i]->initialize(this, &m_tdt);
      m_sequences[i]->setGenericFlag(m_genericFlag);

      // skip booking of tag chain as we're not interested in this for tag and probe analysis
      ChainString chainName( m_sequences[i]->testChains() ) ;
      if ( chainName.extra().find("_tag")!=std::string::npos ) continue ;
      
      m_sequences[i]->book();
    }
    m_firstRun = false;
  }

  msg(MSG::DEBUG) << " configured " << m_sequences.size() << " sequences" << endmsg;
  
  msg(MSG::DEBUG) << " ----- exit book() ----- " << endmsg;
  return StatusCode::SUCCESS;
  
}
 




StatusCode TrigTestBase::fill() {

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- enter fill() ----- " << endmsg;
   }

  //  std::cout << "TrigTestBase::fill()      " << gDirectory->GetName() << "\tslice: " << m_sliceTag << std::endl;

  std::vector<std::string> selectChains  = m_tdt->getListOfTriggers( "HLT_.*" );

  msg(MSG::DEBUG) << " TDT selected chains " << selectChains.size() << endmsg;
  
  //  std::cout << " TDT selected chains " << selectChains.size() << std::endl;
  
  int passed_count = 0;
  
  /// print out all the configured chains if need be
  bool _first = true;
  for ( unsigned i=0 ; i<selectChains.size() ; i++ ) {
    if ( _first ) ATH_MSG_DEBUG( "\tchain " << selectChains[i] << " from TDT" );
  }
  _first = false;
  
  //  std::cout << "TrigTestBase::selectedChains size: " << selectChains.size() << std::endl;

  for ( unsigned i=0 ; i<selectChains.size() ; i++ ) {
    
    //    std::cout << "\t chain: " << selectChains[i] << "\tpassed: " << m_tdt->isPassed( selectChains[i] ) << std::endl;

    if ( m_tdt->isPassed(selectChains[i]) ) {
      passed_count++;
      ATH_MSG_DEBUG( "chain " << selectChains[i] << "\tpass: " << m_tdt->isPassed(selectChains[i]) << "\tprescale: " << m_tdt->getPrescale(selectChains[i]) );
     }
  }

  //  std::cout << "sequences::size: " << m_sequences.size() << std::endl;
  
  for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) { 

    //    std::cout << "execute sequence: " << m_sequences[i]->name() << std::endl;

    m_sequences[i]->execute();

  }

  if(msg().level() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << " ----- exit fill() ----- " << endmsg;
  }

  //  std::cout << "TrigTestBase::fill() exit " << gDirectory->GetName() << "\tslice: " << m_sliceTag << std::endl;


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

#ifdef ManagedMonitorToolBase_Uses_API_201704
  bool endOfRun       = endOfRunFlag();
#endif

  msg(MSG::DEBUG) << " ----- enter proc() ----- " << endmsg;
  if ( m_initialisePerRun && endOfRun ) {
    for ( unsigned i=0 ; i<m_sequences.size() ; i++ ) m_sequences[i]->finalize();
    m_fileopen = false;

    // deleting the instances of the initialised TnP_tool
    for ( unsigned i=0 ; i<m_TnP_tools.size() ; i++ ) {
      if ( m_TnP_tools[i] != 0 ) delete m_TnP_tools[i] ;
    }
  }
  msg(MSG::DEBUG) << " ====== exit proc() ====== " << endmsg;
  return StatusCode::SUCCESS;
}








