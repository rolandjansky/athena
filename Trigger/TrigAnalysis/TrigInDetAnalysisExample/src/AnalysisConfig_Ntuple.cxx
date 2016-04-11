//
//   @file    AnalysisConfig_Ntuple.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    

// McParticleEvent includes
#include <cstdio>

#include <sys/time.h> 

#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"

#include "TrigInDetAnalysis/Filter_AcceptAll.h"

#include "TrigInDetAnalysisUtils/Filter_etaPT.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/Filters.h"

#include "TrigInDetAnalysisExample/AnalysisConfig_Ntuple.h"
// #include "TrigInDetAnalysisUtils/OfflineObjectSelection.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"


#include "VxVertex/VxContainer.h"

#include "muonEvent/MuonContainer.h"

#include "egammaEvent/ElectronContainer.h"

#include "tauEvent/TauJetContainer.h"

//#include "JetEvent/JetCollection.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDecisionTool/ExpertMethods.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// xAOD
// not actual XAOD, but this file safely includes 
// the xAOD::TrackParticle header if it exists
#include "TrkParticleCreator/TrackParticleCreatorTool.h"




bool tida_first = true;



std::string date() { 
  time_t _t;
  time(&_t);
  std::string _mtime = ctime(&_t);
  _mtime.erase( std::remove(_mtime.begin(), _mtime.end(), '\n'), _mtime.end() );
  return _mtime;
}


//function to find true taus
const HepMC::GenParticle* fromParent( int pdg_id, const HepMC::GenParticle* p, bool printout=false ) { 

  if ( p==0 ) return 0;
  if (std::abs(p->pdg_id())==11 || std::abs(p->pdg_id())==13 ) return 0; //don't want light leptons from tau decays
  if ( std::abs(p->pdg_id())==pdg_id ) return p;   /// recursive stopping conditions
    
  const HepMC::GenVertex* vertex = p->production_vertex();
  
  if ( vertex==0 ) return 0; // has no production vertex !!!
  if ( vertex->particles_in_size() < 1 ) return 0;  /// recursive stopping conditions
  //if ( vertex->particles_in_size() > 1 ) std::cout << "more than 1 parent!!!" << std::endl;
  
  if ( printout ) { 
    TruthParticle t(p);
    std::cout << "particle " << *p << "  " << t.pdgId() << "\tparent " << p << std::endl;
  }

  HepMC::GenVertex::particles_in_const_iterator in  = vertex->particles_in_const_begin();
  HepMC::GenVertex::particles_in_const_iterator end = vertex->particles_in_const_end();
  while ( in!=end ) {
    const HepMC::GenParticle* parent = fromParent( pdg_id, *in, printout );
    TruthParticle t(*in);
    if ( printout ) std::cout << "\tvalue for particle " << *in << "  " << t.pdgId() << "\tparent " << parent << std::endl;
    if ( parent && std::abs(parent->pdg_id())==pdg_id) { 
      if ( printout ) std::cout << "found tau! - in parents" << std::endl; 
      return parent;
    }   /// recursive stopping conditions
    in++;
  }
  
  return 0;
}
  





void AnalysisConfig_Ntuple::loop() {

  m_provider->msg(MSG::INFO) << "[91;1m" << "AnalysisConfig_Ntuple::loop() for " << m_analysisInstanceName << " compiled " << __DATE__ << " " << __TIME__ << "\t: " << date() << "[m" << endreq;

	// get (offline) beam position
	double xbeam = 0;
	double ybeam = 0;
	double zbeam = 0;
	std::vector<double> beamline;
	if ( m_iBeamCondSvc ) {

#ifdef EIGEN_GEOMETRY_MODULE_H	  
	  const Amg::Vector3D& vertex = m_iBeamCondSvc->beamPos();
	  xbeam = vertex[0];
	  ybeam = vertex[1];
	  zbeam = vertex[2];
#else
	  HepGeom::Point3D<double> vertex = m_iBeamCondSvc->beamPos();
	  xbeam = vertex.x();
	  ybeam = vertex.y();
	  zbeam = vertex.z();
#endif
	  m_provider->msg(MSG::INFO) << " using beam position\tx=" << xbeam << "\ty=" << ybeam << "\tz=" << zbeam <<endreq; 
	  beamline.push_back(xbeam);
	  beamline.push_back(ybeam);
	  beamline.push_back(zbeam);
	  m_provider->msg(MSG::INFO) << " beamline values : " << beamline[0] << "\t" << beamline[1]  << "\t" << beamline[2] << endreq;	
	}
	else { 
	  m_provider->msg(MSG::INFO) << " could not find BeamCondSvc " << endreq;
	}

	// get (online) beam position
	double xbeam_online = 0;
	double ybeam_online = 0;
	double zbeam_online = 0;

	std::vector<double> beamline_online;

	if ( m_iOnlineBeamCondSvc ) {

#ifdef EIGEN_GEOMETRY_MODULE_H	  
	  const Amg::Vector3D& vertex = m_iOnlineBeamCondSvc->beamPos();
	  xbeam_online = vertex[0];
	  ybeam_online = vertex[1];
	  zbeam_online = vertex[2];
#else
	  // TRKPARAMETERS_MEASUREDPERIGEE_H
	  HepGeom::Point3D<double> vertex = m_iOnlineBeamCondSvc->beamPos();
	  xbeam_online = vertex.x();
	  ybeam_online = vertex.y();
	  zbeam_online = vertex.z();
#endif
	  beamline_online.push_back( xbeam_online );
	  beamline_online.push_back( ybeam_online );
	  beamline_online.push_back( zbeam_online );

	  m_provider->msg(MSG::INFO) << " using online beam position" 
				     << "\tx=" << xbeam_online 
				     << "\ty=" << ybeam_online 
				     << "\tz=" << zbeam_online << endreq; 
	}
	else { 
	  m_provider->msg(MSG::INFO) << " could not find OnlineBeamCondSvc " << endreq;
	}

	m_provider->msg(MSG::INFO) << " offline beam position\tx=" << xbeam        << "\ty=" << ybeam        << "\tz=" << zbeam        << endreq; 
	m_provider->msg(MSG::INFO) << " online  beam position\tx=" << xbeam_online << "\ty=" << ybeam_online << "\tz=" << zbeam_online << endreq; 

	/// list the configured chains

	static std::set<std::string> configuredHLTChains;

	std::vector<ChainString> chainNames;

	if ( tida_first ) { 

		std::vector<std::string> configuredChains  = (*m_tdt)->getListOfTriggers("L2_.*, EF_.*, HLT_.*");

		m_provider->msg(MSG::DEBUG) << "[91;1m" << configuredChains.size() << " Configured Chains" << "[m" << endreq;
		for ( unsigned i=0 ; i<configuredChains.size() ; i++ ) { 
		  m_provider->msg(MSG::DEBUG) << "[91;1m" << "Chain " << configuredChains[i] << "   (ACN)[m" << endreq;
		  configuredHLTChains.insert( configuredChains[i] );
		  
		}

		tida_first = false;

		//		std::cout << "input chains" << std::endl;
		//		for ( unsigned ic=0 ; ic<m_chainNames.size() ; ic++ ) std::cout << "chains " << ic << "\t" << m_chainNames[ic] << std::endl;


		std::vector<ChainString>::iterator chainitr = m_chainNames.begin();

		//		std::cout << "SUTT number of chains " << m_chainNames.size() << std::endl; 

		/// handle wildcard chain selection - but only the first time
		while ( chainitr!=m_chainNames.end() ) {

		  //		  std::cout << "chain name " << *chainitr << " (ACN) " << std::endl;

		  /// get chain
		  ChainString& chainName = (*chainitr);

		  //		  std::cout << "chain name " << chainName << " (ChainString roi " << chainName.roi() << "\tvtx " << chainName.vtx() << " ACN)" << std::endl;

		  /// check for wildcard ...
		  //if ( chainName.head().find("*")!=std::string::npos ) { 

		    //		    std::cout << "wildcard chains: " << chainName << std::endl;


		    /// get matching chains
		    std::vector<std::string> selectChains  = (*m_tdt)->getListOfTriggers( chainName.head() );

		    //		    std::cout << "selected chains for " << chainName << " -> " << selectChains.size() << " chains" << std::endl;

		    for ( unsigned iselected=0 ; iselected<selectChains.size() ; iselected++ ) {
 
		      if ( chainName.tail()!="" )    selectChains[iselected] += ":key="+chainName.tail();
		      if ( chainName.extra()!="" )   selectChains[iselected] += ":index="+chainName.extra();
		   
		      if ( chainName.element()!="" ) selectChains[iselected] += ":te="+chainName.element(); 
		      if ( chainName.roi()!="" )     selectChains[iselected] += ":roi="+chainName.roi();
		      if ( chainName.vtx()!="" )     selectChains[iselected] += ":vtx="+chainName.vtx();
		     
		      if ( !chainName.passed() )    selectChains[iselected] += ";DTE";
		     
		      /// replace wildcard with actual matching chains ...
		      chainNames.push_back( ChainString(selectChains[iselected]) );

		      m_provider->msg(MSG::INFO) << "[91;1m" << "Matching chain " << selectChains[iselected] << "[m" << endreq;
		     
		    }
		  
		    chainitr++;
		}
		
		//	  std::cout << "output chains" << std::endl;
		//	  for ( unsigned ic=0 ; ic<m_chainNames.size() ; ic++ ) std::cout << "chains " << ic << "\t" << m_chainNames[ic] << std::endl;

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
	if(m_TruthPdgId!=0 && m_TruthPdgId!=15) truthFilter = &filter_pdgIdpTeta;

	TrigTrackSelector selectorTruth( truthFilter ); 
	// TrigTrackSelector selectorRef( &filterRef ); 
	TrigTrackSelector selectorRef( &filter_etaPT ); 
	TrigTrackSelector selectorTest( &filter ); 
	//	TrigTrackSelector selectorFTK( &filter_etaPT ); 

	selectorTruth.setBeamline( xbeam, ybeam, zbeam ); 
	selectorRef.setBeamline( xbeam, ybeam, zbeam ); 
	selectorTest.setBeamline( xbeam_online, ybeam_online, zbeam_online ); 


	selectorTruth.correctTracks( true );
	selectorRef.correctTracks( true );
	selectorTest.correctTracks( true );
	

	// clear the ntuple TIDA::Event class
	m_event->clear();


	const EventInfo* pEventInfo = 0;
	unsigned run_number         = 0;
	unsigned event_number       = 0;

	unsigned lumi_block         = 0;
	unsigned bunch_crossing_id  = 0;

	unsigned time_stamp = 0;
	double mu_val = 0;

	if ( m_provider->evtStore()->retrieve(pEventInfo).isFailure() ) {
		m_provider->msg(MSG::DEBUG) << "Failed to get EventInfo " << endreq;
	} 
	else {
		run_number        = pEventInfo->event_ID()->run_number();
		event_number      = pEventInfo->event_ID()->event_number();
		lumi_block        = pEventInfo->event_ID()->lumi_block();
		time_stamp        = pEventInfo->event_ID()->time_stamp();
		bunch_crossing_id = pEventInfo->event_ID()->bunch_crossing_id();
		mu_val            = pEventInfo->averageInteractionsPerCrossing();
	}

	m_provider->msg(MSG::INFO) << "run "     << run_number 
		<< "\tevent " << event_number 
		<< "\tlb "    << lumi_block << endreq;

	m_event->run_number(run_number);
	m_event->event_number(event_number);
	m_event->lumi_block(lumi_block);
	m_event->time_stamp(time_stamp);
	m_event->bunch_crossing_id(bunch_crossing_id);
	m_event->mu(mu_val);



	//////////////////////////////////////////////////////////////////////////////
	/// some noddy debug code

	//    Trig::ChainGroupManager* manager = (*m_tdt)->getChainGroupManager();
	//    const Trig::ConfigurationAccess& CA = (*m_tdt)->config();
	//    const Trig::DecisionAccess&      DA = (*m_tdt)->decision();

	// build a chain group on the fly as if we were users
	//    std::cout << CA.testFunction("L2_*,EF_*") << std::endl;

	// build a chain group on the fly and use the reference
	const Trig::ChainGroup* L2chain=(*m_tdt)->getChainGroup("L2_.*");
	const Trig::ChainGroup* EFchain=(*m_tdt)->getChainGroup("EF_.*");
	const Trig::ChainGroup* HLTchain=(*m_tdt)->getChainGroup("HLT_.*");

	//	m_provider->msg(MSG::DEBUG) << "[91;1m" 
	m_provider->msg(MSG::INFO) << "[91;1m" 
		<< "L2 pass " << L2chain->isPassed()  << "\t" 
		<< "EF pass " << EFchain->isPassed()  << "\t" 
		<< "HLT pass " << HLTchain->isPassed() << "[m"
		<< endreq;

	
	std::vector<std::string> _conf = (*m_tdt)->getListOfTriggers("HLT_.*");
	
	m_provider->msg(MSG::INFO) << endreq;
	m_provider->msg(MSG::INFO) << "configured chains " << _conf.size() << endreq;

#if 0
	for ( unsigned ic=0 ; ic<_conf.size() ; ic++ ) { 
	  bool p = (*m_tdt)->isPassed( _conf[ic] );
	  
	  if ( p ) m_provider->msg(MSG::INFO) << "[91;1m" << " Configured Chain " << _conf[ic] << " " << p << "[m" << endreq;
	  else     m_provider->msg(MSG::INFO)               << " Configured Chain " << _conf[ic] << " " << p           << endreq;

	}

	m_provider->msg(MSG::INFO) << endreq;
#endif

	///////////////////////////////////////////////////////////////////////////////



	/// check whether the chains have actually run, otherwise there's no point
	/// doing anything

	bool analyse = false;
	// bool analyse = true;


	unsigned _decisiontype = TrigDefs::Physics;
	
	if ( requireDecision() ) _decisiontype = TrigDefs::requireDecision;
      

	int passed_chains = 0;

	m_provider->msg(MSG::INFO) << "Checking " << m_chainNames.size() << " chains" << endreq;
	
	if ( m_chainNames.empty() ) {
	  m_provider->msg(MSG::WARNING) << "No chains to check" << endreq;
	  //  return;
	}

	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  
		const std::string& chainName = m_chainNames[ichain].head();

		// Only for trigger chains

		//		m_provider->msg(MSG::INFO) << "\tchain " << chainName << endreq;
		

		if (chainName.find("L2")  == std::string::npos && 
		    chainName.find("EF")  == std::string::npos && 
		    chainName.find("HLT") == std::string::npos ) continue;

		if ( configuredHLTChains.find(chainName)==configuredHLTChains.end() ) {
			m_provider->msg(MSG::WARNING) << "[91;1m" << "Chain " << chainName 
				<< " is not configured for this event" << "[m"
				<< endreq;
			continue;
		}

		
		std::string roistring = "";
		if ( m_chainNames[ichain].roi()!=""  ) roistring += "\troi " + m_chainNames[ichain].roi();  


		//		const Trig::ChainGroup* _chain=(*m_tdt)->getChainGroup(chainName);
		//		m_provider->msg(MSG::INFO) << "Chain "  << chainName << "\troi " << roistring 
		//					   << "\tpass " << _chain->isPassed() << "\tCH  FIXED" << endreq;

		bool passPhysics = (*m_tdt)->isPassed(chainName); 

		m_provider->msg(MSG::INFO) << "Chain "  << chainName << "\troi " << roistring 
					   << "\tpres " << (*m_tdt)->getPrescale(chainName)
					   << ( passPhysics ? "[91;1m" : "" ) << "\tpass physics  " <<  passPhysics << ( passPhysics ? "[m" : "" ) 
					   << "\t: ( pass " << (*m_tdt)->isPassed(chainName, _decisiontype ) << "\tdec type " << _decisiontype << " ) " << endreq;


		//		if ( (*m_tdt)->isPassed(chainName) ) { 
		if ( (*m_tdt)->isPassed(chainName, _decisiontype) ) { 
		  analyse = true;
		  passed_chains++;
		}

	}/// finished loop over chains


	/// bomb out if no chains passed and not told to keep all events  
	if ( !analyse && !m_keepAllEvents ) { 
	  m_provider->msg(MSG::INFO) << "No chains passed unprescaled - not processing this event: " << run_number << " " << event_number << " " << lumi_block << endreq; 
	  return;
	}
	

	m_provider->msg(MSG::INFO) << "Chains passed " << passed_chains << endreq;


	/// for Monte Carlo get the truth particles if requested to do so

	//  const TruthParticleContainer*  mcpartTES = 0;

	selectorTruth.clear();

	m_provider->msg(MSG::INFO) << "MC Truth flag " << m_mcTruth << endreq; 
	const TrigInDetTrackTruthMap* truthMap = 0;
	bool foundTruth = false;
	if ( m_mcTruth && m_TruthPdgId!=15) { 
		m_provider->msg(MSG::INFO) << "getting Truth" << endreq; 
		if ( m_provider->evtStore()->retrieve(truthMap, "TrigInDetTrackTruthMap").isFailure()) {
		        // m_provider->msg(MSG::WARNING) << "TrigInDetTrackTruthMap not found" << endreq;
			m_hasTruthMap = false;
		}
		else {
			m_provider->msg(MSG::INFO) << "TrigInDetTrackTruthMap found" << endreq;
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
		else { 
			m_provider->msg(MSG::WARNING) << "Truth not found - none whatsoever!" << endreq; 
		}
	}


	/// No lovely truth particle collections, so will need to 
	/// navigate through the egregious McTruthCollection, looking 
	/// at the GenParticles on each GenVertex in the collection
	/// lovely...

	if ( m_mcTruth && !foundTruth ) { 

		m_provider->msg(MSG::INFO) << "getting Truth" << endreq; 

		/// selectTracks<TruthParticleContainer>( &selectorTruth, "INav4MomTruthEvent" );

		const DataHandle<McEventCollection> mcevent;

		/// now as a check go through the GenEvent collection

		std::string collectionNames[4] = { "GEN_AOD", "TruthEvent", "", "G4Truth" };

		std::string collectionName = "";

		bool foundcollection = false;

		for ( int ik=0 ; ik<4 ; ik++ ) { 

			m_provider->msg(MSG::INFO) << "Try McEventCollection: " << collectionNames[ik] << endreq;

			if (!m_provider->evtStore()->contains<McEventCollection>(collectionNames[ik]) ) { 
				m_provider->msg(MSG::INFO) << "No McEventCollection: " << collectionNames[ik] << endreq;
				continue;
			}

			m_provider->msg(MSG::INFO) << "evtStore()->retrieve( mcevent, " << collectionNames[ik] << " )" << endreq;  

			if ( m_provider->evtStore()->retrieve( mcevent, collectionNames[ik] ).isFailure() ) {     
				m_provider->msg(MSG::INFO) << "Failed to get McEventCollection: " << collectionNames[ik] << endreq;
			}
			else { 
				// found this collectionName
				collectionName = collectionNames[ik];
				m_provider->msg(MSG::INFO) << "Found McEventCollection: " << collectionName << endreq;
				foundcollection = true;
				break;
			}
		}

		// not found any collection
		if ( !foundcollection ) { 

			m_provider->msg(MSG::WARNING) << "No MC Truth Collections of any sort, whatsoever!!!" << endreq;

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

		m_provider->msg(MSG::INFO) << "Found McEventCollection: " << collectionName << "\tNevents " << mcevent->size() << endreq;

		/// count the number of interactions of each sort
		/// this is actually *very stupid*, there are a *lot*
		/// of "processes" with *no* particles in them for some 
		/// reason, whoever programed this f**cked up structure 
		/// that needs this sort of fannying around to navigate 
		/// through ***REMOVED***

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
				//	m_provider->msg(MSG::INFO) << "Found " << ie << "\tpid " << pid << "\t with " << ip << " TruthParticles (GenParticles)" << endreq;
				++ie_ip;  
				ip += _ip;
			}
		}

		m_provider->msg(MSG::INFO) << "Found " << ip << " TruthParticles (GenParticles) in " << ie_ip << " GenEvents out of " << ie << endreq;

		m_provider->msg(MSG::INFO) << "selected " << selectorTruth.size() << " TruthParticles (GenParticles)" << endreq;

		////////////////////////////////////////////////////////////////////////////////////////

		if ( !(ip>0) ) {    
			m_provider->msg(MSG::WARNING) << "NO TRUTH PARTICLES - returning" << endreq;
			return; /// need to be careful here, if not requiring truth *only* should not return
		}

	}



	// clear the ntuple TIDA::Event class
	m_event->clear();


	/// get (offline) reference tracks 

	/// get offline tracks

	m_provider->msg(MSG::INFO) << " Offline tracks " << endreq;

	selectorRef.clear();

	if (m_provider->evtStore()->contains<Rec::TrackParticleContainer>("TrackParticleCandidate")) {
	  selectTracks<Rec::TrackParticleContainer>( &selectorRef, "TrackParticleCandidate" );
        }
#ifdef XAODTRACKING_TRACKPARTICLE_H
	else if (m_provider->evtStore()->contains<xAOD::TrackParticleContainer>("InDetTrackParticles")) {
	  selectTracks<xAOD::TrackParticleContainer>( &selectorRef, "InDetTrackParticles" );
	}
#endif
	else { 
	  m_provider->msg(MSG::WARNING) << " Offline tracks not found " << endreq;
	}
	

	/// get the offline vertices into our structure
	
	std::vector<TIDA::Vertex> vertices;

#ifndef XAODTRACKING_VERTEX_H
	
	const VxContainer* primaryVtxCollection;
	if ( m_provider->evtStore()->retrieve(primaryVtxCollection, "VxPrimaryCandidate").isFailure()) {
	  m_provider->msg(MSG::WARNING) << "Primary vertex container not found" << endreq;
	}
	else {
	  VxContainer::const_iterator vtxitr = primaryVtxCollection->begin();
	  for ( ; vtxitr != primaryVtxCollection->end(); ++vtxitr) {
	    if ( (*vtxitr)->vxTrackAtVertex()->size()>0 ) {
	      vertices.push_back( TIDA::Vertex( (*vtxitr)->recVertex().position().x(),
					       (*vtxitr)->recVertex().position().y(),
					       (*vtxitr)->recVertex().position().z(),
#ifdef EIGEN_GEOMETRY_MODULE_H	  
					       /// variances
					       (*vtxitr)->recVertex().covariancePosition()(Trk::x,Trk::x),
					       (*vtxitr)->recVertex().covariancePosition()(Trk::y,Trk::y),
					       (*vtxitr)->recVertex().covariancePosition()(Trk::z,Trk::z),
#else
					       /// variances
					       (*vtxitr)->recVertex().errorPosition().covValue(Trk::x,Trk::x),
					       (*vtxitr)->recVertex().errorPosition().covValue(Trk::y,Trk::y),
					       (*vtxitr)->recVertex().errorPosition().covValue(Trk::z,Trk::z),
#endif
					       (*vtxitr)->vxTrackAtVertex()->size(),
					       /// quality
					       (*vtxitr)->recVertex().fitQuality().chiSquared(),
					       (*vtxitr)->recVertex().fitQuality().numberDoF() ) );
	    }
	  }
	}



#else


	//	std::vector<TIDA::Vertex> vertices;
	
	m_provider->msg(MSG::INFO) << "fetching AOD Primary vertex container" << endreq;

	const xAOD::VertexContainer* xaodVtxCollection = 0;

	if ( m_provider->evtStore()->retrieve( xaodVtxCollection, "PrimaryVertices" ).isFailure()) {
	  m_provider->msg(MSG::WARNING) << "xAOD Primary vertex container not found with key " << "PrimaryVertices" <<  endreq;
	}
	
	if ( xaodVtxCollection!=0 ) { 
	  
	  m_provider->msg(MSG::INFO) << "xAOD Primary vertex container " << xaodVtxCollection->size() <<  " entries" << endreq;

	  xAOD::VertexContainer::const_iterator vtxitr = xaodVtxCollection->begin();
	  for ( ; vtxitr != xaodVtxCollection->end(); vtxitr++ ) {
	    if ( (*vtxitr)->nTrackParticles()>0 ) {
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
	

	/// add offline Vertices to the Offline chain
	
	
	/// add the truth particles if needed
	
	if ( m_mcTruth ) {
	  m_event->addChain( "Truth" ); 
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorTruth.tracks());
	}


	/// now add the vertices

	for ( unsigned i=0 ; i<vertices.size() ; i++ )  { 
	  m_provider->msg(MSG::INFO) << "vertex " << i << " " << vertices[i] << endreq;
	  m_event->addVertex(vertices[i]);
	}
	

	/// offline object counters 

	int Noff  = 0;
	int Nmu   = 0;
	int Nel   = 0;
	int Ntau1  = 0;
	int Ntau3 = 0;


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
	    std::vector<double> _beamline;
	    _beamline.push_back( selectorRef.getBeamX() );
	    _beamline.push_back( selectorRef.getBeamY() );
	    _beamline.push_back( selectorRef.getBeamZ() );
	    m_event->back().back().addUserData(_beamline);
	  }
	  else { 
	    m_event->back().back().addUserData(beamline);
	  }



	  Noff = selectorRef.tracks().size();
	  
	  m_provider->msg(MSG::DEBUG) << "ref tracks.size() " << selectorRef.tracks().size() << endreq; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  ref track " << ii << " " << *selectorRef.tracks()[ii] << endreq;  
	  
	}

	/// navigate through the requested storegate TEST chains
	
	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  

	  m_provider->msg(MSG::INFO)<< "chain:\t" << m_chainNames[ichain] << endreq;
	  //std::cout << "chain:\t" << m_chainNames[ichain]; 

	  /// get the chain, collection and TE names and track index 

	  const std::string& chainname      = m_chainNames[ichain].head();
	  const std::string& collectionname = m_chainNames[ichain].tail();
	  //	  const std::string& index          = m_chainNames[ichain].extra();
	  //	  const std::string& element        = m_chainNames[ichain].element();

	  //	  std::cout << "\tchain: " << chainname << "\tcollection: " << collectionname << "\tindex: " << index << "\tte: " << element << std::endl;  

	  /// here we *only* want collections with no specified chain
	  /// name, then we look in storegate for the collections directly
	  if ( chainname!="" ) continue;

	  //	  selectorRef.clear();
	  selectorTest.clear();

	  bool found = false;
	  
	  if (m_provider->evtStore()->contains<Rec::TrackParticleContainer>(collectionname)) {
	    found = selectTracks<Rec::TrackParticleContainer>( &selectorTest, collectionname );
	  }
	  else if (m_provider->evtStore()->contains<TrigInDetTrackCollection>(collectionname)) {
	    found = selectTracks<TrigInDetTrackCollection>( &selectorTest, collectionname );
	  }
	  else if (m_provider->evtStore()->contains<TrackCollection>(collectionname)) {
	    found = selectTracks<TrackCollection>( &selectorTest, collectionname );
	  }
#ifdef XAODTRACKING_TRACKPARTICLE_H
	  else if (m_provider->evtStore()->contains<xAOD::TrackParticleContainer>(collectionname)) {
	    found = selectTracks<xAOD::TrackParticleContainer>( &selectorTest, collectionname );
	  }
#endif
	  else { 
	    m_provider->msg(MSG::WARNING) << "\tcollection " << collectionname << " not found" << endreq;
	  }
	  
	  if ( found ) { 
	    m_event->addChain( collectionname );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    m_event->back().back().addTracks(selectorTest.tracks());
	    
	    if ( selectorTest.getBeamX()!=0 || selectorTest.getBeamY()!=0 || selectorTest.getBeamZ()!=0 ) { 
	      std::vector<double> _beamline;
	      _beamline.push_back( selectorTest.getBeamX() );
	      _beamline.push_back( selectorTest.getBeamY() );
	      _beamline.push_back( selectorTest.getBeamZ() );
	      m_event->back().back().addUserData(_beamline);
	    }
	    else { 
	      m_event->back().back().addUserData(beamline);
	    }
	    
	    int Ntest = selectorTest.tracks().size();
	    
	    m_provider->msg(MSG::INFO) << "collection " << collectionname << "\ttest tracks.size() " << Ntest << endreq; 
	    for ( int ii=Ntest ; ii-- ; ) m_provider->msg(MSG::DEBUG) << "  test track " << ii << " " << *selectorTest.tracks()[ii] << endreq;
	  }
	}	  


	std::string ElectronRef[7] =  { 
	  "Electrons", 
	  "Electrons_TightCB", "Electrons_MediumCB", "Electrons_LooseCB",
	  "Electrons_TightLH", "Electrons_MediumLH", "Electrons_LooseLH" };
 
	bool ElectronTypes[7] = { 
	  m_doElectrons, 
	  m_doElectrons_tightCB,   m_doElectrons_mediumCB,   m_doElectrons_looseCB,  
	  m_doElectrons_tightLH,   m_doElectrons_mediumLH,   m_doElectrons_looseLH };

	for ( int ielec=0 ; ielec<7 ; ielec++ ) {
	  /// Fixme: not sure if this code is correct - it always adds selectorRef.tracks() 
	  ///        so presumbably only ever *one* of the m_doElectrons can be set, otherwise 
	  ///        something is messed up here
	  ///        get electrons
	  //
	  //  CK: Maybe just add a check that you don't have more than one m_doElectrons as
	  //      as true, and throw an error if there is more than one as true?
	  if ( ElectronTypes[ielec] ) {   
	    Nel = processElectrons( selectorRef, ielec ); ///
	    m_event->addChain( ElectronRef[ielec] );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    m_event->back().back().addTracks(selectorRef.tracks());
	    if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	      std::vector<double> _beamline;
	      _beamline.push_back( selectorRef.getBeamX() );
	      _beamline.push_back( selectorRef.getBeamY() );
	      _beamline.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(_beamline);
	    }
	    else { 	  
	      m_event->back().back().addUserData(beamline);
	    }
	  }
	}	  


	/// get muons 
	if ( m_doMuons ) { 
	  
	  m_provider->msg(MSG::INFO) << "fetching offline muons " << endreq; 

	  Nmu   = processMuons( selectorRef );

	  m_provider->msg(MSG::INFO) << "found " << Nmu << " offline muons " << endreq; 

	  m_event->addChain("Muons");
	  m_event->back().addRoi(TIDARoiDescriptor(true));
	  m_event->back().back().addTracks(selectorRef.tracks());
	  if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	      std::vector<double> _beamline;
	      _beamline.push_back( selectorRef.getBeamX() );
	      _beamline.push_back( selectorRef.getBeamY() );
	      _beamline.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(_beamline);
	  }
	  else { 	  
	      m_event->back().back().addUserData(beamline);
	  }

	  m_provider->msg(MSG::DEBUG) << "ref muon tracks.size() " << selectorRef.tracks().size() << endreq; 
	  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::INFO) << "  ref muon track " << ii << " " << *selectorRef.tracks()[ii] << endreq;  
	}
	
	/// get one prong taus
	std::string TauRef_1Prong[4] =  { 
	  "Taus_1Prong", 
	  "Taus_Tight_1Prong", "Taus_Medium_1Prong", "Taus_Loose_1Prong" };
 
	bool TauTypes_1Prong[4] = { 
	  m_doTaus_1Prong, 
	  m_doTaus_tight_1Prong,   m_doTaus_medium_1Prong,   m_doTaus_loose_1Prong };

	for ( int itau=0 ; itau<4 ; itau++ ) {
	  // CK: See comments for doElectrons section above for possible issues with the looping
	  //     functionality here
	  if ( TauTypes_1Prong[itau] ) {
	    Ntau1 = processTaus( selectorRef,false, 20000, itau  );
	    m_event->addChain( TauRef_1Prong[itau] );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    m_event->back().back().addTracks(selectorRef.tracks());
	    if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
              std::vector<double> _beamline;
	      _beamline.push_back( selectorRef.getBeamX() );
	      _beamline.push_back( selectorRef.getBeamY() );
	      _beamline.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(_beamline);
	    }
	    else { 	  
	      m_event->back().back().addUserData(beamline);
	    }
	  }
	}
	    

	// for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::INFO) << "  one prong ref tau track " << ii << " " << *selectorRef.tracks()[ii] << endreq;  
	//}

        /// get three prong taus
	std::string TauRef_3Prong[4] =  { 
	  "Taus_3Prong", 
	  "Taus_Tight_3Prong", "Taus_Medium_3Prong", "Taus_Loose_3Prong" };
 
	bool TauTypes_3Prong[4] = { 
	  m_doTaus_3Prong, 
	  m_doTaus_tight_3Prong,   m_doTaus_medium_3Prong,   m_doTaus_loose_3Prong };

	for ( int itau=0 ; itau<4 ; itau++ ) {
	  // See comments for doElectrons section above for possible issues with the looping
	  // funcitonality here
	  if ( TauTypes_3Prong[itau] ) {
	    Ntau3 = processTaus( selectorRef, true, 20000, itau);
	    m_event->addChain( TauRef_3Prong[itau] );
	    m_event->back().addRoi(TIDARoiDescriptor(true));
	    m_event->back().back().addTracks(selectorRef.tracks());
	    if ( selectorRef.getBeamX()!=0 || selectorRef.getBeamY()!=0 || selectorRef.getBeamZ()!=0 ) { 
	      std::vector<double> _beamline;
	      _beamline.push_back( selectorRef.getBeamX() );
	      _beamline.push_back( selectorRef.getBeamY() );
	      _beamline.push_back( selectorRef.getBeamZ() );
	      m_event->back().back().addUserData(_beamline);
	    }
	    else { 	  
	      m_event->back().back().addUserData(beamline);
	    }
	  }
	}
	    
	//  for ( int ii=selectorRef.tracks().size() ; ii-- ; ) m_provider->msg(MSG::INFO) << " 3 prong ref tau track " << ii << " " << *selectorRef.tracks()[ii] << endreq;  
	//    	}	  



	if ( Nmu==0 && Noff==0 && Nel==0 && Ntau1==0 && Ntau3==0 ) { 
	  m_provider->msg(MSG::INFO) << "No offline objects found " << endreq;
	}


	// now loop over all relevant chains to get the trigger tracks...
	for ( unsigned ichain=0 ; ichain<m_chainNames.size() ; ichain++ ) {  

		// create chains for ntpl

		/// get the chain name
		const std::string& chainName = m_chainNames[ichain].head();

		/// and the name of the collection (if any)    
		const std::string& collectionName = m_chainNames[ichain].tail();

		/// and the index of the collection (if any)
		const std::string& collectionName_index_string = m_chainNames[ichain].extra();
		unsigned collectionName_index = 0;
		if ( collectionName_index_string!="" ) collectionName_index = std::atoi( collectionName_index_string.c_str() ); 

		if( chainName.find("L2_")==std::string::npos && 
		    chainName.find("EF_")==std::string::npos && 
		    chainName.find("HLT_")==std::string::npos ) continue;

		
		m_provider->msg(MSG::INFO) << "chain " << chainName 
					   << "\tprescale " << (*m_tdt)->getPrescale(chainName)
					   << "\tpass "     << (*m_tdt)->isPassed(chainName) << " physics " 
					   << "  (req dec " << (*m_tdt)->isPassed(chainName, _decisiontype ) << " dec type " << _decisiontype << ")"
					   << endreq;
		

		/**
		 ** use the TDT ExpertMethods to navigate to get all the collections
		 ** for a given chain
		 **
		 **/

		//    if ( !(*m_tdt)->isPassed( chainName ) ) continue;

		

		/// now decide whether we want all the TEs for this chain, or just those 
		/// that are still active
		unsigned decisiontype;
		//                if ( m_chainNames[ichain].passed() ) decisiontype = TrigDefs::Physics;
                if ( m_chainNames[ichain].passed() ) decisiontype = _decisiontype;
		else                                 decisiontype = TrigDefs::alsoDeactivateTEs;

#if 0
		{
		  std::cout << "SUTT DecisionType check" << std::endl;

		  Trig::FeatureContainer f = (*m_tdt)->features( chainName, TrigDefs::alsoDeactivateTEs);
		  //  Trig::FeatureContainer f = (*m_tdt)->features( chainName );
		  Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()); 
		  Trig::FeatureContainer::combination_const_iterator combEnd(f.getCombinations().end());
		  

		  Trig::FeatureContainer fp = (*m_tdt)->features( chainName, TrigDefs::Physics);
		  //  Trig::FeatureContainer f = (*m_tdt)->features( chainName );
		  Trig::FeatureContainer::combination_const_iterator combp(fp.getCombinations().begin()); 
		  Trig::FeatureContainer::combination_const_iterator combEndp(fp.getCombinations().end());
		  

		  std::cout << "SUTT Chain " << chainName 
			    << "\tNumber of conmbinations DTE: " << (combEnd-comb) << "\tPhysics: " << (combEndp-combp) << std::endl; 
		}
		std::cout << "decidion type " << decisiontype << std::endl;
#endif


		// Get chain combinations and loop on them
		// - loop made on chain selected as the one steering RoI creation

		Trig::FeatureContainer f = (*m_tdt)->features( chainName, decisiontype );  //, TrigDefs::alsoDeactivateTEs);
		//  Trig::FeatureContainer f = (*m_tdt)->features( chainName );
		Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()); 
		Trig::FeatureContainer::combination_const_iterator combEnd(f.getCombinations().end());

		if ( (*m_tdt)->isPassed(chainName, decisiontype ) ) { 
		  m_provider->msg(MSG::INFO) << "\tfetching features for chain " <<  decisiontype << "     " << chainName << "\t" << combEnd-comb << " combinations" << endreq;
		}

		if ( (*m_tdt)->isPassed(chainName, _decisiontype ) ) { 
		  m_provider->msg(MSG::INFO) << "\tfetching features for chain " << _decisiontype << "(RQ) " << chainName << "\t" << combEnd-comb << " combinations" << endreq;
		}


		{
		  Trig::FeatureContainer f = (*m_tdt)->features( chainName, _decisiontype );  //, TrigDefs::alsoDeactivateTEs);
		  Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()); 
		  Trig::FeatureContainer::combination_const_iterator combEnd(f.getCombinations().end());
		  
		  m_provider->msg(MSG::INFO) << "[91;1m" << "\tpassed combinations   chain " << chainName << "\t" 
					     << combEnd-comb << " combinations" 
					     << "\tdecision " << (*m_tdt)->isPassed(chainName, _decisiontype )  << "[m"  
					     << endreq;
		}		


		if ( comb==combEnd ) { 
			m_provider->msg(MSG::INFO) << "\tno features for chain " << chainName << endreq;
			continue;
		}

		m_event->addChain( m_chainNames[ichain] );

		TIDA::Chain& chain = m_event->back();


		int icomb = 0;

		const IRoiDescriptor* iroiptr = 0; 

		for( ; comb!=combEnd ; ++comb) {

		        m_provider->msg(MSG::INFO) << "Chain " << chainName << "\tcombination " << icomb << endreq;
			//		std::cout << "Chain " << chainName << "\tcombination " << icomb << std::endl;;

		        icomb++;
		  

			//   now add rois to this ntuple chain

			// Get seeding RoI
			// std::vector< Trig::Feature<TrigRoiDescriptor> > _rois = c->get<TrigRoiDescriptor>("initialRoI", TrigDefs::alsoDeactivateTEs);
			// std::vector< Trig::Feature<TrigRoiDescriptor> > _rois = c->get<TrigRoiDescriptor>("forID", TrigDefs::alsoDeactivateTEs);

#if 0
			/// check bjet roidescriptors ...
			{
			  std::string keys[5] = { "TrigJetRec", "TrigSplitJet", "TrigSuperRoi", "forID", "" };
			  
			  for ( unsigned iroi=0 ; iroi<5 ; iroi++ ) {  
			    std::vector< Trig::Feature<TrigRoiDescriptor> > rois = comb->get<TrigRoiDescriptor>(keys[iroi]);
			    m_provider->msg(MSG::INFO) << "\trois: " << keys[iroi] << "\tsize " << rois.size() << endreq; 
			    for ( unsigned ij=0 ; ij<rois.size() ; ij++ ) m_provider->msg(MSG::INFO) << "\tRoi  " <<keys[iroi] << "\t" << *rois[ij].cptr() << endreq;
			    for ( unsigned ij=0 ; ij<rois.size() ; ij++ ) m_provider->msg(MSG::INFO) << "\t\tRoi  " <<keys[iroi] << "\t" << *rois[ij].cptr() << endreq; 
			  }
			}
#endif

			/// need some way to specify which RoiDescriptor we are really interested in ...

			std::string roi_name = m_chainNames[ichain].roi();
			std::string vtx_name = m_chainNames[ichain].vtx();

			std::vector< Trig::Feature<TrigRoiDescriptor> > _rois;
			
			//			std::cout << "chain " << chainName << "\troi_name " << roi_name << std::endl;

			if ( roi_name!="" ) { 

			  _rois = comb->get<TrigRoiDescriptor>(roi_name);

			  //			  std::cout << "roi_name " << roi_name << std::endl;

			  if ( _rois.size()>0 ) { 
			    for ( unsigned ir=0 ; ir<_rois.size() ; ir++ ) m_provider->msg(MSG::INFO) << "\t\tRetrieved roi  " << roi_name << "\t" << *_rois[ir].cptr() << endreq; 
			  }
			  else { 
			    m_provider->msg(MSG::WARNING) << "\t\tRequested roi  " << roi_name << " not found" << endreq; 
			  }
			}
			else { 
			  _rois = comb->get<TrigRoiDescriptor>("forID1"); 
			  if ( _rois.empty() ) _rois = comb->get<TrigRoiDescriptor>("forID"); 
			  if ( _rois.empty() ) _rois = comb->get<TrigRoiDescriptor>(""); 
			  if ( _rois.empty() ) _rois = comb->get<TrigRoiDescriptor>("initialRoI"); 
			}			  

			if ( _rois.empty() ) continue;

			if ( iroiptr==0 ) { 
			  iroiptr = _rois[0].cptr();
			}
			else { 
			  if ( iroiptr == _rois[0].cptr() ) { 
			    std::cout << "found RoI before " << *_rois[0].cptr() << std::endl;
			    continue;
			  }
			}
			
			// notify if have multiple RoIs (get this for FS chains)
			if( _rois.size()>1) {
			  m_provider->msg(MSG::INFO) << "\tMore than one RoI found for seeded chain " << chainName << ": not yet supported" << endreq;
			  //continue; 
			}

			TIDARoiDescriptor* roiInfo = 0;
			if( !_rois.empty() ) {
		
			  for (  unsigned itmp=0  ;  itmp<_rois.size()  ;  itmp++ ) {
			    
			    const TrigRoiDescriptor* roid = _rois[itmp].cptr();
   
			    m_provider->msg(MSG::INFO) << "\tchain " << chainName << " RoI descriptor " << itmp << " " << *roid << endreq;
			    
			    if ( itmp==0 ){

			      TIDARoiDescriptor* roi_tmp = new TIDARoiDescriptor(TIDARoiDescriptorBuilder(*roid));
			      
			      //   m_provider->msg(MSG::INFO) << "using chain roi " << *roid << endreq;
			      m_provider->msg(MSG::INFO) << "TIDARoi " << *roi_tmp << endreq;
			      
			      /// this should *never* be the case, and we should only run this 
			      /// bit of code once the first time round the loop anyhow
			      if ( roiInfo!=0 ) delete roiInfo;
			      
			      roiInfo = roi_tmp;
			      //		if( chainName.find("_FS")!=std::string::npos && roiInfo->eta()==0 && roiInfo->phi()==0 ) {
			      //		roiInfo->phiHalfWidth(M_PI);
			      //		roiInfo->etaHalfWidth(3);
			    }
			  }
 

			}
			else { 
				m_provider->msg(MSG::INFO) << "\troi not found" <<  endreq;
			}

			// get the tracks from this roi

			selectorTest.clear();


			m_provider->msg(MSG::INFO) <<  "AC Ntple [91;1m" << endreq;


			//EF and HLT track EDM
			if (collectionName.find("InDetTrigParticleCreation")!=std::string::npos || 
			    chainName.find("EF_")!=std::string::npos ||
			    chainName.find("HLT_")!=std::string::npos ) {
			  if      ( selectTracks<Rec::TrackParticleContainer>( &selectorTest, comb, collectionName ) );
			  else if ( selectTracks<TrackCollection>( &selectorTest, comb, collectionName) );
			  else if ( selectTracks<TrigInDetTrackCollection>( &selectorTest, comb, truthMap, collectionName, collectionName_index ) );
#ifdef XAODTRACKING_TRACKPARTICLE_H
			  else {
			    m_provider->msg(MSG::INFO) << "\tsearch for xAOD::TrackParticle " << collectionName << endreq;  
			    if ( selectTracks<xAOD::TrackParticleContainer>( &selectorTest, comb, collectionName ) ) m_provider->msg(MSG::WARNING) << "\tFound xAOD collection " << collectionName << " (Ntple)"  << endreq;  
			    else m_provider->msg(MSG::WARNING) << "\tNo track collection " << collectionName << " found"  << endreq;  
			  }
#else
			  else m_provider->msg(MSG::WARNING) << "\tNo track collection " << collectionName << " found"  << endreq;  
#endif
			}
			else {
			  //L2 track EDM
			  if ( chainName.find("L2_")!=std::string::npos ) { 
			    if      ( selectTracks<TrigInDetTrackCollection>( &selectorTest, comb, truthMap, collectionName, collectionName_index ) );
			    else if ( selectTracks<Rec::TrackParticleContainer>( &selectorTest, comb, collectionName ) );
			    else if ( selectTracks<TrackCollection>( &selectorTest, comb, collectionName ) );
#ifdef XAODTRACKING_TRACKPARTICLE_H
			    else if ( selectTracks<xAOD::TrackParticleContainer>( &selectorTest, comb, collectionName ) );
#endif
			    else m_provider->msg(MSG::WARNING) << "\tNo track collection " << collectionName << " found"  << endreq;  
			  }
			}

			m_provider->msg(MSG::INFO) << "[m" << endreq;


			/// fetch vertices if available ...

			std::vector<TIDA::Vertex> tidavertices;	

#ifndef XAODTRACKING_VERTEX_H

			/// what is this doing? Why is it just fetching but not assigning to anything ????? who write this?
			
			m_provider->msg(MSG::INFO) << "\tNo VxContainer for chain " << chainName << " for key " << vtx_name << endreq;

			std::vector< Trig::Feature<VxContainer> > trigvertices = comb->get<VxContainer>(vtx_name);

			if ( trigvertices.empty() ) { 
			  m_provider->msg(MSG::INFO) << "\tNo VxContainer for chain " << chainName << " for key " << vtx_name << endreq;
			}
			else {
			  
			  for (  unsigned iv=0  ;  iv<trigvertices.size()  ;  iv++ ) {
			    
			    const VxContainer* vert = trigvertices[iv].cptr();
			    
			    m_provider->msg(MSG::INFO) << "\t" << iv << "  VxContainer for " << chainName << " " << vert << " " << vtx_name << endreq;

			    VxContainer::const_iterator vtxitr = vert->begin();
			  
			    for ( ; vtxitr != vert->end(); ++vtxitr) {
			      if ( (*vtxitr)->vxTrackAtVertex()->size()>0 ) {
				tidavertices.push_back( TIDA::Vertex( (*vtxitr)->recVertex().position().x(),
								     (*vtxitr)->recVertex().position().y(),
								     (*vtxitr)->recVertex().position().z(),
#ifdef EIGEN_GEOMETRY_MODULE_H	  
								     /// variances
								     (*vtxitr)->recVertex().covariancePosition()(Trk::x,Trk::x),
								     (*vtxitr)->recVertex().covariancePosition()(Trk::y,Trk::y),
								     (*vtxitr)->recVertex().covariancePosition()(Trk::z,Trk::z),
#else 
								     /// variances
								     (*vtxitr)->recVertex().errorPosition().covValue(Trk::x,Trk::x),
								     (*vtxitr)->recVertex().errorPosition().covValue(Trk::y,Trk::y),
								     (*vtxitr)->recVertex().errorPosition().covValue(Trk::z,Trk::z),
#endif	
								     /// quality
								     (*vtxitr)->vxTrackAtVertex()->size(),
								     (*vtxitr)->recVertex().fitQuality().chiSquared(),
								     (*vtxitr)->recVertex().fitQuality().numberDoF() ) );
				
				m_provider->msg(MSG::INFO)<< "\tvertex " << tidavertices.back() << endreq;
			      }
			    }

			  }
			}
			  
#else			
			/// now also add xAOD vertices
			
			m_provider->msg(MSG::INFO) << "\tFetch xAOD::VertexContainer for chain " << chainName << " with key " << vtx_name << endreq;

			std::vector< Trig::Feature<xAOD::VertexContainer> > xaodtrigvertices = comb->get<xAOD::VertexContainer>(vtx_name);

			if ( xaodtrigvertices.empty() ) { 
			  m_provider->msg(MSG::WARNING) << "\tNo xAOD::VertexContainer for chain " << chainName << " for key " << vtx_name << endreq;
			}
			else {

			    m_provider->msg(MSG::INFO) << "\txAOD::VertexContainer found with size  " << xaodtrigvertices.size() << "\t" << vtx_name << endreq;

			    for (  unsigned iv=0  ;  iv<xaodtrigvertices.size()  ;  iv++ ) {
			    
			      const xAOD::VertexContainer* vert = xaodtrigvertices[iv].cptr();
			    
			      m_provider->msg(MSG::INFO) << "\t" << iv << "  xAOD VxContainer for " << chainName << " " << vert << " key " << vtx_name << endreq;
			      
			      xAOD::VertexContainer::const_iterator vtxitr = vert->begin();
			  
			      for ( ; vtxitr != vert->end(); ++vtxitr) {
				if ( (*vtxitr)->nTrackParticles()>0 ) {
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
#endif
  
 
			const std::vector<TIDA::Track*>& testTracks = selectorTest.tracks();
			m_provider->msg(MSG::DEBUG) << "\ttest tracks.size() " << testTracks.size() << endreq; 
			for (unsigned int ii=0; ii < testTracks.size(); ii++) {
				m_provider->msg(MSG::DEBUG) << "  test track " << ii << "for chain " << chainName + ":" + collectionName << " " << *testTracks[ii] << endreq;  
			}


			// only add chain if there are any rois - also add beamline position for postprocessing


			if ( roiInfo == 0 ) { 
			  //		chain.addRoi( *roiInfo );
			  //		chain.back().addTracks(testTracks);
			  //		chain.back().addVertices(tidavertices);
			  //		chain.back().addUserData(beamline);
			  //			}      
			  //		else {
			  if ( testTracks.size()>0 ) m_provider->msg(MSG::WARNING) << "\ttest tracks.size() " << testTracks.size() << "found but no roi!!!" << endreq; 
			  roiInfo = new TIDARoiDescriptor(true);
			  //		  roiInfo->phiHalfWidth(M_PI);
			  //		  roiInfo->etaHalfWidth(3);
			}
			
			chain.addRoi( *roiInfo );
			chain.back().addTracks(testTracks);
			chain.back().addVertices(tidavertices);
			chain.back().addUserData(beamline_online);
			if ( selectorTest.getBeamX()!=0 || selectorTest.getBeamY()!=0 || selectorTest.getBeamZ()!=0 ) { 
			  std::vector<double> _beamline;
			  _beamline.push_back( selectorTest.getBeamX() );
			  _beamline.push_back( selectorTest.getBeamY() );
			  _beamline.push_back( selectorTest.getBeamZ() );
			  chain.back().addUserData(_beamline);
			}
			else { 	  
			  if ( beamline_online.size()>3 ) chain.back().addUserData(beamline_online);
			}

		     
			//			m_provider->msg(MSG::INFO) << " done" << endreq;      

			if ( roiInfo ) delete roiInfo;
			roiInfo = 0;
		}

	}

	if ( m_printInfo ) m_provider->msg(MSG::INFO) << "FILL TREE\n" << (*m_event) << endreq;      
	if ( mTree ) mTree->Fill();

}




/// setup the analysis the analysis, retrieve the tools etc 

void AnalysisConfig_Ntuple::book() { 

	m_provider->msg(MSG::INFO) << "AnalysisConfig_Ntuple::book() name " << name() << endreq;   

	/// flag should be called m_fileIsNotOpen really, so is 
	/// if m_fileIsNotOpen open file, 
	/// if !m_fileIsNotOpen, then close file etc 
	if ( !m_finalised ) { 
		m_provider->msg(MSG::INFO) << "AnalysisConfig_Ntuple::book() not booking " << name() << endreq;   
		return;
	}


	// get the beam condition services - one for online and one for offline

	m_iBeamCondSvc = 0;
	if ( m_provider->service( "BeamCondSvc", m_iBeamCondSvc ).isFailure() )  { 
	  m_provider->msg(MSG::WARNING) << " failed to retrieve BeamCondSvc: " << "BeamCondSvc" << endreq;
	}
	else { 
	  m_provider->msg(MSG::INFO) << " successfully retrieves BeamCondSvc: " << "BeamCondSvc" << endreq;
	}

	m_iOnlineBeamCondSvc = 0;
	if ( m_provider->service( "InDetBeamSpotOnline", m_iOnlineBeamCondSvc ).isFailure() )  { 
	  m_provider->msg(MSG::WARNING) << " failed to retrieve Online BeamCondSvc " << "InDetBeamSpotOnline" << endreq;
	}
	else { 
	  m_provider->msg(MSG::INFO) << " successfuly retrieved Online BeamCondSvc " << "InDetBeamSpotOnline" << endreq;
	}

	// get the TriggerDecisionTool

	if( m_tdt->retrieve().isFailure() ) {
		m_provider->msg(MSG::FATAL) << " Unable to retrieve the TrigDecisionTool: Please check job options file" << endreq;
		//    return StatusCode::FAILURE;
		return;
	}

	m_provider->msg(MSG::INFO) << "[91;1m" << " Successfully retrived the TrigDecisionTool" << "[m" << endreq;
	m_provider->msg(MSG::INFO) << "[91;1m" << " booking ntuple" << "[m" << endreq;
	m_provider->msg(MSG::INFO) << "[91;1m" << " trying to create new ntple file" << "[m" << endreq;

	gDirectory->pwd();

	/// save the current directory so we can return there after
	TDirectory* dir = gDirectory;

	static bool first_open = true;

	std::string outputFileName = m_outputFileName;

	if ( genericFlag() ) { 
		static int file_index = 0;
		std::string::size_type pos = outputFileName.find(".root");
		if ( pos != std::string::npos ) outputFileName.erase(pos, outputFileName.size());
		char file_label[64];
		sprintf( file_label, "-%04d.root", file_index++ );
		outputFileName += file_label;
	} 

	m_provider->msg(MSG::INFO) << "book() Writing to file " << outputFileName << endreq;

	gDirectory->pwd();

	if ( first_open || genericFlag() ) {
		/// create a brand new ntple
		mFile = new TFile( outputFileName.c_str(), "recreate"); 

		TTree*  dataTree = new TTree("dataTree", "dataTree");
		TString releaseData(m_releaseData.c_str());
		dataTree->Branch( "ReleaseMetaData", "TString", &releaseData);
		dataTree->Fill();
		dataTree->Write("", TObject::kOverwrite);
		delete dataTree;


		mTree = new TTree("tree", "tree");
		mTree->Branch( "TIDA::Event", "TIDA::Event", m_event, 6400, 1 );

		
	}
	else { 
		/// update the ntple from the file  
		mFile = new TFile( outputFileName.c_str(), "update");
		mTree = (TTree *)mFile->Get("tree");
		mTree->SetBranchAddress( "TIDA::Event", &m_event );
	}

	mDir = gDirectory;

	first_open = false;


	m_provider->msg(MSG::DEBUG) << "change directory " << name() << "  " << dir->GetName() << endreq;

	//	std::cout << "change directory " << name() << "  " << dir->GetName() << std::endl;
	/// go back to original directory
	dir->cd();

	//  gDirectory->pwd();

	m_finalised = false; // flag we have an open file that is not yet finalised

	tida_first = true;

	m_provider->msg(MSG::INFO) << "AnalysisConfig_Ntuple::book() exiting" << endreq;   

}




/// finalise the analysis - take ratios for efficiencies etc

void AnalysisConfig_Ntuple::finalize() { 

	//  gDirectory->pwd();


	/// NB: flag this round the other way for multiple files
	if ( m_finalised ) { 
		m_provider->msg(MSG::INFO) << "AnalysisConfig_Ntuple::finalise() flagged, not finalising  " << m_provider->name() << "\t" << mTree->GetEntries() << " entries" << endreq; 
		return;
	}

	m_provider->msg(MSG::INFO) << "AnalysisConfig_Ntuple::finalise() writing " << m_provider->name() << "\t" << mTree->GetEntries() << " entries" << endreq; 

	TDirectory* directory = gDirectory; 

	//	std::cout << "change directory " << name() << "  " << mDir->GetName() << std::endl;

	m_provider->msg(MSG::DEBUG) << "change directory " << name() << "  " << mDir->GetName() << endreq;


	mDir->cd();

	//  gDirectory->pwd();

	if ( mTree ) mTree->Write("", TObject::kOverwrite);

	//  mFile->Write();
	if ( mFile ) mFile->Close();


	m_finalised = true; /// flag that we have finalised and closed this file

	// mTree "belongs" to the mFile so was (possibly) deleted on the mFile->Close();
	// so don't delete it ! 
	// delete mTree; 
	delete mFile;

	mTree = 0;
	mFile = 0; 

	//  f.Write();
	//  f.Close();

	//	std::cout << "change directory " << name() << "  " << directory->GetName() << std::endl;

	directory->cd();

	//  gDirectory->pwd();

}



