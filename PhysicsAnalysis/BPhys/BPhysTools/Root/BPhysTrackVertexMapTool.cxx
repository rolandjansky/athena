/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// system include:
#include "boost/format.hpp"
#include "boost/tokenizer.hpp"

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHypoHelper.h"

// Local include(s):
#include "BPhysTools/BPhysTrackVertexMapTool.h"

namespace xAOD {
  
  //--------------------------------------------------------------------------
  // Static utility method to prefix every line by a certain string
  //--------------------------------------------------------------------------
  std::string BPhysTrackVertexMapTool::wrapLines(std::string lines,
						 std::string prefix) {
    
    std::string ostr;
    std::istringstream stream(lines);
    std::string line;
    while ( std::getline(stream, line) ) {
      if ( !ostr.empty() ) ostr += "\n";
      ostr += prefix + line;
    }
    return ostr;
  }
  //--------------------------------------------------------------------------
  // Constructor
  //--------------------------------------------------------------------------
  BPhysTrackVertexMapTool::BPhysTrackVertexMapTool( const std::string& name )
    : asg::AsgTool( name ),
      m_tracks(NULL), m_tracksAux(NULL), m_pvtxContainer(NULL),
      m_svtxContainer(NULL), m_svtxAuxContainer(NULL), m_refPVContainer(NULL),
      m_refPVAuxContainer(NULL),
      m_nEvtsSeen (0), m_cachedRun(-1), m_cachedEvent(-1) {

#ifdef ASGTOOL_ATHENA
    declareInterface< IBPhysTrackVertexMapTool >( this );
#endif // ASGTOOL_ATHENA

    // Necessary containers
    declareProperty("VertexContainerName", m_vertexContainerName);
    declareProperty("TrackParticleContainerName",
		    m_trackParticleContainerName="InDetTrackParticles");
    declareProperty("PVContainerName", m_pvContainerName = "PrimaryVertices");
    declareProperty("RefPVContainerName", m_refPVContainerName);

    // Maximum number of events to dump maps to log file for 
    declareProperty("DebugTrkToVtxMaxEvents", m_debugTrkToVtxMaxEvents = 0);

    // Prefix for log dump lines 
    declareProperty("DumpPrefix", m_dumpPrefix="TTV> ");

    // Hypothesis name (for mass value pickup)
    declareProperty("HypoName", m_hypoName="__NONE__");
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysTrackVertexMapTool::initialize() {
    
    // Greet the user:
    ATH_MSG_DEBUG( "Initializing xAOD::BPhysTrackVertexMapTool" );
    
    if ( m_vertexContainerName == "" ) {
      ATH_MSG_ERROR("No vertex container name provided!");
    }
    if ( m_refPVContainerName == "" ) {
      ATH_MSG_ERROR("No refitted PV container name provided!");
    }
    if ( m_trackParticleContainerName == "" ) {
      ATH_MSG_ERROR("No track particle container name provided!");
    }
    if ( m_pvContainerName == "" ) {
      ATH_MSG_ERROR("No PV container name provided!");
    }
    // some info for the job log
    ATH_MSG_INFO("VertexContainerName        : " << m_vertexContainerName);
    ATH_MSG_INFO("PVContainerName            : " << m_pvContainerName);
    ATH_MSG_INFO("RefPVContainerName         : " << m_refPVContainerName);
    ATH_MSG_INFO("TrackParticleContainerName : "
		 << m_trackParticleContainerName);
    ATH_MSG_INFO("DebugTrkToVtxMaxEvents     : " << m_debugTrkToVtxMaxEvents);
    ATH_MSG_INFO("DumpPrefix                 : " << m_dumpPrefix);
    ATH_MSG_INFO("HypoName                   : " << m_hypoName);
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysTrackVertexMapTool::finalize() {

    ATH_MSG_DEBUG( "Finalizing xAOD::BPhysTrackVertexMapTool" );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysTrackVertexMapTool::logEvent() {

    ATH_MSG_DEBUG( "logEvent in xAOD::BPhysTrackVertexMapTool" );
  
    // read info into maps cache
    ATH_CHECK(cacheEvent());
    
    // dump info from maps if requested
    if ( doLog() ) {

      ATH_MSG_INFO("Track-to-vertex association map:");
      
      std::cout << summaryToString(m_dumpPrefix) << std::endl;
      
    } // if requested

    // increment counter
    m_nEvtsSeen++;

    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  bool BPhysTrackVertexMapTool::doLog() const {

    return ( m_debugTrkToVtxMaxEvents < 0 ||
	     m_debugTrkToVtxMaxEvents > (int)m_nEvtsSeen );
  }  
  //--------------------------------------------------------------------------
  //
  // Cache maps for current event.
  //
  // Call this once per event.
  // Repeated calls for the same run/event are not updating the cache again.
  //
  //--------------------------------------------------------------------------
  StatusCode BPhysTrackVertexMapTool::cacheEvent() {

    ATH_MSG_DEBUG("BPhysTrackVertexMapTool::cacheEvent -- begin");
    
    const xAOD::EventInfo* eventInfo = NULL;
    ATH_CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));

    if ( m_cachedRun   != (int)eventInfo->runNumber() ||
	 m_cachedEvent != (int)eventInfo->eventNumber() ) {

      // note update
      m_cachedRun   = eventInfo->runNumber();
      m_cachedEvent = eventInfo->eventNumber();

      ATH_MSG_DEBUG("BPhysTrackVertexMapTool::cacheEvent: caching now: "
		    << "run " << m_cachedRun << " event " << m_cachedEvent);
      
      // retrieve primary vertices container
      m_pvtxContainer = NULL;
      ATH_CHECK(evtStore()->retrieve(m_pvtxContainer, m_pvContainerName));
      ATH_MSG_DEBUG("Found PV collection with key " << m_pvContainerName);
      
      // retrieve ID track container
      m_tracks    = NULL;
      m_tracksAux = NULL;
      ATH_CHECK(evtStore()->retrieve(m_tracks, m_trackParticleContainerName));
      if (evtStore()->contains<xAOD::
	  TrackParticleAuxContainer>(m_trackParticleContainerName+"Aux.")) {
	ATH_CHECK(evtStore()->retrieve(m_tracksAux,
				       m_trackParticleContainerName+"Aux."));
      } else {
	ATH_MSG_DEBUG("No aux track collection with key "
		      << m_trackParticleContainerName+"Aux.");
      }
      ATH_MSG_DEBUG("Found track collection with key "
		    << m_trackParticleContainerName);
      
      // vertex container and its auxilliary store
      m_svtxContainer    = NULL;
      m_svtxAuxContainer = NULL;
      ATH_CHECK(evtStore()->retrieve(m_svtxContainer, m_vertexContainerName));
      ATH_CHECK(evtStore()->retrieve(m_svtxAuxContainer,
				     m_vertexContainerName+"Aux."));
      ATH_MSG_DEBUG("Found SV collection with key " << m_vertexContainerName);
      
      // refitted primary vertex container and its auxilliary store
      m_refPVContainer    = NULL;
      m_refPVAuxContainer = NULL;
      ATH_CHECK(evtStore()->retrieve(m_refPVContainer, m_refPVContainerName));
      ATH_CHECK(evtStore()->retrieve(m_refPVAuxContainer,
				     m_refPVContainerName+"Aux."));
      ATH_MSG_DEBUG("Found refitted PV collection with key "
		    << m_refPVContainerName);

      // initialize track, PV and refPV maps
      initTrackVertexMaps(m_tracks, m_pvtxContainer, m_refPVContainer, 
			  m_svtxContainer);
    } // if new run/event
    
    ATH_MSG_DEBUG("BPhysTrackVertexMapTool::cacheEvent -- end");
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  } 
  //--------------------------------------------------------------------------
  //
  // Retrieve primary vertices for ID track from map.
  //
  //--------------------------------------------------------------------------
  std::vector<const xAOD::Vertex*>
  BPhysTrackVertexMapTool::pvsForIDTrack(const xAOD::TrackParticle* track)
    const {
    
    TrackToVertexMap_t::const_iterator it = m_idTrackToPVMap.find(track);

    if ( it != m_idTrackToPVMap.end() ) {
      return it->second;
    } else {
      std::vector<const xAOD::Vertex*> dummy;
      return dummy;
    }
  }
  //--------------------------------------------------------------------------
  //
  // Retrieve refitted primary vertices for ID track from map.
  //
  //--------------------------------------------------------------------------
  std::vector<const xAOD::Vertex*>
  BPhysTrackVertexMapTool::refPVsForIDTrack(const xAOD::TrackParticle* track)
    const {
    
    TrackToVertexMap_t::const_iterator it = m_idTrackToRefPVMap.find(track);

    if ( it != m_idTrackToRefPVMap.end() ) {
      return it->second;
    } else {
      std::vector<const xAOD::Vertex*> dummy;
      return dummy;
    }
  }
  //--------------------------------------------------------------------------
  //
  // Retrieve secondary vertices for ID track from map.
  //
  //--------------------------------------------------------------------------
  std::vector<const xAOD::Vertex*>
  BPhysTrackVertexMapTool::svsForIDTrack(const xAOD::TrackParticle* track)
    const {
    
    TrackToVertexMap_t::const_iterator it = m_idTrackToSVMap.find(track);

    if ( it != m_idTrackToSVMap.end() ) {
      return it->second;
    } else {
      std::vector<const xAOD::Vertex*> dummy;
      return dummy;
    }
  }
  //--------------------------------------------------------------------------
  //
  // Initialize ID tracks, PV and refPV related maps.
  //
  //--------------------------------------------------------------------------
  void BPhysTrackVertexMapTool
  ::initTrackVertexMaps(const xAOD::TrackParticleContainer* tpc,
			const xAOD::VertexContainer*        pvc, 
			const xAOD::VertexContainer*        rpvc, 
			const xAOD::VertexContainer*        svc) {

    // clear previous entries
    m_pvNameMap.clear();
    m_refPVNameMap.clear();
    m_svNameMap.clear();
    m_idTrackNameMap.clear();
    m_idTrackToPVMap.clear();
    m_idTrackToRefPVMap.clear();
    m_idTrackToSVMap.clear();

    // initialize maps for PVs
    for (xAOD::VertexContainer::const_iterator vtxItr = pvc->begin();
	 vtxItr != pvc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      pvName(vtx);
      for (size_t i = 0; i < vtx->nTrackParticles(); ++i) {
	const xAOD::TrackParticle* track = vtx->trackParticle(i);
	// m_idTrackToPVMap[track] = vtx;
	addVertexToTrackVertexMap(m_idTrackToPVMap, track, vtx);
      }
    }
    // initialize maps for refitted PVs
    for (xAOD::VertexContainer::const_iterator vtxItr = rpvc->begin();
	 vtxItr != rpvc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      refPVName(vtx);
      for (size_t i = 0; i < vtx->nTrackParticles(); ++i) {
	const xAOD::TrackParticle* track = vtx->trackParticle(i);
	// m_idTrackToRefPVMap[track] = vtx;
	addVertexToTrackVertexMap(m_idTrackToRefPVMap, track, vtx);
      }
    }

    // initialize maps for SVs
    for (xAOD::VertexContainer::const_iterator vtxItr = svc->begin();
	 vtxItr != svc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      svName(vtx);
      for (size_t i = 0; i < vtx->nTrackParticles(); ++i) {
	const xAOD::TrackParticle* track = vtx->trackParticle(i);
	// m_idTrackToSVMap[track] = vtx;
	addVertexToTrackVertexMap(m_idTrackToSVMap, track, vtx);
      }
    }
    // initialize maps for ID tracks
    for (xAOD::TrackParticleContainer::const_iterator trkItr = tpc->begin();
	 trkItr != tpc->end(); ++trkItr) {
      const xAOD::TrackParticle* track = *trkItr;
      idTrackName(track);
    }
  }
  //--------------------------------------------------------------------------
  //
  // Add vertex to track-to-vertex map with vector of vertices.
  //
  //--------------------------------------------------------------------------
  void BPhysTrackVertexMapTool
  ::addVertexToTrackVertexMap(TrackToVertexMap_t& map,
			      const xAOD::TrackParticle* track,
			      const xAOD::Vertex* vtx) {

    TrackToVertexMap_t::const_iterator it = map.find(track);

    if ( it == map.end() ) {
      map[track] = std::vector<const xAOD::Vertex*>();
    }
    map[track].push_back(vtx);
  }
  //--------------------------------------------------------------------------
  // Lookup name for PV -- add as next if not yet known
  //--------------------------------------------------------------------------
  std::string BPhysTrackVertexMapTool::pvName(const xAOD::Vertex* vtx) {
    
    if ( m_pvNameMap.find(vtx) == m_pvNameMap.end() ) {
      boost::format f("PV%03d");
      f % m_pvNameMap.size();
      m_pvNameMap[vtx] = f.str();
    }
    return m_pvNameMap[vtx];
  }
  //--------------------------------------------------------------------------
  // Lookup name for refitted PV -- add as next if not yet known
  //--------------------------------------------------------------------------
  std::string BPhysTrackVertexMapTool::refPVName(const xAOD::Vertex* vtx) {
    
    if ( m_refPVNameMap.find(vtx) == m_refPVNameMap.end() ) {
      boost::format f("RV%03d"); 
      f % m_refPVNameMap.size();
      m_refPVNameMap[vtx] = f.str();
    }
    return m_refPVNameMap[vtx];
  }
  //--------------------------------------------------------------------------
  // Lookup name for SV -- add as next if not yet known
  //--------------------------------------------------------------------------
  std::string BPhysTrackVertexMapTool::svName(const xAOD::Vertex* vtx) {
    
    if ( m_svNameMap.find(vtx) == m_svNameMap.end() ) {
      boost::format f("SV%03d");
      f % m_svNameMap.size();
      m_svNameMap[vtx] = f.str();
    }
    return m_svNameMap[vtx];
  }
  //--------------------------------------------------------------------------
  // Lookup name for ID track -- add as next if not yet known
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::idTrackName(const xAOD::TrackParticle* track) {
    
    if ( m_idTrackNameMap.find(track) == m_idTrackNameMap.end() ) {
      boost::format f("T%04d");
      f % m_idTrackNameMap.size();
      m_idTrackNameMap[track] = f.str();
    }
    return m_idTrackNameMap[track];
  }
  //--------------------------------------------------------------------------
  // Print Track information to string -- optionally adding PVs or refPVs
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::idTrackToString(const xAOD::TrackParticle* track,
					   unsigned int indent,
					   bool withPV, bool withRefPV,
					   bool withSV) {
    
    std::string sind(indent, ' ');
    boost::format f1("%s %-5s %p (%10.4f, %10.4f, %10.4f) VL %p");
    f1 % sind % idTrackName(track) % track 
      % track->pt() % track->eta() % track->phi() % track->vertex();
    std::string str = f1.str();
    if ( withPV ) {
      TrackToVertexMap_t::iterator it = m_idTrackToPVMap.find(track);
      if ( it != m_idTrackToPVMap.end() ) {
	for ( auto vtx : it->second ) {
	  str += "\n" + pvToString(vtx, indent+2, false);
	}
      } else {
	boost::format f2("\n%s  %s");
	f2 % sind % "NOPV";
	str += f2.str();
	    
      }
    }
    if ( withRefPV ) {
      TrackToVertexMap_t::iterator it = m_idTrackToRefPVMap.find(track);
      if ( it != m_idTrackToRefPVMap.end() ) {
	for ( auto vtx : it->second ) {
	  str += "\n" + refPVToString(vtx, indent+2, false);
	}
      } else {
	boost::format f2("\n%s  %s");
	f2 % sind % "NORV";
	str += f2.str();
      }
    }
    if ( withSV ) {
      TrackToVertexMap_t::iterator it = m_idTrackToSVMap.find(track);
      if ( it != m_idTrackToSVMap.end() ) {
	for ( auto vtx : it->second ) {
	  str += "\n" + svToString(vtx, indent+2, false);
	}
      } else {
	boost::format f2("\n%s  %s");
	f2 % sind % "NOSV";
	str += f2.str();
      }
    }
    return str;
  }  
  //--------------------------------------------------------------------------
  // Print PV information to string -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::pvToString(const xAOD::Vertex* vtx,
				      unsigned int indent,
				      bool withTracks) {

    std::string sind(indent, ' ');
    boost::format f1("%s %-5s %p (%10.4f, %10.4f, %10.4f) NT %4d VT %d");
    f1 % sind % pvName(vtx) % vtx % vtx->x() % vtx->y() % vtx->z()
      % vtx->nTrackParticles() % vtx->vertexType();
    std::string str = f1.str();
    if ( withTracks ) {
      for (size_t i=0; i < vtx->nTrackParticles(); ++i) {
	boost::format f2("\n%s  %4d %s");
	f2 % sind % i 
	  % idTrackToString(vtx->trackParticle(i), 0, false, false);
	str += f2.str();
      } // for
    }

    return str;
  }
  //--------------------------------------------------------------------------
  // Print refitted PV information to string -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::refPVToString(const xAOD::Vertex* vtx,
					 unsigned int indent,
					 bool withTracks) {

    std::string sind(indent, ' ');
    boost::format f1("%s %-5s %p (%10.4f, %10.4f, %10.4f) NT %4d VT %d");
    f1 % sind % refPVName(vtx) % vtx % vtx->x() % vtx->y() % vtx->z()
      % vtx->nTrackParticles() % vtx->vertexType();
    std::string str = f1.str();
    if ( withTracks ) {
      for (size_t i=0; i < vtx->nTrackParticles(); ++i) {
	boost::format f2("\n%s  %4d %s");
	f2 % sind % i 
	  % idTrackToString(vtx->trackParticle(i), 0, false, false);
	str += f2.str();
      } // for
    }

    return str;
  }
  //--------------------------------------------------------------------------
  // Print SV information to string -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::svToString(const xAOD::Vertex* vtx,
				      unsigned int indent,
				      bool withTracks,
				      bool withMasses) {
    
    std::string sind(indent, ' ');
    boost::format f1("%s %-5s %p (%10.4f, %10.4f, %10.4f) NT %4d VT %d");
    f1 % sind % svName(vtx) % vtx % vtx->x() % vtx->y() % vtx->z()
      % vtx->nTrackParticles() % vtx->vertexType();
    std::string str = f1.str();
    if ( withMasses && m_hypoName != "__NONE__" ) {
      // vector of possible hypo names
      std::vector<std::string> hypoNames = getTokens(m_hypoName, "|;/");
      for ( auto hypoName : hypoNames ) {
	BPhysHypoHelper bhh(hypoName, vtx);
	float bMass       = bhh.mass();
	float bMassErr    = bhh.massErr();
	float bMucMass    = getFloat(hypoName+"_MUCALC_mass", vtx);
	float bMucMassErr = getFloat(hypoName+"_MUCALC_massErr", vtx);
	if ( bMass > 0. || bMassErr > 0.
	     || bMucMass > 0. || bMucMassErr > 0. ) {
	  boost::format f3("\n%s  %-10s : mass     : (%15.4f +/- %15.4f) MeV");
	  
	  boost::format f4("\n%s  %-10s : m(MUCALC): (%15.4f +/- %15.4f) MeV");
	  f3 % sind % hypoName % bMass    % bMassErr;
	  f4 % sind % hypoName % bMucMass % bMucMassErr;
	  str += f3.str() + f4.str();
	} // if one > 0.
      } // for hypoNames
    } // if withMasses
    if ( withTracks ) {
      for (size_t i=0; i < vtx->nTrackParticles(); ++i) {
	boost::format f2("\n%s  %4d %s");
	f2 % sind % i 
	  % idTrackToString(vtx->trackParticle(i), 0, false, false);
	str += f2.str();
      } // for
    }
    return str;
  }
  //--------------------------------------------------------------------------
  // Print track container information to string 
  // -- optionally adding PVs and refitted PVs
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::idTracksToString(const xAOD::TrackParticleContainer*
					    tpc,
					    unsigned int indent,
					    bool withPV,
					    bool withRefPV,
					    bool withSV) {

    std::string str;
    std::string sind(indent, ' ');
    str += sind + "ID tracks: (" + tpc->size() + ")\n";
    str += sind + std::string(80-indent, '-');
    // loop over ID tracks
    for (xAOD::TrackParticleContainer::const_iterator trkItr = tpc->begin();
	 trkItr != tpc->end(); ++trkItr) {
      const xAOD::TrackParticle* track = *trkItr;
      str += "\n" 
	+ idTrackToString(track, indent+2, withPV, withRefPV, withSV);
    }
    return str;
  }
  //--------------------------------------------------------------------------
  // Print PV container information to string -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::pvsToString(const xAOD::VertexContainer* pvc,
				       unsigned int indent,
				       bool withTracks) {

    std::string str;
    std::string sind(indent, ' ');
    str += sind + "Primary vertices: (" + pvc->size() + ")\n";
    str += sind + std::string(80-indent, '-');
    for (xAOD::VertexContainer::const_iterator vtxItr = pvc->begin();
	 vtxItr != pvc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      str += "\n" + pvToString(vtx, indent+2, withTracks);
    } // for    

    return str;
  }
  //--------------------------------------------------------------------------
  // Print refitted PV container information to string 
  // -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::refPVsToString(const xAOD::VertexContainer* rpvc,
					  unsigned int indent,
					  bool withTracks) {

    std::string str;
    std::string sind(indent, ' ');
    str += sind + "Refitted primary vertices: (" + rpvc->size() + ")\n";
    str += sind + std::string(80-indent, '-');
    for (xAOD::VertexContainer::const_iterator vtxItr = rpvc->begin();
	 vtxItr != rpvc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      str += "\n" + refPVToString(vtx, indent+2, withTracks);
    } // for    

    return str;
  }
  //--------------------------------------------------------------------------
  // Print SV container information to string -- optionally adding tracks
  //--------------------------------------------------------------------------
  std::string 
  BPhysTrackVertexMapTool::svsToString(const xAOD::VertexContainer* svc,
				       unsigned int indent,
				       bool withTracks,
				       bool withMasses) {

    std::string str;
    std::string sind(indent, ' ');
    str += sind + "Secondary vertices: (" + svc->size() + ")\n";
    str += sind + std::string(80-indent, '-');
    for (xAOD::VertexContainer::const_iterator vtxItr = svc->begin();
	 vtxItr != svc->end(); ++vtxItr) {
      const xAOD::Vertex* vtx = *vtxItr;
      str += "\n" + svToString(vtx, indent+2, withTracks, withMasses);
    } // for    

    return str;
  }
  //--------------------------------------------------------------------------
  // Print a summary of all maps to string -- optionally adding a prefix
  //--------------------------------------------------------------------------
  std::string BPhysTrackVertexMapTool::summaryToString(std::string prefix) {

    boost::format form("%s\n\nRun: %d  Event: %d\n\n");
    form % name() % m_cachedRun % m_cachedEvent;
    std::string dstr = 
      wrapLines("\n"+form.str() +
		pvsToString(m_pvtxContainer, 0, true) + "\n\n" +
		refPVsToString(m_refPVContainer, 0, true) + "\n\n" +
		svsToString(m_svtxContainer, 0, true, true) + "\n\n" +
		idTracksToString(m_tracks, 0, true, true, true) + "\n",
		prefix);

    return dstr;
  }
  //--------------------------------------------------------------------------
  // Pick up a float from StoreGate.
  //--------------------------------------------------------------------------
  float BPhysTrackVertexMapTool::getFloat(std::string name,
					  const xAOD::Vertex* b) {

    float res = -999999.;

    SG::AuxElement::Accessor<float> floatAcc(name);
    if ( floatAcc.isAvailable(*b) ) res = floatAcc(*b);

    return res;
  } 
  //--------------------------------------------------------------------------
  // Tokenize a string using certain separators
  //--------------------------------------------------------------------------
  std::vector<std::string> BPhysTrackVertexMapTool
  ::getTokens(std::string input, std::string seperators) {

    std::vector<std::string> tokens;
    boost::char_separator<char> sep(seperators.c_str());
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer_t;
    Tokenizer_t tokenizer(input, sep);
    for (auto& token : tokenizer) {
      tokens.push_back(token);
    }
    return tokens;
  }
  //--------------------------------------------------------------------------
} // namespace xAOD
