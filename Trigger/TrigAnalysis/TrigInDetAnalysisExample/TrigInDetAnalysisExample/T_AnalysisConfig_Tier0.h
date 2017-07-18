// emacs: this is -*- c++ -*-
//
//   @file    T_AnalysisConfig_Tier0.h
//
//            baseclass template so that we can use in different contexts 
//            in different ways in the monitoring 
//
//            NB: this will be configured to run *either* a standard 
//                analysis, or a "purity" analysis. If a purity analysis, 
//                the trigger tracks become the reference (with all the 
//                selection) and the offline or truth the "test" tracks
//                This would be a simple switch if the reference tracks
//                were in the RoI, but they're not se we need to move the 
//                RoI filtering to the test filter and *not* the reference 
//                filter grrrrrr  
//
// 
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//
//   $Id: T_AnalysisConfig_Tier0.h  Tue 16 May 2017 09:28:55 CEST sutt $

#ifndef TrigInDetAnalysisExample_T_AnalysisConfig_Tier0_H
#define TrigInDetAnalysisExample_T_AnalysisConfig_Tier0_H


#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"

#include "TrigInDetAnalysisExample/Analysis_Tier0.h"
#include "TrigInDetAnalysisExample/VtxAnalysis.h"
#include "TrigInDetAnalysisExample/ChainString.h"

#include "TTree.h"
#include "TFile.h"


// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"



#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"
#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/Filters.h"


#include "VxVertex/VxContainer.h"

#include "muonEvent/MuonContainer.h"

#include "egammaEvent/ElectronContainer.h"

#include "tauEvent/TauJetContainer.h"


#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDecisionTool/ExpertMethods.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// xAOD
// not actual XAOD, but this file safely includes
// the xAOD::TrackParticle header if it exists
#include "TrkParticleCreator/TrackParticleCreatorTool.h"







template<typename T>
void HighestPTOnly( std::vector<T*>& tracks ) { 

  if ( tracks.size()>1 ) {
    
    std::vector<T*> tmp_tracks; 
    
    int ih = 0;
    
    for ( unsigned i=1 ; i<tracks.size() ; i++ ) { 
      if ( std::fabs(tracks[i]->pT())>std::fabs(tracks[ih]->pT()) ) ih = i;
    }
    
    tmp_tracks.push_back( tracks[ih] );
    
    tracks = tmp_tracks;
  }
}






template<typename T>
void FilterPT( std::vector<T*>& tracks, double pt ) { 

  std::vector<T*> tmp_tracks; 
  
  tmp_tracks.reserve( tracks.size() );
  
  for ( unsigned i=0 ; i<tracks.size() ; i++ ) { 
    if ( std::fabs(tracks[i]->pT())>=pt ) tmp_tracks.push_back( tracks[i] );
  }

  if ( tmp_tracks.size()<tracks.size() ) tracks = tmp_tracks;

}





template<typename T>
class T_AnalysisConfig_Tier0 : public T_AnalysisConfig<T> {

public:

  // Full constructor: test/reference/selection
  // - analysisInstanceName: the name of the analysis chain being created
  // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
  // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
  // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
  // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used for RoI position too
  // - all standard operations are performed in loops over 0=test 1=reference 2=selection
  T_AnalysisConfig_Tier0(const std::string& analysisInstanceName,
			 const std::string& testChainName,      const std::string& testType,      const std::string& testKey,
			 const std::string& referenceChainName, const std::string& referenceType, const std::string& referenceKey,
			 TIDARoiDescriptor* roiInfo,
			 TrackFilter*     testFilter,  TrackFilter*     referenceFilter, 
			 TrackAssociator* associator,
			 TrackAnalysis*   analysis) :
    T_AnalysisConfig<T>( analysisInstanceName,
			 testChainName,      testType,      testKey,
			 referenceChainName, referenceType, referenceKey,
			 roiInfo,
			 testFilter, referenceFilter,
			 associator,
			 analysis ),
    m_useBeamCondSvc(false),
    m_doOffline(true),
    m_doMuons(false),
    m_doElectrons(false),
    m_doTaus(false),
    m_doBjets(false),
    m_hasTruthMap(false),
    m_NRois(0),
    m_NRefTracks(0),
    m_NTestTracks(0),
    m_runPurity(false),
    m_shifter(false),
    m_pTthreshold(0),
    m_first(true)
  {
    m_event = new TIDA::Event();
    m_chainNames.push_back(testChainName);
    
#if 0
    ChainString& chain = m_chainNames.back(); 

    std::cout << "\nT_AnalysisConfig_Tier0::name:                " << name() << "\t" << this << std::endl;
    std::cout <<  "T_AnalysisConfig_Tier0::chain specification: " << testChainName << " -> " << chain << "\t" << chain.raw() << std::endl;
    std::cout << "\tchain: " << chain.head()    << std::endl;
    std::cout << "\tkey:   " << chain.tail()    << std::endl;
    std::cout << "\troi:   " << chain.roi()     << std::endl;
    std::cout << "\tvtx:   " << chain.vtx()     << std::endl;
    std::cout << "\tte:    " << chain.element() << std::endl;

    std::cout << "\tpost:  " << chain.post()          << std::endl; 
    std::cout << "\tpt:    " << chain.postvalue("pt") << std::endl;
#endif
    
    m_testType = testType;
  }

  virtual ~T_AnalysisConfig_Tier0() { delete m_event; }

  void setRunPurity( bool b ) { m_runPurity=b; }

  void setShifter( bool b )    { m_shifter=b; }

  void useBeamCondSvc( bool b ) { m_useBeamCondSvc = b; }

public:

  Analysis_Tier0* _analysis;

  /// doesn't work yet
  // void verbose( std::ostream& s ) {
  //   if( m_provider->msg().level() <= MSG::VERBOSE )  m_provider->msg(MSG::VERBOSE) << s << endmsg;
  // }

protected:

  using T_AnalysisConfig<T>::m_provider;
  using T_AnalysisConfig<T>::m_tdt;
  using T_AnalysisConfig<T>::m_mcTruth;

  using T_AnalysisConfig<T>::name;
  using T_AnalysisConfig<T>::m_analysis;

  using T_AnalysisConfig<T>::m_selectorTest;
  using T_AnalysisConfig<T>::m_selectorRef;
  using T_AnalysisConfig<T>::m_associator;
  using T_AnalysisConfig<T>::m_filters;

  // using  T_AnalysisConfig<T>::selectTracks<TrigInDetTrackCollection>;

  // using T_AnalysisConfig<T>::selectTracks;

  virtual void loop() {

    if( m_provider->msg().level() <= MSG::VERBOSE) {
      m_provider->msg(MSG::VERBOSE) <<  "AnalysisConfig_Tier0::loop() for " << T_AnalysisConfig<T>::m_analysisInstanceName <<  endmsg;
    }

    // get (offline) beam position
    double xbeam = 0;
    double ybeam = 0;
    if ( m_iBeamCondSvc ) {

#     ifdef EIGEN_GEOMETRY_MODULE_H
      const Amg::Vector3D& vertex = m_iBeamCondSvc->beamPos();
      xbeam = vertex[0];
      ybeam = vertex[1];
#     else
      HepGeom::Point3D<double> vertex = m_iBeamCondSvc->beamPos();
      xbeam = vertex.x();
      ybeam = vertex.y();
#     endif

      if(m_provider->msg().level() <= MSG::VERBOSE) {
        m_provider->msg(MSG::VERBOSE) << " using beam position\tx=" << xbeam << "\ty=" << ybeam << endmsg;
      }
    }


    if ( m_first ) {

      m_first = false;
      
      m_provider->msg(MSG::VERBOSE) << " using beam position\tx=" << xbeam << "\ty=" << ybeam << endmsg;

      if(m_provider->msg().level() <= MSG::VERBOSE) {
	
	std::vector<std::string> configuredChains  = (*(m_tdt))->getListOfTriggers("L2_.*, EF_.*, HLT_.*");
	
        for ( unsigned i=0 ; i<configuredChains.size() ; i++ ) {
	  m_provider->msg(MSG::VERBOSE)  << "Chain " << configuredChains[i]  << endmsg;
        }

      }


      std::vector<ChainString>::iterator chainitr = m_chainNames.begin();

      std::vector<ChainString> chains;

      /// handle wildcard chain selection - but only the first time
      /// NB: also check all other chains as well - only set up an
      ///     analysis for configured chains
      while ( chainitr!=m_chainNames.end() ) {

        /// get chain
        ChainString& chainName = (*chainitr);

        m_provider->msg(MSG::INFO) << "process chain " << chainName << endmsg;

        /// get matching chains
        std::vector<std::string> selectChains  = (*(m_tdt))->getListOfTriggers( chainName.head() );

	
        for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {

          if ( chainName.tail()!="" )    selectChains[iselected] += ":key="+chainName.tail();
          if ( chainName.extra()!="" )   selectChains[iselected] += ":index="+chainName.extra();
          if ( chainName.element()!="" ) selectChains[iselected] += ":te="+chainName.element();
          if ( chainName.roi()!="" )     selectChains[iselected] += ":roi="+chainName.roi();
          if ( chainName.vtx()!="" )     selectChains[iselected] += ":vtx="+chainName.vtx();
          if ( !chainName.passed() )     selectChains[iselected] += ";DTE";
	  if ( chainName.postcount() )   selectChains[iselected] += ":post:"+chainName.post();

#if 0
	  std::cout << "sorting:: chain specification: " << chainName << "\traw:" << chainName.raw() << std::endl;
	  std::cout << "\tchain: " << chainName.head()    << std::endl;
	  std::cout << "\tkey:   " << chainName.tail()    << std::endl;
	  std::cout << "\tind:   " << chainName.extra()   << std::endl;
	  std::cout << "\troi:   " << chainName.roi()     << std::endl;
	  std::cout << "\tvtx:   " << chainName.vtx()     << std::endl;
	  std::cout << "\tte:    " << chainName.element() << std::endl;
#endif

          /// replace wildcard with actual matching chains ...
          chains.push_back( ChainString(selectChains[iselected]) );

          if(m_provider->msg().level() <= MSG::VERBOSE) {
            m_provider->msg(MSG::VERBOSE) << "Matching chain " << selectChains[iselected] << " (" << chainName.head() << ")" << endmsg;
	  }

        }

        chainitr++;
      }

      // m_chainNames.insert( m_chainNames.end(), chains.begin(), chains.end() );
      m_chainNames = chains;

      for ( unsigned ic=0 ; ic<m_chainNames.size() ; ic++ ) m_provider->msg(MSG::VERBOSE) << "Analyse chain " << m_chainNames[ic] << endmsg;

    } /// end of first event setup


    /// all this should perhaps be class variables
    Filter_True filter;

    Filter_etaPT     filter_etaPT(5,200);
    Filter_Combined filter_truth( &filter_etaPT,   &filter_etaPT);

    /// will need to add a vertex filter at some point probably
    // Filter_Combined filterRef (&filter_offline, &filter_vertex);

    int iRefFilter  = 1;
    int iTestFilter = 0;

    if ( m_runPurity ) { 
      iRefFilter  = 0;
      iTestFilter = 1;
    }
    
    Filter_Combined filterRef(   m_filters[iRefFilter][0],  &filter );
    Filter_Combined filterTest(  m_filters[iTestFilter][0], &filter );

    TrigTrackSelector selectorTruth( &filter_truth );
    TrigTrackSelector selectorRef( &filterRef );
    m_selectorRef = &selectorRef;
    TrigTrackSelector selectorTest( &filterTest );
    m_selectorTest = &selectorTest;


    m_selectorRef->setBeamline(  xbeam, ybeam );
  
    //   m_selectorRef->setBeamline(  -0.693, -0.617 );

    /// now start everything going for this event properly ...

    // clear the ntuple TIDA::Event class
    m_event->clear();

    /// (obviously) get the event info
#ifndef  XAODTRACKING_TRACKPARTICLE_H
    const EventInfo* pEventInfo;
#else
    const xAOD::EventInfo* pEventInfo;
#endif
    unsigned           run_number        = 0;
    unsigned long long event_number      = 0;
    unsigned           lumi_block        = 0;
    unsigned           bunch_crossing_id = 0;
    unsigned           time_stamp        = 0;
    double             mu_val            = 0;

    if ( m_provider->evtStore()->retrieve(pEventInfo).isFailure() ) {
      m_provider->msg(MSG::WARNING) << "Failed to get EventInfo " << endmsg;
    } else {

#ifndef  XAODTRACKING_TRACKPARTICLE_H
      run_number        = pEventInfo->event_ID()->run_number();
      event_number      = pEventInfo->event_ID()->event_number();
      lumi_block        = pEventInfo->event_ID()->lumi_block();
      time_stamp        = pEventInfo->event_ID()->time_stamp();
      bunch_crossing_id = pEventInfo->event_ID()->bunch_crossing_id();
      mu_val            = pEventInfo->averageInteractionsPerCrossing();
#else
      run_number        = pEventInfo->runNumber();
      event_number      = pEventInfo->eventNumber();
      lumi_block        = pEventInfo->lumiBlock();
      time_stamp        = pEventInfo->timeStamp();
      bunch_crossing_id = pEventInfo->bcid();
      mu_val            = pEventInfo->averageInteractionsPerCrossing();
#endif
    }
  
    if(m_provider->msg().level() <= MSG::VERBOSE){
      m_provider->msg(MSG::VERBOSE) << "run "     << run_number
                                    << "\tevent " << event_number
                                    << "\tlb "    << lumi_block << endmsg;
    }
    
    //      m_provider->msg(MSG::INFO) << "run "     << run_number
    //				 << "\tevent " << event_number
    //				 << "\tlb "    << lumi_block << endmsg;

    // std::cout << "run "     << run_number  << "\tevent " << event_number  << "\tlb "    << lumi_block << std::endl;


    // clear the ntuple TIDA::Event class
    m_event->clear();

    m_event->run_number(run_number);
    m_event->event_number(event_number);
    m_event->lumi_block(lumi_block);
    m_event->time_stamp(time_stamp);
    m_event->bunch_crossing_id(bunch_crossing_id);
    m_event->mu(mu_val);

    /// first check whether the chains have actually run, otherwise there's no point
    /// doing anything

    bool analyse = false;
  
    // Check HLTResult

    for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {
      const std::string& chainname = m_chainNames[ichain].head();

      


      //Only for trigger chains
      if ( chainname.find("L2")  == std::string::npos &&
	   chainname.find("EF")  == std::string::npos &&
	   chainname.find("HLT") == std::string::npos ) continue;
      
      if ( m_provider->msg().level() <= MSG::DEBUG ) {
        m_provider->msg(MSG::DEBUG) << "Chain "  << chainname
                                    << "\tpass " << (*m_tdt)->isPassed(chainname)
                                    << "\tpres " << (*m_tdt)->getPrescale(chainname) << endmsg;
      }

      //      std::cout << "Chain "  << chainname << "\tpass " << (*m_tdt)->isPassed(chainname)
      //  		<< "\tpres " << (*m_tdt)->getPrescale(chainname) << std::endl;

      
      if ( (*(m_tdt))->isPassed(chainname) || (*(m_tdt))->getPrescale(chainname) ) analyse = true;
      
    }
    

    if ( (*m_tdt)->ExperimentalAndExpertMethods()->isHLTTruncated() ) {
      m_provider->msg(MSG::WARNING) << "HLTResult truncated, skipping event" << endmsg;
      return;
    }
    
    if ( !this->m_keepAllEvents && !analyse ) {
      //     m_provider->msg(MSG::VERBOSE) << "No chains passed unprescaled - not processing this event" << endmsg;
      if(m_provider->msg().level() <= MSG::VERBOSE)
        m_provider->msg(MSG::VERBOSE) << "No chains passed unprescaled - not processing this event" << endmsg;
      return;
    }

    /// for Monte Carlo get the truth particles if requested to do so

    selectorTruth.clear();

    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "MC Truth flag " << m_mcTruth << endmsg;

    const TrigInDetTrackTruthMap* truthMap = 0;

    if ( m_mcTruth ) {
      if(m_provider->msg().level() <= MSG::VERBOSE ) m_provider->msg(MSG::VERBOSE) << "getting Truth" << endmsg;

      if ( m_provider->evtStore()->retrieve(truthMap, "TrigInDetTrackTruthMap").isFailure()) {
        if(m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE) << "TrigInDetTrackTruthMap not found" << endmsg;
        m_hasTruthMap = false;
      }
      else {
        if(m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE) << "TrigInDetTrackTruthMap found" << endmsg;
        m_hasTruthMap = true;
      }
    }
    

    /// get the offline vertices into our structure

    std::vector<TIDA::Vertex> vertices;
    std::vector<TIDA::Vertex> vertices_rec;

    std::vector<double> refbeamspot;
    std::vector<double> testbeamspot;



#ifndef XAODTRACKING_TRACKPARTICLE_H

    const VxContainer* primaryVtxCollection;

    if ( m_doOffline ) {
      if ( m_provider->evtStore()->template contains<VxContainer>("VxPrimaryCandidate") ) {
        if ( m_provider->evtStore()->retrieve(primaryVtxCollection, "VxPrimaryCandidate").isFailure()) {
          if (m_provider->msg().level() <= MSG::WARNING)
            m_provider->msg(MSG::WARNING) << "Primary vertex container not found" << endmsg;
        }
        else {
          VxContainer::const_iterator vtxitr = primaryVtxCollection->begin();
          for ( ; vtxitr != primaryVtxCollection->end(); ++vtxitr) {
            if ( (*vtxitr)->vxTrackAtVertex()->size()>0 ) {
              vertices.push_back( TIDA::Vertex( (*vtxitr)->recVertex().position().x(),
						(*vtxitr)->recVertex().position().y(),
						(*vtxitr)->recVertex().position().z(),
						0,0,0,
						(*vtxitr)->vxTrackAtVertex()->size() ) );
            }
          }
        }
	
        // filter_vertex.setVertex(vertices);
      }
    }

#else

    //std::vector<TIDA::Vertex> vertices;
    
    m_provider->msg(MSG::VERBOSE) << "fetching AOD Primary vertex container" << endmsg;

    const xAOD::VertexContainer* xaodVtxCollection = 0;

    if ( m_provider->evtStore()->retrieve( xaodVtxCollection, "PrimaryVertices" ).isFailure()) {
      m_provider->msg(MSG::WARNING) << "xAOD Primary vertex container not found with key " << "PrimaryVertices" <<  endmsg;
    }
    
    if ( xaodVtxCollection!=0 ) { 
        
      m_provider->msg(MSG::VERBOSE) << "xAOD Primary vertex container " << xaodVtxCollection->size() <<  " entries" << endmsg;

      xAOD::VertexContainer::const_iterator vtxitr = xaodVtxCollection->begin();
      for ( ; vtxitr != xaodVtxCollection->end(); vtxitr++ ) {
	if ( (*vtxitr)->nTrackParticles()>0 && (*vtxitr)->vertexType()!=0 ) {
	  vertices.push_back( TIDA::Vertex( (*vtxitr)->x(),
					    (*vtxitr)->y(),
					    (*vtxitr)->z(),
					    /// variances
					    (*vtxitr)->covariancePosition()(Trk::x,Trk::x),
					    (*vtxitr)->covariancePosition()(Trk::y,Trk::y),
					    (*vtxitr)->covariancePosition()(Trk::z,Trk::z),
					    (*vtxitr)->nTrackParticles(),
					    /// quality
					    (*vtxitr)->chiSquared(),
					    (*vtxitr)->numberDoF() ) );
	}
      }
    }


#endif


    //    std::cout << "here " << __LINE__ << std::endl;

    /// add the truth particles if needed

    if ( m_mcTruth ) {
      m_event->addChain( "Truth" );
      m_event->back().addRoi(TIDARoiDescriptor());
      m_event->back().back().addTracks(selectorTruth.tracks());
    }

    /// now add the vertices

    if ( m_doOffline ) {
      for ( unsigned i=0 ; i<vertices.size() ; i++ )  {
        if(m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE) << "vertex " << i << " " << vertices[i] << endmsg;
        m_event->addVertex(vertices[i]);
      }
    }

    /// now add the offline tracks and reco objects

    // int Noff = 0;
    std::vector<TIDA::Track*> offline_tracks;
    std::vector<TIDA::Track*> electron_tracks;
    std::vector<TIDA::Track*> muon_tracks;

    std::vector<TIDA::Track*> ref_tracks;
    std::vector<TIDA::Track*> test_tracks;

    offline_tracks.clear();
    electron_tracks.clear();
    muon_tracks.clear();

    ref_tracks.clear();
    test_tracks.clear();

    /// now loop over all relevant chains to get the trigger tracks...
    for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {

      //      std::cout << "\tchain " << m_chainNames[ichain] << std::endl;

      test_tracks.clear();

      /// create chains for ntpl

      // std::string& chainname = chains[ichain];
      const std::string& chainname = m_chainNames[ichain].head();
      const std::string&       key = m_chainNames[ichain].tail();
      const std::string&  vtx_name = m_chainNames[ichain].vtx();
      const std::string&  roi_name = m_chainNames[ichain].roi();
      const std::string&   te_name = m_chainNames[ichain].element();

      m_pTthreshold = 0;  /// why does this need to be a class variable ???

      if ( m_chainNames[ichain].postcount() ) { 
        std::string ptvalue = m_chainNames[ichain].postvalue("pt");
	if ( ptvalue!="" ) m_pTthreshold = std::stod(ptvalue);
      }

      //      std::cout << "\tchain " << m_chainNames[ichain] << "\tchainname " << chainname << "\tvtx " << vtx_name << "\troi " << roi_name << std::endl;

      unsigned _decisiontype = TrigDefs::Physics;
      unsigned  decisiontype;
     
      if ( this->requireDecision() ) _decisiontype =  TrigDefs::requireDecision;
      
      
      if ( m_chainNames[ichain].passed() ) decisiontype = _decisiontype;
      else                                 decisiontype = TrigDefs::alsoDeactivateTEs;

      //      if ( decisiontype==TrigDefs::requireDecision ) std::cout << "\tSUTT TrigDefs::requireDecision " << decisiontype << std::endl;;
      //      if ( decisiontype==TrigDefs::Physics )         std::cout << "\tSUTT TrigDefs::Physics "         << decisiontype << std::endl;;


      /// and the index of the collection (if any)
      const std::string& key_index_string = m_chainNames[ichain].extra();
      unsigned key_index = 0;
      if ( key_index_string!="" ) key_index = std::atoi( key_index_string.c_str() );

      if(m_provider->msg().level() <= MSG::VERBOSE){
        m_provider->msg(MSG::VERBOSE) << "status for chain " << chainname
                                      << "\tpass "           << (*m_tdt)->isPassed(chainname)
                                      << "\tprescale "       << (*m_tdt)->getPrescale(chainname) << endmsg;

        m_provider->msg(MSG::VERBOSE) << "fetching features for chain " << chainname << endmsg;
      }

      if(m_provider->msg().level() <= MSG::VERBOSE){
        m_provider->msg(MSG::VERBOSE) << chainname << "\tpassed: " << (*m_tdt)->isPassed( chainname ) << endmsg;
      }

      //      std::cout << "\tstatus for chain " << chainname
      //		<< "\tpass "           << (*m_tdt)->isPassed( chainname )
      //		<< "\tpassdt "         << (*m_tdt)->isPassed( chainname, decisiontype )
      //		<< "\tprescale "       << (*m_tdt)->getPrescale( chainname ) << std::endl;
	

      //      m_provider->msg(MSG::INFO) << chainname << "\tpassed: " << (*m_tdt)->isPassed( chainname ) << "\t" << m_chainNames[ichain] << "\trun " << run_number << "\tevent " << event_number << endmsg;


      if ( !this->m_keepAllEvents && !(*m_tdt)->isPassed( chainname, decisiontype ) ) continue;

      //      std::cout << "\tprocessing " << std::endl; 

      /// Get chain combinations and loop on them
      /// - loop made on chain selected as the one steering RoI creation
      // Trig::FeatureContainer f = (*m_tdt)->features( chainname, TrigDefs::alsoDeactivateTEs);



      Trig::FeatureContainer f = (*m_tdt)->features( chainname, decisiontype );
      Trig::FeatureContainer::combination_const_iterator c(f.getCombinations().begin());
      Trig::FeatureContainer::combination_const_iterator cEnd(f.getCombinations().end());


      if ( c==cEnd ) {
        if(m_provider->msg().level() <= MSG::VERBOSE){
          m_provider->msg(MSG::VERBOSE) << "No combinations: skipping this chain " << chainname << endmsg;
        }
        continue;
      }

      if(m_provider->msg().level() <= MSG::VERBOSE) {
	m_provider->msg(MSG::VERBOSE) << "combinations for chain " << chainname << " " << (cEnd-c) << endmsg;
      }

      ChainString& chainConfig = m_chainNames[ichain];

      std::string chainName = chainConfig.head();


      m_event->addChain( chainConfig );

      TIDA::Chain& chain = m_event->back();
    
      unsigned icomb = 0;

      for( ; c!=cEnd ; ++c ) {

	icomb++;

        //   now add rois to this ntuple chain

        // Get seeding RoI
        // std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = c->get<TrigRoiDescriptor>("initialRoI", TrigDefs::alsoDeactivateTEs);
        // std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = c->get<TrigRoiDescriptor>("forID", TrigDefs::alsoDeactivateTEs);

        std::vector< Trig::Feature<TrigRoiDescriptor> > initRois;
	
        std::string roi_key = m_chainNames[ichain].roi();

	if ( roi_key=="SuperRoi" && icomb>1 ) continue;
	
	if ( roi_key!="" ) { 
	  initRois = c->get<TrigRoiDescriptor>(roi_key, decisiontype );
	}
	else { 
	  initRois = c->get<TrigRoiDescriptor>("forID", decisiontype ); 
	  if ( initRois.empty() ) initRois = c->get<TrigRoiDescriptor>("", decisiontype ); 
	  if ( initRois.empty() ) initRois = c->get<TrigRoiDescriptor>("initialRoI", decisiontype );
	}

	//	std::cout << "initRois.size() " << initRois.size() << std::endl;

	if ( initRois.empty() ) continue;


	//	for ( unsigned ir=0 ; ir<initRois.size() ; ir++ ) { 
	//	  std::cout << "\t" << ir << "\t" << *initRois[ir].cptr() << std::endl;
	//	}
	
        // Skip chains seeded by multiple RoIs: not yet implemented
        if(initRois.size()>1 && roi_key!="SuperRoi" ) {
          if(m_provider->msg().level() <= MSG::VERBOSE)
            m_provider->msg(MSG::VERBOSE) << " More than one initial RoI found for seeded chain " << chainname << ": not yet supported" << endmsg;
          continue;
        }

        TIDARoiDescriptor* roiInfo = 0;

        if( !initRois.empty() ) {
          const TrigRoiDescriptor* roid = initRois[0].cptr();

          if(m_provider->msg().level() <= MSG::VERBOSE)
            m_provider->msg(MSG::VERBOSE) << " RoI descriptor for seeded chain " << chainname << " " << *roid << endmsg;

          roiInfo = new TIDARoiDescriptor(TIDARoiDescriptorBuilder(*roid));
          //  roiInfo->etaHalfWidth(m_roiInfo->etaHalfWidth());
          //  roiInfo->phiHalfWidth(m_roiInfo->phiHalfWidth());
          //  roiInfo->etaHalfWidth(roid->etaHalfWidth());
          //  roiInfo->phiHalfWidth(roid->phiHalfWidth());
          //  roiInfo->zedHalfWidth(roid->zedHalfWidth());

          if(m_provider->msg().level() <= MSG::VERBOSE)
            m_provider->msg(MSG::VERBOSE) << "using chain roi " << *roid << endmsg;

        }
        else {
          roiInfo = new TIDARoiDescriptor();
          //  roiInfo->etaHalfWidth(5);
          //  roiInfo->phiHalfWidth(M_PI);
          //  roiInfo->zedHalfWidth(m_roiInfo->zedHalfWidth());

          if(m_provider->msg().level() <= MSG::WARNING)
            m_provider->msg(MSG::WARNING) << "roi not found" <<  endmsg;

        }

        if(m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE) << *roiInfo << endmsg;

        m_selectorTest->clear();

        m_provider->msg(MSG::VERBOSE) << "Searching for collection " << key << endmsg;
	// std::cout << "Searching for collection " << key << std::endl;

        /// HLT and EF-like EDM
        if ( key.find("InDetTrigParticleCreation")!=std::string::npos ||
	     key.find("_IDTrig")!=std::string::npos ||
	     key.find("_EFID")!=std::string::npos ||
             chainName.find("EF_")!=std::string::npos ||
             chainName.find("HLT_")!=std::string::npos ) {
#         ifdef XAODTRACKING_TRACKPARTICLE_H
          if      ( this->template selectTracks<xAOD::TrackParticleContainer>( m_selectorTest, c, key ) ) testbeamspot = this->template getBeamspot<xAOD::TrackParticleContainer>( c, key );
	  else if ( this->template selectTracks<Rec::TrackParticleContainer>( m_selectorTest, c, key ) );
#         else
	  if ( this->template selectTracks<Rec::TrackParticleContainer>( m_selectorTest, c, key ) );
#         endif
          else if ( this->template selectTracks<TrackCollection>( m_selectorTest, c, key ) );
          else if ( this->template selectTracks<TrigInDetTrackCollection>( m_selectorTest, c, truthMap, key, key_index ) );
	  else { 
	    //m_provider->msg(MSG::WARNING) << "No track collection " << key << " found"  << endmsg;
	  }
        }
        else {
          /// L2 track EDM
          if ( chainName.find("L2_")!=std::string::npos ) {
            if      ( this->template selectTracks<TrigInDetTrackCollection>( m_selectorTest, c, truthMap, key, key_index ) );
            else if ( this->template selectTracks<Rec::TrackParticleContainer>( m_selectorTest, c, key ) );
            else if ( this->template selectTracks<TrackCollection>( m_selectorTest, c, key ) );
#           ifdef XAODTRACKING_TRACKPARTICLE_H
            else if ( this->template selectTracks<xAOD::TrackParticleContainer>( m_selectorTest, c, key ) ) testbeamspot = this->template getBeamspot<xAOD::TrackParticleContainer>( c, key );
#           endif
            else m_provider->msg(MSG::WARNING) << "No track collection " << key << " found"  << endmsg;
          }
        }
      

        const std::vector<TIDA::Track*>& testtracks = m_selectorTest->tracks();

        m_provider->msg(MSG::VERBOSE) << "test tracks.size() " << testtracks.size() << endmsg;
	// std::cout << "test tracks.size() " << testtracks.size() << std::endl;

	//	std::cout  << "\ttest tracks.size() " << testtracks.size() << std::endl;

        if(m_provider->msg().level() <= MSG::VERBOSE){
          m_provider->msg(MSG::VERBOSE) << "test tracks.size() " << testtracks.size() << endmsg;
          for ( int ii=testtracks.size() ; ii-- ; ) {
            m_provider->msg(MSG::VERBOSE) << "  test track " << ii << " " << *testtracks[ii] << endmsg;
            //test_tracks.push_back(testtracks.at(ii));
          }
        }
      
        chain.addRoi( *roiInfo );

        chain.back().addTracks(testtracks);

        if ( roiInfo ) delete roiInfo;

	/// get vertex information 

	/// now also add xAOD vertices

	//	std::cout << "vertex " << vtx_name << "\tchain " << chainName << "\tconfig " << chainConfig << std::endl;

	if ( vtx_name!="" ) { 

	  m_provider->msg(MSG::VERBOSE) << "\tFetch xAOD::VertexContainer for chain " << chainConfig << " with key " << vtx_name << endmsg;

	  std::vector< Trig::Feature<xAOD::VertexContainer> > xaodtrigvertices = c->get<xAOD::VertexContainer>(vtx_name);
	  
	  if ( xaodtrigvertices.empty() ) { 
	    m_provider->msg(MSG::WARNING) << "\tNo xAOD::VertexContainer for chain " << chainConfig << " for key " << vtx_name << endmsg;
	  }
	  else {
	    
	    m_provider->msg(MSG::VERBOSE) << "\txAOD::VertexContainer found with size  " << xaodtrigvertices.size() << "\t" << vtx_name << endmsg;
	    
	    for (  unsigned iv=0  ;  iv<xaodtrigvertices.size()  ;  iv++ ) {
	      
	      const xAOD::VertexContainer* vert = xaodtrigvertices[iv].cptr();
	      
	      m_provider->msg(MSG::VERBOSE) << "\t" << iv << "  xAOD VxContainer for " << chainConfig << " " << vert << " key " << vtx_name << endmsg;
	      
	      xAOD::VertexContainer::const_iterator vtxitr = vert->begin();
	      
	      for ( ; vtxitr != vert->end(); ++vtxitr) {
		/// leave this code commented so that we have a record of the change - as soon as we can 
		/// fix the missing track multiplicity from the vertex this will need to go back  
		//  if ( ( (*vtxitr)->nTrackParticles()>0 && (*vtxitr)->vertexType()!=0 ) || vtx_name=="EFHistoPrmVtx" ) {
		if ( (*vtxitr)->vertexType()!=0 || vtx_name=="EFHistoPrmVtx" ) {
		  chain.back().addVertex( TIDA::Vertex( (*vtxitr)->x(),
							(*vtxitr)->y(),
							(*vtxitr)->z(),
							/// variances
							(*vtxitr)->covariancePosition()(Trk::x,Trk::x),
							(*vtxitr)->covariancePosition()(Trk::y,Trk::y),
							(*vtxitr)->covariancePosition()(Trk::z,Trk::z),
							(*vtxitr)->nTrackParticles(),
							/// quality
							(*vtxitr)->chiSquared(),
							(*vtxitr)->numberDoF() ) );
		  
		}
	      }
	    }
	    
	  }

	}  /// retrieve online vertices

      }

      if(m_provider->msg().level() <= MSG::VERBOSE) {
        m_provider->msg(MSG::VERBOSE) << "event: " << *m_event << endmsg;
      }


      /// now loop over the rois (again) 

      for ( unsigned  iroi=0 ; iroi<chain.size() ; iroi++ ) {

        m_selectorRef->clear();

	if ( this->filterOnRoi() ) filterRef.setRoi( &chain.rois().at(iroi).roi() );
	else                       filterRef.setRoi( 0 );

        test_tracks.clear();


	/// This is nonsense and needs restructuring - why is the truth and offline selection 
	/// done within this RoI loop? It means the complete offline and truth tracks will be 
	/// retrieved for every RoI ! really we should have the structure 
	///   
	///   - check_a_trigger_chain_has_passed
	///   - get_offline_or_truth_particles
	///   - loop_over_rois
	///     - get_trigger_tracks
	///     - filter_offline_or_truth_reference
	///     - match_tracks
	///     - call_analyis_routine
	///
	/// will leave as it is for the time being

        if(m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE) << "MC Truth flag " << m_mcTruth << endmsg;

        bool foundTruth = false;

        if ( !m_doOffline && m_mcTruth ) {

	  if ( this->filterOnRoi() )  filter_truth.setRoi( &chain.rois().at(iroi).roi() );
	  else                        filter_truth.setRoi( 0 ); // don't filter on RoI unless needed  

          selectorTruth.clear();

          if(m_provider->msg().level() <= MSG::VERBOSE)
            m_provider->msg(MSG::VERBOSE) << "getting Truth" << endmsg;

          if (m_provider->evtStore()->template contains<TruthParticleContainer>("INav4MomTruthEvent")) {
            //ESD
            this->template selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );
            foundTruth = true;
          }
          else if (m_provider->evtStore()->template contains<TruthParticleContainer>("SpclMC")) {
            /// AOD
            this->template selectTracks<TruthParticleContainer>( &selectorTruth, "SpclMC");
            foundTruth = true;
          }
          else if (m_provider->evtStore()->template contains<TruthParticleContainer>("")) {
            /// anything else?
            this->template selectTracks<TruthParticleContainer>( &selectorTruth, "");
            foundTruth = true;
          }
          else
            if(m_provider->msg().level() <= MSG::VERBOSE)
              m_provider->msg(MSG::VERBOSE) << "Truth not found - none whatsoever!" << endmsg;
        }

      
        if ( !m_doOffline && m_mcTruth && !foundTruth ) {
          if(m_provider->msg().level() <= MSG::VERBOSE)
            m_provider->msg(MSG::VERBOSE) << "getting Truth" << endmsg;

          /// selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );

          const DataHandle<McEventCollection> mcevent;

          /// now as a check go through the GenEvent collection

          std::string keys[4] = { "GEN_AOD", "TruthEvent", "", "G4Truth" };

          std::string key = "";

          bool foundcollection = false;

          for ( int ik=0 ; ik<4 ; ik++ ) {
            if( m_provider->msg().level() <= MSG::VERBOSE ) {
              m_provider->msg(MSG::VERBOSE) << "Try McEventCollection: " << keys[ik] << endmsg;
            }

            if ( !m_provider->evtStore()->template contains<McEventCollection>(keys[ik]) ) {
              if( m_provider->msg().level() <= MSG::VERBOSE )
                m_provider->msg(MSG::VERBOSE) << "No McEventCollection: " << keys[ik] << endmsg;
              continue;
            }

            if(m_provider->msg().level() <= MSG::VERBOSE)
              m_provider->msg(MSG::VERBOSE) << "evtStore()->retrieve( mcevent, " << keys[ik] << " )" << endmsg;

            if ( m_provider->evtStore()->template retrieve( mcevent, keys[ik] ).isFailure() ) {
              if(m_provider->msg().level() <= MSG::VERBOSE)
                m_provider->msg(MSG::VERBOSE) << "Failed to get McEventCollection: " << keys[ik] << endmsg;
            }
            else {
              /// found this key
              key = keys[ik];
              if(m_provider->msg().level() <= MSG::VERBOSE)
                m_provider->msg(MSG::VERBOSE) << "Found McEventCollection: " << key << endmsg;
              foundcollection = true;
              break;
            }
          }

          /// not found any truth collection
          if ( !foundcollection ) {
            if(m_provider->msg().level() <= MSG::VERBOSE)
              m_provider->msg(MSG::WARNING) << "No MC Truth Collections of any sort, whatsoever!!!" << endmsg;

            //    m_tree->Fill();
            //    return StatusCode::FAILURE;

            return;
          }

          if ( m_provider->msg().level() <= MSG::VERBOSE ) {
            m_provider->msg(MSG::VERBOSE) << "Found McEventCollection: " << key << "\tNevents " << mcevent->size() << endmsg;
          }

          McEventCollection::const_iterator evitr = mcevent->begin();
          McEventCollection::const_iterator evend = mcevent->end();

          unsigned ie = 0; /// count of "events" - or interactions
          unsigned ip = 0; /// count of particles

          unsigned ie_ip = 0; /// count of "events with some particles"

          while ( evitr!=evend ) {

            int _ip = 0; /// count of particles in this interaction

            int pid = (*evitr)->signal_process_id();


            if ( pid!=0 && (*evitr)->particles_size()>0 ) { /// hooray! actually found a sensible event
              /// go through the particles
              HepMC::GenEvent::particle_const_iterator pitr((*evitr)->particles_begin());
              HepMC::GenEvent::particle_const_iterator pend((*evitr)->particles_end());

              while ( pitr!=pend ) {

                selectorTruth.selectTrack( *pitr );

                ++_ip;

                ++pitr;
              }

            }
            ++ie;
            ++evitr;

            if ( _ip>0 ) {
              /// if there were some particles in this interaction ...
              //      m_provider->msg(MSG::VERBOSE) << "Found " << ie << "\tpid " << pid << "\t with " << ip << " TruthParticles (GenParticles)" << endmsg;
              ++ie_ip;
              ip += _ip;
            }
          }

          if(m_provider->msg().level() <= MSG::VERBOSE){
            m_provider->msg(MSG::VERBOSE) << "Found " << ip << " TruthParticles (GenParticles) in " << ie_ip << " GenEvents out of " << ie << endmsg;
            m_provider->msg(MSG::VERBOSE) << "selected " << selectorTruth.size() << " TruthParticles (GenParticles)" << endmsg;
          }

          if(selectorTruth.size() > 0) foundTruth = true;

          ////////////////////////////////////////////////////////////////////////////////////////


          if ( !(ip>0) ) {
            if (m_provider->msg().level() <= MSG::VERBOSE) m_provider->msg(MSG::WARNING) << "NO TRUTH PARTICLES - returning" << endmsg;
	    return; /// need to be careful here, if not requiring truth *only* should not return
          }
	  
        }
      
        // std::cout << "seeking offline tracks..." << std::endl;

        /// get offline tracks

        // m_provider->msg(MSG::VERBOSE) << " Offline tracks " << endmsg;

        if ( m_doOffline ) {

#         ifdef XAODTRACKING_TRACKPARTICLE_H
          if ( m_provider->evtStore()->template contains<xAOD::TrackParticleContainer>("InDetTrackParticles") ) {
            this->template selectTracks<xAOD::TrackParticleContainer>( m_selectorRef, "InDetTrackParticles" );
	    refbeamspot = this->template getBeamspot<xAOD::TrackParticleContainer>( "InDetTrackParticles" );
          }
          else if (m_provider->evtStore()->template contains<Rec::TrackParticleContainer>("TrackParticleCandidate") ) {
            this->template selectTracks<Rec::TrackParticleContainer>( m_selectorRef, "TrackParticleCandidate" );
          }
#         else
          if (m_provider->evtStore()->template contains<Rec::TrackParticleContainer>("TrackParticleCandidate") ) {
            this->template selectTracks<Rec::TrackParticleContainer>( m_selectorRef, "TrackParticleCandidate" );
          }
#         endif
          else if ( m_provider->msg().level() <= MSG::WARNING ) {
            m_provider->msg(MSG::WARNING) << " Offline tracks not found " << endmsg;
	  }

          ref_tracks = m_selectorRef->tracks();

          if ( m_provider->msg().level() <= MSG::VERBOSE ) {
            m_provider->msg(MSG::VERBOSE) << "ref tracks.size() " << m_selectorRef->tracks().size() << endmsg;
            for ( int ii=m_selectorRef->tracks().size() ; ii-- ; ) {
              m_provider->msg(MSG::VERBOSE) << "  ref track " << ii << " " << *m_selectorRef->tracks()[ii] << endmsg;
	    }
	  }
        }
	else { 
	  /// what is this ???
	  if ( m_mcTruth && foundTruth ){
	    ref_tracks=selectorTruth.tracks();
	  }
	}	  

        test_tracks.clear();

        for ( unsigned itrk=0 ; itrk<chain.rois().at(iroi).tracks().size() ; itrk++ ) {
          test_tracks.push_back(&(chain.rois().at(iroi).tracks().at(itrk)));
        }
	

	//	std::cout << "sutt track multiplicities: offline " << offline_tracks.size() << "\ttest " << test_tracks.size() << std::endl;

        _analysis->setvertices( vertices.size() );  /// what is this for ???

	if ( refbeamspot.size()>0 )  _analysis->setBeamRef(   refbeamspot ); 
	if ( testbeamspot.size()>0 ) _analysis->setBeamTest( testbeamspot ); 

	/// if we want a purity, we need to swap round which tracks are the 
	/// reference tracks and which the test tracks

	if ( m_runPurity ) {

	  if ( this->getUseHighestPT() ) HighestPTOnly( test_tracks );

	  if ( m_pTthreshold>0 ) FilterPT( test_tracks, m_pTthreshold );

	  /// stats book keeping 
	  m_NRois++;
	  m_NRefTracks  += test_tracks.size();
	  m_NTestTracks += ref_tracks.size();
	  	  
	  /// match test and reference tracks
	  m_associator->match( test_tracks, ref_tracks );
	  
	  _analysis->execute( test_tracks, ref_tracks, m_associator );

	}
	else { 

	  /// filter on highest pt track only if required

	  if ( this->getUseHighestPT() ) HighestPTOnly( ref_tracks );

	  /// ignore all tracks belong the specific analysis pt threshold if set
	  
	  if ( m_pTthreshold>0 )         FilterPT( ref_tracks, m_pTthreshold );

	  /// stats book keeping 
	  m_NRois++;
	  m_NRefTracks  += ref_tracks.size();
	  m_NTestTracks += test_tracks.size();
	  
	  /// match test and reference tracks
	  m_associator->match( ref_tracks, test_tracks );
	 
	  //	  std::cout << "SUTT: execute : N tracks " << ref_tracks.size() << " " << test_tracks.size() << std::endl; 

	  
	  _analysis->execute( ref_tracks, test_tracks, m_associator );
	  
	  //	  std::cout << "chain " << m_chainNames[ichain]  << " " << "\tvtx name " << vtx_name << std::endl;

	  if ( vtx_name!="" ) { 
	    /// get vertices for this roi - have to copy to a vector<Vertex*>
	    std::vector<TIDA::Vertex> vr = chain.rois().at(iroi).vertices();
	    std::vector<TIDA::Vertex*> vtx_rec;    
	    for ( unsigned iv=0 ; iv<vr.size() ; iv++ ) vtx_rec.push_back( &vr[iv] );

	    std::vector<TIDA::Vertex*> vtx;
	    if ( this->getVtxIndex()<0 ) { 
	      for ( unsigned iv=0 ; iv<vertices.size() ; iv++ ) vtx.push_back( &vertices[iv] );
	    }
	    else { 
	      if ( vertices.size()>unsigned(this->getVtxIndex()) ) vtx.push_back( &vertices[this->getVtxIndex()] );
	    }

	    _analysis->execute_vtx( vtx, vtx_rec, m_event );
	  }

	}
 
	if ( _analysis->debug() ) { 
	  m_provider->msg(MSG::INFO) << "Missing track for " << m_chainNames[ichain]  
				     << "\trun "             << run_number 
				     << "\tevent "           << event_number 
				     << "\tlb "              << lumi_block << endmsg;     
	}

      }
    }
    
    if ( m_provider->msg().level() <= MSG::VERBOSE ) {
      m_provider->msg(MSG::VERBOSE) << "\n\nEvent " << *m_event << endmsg;
    }
  }



  virtual void book() {

    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::book() " << name() << endmsg;

    // get the beam condition services - one for online and one for offline

    m_iBeamCondSvc = 0;
    if ( m_useBeamCondSvc ) { 
      if ( m_provider->service( "BeamCondSvc", m_iBeamCondSvc ).isFailure() && m_provider->msg().level() <= MSG::ERROR ) {
	m_provider->msg(MSG::ERROR) << " failed to retrieve BeamCondSvc " << endmsg;
      }
    }
    
    // get the TriggerDecisionTool

    if( m_tdt->retrieve().isFailure() ) {
      if(m_provider->msg().level() <= MSG::ERROR)
        m_provider->msg(MSG::ERROR) << " Unable to retrieve the TrigDecisionTool: Please check job options file" << endmsg;
      // return StatusCode::FAILURE;
      return;
    }

    if(m_provider->msg().level() <= MSG::VERBOSE) {
      m_provider->msg(MSG::VERBOSE) << " Successfully retrived the TrigDecisionTool"  << endmsg;
    }


    /// get list of configured triggers
    if (m_provider->msg().level() <= MSG::VERBOSE) {
      std::vector<std::string> configuredChains  = (*(m_tdt))->getListOfTriggers("L2_.*, EF_.*, HLT_.*");

      m_provider->msg(MSG::VERBOSE)  << "Configured chains" << endmsg;
      for ( unsigned i=0 ; i<configuredChains.size() ; i++ ) {
        if( m_provider->msg().level() <= MSG::VERBOSE)
          m_provider->msg(MSG::VERBOSE)  << " Chain " << configuredChains[i]  << endmsg;
      }
    }


    // std::vector<std::string> chains;
    // chains.reserve( m_chainNames.size() );

    std::vector<ChainString>::iterator chainitr = m_chainNames.begin();

    std::vector<ChainString> chains;


    /// handle wildcard chain selection - but only the first time
    /// NB: also check all other chains as well - only set up an
    ///     analysis for configured chains
    while ( chainitr!=m_chainNames.end() ) {

      /// get chain
      ChainString& chainName = (*chainitr);

      /// get matching chains
      std::vector<std::string> selectChains  = (*(m_tdt))->getListOfTriggers( chainName.head() );

      // std::cout << "selected chains " << selectChains.size() << std::endl;

      // if ( selectChains.size()==0 ) m_provider->msg(MSG::WARNING) << "No chains matched for  " << chainName << endmsg;

      for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {

        if ( chainName.tail()!="" )    selectChains[iselected] += ":key="+chainName.tail();
        if ( chainName.extra()!="" )   selectChains[iselected] += ":ind="+chainName.extra();
        if ( chainName.roi()!="" )     selectChains[iselected] += ":roi="+chainName.roi();
        if ( chainName.vtx()!="" )     selectChains[iselected] += ":vtx="+chainName.vtx();
        if ( chainName.element()!="" ) selectChains[iselected] += ":te="+chainName.element();
        if ( !chainName.passed() )     selectChains[iselected] += ";DTE";
	if ( chainName.postcount() )   selectChains[iselected] += ":post:"+chainName.post();


        /// replace wildcard with actual matching chains ...
        chains.push_back( selectChains[iselected] );

        if(m_provider->msg().level() <= MSG::VERBOSE) {
          m_provider->msg(MSG::VERBOSE) << "Matching chain " << selectChains[iselected] << " (" << chainName.head() << endmsg;
	}

      }

      chainitr++;
    }



    // m_chainNames.insert( m_chainNames.end(), chains.begin(), chains.end() );
    m_chainNames = chains;

    for ( unsigned ic=0 ; ic<m_chainNames.size() ; ic++ ) { 
      

      if ( ic>0 ) { 
	m_provider->msg(MSG::WARNING) << "more than one chain configured for this analysis - skipping " << m_chainNames[ic] << endmsg;
	continue;
      }

      m_provider->msg(MSG::VERBOSE) << "Analyse chain " << m_chainNames[ic] << endmsg;


      // m_provider->msg(MSG::VERBOSE)  << "--------------------------------------------------" << endmsg;
      
      
      std::string folder_name = "";
      
      if ( name()!="" )  folder_name = name(); 
      else               folder_name = "HLT/TRIDT/IDMon";  
      
      // unsigned decisiontype;
      // if ( m_chainNames.at(0).passed() ) decisiontype = TrigDefs::Physics;
      // else                               decisiontype = TrigDefs::alsoDeactivateTEs;
      
      /// folder_name.erase(0,3); // erase "L2_" or "EF_" so histograms all go in same chain folder - NO!!!! this means 
      /// they will over write, unless eg L2_, EF_, HLT_ etc is include in the histogram name 
      
      /// don't use test_type now ? 
      if( m_testType != "" ) folder_name = folder_name + "/" + m_testType;
      
      std::string mongroup;


      
      if ( name().find("Shifter")!=std::string::npos || m_shifter ) {
	/// shifter histograms - do not encode chain names
	if      ( m_chainNames.at(ic).tail().find("_FTF") != std::string::npos )              mongroup = folder_name + "/FTF";
	else if ( m_chainNames.at(ic).tail().find("_IDTrig") != std::string::npos || 
		  m_chainNames.at(ic).tail().find("_EFID") != std::string::npos )             mongroup = folder_name + "/EFID";
	else if ( m_chainNames.at(ic).tail().find("L2SiTrackFinder")   != std::string::npos ) mongroup = folder_name + "/L2STAR"+m_chainNames.at(ic).extra();
	else if ( m_chainNames.at(ic).tail().find("InDetTrigParticle") != std::string::npos ) mongroup = folder_name + "/EFID_RUN1";
	else if ( m_chainNames.at(ic).tail().find("_FTKRefit") != std::string::npos )         mongroup = folder_name + "/FTKRefit";
	else if ( m_chainNames.at(ic).tail().find("_FTK")      != std::string::npos )         mongroup = folder_name + "/FTK";
	else                                                                                  mongroup = folder_name + "/Unknown";

	if ( m_chainNames.at(ic).vtx()!="" ) mongroup += "/" + m_chainNames.at(ic).vtx();

      }
      else { 
	/// these are the Expert / non-Shifter histograms - encode the full chain names

	//	std::cout << "\tSUTT folder name " << folder_name << "\thead " << m_chainNames[ic].head() << "\tmongroup " << mongroup << std::endl;

	mongroup = folder_name + "/" + m_chainNames[ic].head();

	std::string track_collection = ""; 

	if ( m_chainNames.at(ic).tail()!="" )  { 
	  track_collection =  "/" + m_chainNames.at(ic).tail();
	  if ( m_chainNames.at(ic).extra()!="" ) track_collection += "_" + m_chainNames.at(ic).extra();
	}

	if ( m_chainNames.at(ic).roi()!="" ) { 
	  if ( track_collection!="" ) track_collection += "_" + m_chainNames[ic].roi();
	  else                        track_collection = "/" + m_chainNames[ic].roi();
	}

	if ( m_chainNames.at(ic).vtx()!="" ) { 
	  if ( track_collection!="" ) track_collection += "_" + m_chainNames[ic].vtx();
	  else                        track_collection  = "/" + m_chainNames[ic].vtx();
	}

	/// add trigger element and roi descriptor names
	if ( m_chainNames.at(ic).element()!="" ) { 
	  if ( track_collection!="" ) track_collection += "_" + m_chainNames[ic].element();
	  else                        track_collection  = "/" + m_chainNames[ic].element();
	}
	
	if ( track_collection!="" )  mongroup += track_collection;

	if ( !m_chainNames.at(ic).passed() )      mongroup += "/DTE";

	//	std::cout << "\n SUTT chain " << m_chainNames.at(ic) << "\tvtx " << m_chainNames.at(ic).vtx() << "\tmongroup " << mongroup << std::endl;
	
      }

      //      std::cout << "SUTT chain " << "\tvtx " << m_chainNames.at(ic).vtx() << "\tmongroup " << mongroup << std::endl;
      
      m_provider->msg(MSG::VERBOSE) << " book mongroup " << mongroup << endmsg;
      
#     ifdef ManagedMonitorToolBase_Uses_API_201401
      m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider, mongroup, ManagedMonitorToolBase::run ) );
#     else
      m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider, mongroup, 
								     ManagedMonitorToolBase::shift, 
								     ManagedMonitorToolBase::run ) );
#   endif
      

      m_analysis->initialise();
      
      _analysis = dynamic_cast<Analysis_Tier0*>(m_analysis);

      _analysis->setevent( m_event ); 

      
      std::map<std::string, TH1*>::const_iterator hitr = _analysis->THbegin();
      std::map<std::string, TH1*>::const_iterator hend = _analysis->THend();
      
      //    std::cout << "\tsutt adding to mongroup   " << mongroup << std::endl;
      
      while ( hitr!=hend ) {
	//  std::cout << "\tsutt addHisto " << hitr->second->GetName() << std::endl;
	m_provider->addHistogram( hitr->second, mongroup );
	hitr++;
      }
      
      
      std::map<std::string, TProfile*>::const_iterator effitr = _analysis->TEffbegin();
      std::map<std::string, TProfile*>::const_iterator effend = _analysis->TEffend();
      
      while ( effitr!=effend ) {
	// std::cout << "\tsutt addProfile " << effitr->second->GetName() << std::endl;
	m_provider->addHistogram( effitr->second, mongroup );
	effitr++;
      }
      
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::book() done" << endmsg;
      
    }

  }



  virtual void finalize() {

    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::finalise() " << m_provider->name() << endmsg;

    m_analysis->finalise();

    m_provider->msg(MSG::INFO) << m_provider->name() << " " << m_chainNames[0] << "   \tNRois processed: " << m_NRois << "\tRef tracks: " << m_NRefTracks << "\tTestTracks: " << m_NTestTracks << endmsg;

    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << m_provider->name() << " finalised" << endmsg;

  }


protected:

  IBeamCondSvc*  m_iBeamCondSvc;
  IBeamCondSvc*  m_iOnlineBeamCondSvc;

  bool           m_useBeamCondSvc;

  TIDA::Event*  m_event;

  TFile*    mFile;
  TTree*    mTree;

  std::vector<ChainString>     m_chainNames;
  std::vector<Analysis_Tier0*> m_analyses;
  std::string m_testType;

  bool m_doOffline;
  bool m_doMuons;
  bool m_doElectrons;
  bool m_doTaus;
  bool m_doBjets;
  bool m_hasTruthMap;
  bool m_doTauThreeProng;
  bool m_tauEtCutOffline;

  std::string m_outputFileName;

  /// output stats
  int m_NRois;
  int m_NRefTracks;
  int m_NTestTracks;

  bool m_runPurity;

  bool m_shifter;

  double m_pTthreshold;

  bool   m_first;
  
};



#endif  // TrigInDetAnalysisExample_T_AnalysisConfig_Tier0_H

