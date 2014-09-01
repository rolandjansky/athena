//
//   @file    AnalysisConfig_Tier0.cxx         
//   
//
//   @author M.Sutton, Stewart Martin-Haugh
// 
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"



#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"
#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/Filters.h"

#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"
#include "TrigInDetAnalysisExample/OfflineObjectSelection.h"


#include "VxVertex/VxContainer.h"

#include "muonEvent/MuonContainer.h"

#include "egammaEvent/ElectronContainer.h"

#include "tauEvent/TauJetContainer.h"

//#include "JetEvent/JetCollection.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDecisionTool/ExpertMethods.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


void AnalysisConfig_Tier0::loop() {

  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) <<  "AnalysisConfig_Tier0::loop() for " << m_analysisInstanceName <<  endreq;
  
  // get (offline) beam position
  double xbeam = 0;
  double ybeam = 0;
  if ( m_iBeamCondSvc ) {

#ifdef EIGEN_GEOMETRY_MODULE_H	  
    const Amg::Vector3D& vertex = m_iBeamCondSvc->beamPos();
    xbeam = vertex[0];
    ybeam = vertex[1];
#else
    HepGeom::Point3D<double> vertex = m_iBeamCondSvc->beamPos();
    xbeam = vertex.x();
    ybeam = vertex.y();
#endif

    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << " using beam position\tx=" << xbeam << "\ty=" << ybeam << endreq; 
  }
  
  static bool first = true;

  if ( first ) { 
    std::vector<std::string> configuredChains  = (*m_tdt)->getListOfTriggers("L2_.*, EF_.*");
    
    for ( unsigned i=0 ; i<configuredChains.size() ; i++ ) { 
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE)  << "Chain " << configuredChains[i]  << endreq;
    }
    
    first = false;
  }

  Filter_True filter;
  //  Filter_etaPT     filterRef(5,500);
  Filter_etaPT     filter_etaPT(5,200);
  Filter_Track filter_offline( 2.5, 1000., 2000., 2000., 1, 6, -1, -1, -2, -2);
  Filter_Vertex filter_vertex(1.5, 1.5);
  Filter_Combined filterRef (&filter_offline, &filter_vertex);
  Filter_Combined filter_truth (&filter_etaPT, &filter_etaPT);
 
  TrigTrackSelector selectorTruth( &filter_truth ); 
  TrigTrackSelector selectorRef( &filterRef ); 
  TrigTrackSelector selectorElectrons( &filterRef ); 
  TrigTrackSelector selectorTest( &filter ); 
  Associator_BestDeltaRMatcher dRmatcher("deltaR", 0.01); 

  // clear the ntuple TrackEvent class
  m_event->clear();


  const EventInfo* pEventInfo;
  unsigned run_number        = 0;
  unsigned event_number      = 0;

  unsigned lumi_block        = 0;
  unsigned bunch_crossing_id = 0;

  unsigned time_stamp = 0;
  double mu_val = 0;

  if ( m_provider->evtStore()->retrieve(pEventInfo).isFailure() ) {
    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "Failed to get EventInfo " << endreq;
  } else {
    run_number        = pEventInfo->event_ID()->run_number();
    event_number      = pEventInfo->event_ID()->event_number();
    lumi_block        = pEventInfo->event_ID()->lumi_block();
    time_stamp        = pEventInfo->event_ID()->time_stamp();
    bunch_crossing_id = pEventInfo->event_ID()->bunch_crossing_id();
    mu_val            = pEventInfo->averageInteractionsPerCrossing();

  }
  if(m_provider->msg().level() <= MSG::VERBOSE){
    m_provider->msg(MSG::VERBOSE) << "run "     << run_number 
				  << "\tevent " << event_number 
				  << "\tlb "    << lumi_block << endreq;
  }

  m_event->run_number(run_number);
  m_event->event_number(event_number);
  m_event->lumi_block(lumi_block);
  m_event->time_stamp(time_stamp);
  m_event->bunch_crossing_id(bunch_crossing_id);
  m_event->mu(mu_val);

  /// first check whether the chains have actually run, otherwise there's no point
  /// doing anything

  bool analyse = false;
 
  for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  
    const std::string& chainname = m_chainNames[ichain].head();
    
    //Only for trigger chains
    if (chainname.find("L2") == std::string::npos && chainname.find("EF") == std::string::npos  ) continue;
    
    if(m_provider->msg().level() <= MSG::VERBOSE){
      m_provider->msg(MSG::VERBOSE) << "Chain " << chainname 
				    << "\tpass " <<(*m_tdt)->isPassed(chainname) 
				    << "\tpres " << (*m_tdt)->getPrescale(chainname) << endreq;
    }
    
    
    if ( (*m_tdt)->isPassed(chainname) ) analyse = true;
    if ( (*m_tdt)->getPrescale(chainname) ) analyse = true; 
    const DataHandle<TrigDec::TrigDecision> td;
    StatusCode sc = m_provider->evtStore()->retrieve(td);
    if (sc.isFailure()) {
      if(m_provider->msg().level() <= MSG::FATAL)
	m_provider->msg(MSG::FATAL) << "Could not find TrigDecision object" << endreq;
      return;
    }
    
    const HLT::HLTResult* r = (&(td->getL2Result()));
    if (r->isHLTResultTruncated()){
      if(m_provider->msg().level() <= MSG::DEBUG)
	m_provider->msg(MSG::DEBUG) << "HLTResult Header: Truncated L2 result for event " << event_number << " in run " << run_number << endreq;
      analyse = false;
      return;
    }
    
  }
  
  if ( !analyse ) { 
    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "No chains passed unprescaled - not processing this event" << endreq; 
    return;
  }

  /// for Monte Carlo get the truth particles if requested to do so

  selectorTruth.clear();

  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) << "MC Truth flag " << m_mcTruth << endreq;
  const TrigInDetTrackTruthMap* truthMap = 0;

  if ( m_mcTruth ) {
    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "getting Truth" << endreq;
    if ( m_provider->evtStore()->retrieve(truthMap, "TrigInDetTrackTruthMap").isFailure()) {
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << "TrigInDetTrackTruthMap not found" << endreq;
      m_hasTruthMap = false;
    }
    else {
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << "TrigInDetTrackTruthMap found" << endreq;
      m_hasTruthMap = true;
    }
  }


  /// get the offline vertices into our structure
  
  std::vector<TrackVertex> vertices;
  
  const VxContainer* primaryVtxCollection;
  if ( m_provider->evtStore()->retrieve(primaryVtxCollection, "VxPrimaryCandidate").isFailure()) {
    if(m_provider->msg().level() <= MSG::WARNING)
      m_provider->msg(MSG::WARNING) << "Primary vertex container not found" << endreq;
  }
  else {
    VxContainer::const_iterator vtxitr = primaryVtxCollection->begin();
    for ( ; vtxitr != primaryVtxCollection->end(); ++vtxitr) {
      if ( (*vtxitr)->vxTrackAtVertex()->size()>0 ) {
	vertices.push_back( TrackVertex( (*vtxitr)->recVertex().position().x(),
					 (*vtxitr)->recVertex().position().y(),
					 (*vtxitr)->recVertex().position().z(),
					 0,0,0,
					 (*vtxitr)->vxTrackAtVertex()->size() ) );
      }
    }
  }
  
  filter_vertex.setVertex(vertices);

 
  
  // clear the ntuple TrackEvent class
  m_event->clear();

  /// add the truth particles if needed

  if ( m_mcTruth ) {
    m_event->addChain( "Truth" ); 
    m_event->back().addRoi(TIDARoiDescriptor());
    m_event->back().back().addTracks(selectorTruth.tracks());
  }

  /// now add the vertices

  for ( unsigned i=0 ; i<vertices.size() ; i++ )  { 
    if(m_provider->msg().level() <= MSG::VERBOSE)
      m_provider->msg(MSG::VERBOSE) << "vertex " << i << " " << vertices[i] << endreq;
    m_event->addVertex(vertices[i]);
  }

  /// now add the offline tracks

  //int Noff = 0;
  std::vector<TrigInDetAnalysis::Track*> offline_tracks; 
  std::vector<TrigInDetAnalysis::Track*> electron_tracks; 
  std::vector<TrigInDetAnalysis::Track*> muon_tracks; 
  std::vector<TrigInDetAnalysis::Track*> test_tracks; 
  offline_tracks.clear();
  electron_tracks.clear();
  muon_tracks.clear();
  test_tracks.clear();

 
  // now loop over all relevant chains to get the trigger tracks...
  for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  

    test_tracks.clear();
    
    // create chains for ntpl
    
    //    std::string& chainname = chains[ichain];
    const std::string& chainname = m_chainNames[ichain].head();
    
    const std::string& key = m_chainNames[ichain].tail();
    
    /// and the index of the collection (if any)
    const std::string& key_index_string = m_chainNames[ichain].extra();
    unsigned key_index = 0;
    if ( key_index_string!="" ) key_index = std::atoi( key_index_string.c_str() );

    if(m_provider->msg().level() <= MSG::VERBOSE){
      m_provider->msg(MSG::VERBOSE) << "status for chain " << chainname 
				    << "\tpass "     << (*m_tdt)->isPassed(chainname) 
				    << "\tprescale " << (*m_tdt)->getPrescale(chainname) 
				    << endreq;
      
      
      m_provider->msg(MSG::VERBOSE) << "fetching features for chain " << chainname << endreq;
    }

    if ( !(*m_tdt)->isPassed( chainname ) ) continue;

    // Get chain combinations and loop on them
    // - loop made on chain selected as the one steering RoI creation
    //    Trig::FeatureContainer f = (*m_tdt)->features( chainname, TrigDefs::alsoDeactivateTEs);

    
    Trig::FeatureContainer f = (*m_tdt)->features( chainname );
    Trig::FeatureContainer::combination_const_iterator c(f.getCombinations().begin()); 
    Trig::FeatureContainer::combination_const_iterator cEnd(f.getCombinations().end());

    if ( c==cEnd )  continue;

    m_event->addChain( m_chainNames[ichain] );
    
    TrackChain& chain = m_event->back();
       

    for( ; c!=cEnd ; ++c) {
      
      //   now add rois to this ntuple chain
      
      // Get seeding RoI
      // std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = c->get<TrigRoiDescriptor>("initialRoI", TrigDefs::alsoDeactivateTEs);
      // std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = c->get<TrigRoiDescriptor>("forID", TrigDefs::alsoDeactivateTEs);

      std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = c->get<TrigRoiDescriptor>("forID");
      if ( initRois.empty() ) initRois = c->get<TrigRoiDescriptor>(""); //  TrigDefs::alsoDeactivateTEs);
      if ( initRois.empty() ) initRois = c->get<TrigRoiDescriptor>("initialRoI"); //TrigDefs::alsoDeactivateTEs);
      
      
      // Skip chains seeded by multiple RoIs: not yet implemented
      if(initRois.size()>1) {
	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << " More than one initial RoI found for seeded chain " << chainname << ": not yet supported" << endreq;
	continue; 
      }
      
      TIDARoiDescriptor* roiInfo = 0;
      
      if( !initRois.empty() ) {
	const TrigRoiDescriptor* roid = initRois[0].cptr();

	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << " RoI descriptor for seeded chain " << chainname << " " << *roid << endreq;

	roiInfo = new TIDARoiDescriptor(TIDARoiDescriptorBuilder(*roid));
	//	roiInfo->etaHalfWidth(m_roiInfo->etaHalfWidth());
	//	roiInfo->phiHalfWidth(m_roiInfo->phiHalfWidth());
	//	roiInfo->etaHalfWidth(roid->etaHalfWidth());
	//	roiInfo->phiHalfWidth(roid->phiHalfWidth());
	//	roiInfo->zedHalfWidth(roid->zedHalfWidth());
	
	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << "using chain roi " << *roid<< endreq;

      }
      else { 
	roiInfo = new TIDARoiDescriptor();
	//	roiInfo->etaHalfWidth(5);
	//	roiInfo->phiHalfWidth(M_PI);
	//	roiInfo->zedHalfWidth(m_roiInfo->zedHalfWidth());

	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << "roi not found" <<  endreq;

	
      }
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << *roiInfo << endreq;
      
      selectorTest.clear();

      if(chainname.find("L2_")!=std::string::npos) {
	selectTracks<TrigInDetTrackCollection>( &selectorTest, c, truthMap, key, key_index );
      }
      
      if(chainname.find("EF_")!=std::string::npos) {
	selectTracks<Rec::TrackParticleContainer>( &selectorTest, c, key );
      }
      
      const std::vector<TrigInDetAnalysis::Track*>& testtracks = selectorTest.tracks();

      if(m_provider->msg().level() <= MSG::VERBOSE){
	m_provider->msg(MSG::VERBOSE) << "test tracks.size() " << testtracks.size() << endreq; 
      for ( int ii=testtracks.size() ; ii-- ; )
	{
	  m_provider->msg(MSG::VERBOSE) << "  test track " << ii << " " << *testtracks[ii] << endreq;  
	  //test_tracks.push_back(testtracks.at(ii));
	}
      }
      chain.addRoi( *roiInfo );

      chain.back().addTracks(testtracks);
      
      if ( roiInfo ) delete roiInfo;

    }
    
    for(unsigned int ich = 0; ich < chain.size(); ich++){ 

      selectorRef.clear();      
      selectorElectrons.clear();      
      filterRef.setRoi( &chain.rois().at(ich).roi() );
      filter_truth.setRoi( &chain.rois().at(ich).roi() );
      test_tracks.clear();
      selectorTruth.clear();
      
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << "MC Truth flag " << m_mcTruth << endreq;
      bool foundTruth = false;
      if ( m_mcTruth ) {
	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << "getting Truth" << endreq;
	if (m_provider->evtStore()->contains<TruthParticleContainer>("INav4MomTruthEvent")) {
	  //ESD                                                                                                       
	  selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );
	  foundTruth = true;
	}
	else if (m_provider->evtStore()->contains<TruthParticleContainer>("SpclMC")) {
	  /// AOD                                                                                                     
	  selectTracks<TruthParticleContainer>( &selectorTruth, "SpclMC");
	  foundTruth = true;
	}
	else if (m_provider->evtStore()->contains<TruthParticleContainer>("")) {
	  /// anything else?                                                                                          
	  selectTracks<TruthParticleContainer>( &selectorTruth, "");
	  foundTruth = true;
	}
	else 
	  if(m_provider->msg().level() <= MSG::VERBOSE)
	    m_provider->msg(MSG::VERBOSE) << "Truth not found - none whatsoever!" << endreq;
      }
    
    if ( m_mcTruth && !foundTruth ) {
      if(m_provider->msg().level() <= MSG::VERBOSE)
	m_provider->msg(MSG::VERBOSE) << "getting Truth" << endreq;

	/// selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );                             

	const DataHandle<McEventCollection> mcevent;

	/// now as a check go through the GenEvent collection                                                         

	std::string keys[4] = { "GEN_AOD", "TruthEvent", "", "G4Truth" };

	std::string key = "";

	bool foundcollection = false;

	for ( int ik=0 ; ik<4 ; ik++ ) {
	  if(m_provider->msg().level() <= MSG::VERBOSE)
	    m_provider->msg(MSG::VERBOSE) << "Try McEventCollection: " << keys[ik] << endreq;

	  if (!m_provider->evtStore()->contains<McEventCollection>(keys[ik]) ) {
	    if(m_provider->msg().level() <= MSG::VERBOSE)
	      m_provider->msg(MSG::VERBOSE) << "No McEventCollection: " << keys[ik] << endreq;
	    continue;
	  }
	  if(m_provider->msg().level() <= MSG::VERBOSE)
	    m_provider->msg(MSG::VERBOSE) << "evtStore()->retrieve( mcevent, " << keys[ik] << " )" << endreq;

	  if ( m_provider->evtStore()->retrieve( mcevent, keys[ik] ).isFailure() ) {
	    if(m_provider->msg().level() <= MSG::VERBOSE)
	      m_provider->msg(MSG::VERBOSE) << "Failed to get McEventCollection: " << keys[ik] << endreq;
	  }
	  else {
	    // found this key                                                                                         
	    key = keys[ik];
	    if(m_provider->msg().level() <= MSG::VERBOSE)
	      m_provider->msg(MSG::VERBOSE) << "Found McEventCollection: " << key << endreq;
	    foundcollection = true;
	    break;
	  }
	}
	// not found any collection                                                                                   
	if ( !foundcollection ) {
	  if(m_provider->msg().level() <= MSG::VERBOSE)
	    m_provider->msg(MSG::VERBOSE) << "No MC Truth Collections of any sort, whatsoever!!!" << endreq;

	  //    m_tree->Fill();                                                                                       
	  //    return StatusCode::FAILURE;                                                                           

	  return;
	}
	if(m_provider->msg().level() <= MSG::VERBOSE)
	  m_provider->msg(MSG::VERBOSE) << "Found McEventCollection: " << key << "\tNevents " << mcevent->size() << endreq \
	  ;

	//      DataVector<HepMC::GenEvent>::iterator                                                                 

	McEventCollection::const_iterator evitr(mcevent->begin());
	McEventCollection::const_iterator evend(mcevent->end());

	unsigned ie = 0; /// count of "events" - or interactions                                                      
	unsigned ip = 0; /// count of particles                                                                       

	unsigned ie_ip = 0; /// count of "events with some particles"

	while ( evitr!=evend ) {

	  int _ip = 0; /// count of particles in this interaction                                                     

	  int pid = (*evitr)->signal_process_id();

	  //      if ( (*evitr)->particles_size()>0 ) std::cout << "process " << "\tpid " << pid << std::endl;        
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
	    //      m_provider->msg(MSG::INFO) << "Found " << ie << "\tpid " << pid << "\t with " << ip << " TruthParticles (GenParticles)" << endreq;                                                                                 
	    ++ie_ip;
	    ip += _ip;
	  }
	}
	if(m_provider->msg().level() <= MSG::VERBOSE){
	  m_provider->msg(MSG::VERBOSE) << "Found " << ip << " TruthParticles (GenParticles) in " << ie_ip << " GenEvents out of " << ie << endreq;
	  m_provider->msg(MSG::VERBOSE) << "selected " << selectorTruth.size() << " TruthParticles (GenParticles)" << endreq;
	}
	if(selectorTruth.size() > 0) foundTruth = true;
	////////////////////////////////////////////////////////////////////////////////////////
      

	  if ( !(ip>0) ) {
	    if(m_provider->msg().level() <= MSG::VERBOSE)
	      m_provider->msg(MSG::WARNING) << "NO TRUTH PARTICLES - returning" << endreq;
	    return; /// need to be careful here, if not requiring truth *only* should not return                        
	  }
      
      }    
    
      /// get offline tracks                                                                                         

      //      m_provider->msg(MSG::INFO) << " Offline tracks " << endreq;

      if (m_provider->evtStore()->contains<Rec::TrackParticleContainer>("TrackParticleCandidate")) {
	selectTracks<Rec::TrackParticleContainer>( &selectorRef, "TrackParticleCandidate" );
      }
      else {
	if(m_provider->msg().level() <= MSG::WARNING)
	  m_provider->msg(MSG::WARNING) << " Offline tracks not found " << endreq;
      }

      if(chainname.find("EF_e")!=std::string::npos || chainname.find("L2_e")!=std::string::npos) {

	const ElectronContainer *MyElectronContainer = 0;
	std::string m_ElectronContainerName = "ElectronAODCollection";
	
	bool eCont=false;
	if( m_provider->evtStore()->contains<ElectronContainer>(m_ElectronContainerName) ) {
	  StatusCode sc=m_provider->evtStore()->retrieve(MyElectronContainer, m_ElectronContainerName);
	  if( sc.isFailure()  || !MyElectronContainer ) {
	    if(m_provider->msg().level() <= MSG::WARNING){
	      m_provider->msg(MSG::WARNING) << "Error retrieving " << m_ElectronContainerName
					    << " after contains" << endreq;
	    }
	  } else {
	    eCont=true;
	  }
	} else {
	  if(m_provider->msg().level() <= MSG::WARNING){
	    m_provider->msg(MSG::WARNING) << "Error retrieving " << m_ElectronContainerName
					  << " !" << endreq;
	  }
	}
	if (eCont) {
	  ElectronContainer::const_iterator itr = MyElectronContainer->begin();
	  ElectronContainer::const_iterator itr_end = MyElectronContainer->end();

	  if(m_provider->msg().level() <= MSG::VERBOSE){
	    m_provider->msg(MSG::VERBOSE) << "Event with " <<  MyElectronContainer->size()
					  << " Electron object(s) " << endreq;
	  }
	  
	  for(; itr !=itr_end; ++itr){
	    if(m_provider->msg().level() <= MSG::VERBOSE){
	      m_provider->msg(MSG::VERBOSE) << " Electron "  << (*itr) << 
		") with cluster (" << (*itr)->cluster() << 
		") || eta= " << (*itr)->eta() << 
		"  ,phi= " << (*itr)->phi() << 
		"  ,et= " << (*itr)->et() <<  
		"  ,author= " << (*itr)->author() <<
		"  ,trackParticle= " << (*itr)->trackParticle() <<  
		"  ,conversion= " << (*itr)->conversion()<< 
		"  ,mediumPP= " << ((*itr)->isem(egammaPID::ElectronMediumPP)==0) << endreq;
	    }
	    
	    if (TrigInDetAnalysis::IsGoodOffline(*(*itr))) {
		selectorElectrons.selectTrack( (*itr)->trackParticle());
	    }
	    
	  }
	}

      } // do electron selection
      
      if ( m_doOffline ) { 
	
    //Noff = selectorRef.tracks().size();
	offline_tracks = selectorRef.tracks();
	
	if(m_provider->msg().level() <= MSG::VERBOSE){
	  m_provider->msg(MSG::VERBOSE) << "ref tracks.size() " << selectorRef.tracks().size() << endreq; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; )
	    m_provider->msg(MSG::VERBOSE) << "  ref track " << ii << " " << *selectorRef.tracks()[ii] << endreq; 
	}
      }
      
      if ( m_mcTruth && foundTruth ){
	offline_tracks=selectorTruth.tracks();
      }

      if((chainname.find("EF_e")!=std::string::npos || chainname.find("L2_e")!=std::string::npos) && m_testType != "")
	offline_tracks = selectorElectrons.tracks();
      
      test_tracks.clear();
      for(unsigned int itrk=0; itrk< chain.rois().at(ich).tracks().size(); itrk++)
	{
	  test_tracks.push_back(&(chain.rois().at(ich).tracks().at(itrk)));
        }

      //      m_analyses.at(ichain)->setvertices(vertices.size());
      m_analysisT0->setvertices(vertices.size());
      dRmatcher.match(offline_tracks, test_tracks);
      //      m_analyses.at(ichain)->execute(offline_tracks,test_tracks,&dRmatcher);
      m_analysisT0->execute(offline_tracks,test_tracks,&dRmatcher);
    
    }
  }


}



/// setup the analysis the analysis, retrieve the tools etc 

void AnalysisConfig_Tier0::book() { 
  
  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::book() " << name() << endreq;   

  // get the beam condition services - one for online and one for offline
  
  m_iBeamCondSvc = 0;
  if ( m_provider->service( "BeamCondSvc", m_iBeamCondSvc ).isFailure() )  { 
    if(m_provider->msg().level() <= MSG::ERROR)
      m_provider->msg(MSG::ERROR) << " failed to retrieve BeamCondSvc " << endreq;
  }
  
  // get the TriggerDecisionTool

  if( m_tdt->retrieve().isFailure() ) {
    if(m_provider->msg().level() <= MSG::ERROR)
      m_provider->msg(MSG::ERROR) << " Unable to retrieve the TrigDecisionTool: Please check job options file" << endreq;
    //    return StatusCode::FAILURE;
    return;
  }
  
  if(m_provider->msg().level() <= MSG::VERBOSE){
    m_provider->msg(MSG::VERBOSE) << " Successfully retrived the TrigDecisionTool"  << endreq;
    
    m_provider->msg(MSG::VERBOSE) << " booking mongroup" << endreq;
  }


  std::string folder_name = m_chainNames.at(0).head();
  folder_name.erase(0,3); // erase "L2_" or "EF_" so histograms all go in same chain folder
  if(m_testType != "") folder_name = folder_name + "/" + m_testType;
  else folder_name = folder_name + "/offline_tracks"; 

#ifdef ManagedMonitorToolBase_Uses_API_201401

  m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider,"HLT/TIDAMon/" + folder_name +"/"
								 + m_chainNames.at(0).tail() +"/" + m_chainNames.at(0).extra() + "/",
								 ManagedMonitorToolBase::run ) );
#else

  m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider,"HLT/TIDAMon/" + folder_name +"/"
								 + m_chainNames.at(0).tail() +"/" + m_chainNames.at(0).extra() + "/",
								 ManagedMonitorToolBase::shift,
								 ManagedMonitorToolBase::run ) );
#endif

  m_analysis->initialise();

  m_analysisT0 = dynamic_cast<Analysis_Tier0*> (m_analysis);

  std::map<std::string, TH1*>::const_iterator hitr = m_analysisT0->THbegin();
  std::map<std::string, TH1*>::const_iterator hend = m_analysisT0->THend();
  
  while ( hitr!=hend ) { 
    m_provider->addHistogram( hitr->second, "HLT/TIDAMon/" + folder_name +"/" 
			      + m_chainNames.at(0).tail() + "/" + m_chainNames.at(0).extra() + "/");

    hitr++; 
  }
  
  std::map<std::string, TProfile*>::const_iterator effitr = m_analysisT0->TEffbegin();
  std::map<std::string, TProfile*>::const_iterator effend = m_analysisT0->TEffend();

  while ( effitr!=effend ) {
    m_provider->addHistogram( effitr->second, "HLT/TIDAMon/" + folder_name +"/"
			      + m_chainNames.at(0).tail() + "/" + m_chainNames.at(0).extra() + "/");
    
    effitr++;
  }

  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::book() done" << endreq;   
  
}




/// finalise the analysis - take ratios for efficiencies etc

void AnalysisConfig_Tier0::finalize() { 

  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) << "AnalysisConfig_Tier0::finalise() " << m_provider->name() << endreq; 

  m_analysis->finalise();

  if(m_provider->msg().level() <= MSG::VERBOSE)
    m_provider->msg(MSG::VERBOSE) << m_provider->name() << " finalised" << endreq; 

}








