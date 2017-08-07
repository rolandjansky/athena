/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysVertexTrackBase.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Base class for vertex-track related classes in need of
// track-to-vertex association handling.
//
// Derived classes should overwrite the following methods instead
// of the orginal initialize() etc methods:
// - initializeHook()
// - finalizeHook()
// - addBranchesHook()       -- only called at end of addBranches()
// - addBranchesSVLoopHook() -- preferred (inside SV loop)
//
// From within addBranchesSVLoopHook()
// - calculateValues()
// should be called and the following hook methods should be overwritten:
// - calcValuesHook() -- called for the actual calculation of values
// - fastFillHook()   -- called to check and apply caching of values
//
// For an usage example see BVertexTrackIsoTool and BPHY8.py .
//
// Job options provided by this class:
// - BranchPrefixes             -- vector of prefixes to assign to
//                                 added branches
//                                 (Must be of same size as VertexContainerNames
//                                  and in the same order.)
// - BranchBaseName             -- assign the base name of added branches
//                                 (default: iso)
// - BranchSuffix               -- assign the suffix of added branches
//                                 (default: empty = none)
// - VertexContainerNames       -- names of containers for secondary vertices
// - TrackParticleContainerName -- name of container for TrackParticles
// - TrackToVertexTool          -- ToolHandle for track-to-vertex tool
// - TrackSelectionTools        -- Array of ToolHandles for track
//                                 selection tools; each tool should
//                                 be named XXXX_YYYY, where the YYYY
//                                 suffix string which needs to be unique;
//                                 will be used to name the isolation track
//                                 category (part of the new attribute names)
// - RefPVContainerNames        -- List of refitted PV container names.
//                                 (Must be of same size as VertexContainerNames
//                                  and in the same order.)
// - DoVertexType               -- PV-to-SV association types to be
//                                 considered (bitwise variable, see
//                                 xAODBPhys::BPhysHelper.
// - UseTrackTypes              -- List of or-ed bit-wise selection of
//                                 track sets to consider:
//                                 bit : meaning
//                                 0   : tracks close to PV associated
//                                       with SV
//                                 1   : tracks associated with dummy PV
//                                       ("type-0 PV tracks")
//                                 2   : tracks associated with PV of type 1
//                                 3   : tracks associated with PV of type 2
//                                 4   : tracks associated with PV of type 3
//                                 5   : all tracks not selected by bits
//                                       0 to 4.
//                                 (E.g. 63 means to consider all tracks.)
// - IncPrecVerticesInDecay     -- Include preceeding vertices in search
//                                 for ID tracks and muons from decaying
//                                 particle.  (May be a bit slower but
//                                 more accurate.  Double-counting of track
//                                 or muon objects is excluded.
//                                 Default: True)
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TVector3.h"
#include "TString.h"
#include "boost/format.hpp"
#include <algorithm>
#include <sstream>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BPhysVertexTrackBase::BaseItem::BaseItem(std::string Name,
					   std::string Bname,
					   std::string Prefix) :
    name(Name), bname(Bname), prefix(Prefix) {
  }
  
  BPhysVertexTrackBase::BaseItem::~BaseItem() {
  }
  
  void BPhysVertexTrackBase::BaseItem::setup(std::string Name,
					     std::string Bname,
					     std::string Prefix) {
    name     = Name;
    bname    = Bname;
    prefix   = Prefix;
  }

  void BPhysVertexTrackBase::BaseItem::setPrefix(std::string Prefix) {
    prefix = Prefix;
  }
  
  void BPhysVertexTrackBase::BaseItem::resetVals() {
    // needs to be implemented by derived class
  }

  std::string BPhysVertexTrackBase::BaseItem::buildName(std::string qualifier,
							std::string suffix) {
    boost::format f("%s%s%s%s%s");
    f % (prefix.length() > 0 ? prefix+"_" : "")
      % (bname.length() > 0 ? bname+"_" : "")
      % (qualifier.length() > 0 ? qualifier+"_" : "")
      % name
      % suffix;
    return f.str();
  }
  //--------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  // static members
  const int          BPhysVertexTrackBase::n_track_types    = 6;
  const std::string  BPhysVertexTrackBase::track_type_str[] =
    {"ASSOCPV", "PVTYPE0", "PVTYPE1", "PVTYPE2", "PVTYPE3", "NONE"};
  const unsigned int BPhysVertexTrackBase::track_type_bit[] =
    {0x1, 0x2, 0x4, 0x8, 0x10, 0x20};
  unsigned int BPhysVertexTrackBase::track_type_all_cached = 0x0;
  
  // static methods
  const std::string
  BPhysVertexTrackBase::tts(BPhysVertexTrackBase::track_type type) {
    return track_type_str[type];
  }
  
  unsigned int
  BPhysVertexTrackBase::ttb(BPhysVertexTrackBase::track_type type) {
    return track_type_bit[type];
  }
  
  unsigned int BPhysVertexTrackBase::ttall() {
    if ( track_type_all_cached == 0x0 ) {
      for (unsigned int i=0; i<n_track_types; ++i) {
	track_type_all_cached |= track_type_bit[i];
      }
    }
    return track_type_all_cached;
  }
  //--------------------------------------------------------------------------
  // Static utility method to prefix every line by a certain string
  //--------------------------------------------------------------------------
  std::string BPhysVertexTrackBase::wrapLines(std::string lines,
					      std::string prefix) {
    
    std::string ostr;
    std::istringstream stream(lines);
    std::string line;
    while ( std::getline(stream, line) ) {
      if ( !ostr.length() == 0 ) ostr += "\n";
      ostr += prefix + line;
    }
    return ostr;
  }
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  BPhysVertexTrackBase::BPhysVertexTrackBase(const std::string& t,
					     const std::string& n,
					     const IInterface*  p)
    : AthAlgTool(t,n,p), m_trackToVertexTool("Reco::TrackToVertex"),
      m_tracks(NULL), m_tracksAux(NULL), m_nEvtsSeen(0) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare branch prefix
    declareProperty("BranchPrefixes", m_branchPrefixes);
    declareProperty("BranchBaseName", m_branchBaseName = "iso");
    declareProperty("BranchSuffix"  , m_branchSuffix = ""     );

    // Necessary containers
    declareProperty("VertexContainerNames"  , m_vertexContainerNames);
    declareProperty("TrackParticleContainerName",
		    m_trackParticleContainerName);
    declareProperty("TrackToVertexTool"     , m_trackToVertexTool);
    declareProperty("TrackSelectionTools"   , m_trackSelectionTools);
    declareProperty("PVContainerName", m_pvContainerName = "PrimaryVertices");
    declareProperty("RefPVContainerNames"   , m_refPVContainerNames);
    declareProperty("DoVertexType"          , m_doVertexType = 63);
    declareProperty("UseTrackTypes"         , m_useTrackTypes = {7});
    declareProperty("IncPrecVerticesInDecay", m_incPrecVerticesInDecay = true);
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::initialize() {
  
    ATH_MSG_DEBUG("BPhysVertexTrackBase::initialize() -- begin");

    if ( m_vertexContainerNames.size() == 0 ) {
      ATH_MSG_ERROR("No vertex container names provided!");
    }
    if ( m_refPVContainerNames.size() == 0 ) {
      ATH_MSG_ERROR("No refitted PV container names provided!");
    }
    if ( m_trackParticleContainerName == "" ) {
      ATH_MSG_ERROR("No track particle container name provided!");
    }
    if ( m_pvContainerName == "" ) {
      ATH_MSG_ERROR("No PV container name provided!");
    }
    if ( m_vertexContainerNames.size() != m_refPVContainerNames.size() ) {
      ATH_MSG_ERROR("Size mismatch of VertexContainerNames ("
		    << m_vertexContainerNames.size()
		    << ") and RefPVContainerNames ("
		    << m_refPVContainerNames.size() << ") lists!");
    }      
    
    if ( m_vertexContainerNames.size() != m_branchPrefixes.size() ) {
      ATH_MSG_ERROR("Size mismatch of VertexContainerNames ("
		    << m_vertexContainerNames.size()
		    << ") and BranchPrefixes ("
		    << m_branchPrefixes.size() << ") lists!");
    }      
    
    // TrackToVertexTool
    ATH_CHECK(m_trackToVertexTool.retrieve());

    // TrackSelectionTools
    for (auto selTool : m_trackSelectionTools ) {
      ATH_CHECK(selTool.retrieve());
    }
    
    // initialize PV-to-SV association type vector
    initPvAssocTypeVec();

    ATH_MSG_DEBUG("BPhysVertexTrackBase::initialize() -- end");

    return initializeHook();
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::finalize() {

    ATH_MSG_DEBUG("BPhysVertexTrackBase::finalize()");

    // everything all right
    return finalizeHook();
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::addBranches() const {

    ATH_MSG_DEBUG("BPhysVertexTrackBase::addBranches() -- begin");

    // counter
    m_nEvtsSeen++;

    // retrieve primary vertices container
    m_pvtxContainer = NULL;
    CHECK(evtStore()->retrieve(m_pvtxContainer, m_pvContainerName));
    ATH_MSG_DEBUG("Found PV collection with key " << m_pvContainerName);


    // retrieve ID track container
    m_tracks    = NULL;
    m_tracksAux = NULL;
    CHECK(evtStore()->retrieve(m_tracks, m_trackParticleContainerName));
    if (evtStore()->contains<xAOD::
	TrackParticleAuxContainer>(m_trackParticleContainerName+"Aux.")) {
      CHECK(evtStore()->retrieve(m_tracksAux,
				 m_trackParticleContainerName+"Aux."));
    } else {
      ATH_MSG_DEBUG("No aux track collection with key "
		    << m_trackParticleContainerName+"Aux.");
    }
    ATH_MSG_DEBUG("Found track collection with key "
		  << m_trackParticleContainerName);
    
    // Loop over all vertex containers
    for (size_t i=0; i<m_vertexContainerNames.size(); ++i) {
    // vertex container and its auxilliary store
      const xAOD::VertexContainer*     m_svtxContainer    = NULL;
      const xAOD::VertexAuxContainer*  m_svtxAuxContainer = NULL;
      // refitted primary vertex container and its auxilliary store
      const xAOD::VertexContainer*     m_refPVContainer    = NULL;
      const xAOD::VertexAuxContainer*  m_refPVAuxContainer = NULL;

      // retrieve from StoreGate
      CHECK(evtStore()->retrieve(m_svtxContainer, m_vertexContainerNames[i]));
      CHECK(evtStore()->retrieve(m_svtxAuxContainer,
				 m_vertexContainerNames[i]+"Aux."));
      ATH_MSG_DEBUG("Found SV collection with key "
		    << m_vertexContainerNames[i]);
      CHECK(evtStore()->retrieve(m_refPVContainer   ,
				 m_refPVContainerNames[i]));
      CHECK(evtStore()->retrieve(m_refPVAuxContainer,
				 m_refPVContainerNames[i]+"Aux."));
      ATH_MSG_DEBUG("Found refitted PV collection with key "
		    << m_refPVContainerNames[i]);
      
      // vertex container depending setup in derived class
      CHECK(addBranchesVCSetupHook(i));
      
      // loop over secondary vertices
      for (xAOD::VertexContainer::const_iterator vtxItr =
	     m_svtxContainer->begin(); vtxItr!=m_svtxContainer->end();
	   ++vtxItr) {

	CHECK(addBranchesSVLoopHook(*vtxItr));
	
      } // end of loop over vertices
    } // end of loop over vertex container names
    
    ATH_MSG_DEBUG("BPhysVertexTrackBase::addBranches() -- end");

    // nothing to do here
    return addBranchesHook();
  }

  //--------------------------------------------------------------------------
  // Hook method for initialize() -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::initializeHook() {

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Hook method for finalize() -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::finalizeHook() {

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Hook method for addBranches() -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::addBranchesHook() const {

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Hook method for addBranches() VC setup
  // -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::addBranchesVCSetupHook(size_t ivc) const {

    // just to avoid a compiler warning
    ATH_MSG_DEBUG("addBranchesVCSetupHook: Vertex container index " << ivc
		  << " for collection " << m_vertexContainerNames[ivc]
		  << " with prefix " << m_branchPrefixes[ivc]);
    
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Hook method for addBranches() SV loop -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode
  BPhysVertexTrackBase::addBranchesSVLoopHook(const xAOD::Vertex* vtx) const {

    // just to avoid a compiler warning
    ATH_MSG_DEBUG("addBranchesSVLoopHook: Vertex " << vtx);
    
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Calculate values -- used by calculateValues()
  // -- to be overwritten by derived class
  //--------------------------------------------------------------------------
  StatusCode
  BPhysVertexTrackBase::calcValuesHook(const xAOD::Vertex* vtx,
				       const unsigned int ipv,
				       const unsigned int its,
				       const unsigned int itt) const {

    // just to avoid a compiler warning
    ATH_MSG_DEBUG("calcIsolationOpti:  vtx: " << vtx << ", ipv: " << ipv
		  << ", its: " << its << ", itt: " << itt);

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Fill values from cache if found -- used by calculateValues()
  // -- to be overwritten by derived class
  //--------------------------------------------------------------------------  
  bool BPhysVertexTrackBase::fastFillHook(const xAOD::Vertex* vtx,
					  const int ipv) const {

    // just to avoid a compiler warning
    ATH_MSG_DEBUG("fastIsoFill:  vtx: " << vtx << ", ipv: " << ipv);
    
    return false;
  }
  //--------------------------------------------------------------------------
  // Calculation loops -- needs to be called from inside the implementation
  // of addBranchesSVLoopHook() in the derived class.
  // Derived class also needs to provide override methods for
  // - fastFillHook   -- needs to return true if cached value is used
  // - calcValuesHook -- actually calculating value(s)
  //--------------------------------------------------------------------------  
  StatusCode
  BPhysVertexTrackBase::calculateValues(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BPhysVertexTrackBase::calculateValues -- begin");
    
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    m_pvAssocResMap.clear();

    const xAOD::BPhysHelper cand(vtx);
    for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
      if ( ipv == 0 || ! fastFillHook(vtx, ipv) ) { 
	for (unsigned int its = 0; its < nTrackSels; ++its) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    ATH_MSG_DEBUG("Calling calcValuesHook with ipv: " << ipv
			  << ", its: " << its << ", itt: " << itt);
	    CHECK(calcValuesHook(vtx, ipv, its, itt));
	  } // for itt
	} // for its
	// cache result index -- only needed once per ipv value
	m_pvAssocResMap[buildPvAssocCacheName(vtx, ipv)] = ipv;
	ATH_MSG_DEBUG("calculateValues: cache index: "
		      << buildPvAssocCacheName(vtx, ipv)
		      << " -- cached ipv: " << ipv);
      } // if !fastFillHook()
    } // for ipv
      
    return StatusCode::SUCCESS;
  }
  //------------------------------------------------------------------------- 
  // Build SV-to-PV association cache index string
  //------------------------------------------------------------------------- 
  std::string
  BPhysVertexTrackBase::buildPvAssocCacheName(const xAOD::Vertex* vtx,
					      const int ipv) const {
    xAOD::BPhysHelper cand(vtx);
    boost::format f("SV_%p_RPV_%p");
    f % cand.vtx() % cand.pv(m_pvAssocTypes[ipv]);
    
    return f.str();
  }
  //--------------------------------------------------------------------------  
  // getTrackCandPVLogChi2()
  // Calculate the logChi2 (= log((d0/d0e)^2+(z0/z0e)^2) contribution of a
  // track at the position closest to the PV associated with the SV. 
  //--------------------------------------------------------------------------
  double BPhysVertexTrackBase::getTrackCandPVLogChi2(const xAOD::TrackParticle*
						     track,
						     const Amg::Vector3D& pos
						     ) const {
    return getTrackLogChi2DCA(track, pos, false)[4];
  }
  //--------------------------------------------------------------------------  
  // getTrackLogChi2DCA()
  // Calculate logChi2 (= log((d0/d0e)^2+(z0/z0e)^2) contribution of a
  // track at the position closest to a position and
  // the distance of closest approach of a track w.r.t.
  // a position.  Either only in the transverse plane or in 3 dimensions.
  //--------------------------------------------------------------------------
  std::vector<double>
  BPhysVertexTrackBase::getTrackLogChi2DCA(const xAOD::TrackParticle* track,
					   const Amg::Vector3D& pos,
					   bool doDCAin3D) const {
    // presets
    std::vector<double> res = {-999., -99., -999., -99., -100., -100., -1.};
    
    if ( track != NULL ) {
      const Trk::Perigee* trkPerigee =
	m_trackToVertexTool->perigeeAtVertex(*track, pos);
      if ( trkPerigee != NULL  ) {
	res[0] = trkPerigee->parameters()[Trk::d0];
	res[2] = trkPerigee->parameters()[Trk::z0];
	const AmgSymMatrix(5)* locError = trkPerigee->covariance();
	if ( locError != NULL ) {
	  res[1] = Amg::error(*locError, Trk::d0);
	  res[3] = Amg::error(*locError, Trk::z0);
	  if ( fabs(res[1]) > 0. && fabs(res[3]) > 0.) { 
	    res[4] = log( pow(res[0]/res[1], 2.) + pow(res[2]/res[3], 2.) );
	    res[6] = 2.; // ok
	  } else {
	    ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
			    << " d0 = " << res[0] << ", d0Err = " << res[1]
			    << ", z0 = " << res[2] << ", z0Err = " << res[3]);
	  }
	  res[5] = doDCAin3D ? sqrt( pow(res[0], 2.) + pow(res[2], 2.) ) : res[0];
	  res[6] += 1.; // ok
	} else {
	  ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
			  " locError pointer is NULL!");
	} // locError != NULL
	delete trkPerigee;
	trkPerigee = NULL;
      } else {
      ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
		      " trkPerigee pointer is NULL!");
      } // if trkPerigee
    } else {
      ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
		      " track pointer is NULL!");
      res[6] = -2.;
    }
    return res;
  }
  //--------------------------------------------------------------------------
  // detTrackTypes(): returns a bit pattern of the applicable
  // track types from {ASSOCPV, PVTYPE0, PVTYPE1, PVTYPE2, PVTYPE3, NONE}
  // (or'd).
  //--------------------------------------------------------------------------
  int BPhysVertexTrackBase::detTrackTypes(const xAOD::TrackParticle* track,
					  const xAOD::Vertex* candPV) const {
    int bits = 0x0;

    // PVTYPE0 - PVTYPE3, NONE
    if ( track->vertex() != NULL ) {
      int vtxType = track->vertex()->vertexType();
      if ( vtxType > -1 && vtxType < 4 ) {
	bits |= track_type_bit[vtxType+1];
      } else {
	bits |= track_type_bit[NONE];
      }
    }
    // ASOCPV
    if ( candPV != NULL ) {
      bool found(false);
      for (size_t i=0; i<candPV->nTrackParticles(); ++i) {
	if ( track == candPV->trackParticle(i) ) {
	found = true;
	break;
	}
      }
      if ( found ) bits |= track_type_bit[ASSOCPV];
    } // if candPV != NULL
    
    return bits;
  }
  //--------------------------------------------------------------------------
  // findAllTracksInDecay: returns a vector of xAOD::TrackParticle objects
  // found in this vertex and subsequent decay vertices (if chosen).
  //--------------------------------------------------------------------------
  TrackBag BPhysVertexTrackBase::findAllTracksInDecay(xAOD::BPhysHelper& vtx)
    const {

    TrackBag tracks;
    findAllTracksInDecay(vtx, tracks);

    return tracks;
  }
  //--------------------------------------------------------------------------
  // findAllTracksInDecay: fills a vector of xAOD::TrackParticle objects
  // found in this vertex and subsequent decay vertices (if chosen).
  // Method avoids duplicate entries in vector.
  // Recursively calls itself if necessary.
  //--------------------------------------------------------------------------
  void BPhysVertexTrackBase::findAllTracksInDecay(xAOD::BPhysHelper& vtx,
						  TrackBag& tracks)
    const {

    for (unsigned int i=0; i < vtx.vtx()->nTrackParticles(); ++i) {
      const xAOD::TrackParticle* track = vtx.vtx()->trackParticle(i);
      if ( std::find(tracks.begin(),tracks.end(),track) == tracks.end() ) {
	tracks.push_back(track);
      } // if
    } // for
    // loop over preceeding vertices
    if ( m_incPrecVerticesInDecay ) {
      for (int ivtx = 0; ivtx < vtx.nPrecedingVertices(); ++ivtx) {
	xAOD::BPhysHelper precVtx(vtx.precedingVertex(ivtx));
	findAllTracksInDecay(precVtx, tracks);
      } // if
    } // for 
  }
  //--------------------------------------------------------------------------
  // findAllMuonsInDecay: returns a vector of xAOD::Muon objects
  // found in this vertex and subsequent decay vertices (if chosen).
  //--------------------------------------------------------------------------
  MuonBag BPhysVertexTrackBase::findAllMuonsInDecay(xAOD::BPhysHelper& vtx)
    const {

    MuonBag muons;
    findAllMuonsInDecay(vtx, muons);
      
    return muons;
  }
  //--------------------------------------------------------------------------
  // findAllMuonsInDecay: fills vector of xAOD::Muon objects
  // found in this vertex and subsequent decay vertices (if chosen).
  // Method avoids duplicate entries in vector.
  // Recursively calls itself if necessary.
  //--------------------------------------------------------------------------
  void BPhysVertexTrackBase::findAllMuonsInDecay(xAOD::BPhysHelper& vtx,
						 MuonBag& muons)
    const {

    for (int i=0; i < vtx.nMuons(); ++i) {
      if ( std::find(muons.begin(),muons.end(),vtx.muon(i)) == muons.end() ) {
	muons.push_back(vtx.muon(i));
      } // if
    } // for
    // loop over preceeding vertices
    if ( m_incPrecVerticesInDecay ) {
      for (int ivtx = 0; ivtx < vtx.nPrecedingVertices(); ++ivtx) {
	xAOD::BPhysHelper precVtx(vtx.precedingVertex(ivtx));
	findAllMuonsInDecay(precVtx, muons);
      } // for
    } // if
  }
  //--------------------------------------------------------------------------
  // findAllMuonsIdTracksInDecay: returns a vector of xAOD::TrackParticle
  // objects found in this vertex and subsequent decay vertices.
  // Returns the tracks.
  // The vector of track pointers reeturned may contain NULL elements.
  //--------------------------------------------------------------------------
  TrackBag
  BPhysVertexTrackBase::findAllMuonIdTracksInDecay(xAOD::BPhysHelper& vtx,
						   MuonBag& muons) const {

    TrackBag tracks;
    muons = findAllMuonsInDecay(vtx);

    for (MuonBag::const_iterator muItr = muons.begin(); muItr != muons.end();
	 ++muItr) {
      const xAOD::TrackParticle* track =
	(*muItr)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      tracks.push_back(track);
    } // for 
    
    return tracks;
  }
  //--------------------------------------------------------------------------
  // findMuonRefTrackMomenta: returns a vector<TVector3> containing the
  // three momenta of refitted tracks identified as muons.
  // The vector may contain (0,0,0) elements indicating an error.
  //--------------------------------------------------------------------------
  std::vector<TVector3>
  BPhysVertexTrackBase::findMuonRefTrackMomenta(xAOD::BPhysHelper& vtx,
						MuonBag& muons) const {

    std::vector<TVector3> refMuTracks;

    // quick solution if nRefTrks == nMuons:
    if ( vtx.nRefTrks() == vtx.nMuons() && !m_incPrecVerticesInDecay ) {
      muons = vtx.muons();
      for ( auto refMuTrack : vtx.refTrks() ) {
	refMuTracks.push_back(refMuTrack);
      }
    } else {
      TrackBag muonIdTracks = findAllMuonIdTracksInDecay(vtx, muons);
      if ( vtx.nRefTrks() == (int)vtx.vtx()->nTrackParticles() ) {
	for (int i=0; i<vtx.nRefTrks(); ++i) {
	  const xAOD::TrackParticle* otp =
	    (const xAOD::TrackParticle*)vtx.refTrkOrigin(i);
	  if ( otp != NULL ) {
	    if ( std::find(muonIdTracks.begin(), muonIdTracks.end(), otp)
		 != muonIdTracks.end() ) {
	      refMuTracks.push_back(vtx.refTrk(i));
	    }
	  } else {
	    ATH_MSG_WARNING("BPhysVertexTrackBase::findMuonRefTrackMomenta():"
			    " refTrkOrigin == NULL for refTrk # "
			    << i << " !");
	  }
	} // for
      } else {
	ATH_MSG_WARNING("BPhysVertexTrackBase::findMuonRefTrackMomenta():"
			" size mismatch #refTrks = " << vtx.nRefTrks()
			<< "#trackParticles = " << vtx.vtx()->nTrackParticles()
			<< " !");
      } // if nRefTracks == nTrackParticles
      // loop over preceeding vertices -- only if not all refMuTrks found yet
      if ( m_incPrecVerticesInDecay && muons.size() > refMuTracks.size() ) {
	for (int ivtx = 0; ivtx < vtx.nPrecedingVertices(); ++ivtx) {
	  xAOD::BPhysHelper precVtx(vtx.precedingVertex(ivtx));
	  std::vector<TVector3> precRefMuTracks =
	    findMuonRefTrackMomenta(precVtx, muons);
	  // append only if not yet contained in
	  for ( auto precRefMuTrack : precRefMuTracks ) {
	    if ( std::find(refMuTracks.begin(), refMuTracks.end(),
			   precRefMuTrack) == refMuTracks.end() ) {
	      refMuTracks.push_back(precRefMuTrack);
	    } // if
	  } // for 
	} // for ivtx
      } // if
    } // if (shortcut)

    // debug output
    if ( msgLvl( MSG::DEBUG ) ) { 
	ATH_MSG_DEBUG("BPhysVertexTrackBase::findMuonRefTrackMomenta():"
		      << " #muons: " << muons.size()
		      << "  #refMuTrks: " << refMuTracks.size());
	TString str = Form(">> refMuTracks(%d):\n", (int)refMuTracks.size());
	for (unsigned int i=0; i < refMuTracks.size(); ++i) {
	  str += Form("(%10.4f,%10.4f,%10.4f) ",
		      refMuTracks[i].x(), refMuTracks[i].y(),
		      refMuTracks[i].z());
	}
	ATH_MSG_DEBUG(str.Data());
      }
    
    return refMuTracks;
  }
  
  //--------------------------------------------------------------------------
  // selectTracks: returns a vector of xAOD::TrackParticle objects
  // seleted from the input track collection according to the selection
  // criteria and with respect to the B candidate vertex.
  //--------------------------------------------------------------------------
  TrackBag BPhysVertexTrackBase::selectTracks(const
					      xAOD::TrackParticleContainer*
					      inpTracks,
					      xAOD::BPhysHelper& cand,
					      const unsigned int ipv,
					      const unsigned int its,
					      const unsigned int itt) const {

    return selectTracks(inpTracks, findAllTracksInDecay(cand), cand,
			ipv, its, itt);
  }
  //--------------------------------------------------------------------------
  // selectTracks: returns a vector of xAOD::TrackParticle objects
  // seleted from the input track collection according to the selection
  // criteria and with respect to the B candidate vertex.
  //--------------------------------------------------------------------------
  TrackBag BPhysVertexTrackBase::selectTracks(const
					      xAOD::TrackParticleContainer*
					      inpTracks,
					      const TrackBag& exclTracks,
					      xAOD::BPhysHelper& cand,
					      const unsigned int ipv,
					      const unsigned int its,
					      const unsigned int itt) const {

    const xAOD::Vertex* candRefPV  = cand.pv(m_pvAssocTypes[ipv]);
    const xAOD::Vertex* candPV     = cand.origPv(m_pvAssocTypes[ipv]);

    ATH_MSG_DEBUG("selectTracks: Found " << exclTracks.size()
		  << " " << exclTracks
		  << " for decay candidate " << cand.vtx()
		  << "; candPV: " << candPV << " candRefPV: " << candRefPV);
    
    // tracks to be considered
    TrackBag tracks;
    for (xAOD::TrackParticleContainer::const_iterator trkItr =
	   inpTracks->begin(); trkItr != inpTracks->end(); ++trkItr) {
      const xAOD::TrackParticle* track = *trkItr;
      // track selection check
      if ( ! m_trackSelectionTools[its]->accept(*track, candRefPV) ) continue;
      // track type check
      if ( ! ((unsigned int)m_useTrackTypes[itt] == ttall() ||
	      (detTrackTypes(track, candPV) & m_useTrackTypes[itt]) > 0x0) )
	continue; 
      // track not in list of tracks to exclude
      if ( std::find(exclTracks.begin(), exclTracks.end(), track)
	   != exclTracks.end() ) continue;
      // tracks that survived so far
      tracks.push_back(track);
    } // for

    ATH_MSG_DEBUG("selectTracks: Selected " << tracks.size()
		  << " " << tracks
		  << " to be considered for candidate " << cand.vtx());
    
    return tracks;
  }
  //--------------------------------------------------------------------------
  // buildBranchName: build branch name from track selection, primary vertex
  // association and track type qualifiers.
  //--------------------------------------------------------------------------
  std::string BPhysVertexTrackBase::buildBranchBaseName(unsigned int its,
							unsigned int ipv,
							unsigned int itt)
    const {
    
    ATH_MSG_DEBUG("BPhysVertexTrackBase::buildBranchBaseName -- begin");
    
    std::string tsName   = m_trackSelectionTools[its].name();
    std::string pvAssoc  = xAOD::BPhysHelper::pv_type_str[m_pvAssocTypes[ipv]];

    // need to get part of tsname after last underscore
    std::size_t ipos = tsName.find_last_of("_");
    if ( ipos != std::string::npos ) tsName = tsName.substr(ipos+1);

    // format it nicely
    boost::format f("T%02d_%s_%s%s");
    f % m_useTrackTypes[itt] % tsName % pvAssoc;
    f % (m_branchSuffix.length() > 0 ? "_"+m_branchSuffix : "");
    
    ATH_MSG_DEBUG("BPhysVertexBaseTrackBase::buildBranchBaseName: " << f.str());

    return f.str();
  }
  //--------------------------------------------------------------------------
  //
  // Initialize PV-to-SV association type vector
  //
  //--------------------------------------------------------------------------
  void BPhysVertexTrackBase::initPvAssocTypeVec() {

    m_pvAssocTypes.clear();
    for (unsigned int i=0; i<xAOD::BPhysHelper::n_pv_types; ++i) {
      if ( (m_doVertexType & (1 << i)) > 0 )
	m_pvAssocTypes.push_back((xAOD::BPhysHelper::pv_type)i);
    }
  }
  //--------------------------------------------------------------------------  
}
