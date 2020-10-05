/**
 **     @file    AnalysisConfigMT_Ntuple.cxx
 **
 **     @author  mark sutton
 **
 **     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysisExample/AnalysisConfigMT_Ntuple.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "TrigInDetAnalysis/Filter_AcceptAll.h"
#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/Filters.h"

#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"

std::string date();


//function to find true taus
const HepMC::GenParticle* fromParent( int pdg_id, const HepMC::GenParticle* p, bool printout=false );
  


template<class T>
void remove_duplicates(std::vector<T>& vec) {
  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}




void AnalysisConfigMT_Ntuple::loop() {

        m_provider->msg(MSG::DEBUG) << "[91;1m" << "AnalysisConfig_Ntuple::loop() for " << m_analysisInstanceName 
				   << " compiled " << __DATE__ << " " << __TIME__ << "\t: " << date() << "[m" << endmsg;

	// get (offline) beam position
	double xbeam = 0;
	double ybeam = 0;
	double zbeam = 0;
	std::vector<double> beamline;

	bool foundOffline = false;

	if ( m_iBeamCondSvc ) {

	  const Amg::Vector3D& vertex = m_iBeamCondSvc->beamPos();
	  xbeam = vertex[0];
	  ybeam = vertex[1];
	  zbeam = vertex[2];

	  /// leave this code commented here - useful for debugging 
	  //	  m_provider->msg(MSG::INFO) << " using beam position\tx=" << xbeam << "\ty=" << ybeam << "\tz=" << zbeam <<endmsg; 
	  beamline.push_back(xbeam);
	  beamline.push_back(ybeam);
	  beamline.push_back(zbeam);
	  //     m_provider->msg(MSG::INFO) << " beamline values : " << beamline[0] << "\t" << beamline[1]  << "\t" << beamline[2] << endmsg;	
	}

	// get (online) beam position
	double xbeam_online = 0;
	double ybeam_online = 0;
	double zbeam_online = 0;

	std::vector<double> beamline_online;

	if ( m_iOnlineBeamCondSvc ) {

	  const Amg::Vector3D& vertex = m_iOnlineBeamCondSvc->beamPos();
	  xbeam_online = vertex[0];
	  ybeam_online = vertex[1];
	  zbeam_online = vertex[2];

	  beamline_online.push_back( xbeam_online );
	  beamline_online.push_back( ybeam_online );
	  beamline_online.push_back( zbeam_online );

	  //	  m_provider->msg(MSG::INFO) << " using online beam position" 
	  //				     << "\tx=" << xbeam_online 
	  //				     << "\ty=" << ybeam_online 
	  //				     << "\tz=" << zbeam_online << endmsg; 
	}

	//	m_provider->msg(MSG::INFO) << " offline beam position\tx=" << xbeam        << "\ty=" << ybeam        << "\tz=" << zbeam        << endmsg; 
	//	m_provider->msg(MSG::INFO) << " online  beam position\tx=" << xbeam_online << "\ty=" << ybeam_online << "\tz=" << zbeam_online << endmsg; 

	/// list the configured chains

	static std::set<std::string> configuredHLTChains;

	std::vector<ChainString> chainNames;

	static bool tida_first = true;
	
	if ( tida_first ) { 

		std::vector<std::string> configuredChains  = (*m_tdt)->getListOfTriggers("L2_.*, EF_.*, HLT_.*");

		if (m_provider->msg().level() <= MSG::VERBOSE) {
		  m_provider->msg(MSG::VERBOSE) << "[91;1m" << configuredChains.size() << " Configured Chains" << "[m" << endmsg;
		}

		for ( unsigned i=0 ; i<configuredChains.size() ; i++ ) { 
		  if (m_provider->msg().level() <= MSG::VERBOSE) {
		    m_provider->msg(MSG::VERBOSE) << "[91;1m" << "Chain " << configuredChains[i] << "   (ACN)[m" << endmsg;
		  }
		  configuredHLTChains.insert( configuredChains[i] );
		}

		tida_first = false;

		std::vector<ChainString>::iterator chainitr = m_chainNames.begin();

		/// handle wildcard chain selection - but only the first time
		while ( chainitr!=m_chainNames.end() ) {

		  /// get chain
		  ChainString& chainName = (*chainitr);

		  /// get matching chains
		  
		  std::vector<std::string> selectChains;
		  selectChains.clear();
		  if ( chainitr->head()=="" ) selectChains.push_back("");
		  else                        selectChains = (*m_tdt)->getListOfTriggers( chainName.head() );
		  
		  for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {
 
		      if ( chainName.tail()!="" )    selectChains[iselected] += ":key="+chainName.tail();
		      if ( chainName.extra()!="" )   selectChains[iselected] += ":index="+chainName.extra();
		   
		      if ( chainName.element()!="" ) selectChains[iselected] += ":te="+chainName.element(); 
		      if ( chainName.roi()!="" )     selectChains[iselected] += ":roi="+chainName.roi();
		      if ( chainName.vtx()!="" )     selectChains[iselected] += ":vtx="+chainName.vtx();
		     
		      if ( !chainName.passed() )    selectChains[iselected] += ";DTE";
		     
		      /// replace wildcard with actual matching chains ...
		      chainNames.push_back( ChainString(selectChains[iselected]) );

		      m_provider->msg(MSG::INFO) << "[91;1m" << "Matching chain " << selectChains[iselected] << "[m" << endmsg;

		      /// if this has a cosmic chain, set the fiducial radius to be very large to 
		      /// allow the production vertex of the cosmic to be included
		      if ( selectChains[iselected].find("cosmic")!=std::string::npos ) m_fiducial_radius = 1e10; 
		     
		  }
		  
		  chainitr++;
		}
		
		m_chainNames = chainNames;

	}

	Filter_AcceptAll filter;
	/// FIXME: should really have hardcoded limits encoded as 
	///        const variables 
	Filter_etaPT       filterRef(5,500);   
	Filter_etaPT       filter_etaPT(3.0,1000);
	Filter_pdgIdpTeta  filter_pdgIdpTeta(m_TruthPdgId,3.0,1000); // |eta|<3, pt>1GeV 

	TrackFilter*        truthFilter = &filter_etaPT;

	//tau filtering done separately to include mothers
	if ( m_TruthPdgId!=0 && m_TruthPdgId!=15 ) truthFilter = &filter_pdgIdpTeta;

	TrigTrackSelector selectorTruth( truthFilter, m_fiducial_radius ); 

	TrigTrackSelector selectorRef( &filter_etaPT ); 
	TrigTrackSelector selectorTest( &filter ); 


	selectorTruth.setBeamline( xbeam, ybeam, zbeam ); 
	selectorRef.setBeamline( xbeam, ybeam, zbeam ); 
	selectorTest.setBeamline( xbeam_online, ybeam_online, zbeam_online ); 


	selectorTruth.correctTracks( true );
	selectorRef.correctTracks( true );
	selectorTest.correctTracks( true );
	

	// clear the ntuple TIDA::Event class
	m_event->clear();

	const xAOD::EventInfo* pEventInfo = 0;

	unsigned           run_number         = 0;
	unsigned long long event_number       = 0;
	unsigned           lumi_block         = 0;
	unsigned           bunch_crossing_id  = 0;
	unsigned           time_stamp         = 0;
	double             mu_val             = 0;

	if ( m_provider->evtStore()->retrieve(pEventInfo).isFailure() ) {
		m_provider->msg(MSG::DEBUG) << "Failed to get EventInfo " << endmsg;
	} 
	else {
		run_number        = pEventInfo->runNumber();
		event_number      = pEventInfo->eventNumber();
		lumi_block        = pEventInfo->lumiBlock();
		time_stamp        = pEventInfo->timeStamp();
		bunch_crossing_id = pEventInfo->bcid();
		mu_val            = pEventInfo->averageInteractionsPerCrossing();
	}

	m_provider->msg(MSG::DEBUG) << "run "     << run_number 
				    << "\tevent " << event_number 
				    << "\tlb "    << lumi_block << endmsg;

	m_event->run_number(run_number);
	m_event->event_number(event_number);
	m_event->lumi_block(lumi_block);
	m_event->time_stamp(time_stamp);
	m_event->bunch_crossing_id(bunch_crossing_id);
	m_event->mu(mu_val);

	// build a chain group on the fly and use the reference
	const Trig::ChainGroup* L2chain=(*m_tdt)->getChainGroup("L2_.*");
	const Trig::ChainGroup* EFchain=(*m_tdt)->getChainGroup("EF_.*");
	const Trig::ChainGroup* HLTchain=(*m_tdt)->getChainGroup("HLT_.*");

	m_provider->msg(MSG::DEBUG) << "[91;1m" 
		<< "L2 pass " << L2chain->isPassed()  << "\t" 
		<< "EF pass " << EFchain->isPassed()  << "\t" 
		<< "HLT pass " << HLTchain->isPassed() << "[m"
		<< endmsg;
       
	/// check whether the chains have actually run, otherwise there's no point
	/// doing anything

	bool analyse = false;

	unsigned decisiontype_ = TrigDefs::Physics;
	
	if ( requireDecision() ) decisiontype_ = TrigDefs::requireDecision;


	/// bomb out if no chains passed and not told to keep all events  

	int passed_chains = 0;

	m_provider->msg(MSG::DEBUG) << "Checking " << m_chainNames.size() << " chains" << endmsg;
	
	if ( m_chainNames.empty() ) {
	  m_provider->msg(MSG::WARNING) << "No chains to check" << endmsg;
	  return;
	}

	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {
  
		const std::string& chainName = m_chainNames[ichain].head();

		// Only for trigger chains

		if (chainName.find("L2")  == std::string::npos && 
		    chainName.find("EF")  == std::string::npos && 
		    chainName.find("HLT") == std::string::npos ) continue;

		if ( configuredHLTChains.find(chainName)==configuredHLTChains.end() ) {
			m_provider->msg(MSG::WARNING) << "[91;1m" << "Chain " << chainName 
				<< " is not configured for this event" << "[m"
				<< endmsg;
			continue;
		}

		
		std::string roistring = "";
		if ( m_chainNames[ichain].roi()!=""  ) roistring += "\troi " + m_chainNames[ichain].roi();  

		bool passPhysics = (*m_tdt)->isPassed(chainName); 

		m_provider->msg(MSG::DEBUG) << "Chain "  << chainName << "\troi " << roistring 
					   << "\tpres " << (*m_tdt)->getPrescale(chainName)
					   << ( passPhysics ? "[91;1m" : "" ) << "\tpass physics  " <<  passPhysics << ( passPhysics ? "[m" : "" ) 
					   << "\t: ( pass " << (*m_tdt)->isPassed(chainName, decisiontype_ ) << "\tdec type " << decisiontype_ << " ) " << endmsg;

		if ( (*m_tdt)->isPassed(chainName, decisiontype_ ) ) { 
		  analyse = true;
		  passed_chains++;
		}

	} /// finished loop over chains



	/// bomb out if no chains passed and not told to keep all events and found no 
	/// offline objects 
	if ( !analyse && !m_keepAllEvents && !foundOffline ) { 
	  m_provider->msg(MSG::DEBUG) << "No chains passed unprescaled - not processing this event: " << run_number << " " << event_number << " " << lumi_block << endmsg; 
	  return;
	}
	

	m_provider->msg(MSG::DEBUG) << "Chains passed " << passed_chains << endmsg;


	/// for Monte Carlo get the truth particles if requested to do so

	//  const TruthParticleContainer*  mcpartTES = 0;

	selectorTruth.clear();

	m_provider->msg(MSG::DEBUG) << "MC Truth flag " << m_mcTruth << endmsg; 
	const TrigInDetTrackTruthMap* truthMap = 0;
	bool foundTruth = false;
	if ( m_mcTruth && m_TruthPdgId!=15) { 
		m_provider->msg(MSG::DEBUG) << "getting Truth" << endmsg; 
		if ( m_provider->evtStore()->retrieve(truthMap, "TrigInDetTrackTruthMap").isFailure()) {
			m_hasTruthMap = false;
		}
		else {
			m_hasTruthMap = true;
		}
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
		else if (m_provider->evtStore()->contains<xAOD::TruthParticleContainer>("TruthParticles")) {
			/// anything else?
		        selectTracks<xAOD::TruthParticleContainer>( &selectorTruth, "TruthParticles" );
			foundTruth = true;
		}
		else if (m_provider->evtStore()->contains<xAOD::TruthParticleContainer>("")) {
			/// anything else?
		        selectTracks<xAOD::TruthParticleContainer>( &selectorTruth, "" );
			foundTruth = true;
		}
		else { 
			m_provider->msg(MSG::WARNING) << "Truth not found - none whatsoever!" << endmsg; 
		}
	}


	/// No lovely truth particle collections, so will need to 
	/// navigate through the egregious McTruthCollection, looking 
	/// at the GenParticles on each GenVertex in the collection
	/// lovely...

	if ( m_mcTruth && !foundTruth ) { 

		m_provider->msg(MSG::DEBUG) << "getting Truth" << endmsg; 

		/// selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );

		const DataHandle<McEventCollection> mcevent;

		/// now as a check go through the GenEvent collection

		std::string collectionNames[4] = { "GEN_AOD", "TruthEvent", "", "G4Truth" };

		std::string collectionName = "";

		bool foundcollection = false;

		for ( int ik=0 ; ik<4 ; ik++ ) { 

			m_provider->msg(MSG::DEBUG) << "Try McEventCollection: " << collectionNames[ik] << endmsg;

			if (!m_provider->evtStore()->contains<McEventCollection>(collectionNames[ik]) ) { 
				m_provider->msg(MSG::DEBUG) << "No McEventCollection: " << collectionNames[ik] << endmsg;
				continue;
			}

			m_provider->msg(MSG::DEBUG) << "evtStore()->retrieve( mcevent, " << collectionNames[ik] << " )" << endmsg;  

			if ( m_provider->evtStore()->retrieve( mcevent, collectionNames[ik] ).isFailure() ) {     
				m_provider->msg(MSG::DEBUG) << "Failed to get McEventCollection: " << collectionNames[ik] << endmsg;
			}
			else { 
				// found this collectionName
				collectionName = collectionNames[ik];
				m_provider->msg(MSG::DEBUG) << "Found McEventCollection: " << collectionName << endmsg;
				foundcollection = true;
				break;
			}
		}

		// not found any collection
		if ( !foundcollection ) { 

			m_provider->msg(MSG::WARNING) << "No MC Truth Collections of any sort, whatsoever!!!" << endmsg;

			//    m_tree->Fill();

			//    return StatusCode::FAILURE;

			return;
		}

		//////////////////////////////////////////////////////////////////////////////////
		/// This is a nasty hack  to find the truth collections 
		/// why, why, why, why, why oh why, is it sooooo 
		/// difficult to navigate through this nonsense  
		/// it's not at all tidy, and should be rewritten, 
		/// but probably never will be

		m_provider->msg(MSG::DEBUG) << "Found McEventCollection: " << collectionName << "\tNevents " << mcevent->size() << endmsg;

		/// count the number of interactions of each sort
		/// this is actually *very stupid*, there are a *lot*
		/// of "processes" with *no* particles in them for some 
		/// reason, whoever programed this f**cked up structure 
		/// that needs this sort of fannying around to navigate 

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
		      
		      //	int pdg_id = (*pitr)->pdg_id();
		      //	std::cout << ip++ << "\tparticle " << pdg_id << "\t" << "(*pitr)->pT()" << std::endl; 
		      
		      //if tau job, only select if have a tau somewhere along chain, otherwise just add
		      if ( (m_TruthPdgId==15 && fromParent(m_TruthPdgId, *pitr)!=0) || m_TruthPdgId!=15 ) {
			
			/// select the ones of interest 
			selectorTruth.selectTrack( *pitr );
		      }
		      
		      
		      ++_ip;
		      
		      ++pitr; 
		      
		    }
		    
		    
		  }
		  
		  ++ie;
		  ++evitr;
		  
		  if ( _ip>0 ) {
		    /// if there were some particles in this interaction ...
		    //	m_provider->msg(MSG::INFO) << "Found " << ie << "\tpid " << pid << "\t with " << ip << " TruthParticles (GenParticles)" << endmsg;
		    ++ie_ip;  
		    ip += _ip;
		  }
		}
		
		m_provider->msg(MSG::DEBUG) << "Found " << ip << " TruthParticles (GenParticles) in " << ie_ip << " GenEvents out of " << ie << endmsg;
		
		m_provider->msg(MSG::DEBUG) << "selected " << selectorTruth.size() << " TruthParticles (GenParticles)" << endmsg;

		////////////////////////////////////////////////////////////////////////////////////////

		if ( !(ip>0) ) {    
			m_provider->msg(MSG::WARNING) << "NO TRUTH PARTICLES - returning" << endmsg;
			return; /// need to be careful here, if not requiring truth *only* should not return
		}

	}



	// clear the ntuple TIDA::Event class
	m_event->clear();

	/// get (offline) reference tracks 

	/// get offline tracks

	m_provider->msg(MSG::DEBUG) << " Offline tracks " << endmsg;

	selectorRef.clear();

	if      (m_provider->evtStore()->contains<xAOD::TrackParticleContainer>("InDetTrackParticles")) {
	  selectTracks<xAOD::TrackParticleContainer>( &selectorRef, "InDetTrackParticles" );
	}
	else if (m_provider->evtStore()->contains<Rec::TrackParticleContainer>("TrackParticleCandidate")) {
	  selectTracks<Rec::TrackParticleContainer>( &selectorRef, "TrackParticleCandidate" );
        }
	else { 
	  m_provider->msg(MSG::WARNING) << " Offline tracks not found " << endmsg;
	}
	

	/// get the offline vertices into our structure
	
	std::vector<TIDA::Vertex> vertices;

	//	std::vector<TIDA::Vertex> vertices;
	
	m_provider->msg(MSG::VERBOSE) << "fetching AOD Primary vertex container" << endmsg;

	const xAOD::VertexContainer* xaodVtxCollection = 0;

	if ( m_provider->evtStore()->retrieve( xaodVtxCollection, "PrimaryVertices" ).isFailure()) {
	  m_provider->msg(MSG::WARNING) << "xAOD Primary vertex container not found with key " << "PrimaryVertices" <<  endmsg;
	}
	
	if ( xaodVtxCollection!=0 ) { 
	  
	  m_provider->msg(MSG::DEBUG) << "xAOD Primary vertex container " << xaodVtxCollection->size() <<  " entries" << endmsg;

	  xAOD::VertexContainer::const_iterator vtxitr = xaodVtxCollection->begin();

	  for ( ; vtxitr != xaodVtxCollection->end(); vtxitr++ ) {

	    /// useful debug information - leave in 
	    //	    std::cout << "SUTT  xAOD::Vertex::type() " << (*vtxitr)->type() << "\tvtxtype " << (*vtxitr)->vertexType() << "\tntrax " << (*vtxitr)->nTrackParticles() << std::endl; 

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


	/// add offline Vertices to the Offline chain
	
	
	/// add the truth particles if needed
	
	if ( m_mcTruth ) {
	  m_event->addChain( "Truth" ); 
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorTruth.tracks());
	}


	/// now add the vertices

	/// useful debug information - leave in  
	//	std::cout << "SUTT Nvertices " << vertices.size() << "\ttype 101 " << vertices_full.size() << std::endl;

#if 0
	/// don;t add them to the event - since now we store them in the Vertex chain ...
	for ( unsigned i=0 ; i<vertices.size() ; i++ )  { 
	  m_provider->msg(MSG::DEBUG) << "vertex " << i << " " << vertices[i] << endmsg;
	  m_event->addVertex(vertices[i]);
	}
#endif	

	/// offline object counters 

	int Noff  = 0;
	int Nmu   = 0;
	int Nel   = 0;
        int Ntau  = 0;


	/// now add the offline vertices

	if ( m_doOffline || m_doVertices ) { 	  
	  m_event->addChain( "Vertex" );
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addVertices( vertices );
	}	 


	/// now add the offline tracks

	if ( m_doOffline ) { 
	  
	  m_event->addChain( "Offline" );
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorRef.tracks());
	
	  if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	    std::vector<double> beamline_;
	    beamline_.push_back( selectorRef.getBeamX() );
	    beamline_.push_back( selectorRef.getBeamY() );
	    beamline_.push_back( selectorRef.getBeamZ() );
	    m_event->back().back().addUserData(beamline_);
	  }
	  else { 
	    m_event->back().back().addUserData(beamline);
	  }



	  Noff = selectorRef.tracks().size();
	  
	  m_provider->msg(MSG::DEBUG) << "ref tracks.size() " << selectorRef.tracks().size() << endmsg; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  ref track " << ii << " " << *selectorRef.tracks()[ii] << endmsg;  
	  
	}

	/// navigate through the requested storegate TEST chains
	
	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  
	  
	  /// keep this printout here, but commented for usefull debug purposes ...
	  //	  m_provider->msg(MSG::INFO)<< "chain:\t" << m_chainNames[ichain] << endmsg;

	  /// get the chain, collection and TE names and track index 

	  std::string chainname      = m_chainNames[ichain].head();
	  std::string collectionname = m_chainNames[ichain].tail();
	  std::string vtx_name       = m_chainNames[ichain].vtx();


	  if ( chainname!="" )      continue;
	  if ( collectionname=="" ) continue;

	  chainname = collectionname;
	  if ( vtx_name!="" ) chainname += ":" + vtx_name; 

	  /// useful debug information - leave this here

	  //	  const std::string& index          = m_chainNames[ichain].extra();
	  //	  const std::string& element        = m_chainNames[ichain].element();

	  //	  std::cout << "\tchain: " << chainname << "\tcollection: " << collectionname << "\tindex: " << index << "\tte: " << element << std::endl;  

	  /// here we *only* want collections with no specified chain
	  /// name, then we look in storegate for the collections directly

	  selectorTest.clear();

	  bool found = false;
	  
	  std::string collection_test = collectionname;
	  size_t pos = collectionname.find("/");
	  if ( pos!=std::string::npos ) collection_test = collectionname.substr( pos+1, collectionname.size()-pos );

	  if (m_provider->evtStore()->contains<Rec::TrackParticleContainer>(collection_test)) {
	    found = selectTracks<Rec::TrackParticleContainer>( &selectorTest, collectionname );
	  }
	  else if (m_provider->evtStore()->contains<xAOD::TrackParticleContainer>(collection_test)) {
	    found = selectTracks<xAOD::TrackParticleContainer>( &selectorTest, collectionname );
	  }
	  else if (m_provider->evtStore()->contains<TrigInDetTrackCollection>(collection_test)) {
	    found = selectTracks<TrigInDetTrackCollection>( &selectorTest, collectionname );
	  }
	  else if (m_provider->evtStore()->contains<TrackCollection>(collection_test)) {
	    found = selectTracks<TrackCollection>( &selectorTest, collectionname );
	  }
	  else { 
	    m_provider->msg(MSG::WARNING) << "\tcollection " << collectionname << " not found" << endmsg;
	  }
	  

	  /// now retrieve any verttices for the analysis

	  std::vector<TIDA::Vertex> tidavertices;

	  m_provider->msg(MSG::DEBUG) << "\tFetch xAOD::VertexContainer with key " << vtx_name << endmsg;
	    
	  if ( vtx_name!="" ) { 
	        
	    m_provider->msg(MSG::DEBUG) << "\tFetch xAOD::VertexContainer with key " << vtx_name << endmsg;
	        
	    /// MT Vertex access
	        
	    const xAOD::VertexContainer* xaodVtxCollection = 0;
	    
	    if ( m_provider->evtStore()->retrieve( xaodVtxCollection, vtx_name ).isFailure() ) {
	      m_provider->msg(MSG::WARNING) << "xAOD vertex container not found with key " << vtx_name <<  endmsg;
	    }
	    
	    if ( xaodVtxCollection!=0 ) { 
	            
	      m_provider->msg(MSG::DEBUG) << "\txAOD::VertexContainer found with size  " << xaodVtxCollection->size()
					  << "\t" << vtx_name << endmsg;
	            
	      xAOD::VertexContainer::const_iterator vtxitr = xaodVtxCollection->begin(); 
	            
	      for (  ; vtxitr!=xaodVtxCollection->end()  ;  vtxitr++ ) {
		
		/// leave this code commented so that we have a record of the change - as soon as we can 
		/// fix the missing track multiplicity from the vertex this will need to go back  
		//  if ( ( (*vtxitr)->nTrackParticles()>0 && (*vtxitr)->vertexType()!=0 ) || vtx_name=="EFHistoPrmVtx" ) {

		// useful debug comment, left for debugging purposes ...
		//		std::cout << "SUTT  xAOD::Vertex::type() " << (*vtxitr)->type() 
		//			  << "\tvtxtype " << (*vtxitr)->vertexType() 
		//			  << "\tntrax "   << (*vtxitr)->nTrackParticles() 
		//			  << "\tz "       << (*vtxitr)->z() << std::endl; 

		if ( (*vtxitr)->vertexType()!=0  || vtx_name=="EFHistoPrmVtx" ) {
		  tidavertices.push_back( TIDA::Vertex( (*vtxitr)->x(),
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



	  if ( found ) { 
	    
	    m_event->addChain( chainname );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    if ( vtx_name!="" ) m_event->back().back().addVertices( tidavertices );
	    m_event->back().back().addTracks(selectorTest.tracks());

	    if ( selectorTest.getBeamX()!=0 || selectorTest.getBeamY()!=0 || selectorTest.getBeamZ()!=0 ) { 
	      std::vector<double> beamline_;
	      beamline_.push_back( selectorTest.getBeamX() );
	      beamline_.push_back( selectorTest.getBeamY() );
	      beamline_.push_back( selectorTest.getBeamZ() );
	      m_event->back().back().addUserData(beamline_);
	    }
	    else { 
	      m_event->back().back().addUserData(beamline);
	    }
	    
	    int Ntest = selectorTest.tracks().size();
	    
	    m_provider->msg(MSG::DEBUG) << "collection " << collectionname << "\ttest tracks.size() " << Ntest << endmsg; 
	    for ( int ii=Ntest ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  test track " << ii << " " << *selectorTest.tracks()[ii] << endmsg;
	  }
	}	  


	std::string ElectronRef[7] =  { 
	  "", 
	  "TightCB", "MediumCB", "LooseCB",
	  "TightLH", "MediumLH", "LooseLH" };
 

	/// new electron selection 

	for ( size_t ielec=0 ; ielec<m_electronType.size() ; ielec++ ) {
	  /// hmm, if we stored the types as a map it would be more 
	  /// straightforward than having to stick all this in a loop

	  int itype = -1;
	  for ( int it=0 ; it<7 ; it++ ) if ( m_electronType[ielec]==ElectronRef[it] ) itype = it; 
	  if ( itype<0 ) continue;

	  std::vector<TrackTrigObject> elevec;
	  
	  /// useful debug information ...
	  //	  std::cout << "\tElectrons selection " << ielec << " " << m_electronType[ielec] 
	  //		    << "\t" << itype << " " << ElectronRef[itype] << "\t" << m_rawElectrons[ielec] << std::endl;
	  
	  int Nel_ = processElectrons( selectorRef, &elevec, itype, ( m_rawElectrons[ielec]=="raw" ? true : false ) );
	  
	  	  
          if ( Nel_ < 1 ) continue;
      
          Nel += Nel_;	

	  std::string echain = std::string("Electrons");
          if ( m_electronType[ielec]!="" )    echain += "_" + m_electronType[ielec];
	  if ( m_rawElectrons[ielec]=="raw" ) echain += "_raw";
	  
	  m_event->addChain( echain );
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorRef.tracks());
	  m_event->back().back().addObjects( elevec );

	  // leave this in util fully validated ...
	  // std::cout << m_event->back() << std::endl;
	  
	  if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	    std::vector<double> beamline_;
	    beamline_.push_back( selectorRef.getBeamX() );
	    beamline_.push_back( selectorRef.getBeamY() );
	    beamline_.push_back( selectorRef.getBeamZ() );
	    m_event->back().back().addUserData(beamline_);
	  }
	  else { 	  
	    m_event->back().back().addUserData(beamline);
	  }
	}
	
       

	std::string MuonRef[5] =  { "", "Tight", "Medium", "Loose", "VeryLoose" };

	/// get muons 
	for ( size_t imuon=0 ; imuon<m_muonType.size() ; imuon++ ) {
	  
	  m_provider->msg(MSG::DEBUG) << "fetching offline muons " << endmsg; 

          int muonType = -1;
          for ( int it=0 ; it<5 ; it++ ) if ( m_muonType[imuon] == MuonRef[it] ) muonType=it; 
          if ( muonType<0 ) continue; 

	  int Nmu_ = processMuons( selectorRef, muonType );

          if ( Nmu_ < 1 ) continue;

          Nmu += Nmu_;

	  m_provider->msg(MSG::DEBUG) << "found " << Nmu << " offline muons " << endmsg; 

          std::string mchain = "Muons";
          if ( m_muonType[imuon]!="" )  mchain += "_" + m_muonType[imuon];

	  m_event->addChain(mchain);
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorRef.tracks());
	  if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	      std::vector<double> beamline_;
	      beamline_.push_back( selectorRef.getBeamX() );
	      beamline_.push_back( selectorRef.getBeamY() );
	      beamline_.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(beamline_);
	  }
	  else { 	  
	      m_event->back().back().addUserData(beamline);
	  }

	  m_provider->msg(MSG::DEBUG) << "ref muon tracks.size() " << selectorRef.tracks().size() << endmsg; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  ref muon track " << ii << " " << *selectorRef.tracks()[ii] << endmsg;  
	}
	



	/// get muons 
	if ( m_doMuonsSP ) { 
	  
	  m_provider->msg(MSG::DEBUG) << "fetching offline muons " << endmsg; 

          int muonType = 0;

	  Nmu += processMuons( selectorRef, muonType );

	  m_provider->msg(MSG::DEBUG) << "found " << Nmu << " offline muons " << endmsg; 

	  m_event->addChain("MuonsSP");
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorRef.tracks());

	  m_provider->msg(MSG::DEBUG) << "ref muon tracks.size() " << selectorRef.tracks().size() << endmsg; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  ref muon track " << ii << " " << *selectorRef.tracks()[ii] << endmsg;  
	}
	


	/// new tau selection 
	std::string TauRef[4] = { "", "Tight", "Medium", "Loose" };
	

	for ( size_t itau=0 ; itau<m_tauType.size() ; itau++ ) {
	  /// hmm, if we stored the types as a map it would be more 
	  /// straightforward than having to stick all this in a loop

	  int itype = -1;
	  for ( int it=0 ; it<4 ; it++ ) if ( m_tauType[itau]==TauRef[it] ) itype = it; 
	  if ( itype<0 ) continue;

	  /// use same threshold for 1 and 3 prong ??
	  int requireNtracks = 0;
	  if  ( m_tauProngs[itau]=="3Prong" ) requireNtracks = 3;	
	  if  ( m_tauProngs[itau]=="1Prong" ) requireNtracks = 1;

	  std::vector<TrackTrigObject> tauvec; 

	  int Ntau_ = processTaus( selectorRef, &tauvec, itype, requireNtracks, 20000 ); 

	  Ntau += Ntau_;

	  if ( Ntau_ > 0 ) { 
	    /// only add a tau collection if there are actually the 
	    /// relevant taus
	    std::string tchain = std::string("Taus");
	    if (   m_tauType[itau] != "" ) tchain += "_" + m_tauType[itau];
	    if ( m_tauProngs[itau] != "" ) tchain += "_" + m_tauProngs[itau];
	    
	    m_event->addChain( tchain );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    m_event->back().back().addTracks(selectorRef.tracks());
	    m_event->back().back().addObjects( tauvec ) ; 

	    // leave this in util fully validated ...
	    //	    std::cout << m_event->back() << std::endl;

	    if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	      std::vector<double> beamline_;
	      beamline_.push_back( selectorRef.getBeamX() );
	      beamline_.push_back( selectorRef.getBeamY() );
	      beamline_.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(beamline_);
	    }
	    else { 	  
	      m_event->back().back().addUserData(beamline);
	    }
	  }
	}
	
	/// useful debug information 0 leave here 
	//	std::cout << "SUTT Ntaus: " << Ntau << std::endl;
	
	if ( Nmu==0 && Noff==0 && Nel==0 && Ntau==0 ) m_provider->msg(MSG::DEBUG) << "No offline objects found " << endmsg;
	else foundOffline = true;


	// now loop over all relevant chains to get the trigger tracks...
	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  

		// create chains for ntpl

		/// get the chain name
		const std::string& chainName = m_chainNames[ichain].head();

		/// and the name of the collection (if any)    
		const std::string& collectionName = m_chainNames[ichain].tail();


		if( chainName.find("L2_")==std::string::npos && 
		    chainName.find("EF_")==std::string::npos && 
		    chainName.find("HLT_")==std::string::npos ) continue;

		
		m_provider->msg(MSG::DEBUG) << "chain " << chainName 
					    << "\tprescale " << (*m_tdt)->getPrescale(chainName)
					    << "\tpass "     << (*m_tdt)->isPassed(chainName) << " physics " 
					    << "  (req dec " << (*m_tdt)->isPassed(chainName, decisiontype_ ) << " dec type " << decisiontype_ << ")"
					    << endmsg;
		
		/// now decide whether we want all the TEs for this chain, or just those 
		/// that are still active
		unsigned decisiontype;
                if ( m_chainNames[ichain].passed() ) decisiontype = decisiontype_;
		else                                 decisiontype = TrigDefs::alsoDeactivateTEs;


		/// if the chain did not pass, skip this chain completely 
		if ( !(*m_tdt)->isPassed( chainName, decisiontype_ ) ) continue;


		/// new MT TDT feature access  
		

		std::string roi_key  = m_chainNames[ichain].roi();
		std::string vtx_name = m_chainNames[ichain].vtx();


#if 0
		/// this code needs to be here to be eventually replaced when 
		/// the TDT combination feature retrieval has been implemented
		/// at that point it can be replaced by the appropriate 
		/// code using the new TDT feature access

		if ( roi_name!="" ) { 

		  std::string roi_name_tmp = roi_name;
		  std::string roi_tename   = "";
		  
		  if ( roi_name.find("/")!=std::string::npos ) { 
		    roi_name_tmp = roi_name.substr( roi_name.find("/")+1, roi_name.size()-roi_name.find("/") );
		    roi_tename   = roi_name.substr( 0, roi_name.find("/") );
		  }
		  
		  roist = comb->get<TrigRoiDescriptor>( roi_name_tmp, decisiontype_, roi_tename );
		  
		  if ( roist.size()>0 ) { 
		    for ( unsigned ir=0 ; ir<roist.size() ; ir++ ) m_provider->msg(MSG::DEBUG) << "\t\tRetrieved roi  " << roi_name << "\t" << *roist[ir].cptr() << endmsg; 
		  }
		  else { 
		    m_provider->msg(MSG::WARNING) << "\t\tRequested roi  " << roi_name << " not found" << endmsg; 
		  }
		  
		}
		else { 
		  roist = comb->get<TrigRoiDescriptor>("forID1"); 
		  if ( roist.empty() ) roist = comb->get<TrigRoiDescriptor>("forID"); 
		  if ( roist.empty() ) roist = comb->get<TrigRoiDescriptor>(""); 
		  if ( roist.empty() ) roist = comb->get<TrigRoiDescriptor>("initialRoI"); 
		}			  
#endif
		

		unsigned feature_type = TrigDefs::lastFeatureOfType;

		if ( roi_key!="" ) feature_type = TrigDefs::allFeaturesOfType;

		std::vector< TrigCompositeUtils::LinkInfo<TrigRoiDescriptorCollection> > rois = 
		  (*m_tdt)->template features<TrigRoiDescriptorCollection>( chainName, 
									    decisiontype, 
									    roi_key, 
									    //   TrigDefs::lastFeatureOfType, 
									    //   TrigDefs::allFeaturesOfType,
									    feature_type,
									    "roi" );
		 
		/// leave this here for the moment until we know everything is working ...
		// const unsigned int featureCollectionMode = const std::string& navElementLinkKey = "roi") const;
		
		int iroi = 0; /// count of how many rois processed so far

		/// if no rois for this chain then move along

		if ( rois.size()==0 ) continue;

		/// create the analysis chain

		m_event->addChain( m_chainNames[ichain] );

		TIDA::Chain& chain = m_event->back();


		for ( const TrigCompositeUtils::LinkInfo<TrigRoiDescriptorCollection> roi_info : rois ) {
		    
		  iroi++;

		  /// don't extract any additional rois if a superRoi is requested: 
		  /// In this case, the superRoi would be shared between the different 
		  /// chains 

		  if ( roi_key=="SuperRoi" && iroi>1 ) continue; 
		  
		  const ElementLink<TrigRoiDescriptorCollection> roi_link = roi_info.link;

		  /// check this is not a spurious TDT match
		  if ( roi_key!="" && roi_link.dataID()!=roi_key ) continue;

		  const TrigRoiDescriptor* const* roiptr = roi_link.cptr();

		  if ( roiptr == 0 ) { 
		    //    std::cerr << "\treadback link is null  DAMMIT !!!" << std::endl;
		    continue;
		  }  

		  if (m_provider->msg().level() <= MSG::VERBOSE) {
		    m_provider->msg(MSG::VERBOSE) << " RoI descriptor for seeded chain " << chainName << " " << **roiptr << endmsg;
		  }

		  TIDARoiDescriptor* roi_tmp = new TIDARoiDescriptor( TIDARoiDescriptorBuilder(**roiptr) );		   

		  /// get the tracks 


		  m_provider->msg(MSG::VERBOSE) << "TIDARoi " << *roi_tmp << "\tcollectionName: " << collectionName << endmsg;
			      
		  /// this should *never* be the case, and we should only run this 
		  /// bit of code once the first time round the loop anyhow
			      
		  selectorTest.clear();


		  if ( chainName.find("HLT_")!=std::string::npos ) {
		    if ( selectTracks<xAOD::TrackParticleContainer>( &selectorTest, roi_link,  collectionName ) ); 
		    else {
		      if (m_provider->msg().level() <= MSG::DEBUG) {
			m_provider->msg(MSG::WARNING) << "\tNo track collection " << collectionName << " found"  << endmsg;  
		      }
		    }
		  }
		  
		  /// fetch vertices if available ...
		  
		  std::vector<TIDA::Vertex> tidavertices;	
		  
		  if ( vtx_name!="" ) { 
		    
		    m_provider->msg(MSG::DEBUG) << "\tFetch xAOD::VertexContainer for chain " << chainName << " with key " << vtx_name << endmsg;
		    
		    /// MT Vertex access
		    
		    std::pair< xAOD::VertexContainer::const_iterator, 
			       xAOD::VertexContainer::const_iterator > vtx_itrpair = getCollection<xAOD::VertexContainer>( roi_link, vtx_name );
		    
		    if ( vtx_itrpair.first == vtx_itrpair.second ) { 
		      m_provider->msg(MSG::WARNING) << "\tNo xAOD::Vertex for chain " << chainName << " for key " << vtx_name << endmsg;
		    }
		    else {
		      
		      m_provider->msg(MSG::DEBUG) << "\txAOD::VertexContainer found with size  " << (vtx_itrpair.second - vtx_itrpair.first) 
						 << "\t" << vtx_name << endmsg;
		      
		      xAOD::VertexContainer::const_iterator vtxitr = vtx_itrpair.first; 
		      
		      for (  ; vtxitr!=vtx_itrpair.second  ;  vtxitr++ ) {
			
			/// leave this code commented so that we have a record of the change - as soon as we can 
			/// fix the missing track multiplicity from the vertex this will need to go back  
			//  if ( ( (*vtxitr)->nTrackParticles()>0 && (*vtxitr)->vertexType()!=0 ) || vtx_name=="EFHistoPrmVtx" ) {
			if ( (*vtxitr)->vertexType()!=0  || vtx_name=="EFHistoPrmVtx" ) {
			  tidavertices.push_back( TIDA::Vertex( (*vtxitr)->x(),
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

#if 0 
		  //// not yet ready to get the jet yet - this can come once everything else is working 
		  // now get the jets if they are present
		  std::vector<TrackTrigObject> jets; 
		  if ( chainName.find("HLT_j")!=std::string::npos ) { 
		    if ( get_jets( comb, jets ) == 0 ) m_provider->msg(MSG::WARNING) << "\tjets could not be retrieved " << endmsg; 
		  }			  		    
#endif
		  
		  const std::vector<TIDA::Track*>& testTracks = selectorTest.tracks();
		  m_provider->msg(MSG::DEBUG) << "\ttest tracks.size() " << testTracks.size() << endmsg; 
		  for (unsigned int ii=0; ii < testTracks.size(); ii++) {
		    m_provider->msg(MSG::DEBUG) << "  test track " << ii << "for chain " << chainName + ":" + collectionName << " " << *testTracks[ii] << endmsg;  
		  }
		  
		  
		  // only add chain if there are any rois - also add beamline position for postprocessing
		  
		  
		  if ( roi_tmp == 0 ) { 
		    if ( testTracks.size()>0 ) m_provider->msg(MSG::WARNING) << "\ttest tracks.size() " << testTracks.size() << "found but no roi!!!" << endmsg;
		    roi_tmp = new TIDARoiDescriptor(true);
		  }
		  
		  
		  
		  chain.addRoi( *roi_tmp );
		  chain.back().addTracks(testTracks);
		  chain.back().addVertices(tidavertices);
		  chain.back().addUserData(beamline_online);
		  
#if 0
		  /// jets can't be added yet
		  if ( chainName.find("HLT_j")!=std::string::npos ) chain.back().addObjects( jets );
#endif
		  
		  if ( selectorTest.getBeamX()!=0 || selectorTest.getBeamY()!=0 || selectorTest.getBeamZ()!=0 ) { 
		    std::vector<double> beamline_;
		    beamline_.push_back( selectorTest.getBeamX() );
		    beamline_.push_back( selectorTest.getBeamY() );
		    beamline_.push_back( selectorTest.getBeamZ() );
		    chain.back().addUserData(beamline_);
		  }
		  else { 	  
		    if ( beamline_online.size()>3 ) chain.back().addUserData(beamline_online);
		  }
		  
		  if ( roi_tmp ) delete roi_tmp;
		  roi_tmp = 0;
		}
		
		
	}

#if 0
	/// don;t include this code at the moment ...

	{ 
	  /// strip out the offline tracks not in any Roi ...

	  if ( filterOnRoi() || m_ptmin>0 ) { 
	    
	    TIDA::Chain* offline = 0;
	    
	    std::vector<std::string> chainnames = m_event->chainnames();
	    
	    /// get the offline chain
	    
	    for ( size_t ic=chainnames.size() ; ic-- ; ) {
	      if ( chainnames[ic] == "Offline" ) {
		offline = &(m_event->chains()[ic]);
		break;
	      }
	    }
	    
	    if ( offline ) { 
	      
	      std::vector<TIDA::Chain>& chains = m_event->chains();
	      std::vector<TIDA::Chain>::iterator citr = chains.begin();
	      
	      std::vector<std::pair<double,double> > philims;
	      
	      for ( ; citr!=chains.end() ; citr++ ) {
		if ( citr->name().find("HLT_")!=std::string::npos ) { 
		  for ( size_t ir=0 ; ir<citr->size() ; ir++ ) {
		    TIDARoiDescriptor& roi = citr->rois()[ir].roi();
		    if ( roi.composite() ) { 
		      for ( size_t isub=0 ; isub<roi.size() ; isub++ ) { 
			philims.push_back( std::pair<double,double>( roi[isub]->phiMinus(), roi[isub]->phiPlus() ) ); 
		      }
		    }
		    else philims.push_back( std::pair<double,double>( roi.phiMinus(), roi.phiPlus() ) ); 
		  }
		}
	      }
	      
	      remove_duplicates( philims );

	      for ( size_t iroi=0 ; iroi<offline->size() ; iroi++ ) {
		
		std::vector<TIDA::Track>& tracks = offline->rois()[iroi].tracks();
		
		/// may well put the reporting back in, so leaving this 
		/// this in place  
		//  size_t Noffline = tracks.size();

		for ( std::vector<TIDA::Track>::iterator it=tracks.begin() ; it<tracks.end() ; ) {
		  bool inc = true;
		  if ( m_ptmin>0 ) { 
		    if ( std::fabs(it->pT())<m_ptmin ) { inc=false; tracks.erase( it ); }
		  }
		  if ( inc && filterOnRoi() ) { 
		    bool remove_track = true;
		    for ( size_t isub=0 ; isub<philims.size() ; isub++ ) { 
		      
		      if ( philims[isub].first < philims[isub].second ) { 
			if ( it->phi()>=philims[isub].first && it->phi()<=philims[isub].second ) { 
			  remove_track = false; 
			  break;
			}
		      }
		      else  {
			if ( it->phi()>=philims[isub].first || it->phi()<=philims[isub].second ) { 
			  remove_track = false; 
			  break;
			}
		      }
		    }
		    if ( remove_track ) { inc=false; tracks.erase( it ); }
		  }
		  if ( inc ) it++;
		}
		
		/// may well put the reporting back in, so leaving this 
		/// this in place  		
		//  m_provider->msg(MSG::DEBUG) << "TIDA::Roi offline track reduction: " << Noffline << " -> " << tracks.size() << endmsg;
		
	      }
	     
	    }
	    	    
	  }
	}

#endif

	if ( mTree ) mTree->Fill();
	
}



