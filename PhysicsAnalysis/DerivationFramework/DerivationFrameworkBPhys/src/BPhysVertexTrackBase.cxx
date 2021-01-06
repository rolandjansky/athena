/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
//                                 xAODBPhys::BPhysHelper)
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
//                                 5   : tracks associated with PV with types
//                                       other than 0 to 4.
//                                 6   : tracks with missing pointer to
//                                       PV (NULL pointer)
//                                 7-22: tracks being closest to assoc. PV
//                                     useRefittedPVs doDCAin3D chi2DefToUse
//                                 7   :    yes           no        0
//                                 8   :    no            no        0
//                                 9   :    yes           yes       0
//                                 10  :    no            yes       0
//                                 11  :    yes           no        1
//                                 12  :    no            no        1
//                                 13  :    yes           yes       1
//                                 14  :    no            yes       1
//                                 15  :    yes           no        2
//                                 16  :    no            no        2
//                                 17  :    yes           yes       2
//                                 18  :    no            yes       2
//                                 19  :    yes           --        3
//                                 20  :    no            --        3
//                                 21  :    yes           --        4
//                                 22  :    no            --        4
//                                 23  :    yes           --        5
//                                 24  :    no            --        5
//                                 25  :    yes           yes       6
//                                 26  :    no            yes       6
//                                 27  :    yes           yes       7
//                                 28  :    no            yes       7
//                                 29  :    yes           yes       8
//                                 30  :    no            yes       8
//                                 31  :    yes           yes       9
//                                 32  :    no            yes       9
//                                 useRefittedPVs:
//                                   replace PV associated to decay candidate
//                                   by the refitted PV
//                                 doDCAin3D:
//                                   use d0 and z0 in the determination of
//                                   of the point of closest approach of
//                                   a track to a vertex
//                                 chi2DefToUse:
//                                   PV uncertainties in the chi2 calculation
//                                   in addition to track uncertainties
//                                   0 : use old method
//                                       (only track uncertainties)
//                                   1 : from track perigee with
//                                       uncertainties from track and vertex
//                                   2 : simple extrapolation from track
//                                       parameters with uncertainties from
//                                       track and vertex (extrapolation
//                                       used for track swimming)
//                                   3 : CalcLogChi2toPV method from NtupleMaker
//                                       using xAOD::TrackingHelpers.
//                                       (only track uncertainties)
//                                   4 : CalcLogChi2toPV method from NtupleMaker
//                                       using xAOD::TrackingHelpers.
//                                       (track and vertex uncertainties)
//                                   5 : use TrackVertexAssociationTool
//                                   6 : full 3D chi2 from track perigee
//                                       with uncertainties from track and
//                                       vertex (sum of 3x3 covariance matrices)
//                                   7 : full 3D chi2 from track perigee with
//                                       uncertainties from track and vertex
//                                       (sum of 2x2 covariance matrices)
//                                   8 : simple extrapolation from track
//                                       parameters with uncertainties
//                                       from track and vertex
//                                       (sum of 3x3 covariance matrices)
//                                   9   simple extrapolation from track
//                                       parameters with uncertainties
//                                       from track and vertex
//                                       (sum of 2x2 covariance matrices)
//                                 (E.g. 127 means to consider all tracks.)
// - IncPrecVerticesInDecay     -- Include preceeding vertices in search
//                                 for ID tracks and muons from decaying
//                                 particle.  (May be a bit slower but
//                                 more accurate.  Double-counting of track
//                                 or muon objects is excluded.
//                                 Default: True)
// - MinNTracksInPV             -- Minimum number of tracks in PV for
//                                 PV to be considered in calculation
//                                 of closest PV to a track
// - PVTypesToConsider          -- List of primary vertex types to consider
//                                 in calculation of closest PV to a track
// - DebugTrackTypes            -- Count tracks of specific types (bit
//                                 patterns w.r.t. vertex association)
//                                 and dump statistics to log file
//                                 0 : disabled
//                                 1 : count tracks of certain types
// - DebugTracksInEvents        -- debug track selections in detail for
//                                 a list of event numbers.
//
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"
#include "TVector3.h"
#include "TString.h"
#include "boost/format.hpp"
#include <algorithm>
#include <sstream>
#include <limits>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BPhysVertexTrackBase::BaseItem::BaseItem(std::string Name,
					   std::string Bname,
					   std::string Prefix) :
    name(std::move(Name)), bname(std::move(Bname)), prefix(std::move(Prefix)) {
  }
  
  BPhysVertexTrackBase::BaseItem::~BaseItem() {
  }
  
  void BPhysVertexTrackBase::BaseItem::setup(std::string Name,
					     std::string Bname,
					     std::string Prefix) {
    name     = std::move(Name);
    bname    = std::move(Bname);
    prefix   = std::move(Prefix);
  }

  void BPhysVertexTrackBase::BaseItem::setPrefix(std::string Prefix) {
    prefix = std::move(Prefix);
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

  std::string BPhysVertexTrackBase::BaseItem::toString() const {
    boost::format f("nm: %s\nbn: %s");
    f % name % bname;
    return f.str();
  }
  //-------------------------------------------------------------------------
  //
  // helper class (for track types)
  //
  BPhysVertexTrackBase::TrackTypeCounter::
  TrackTypeCounter(BPhysVertexTrackBase& Parent, std::string Name)
    : name(std::move(Name)), m_parent(Parent) {
  }

  BPhysVertexTrackBase::TrackTypeCounter::~TrackTypeCounter() {
  }

  void BPhysVertexTrackBase::TrackTypeCounter::addToCounter(uint64_t atype,
                                                            uint64_t rtype,
                                                            std::string prefix,
                                                            std::string suffix,
                                                            uint64_t counts) {
    boost::format f("%sT%010d_R%010d%s");
    f % (prefix.length() > 0 ? prefix+"_" : "")
      % atype
      % m_parent.m_useTrackTypes[rtype]
      % (suffix.length() > 0 ? "_"+suffix : "");

    addToCounter(f.str(), atype, counts);
  }
  
  void BPhysVertexTrackBase::TrackTypeCounter::addToCounter(std::string name,
                                                            uint64_t atype,
                                                            uint64_t counts) {

    NameCountMap_t::const_iterator it = m_cnts.find(name);

    if ( it != m_cnts.end() ) {
      m_cnts[name].first += counts;
    } else {
      m_cnts[name] = std::make_pair(counts, atype);
    }
  }
  
  std::string BPhysVertexTrackBase::TrackTypeCounter::
  countsToString(uint indent) const {

    boost::format f("%sCounters for %s:\n");
    f % boost::io::group(std::setw(indent), " ") % name; 
    std::string str = f.str();
    
    int lmax(0);
    for (NameCountMap_t::const_iterator it = m_cnts.begin();
         it != m_cnts.end(); ++it) {
      lmax = std::max(lmax, (int)(it->first).length());
    }

    for (NameCountMap_t::const_iterator it = m_cnts.begin();
         it != m_cnts.end(); ++it) {
      boost::format f("%s%-s : %10lld %33s");
      f % boost::io::group(std::setw(indent+4), " ")
        % boost::io::group(std::setw(lmax), it->first)
        % (it->second).first
        % std::bitset<33>((it->second).second).to_string();
      str += f.str() + "\n";
    }
    // clean up last newline
    str.erase(str.length()-1);
    
    return str;
  }
  //--------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  // static members
  const int          BPhysVertexTrackBase::n_track_types    = 33;
  const std::string  BPhysVertexTrackBase::track_type_str[] =
    {"ASSOCPV", "PVTYPE0", "PVTYPE1", "PVTYPE2", "PVTYPE3", "NONE", "NULLVP",
     "CAPVRFN3U0", "CAPVNRN3U0", "CAPVRF3DU0", "CAPVNR3DU0",
     "CAPVRFN3U1", "CAPVNRN3U1", "CAPVRF3DU1", "CAPVNR3DU1",
     "CAPVRFN3U2", "CAPVNRN3U2", "CAPVRF3DU2", "CAPVNR3DU2",
     "CAPVRFNNU3", "CAPVNRNNU3", "CAPVRFNNU4", "CAPVNRNNU4",
     "CAPVRFNNU5", "CAPVNRNNU5", "CAPVRFNNU6", "CAPVNRNNU6",
     "CAPVRFNNU7", "CAPVNRNNU7", "CAPVRFNNU8", "CAPVNRNNU8",
     "CAPVRFNNU9", "CAPVNRNNU9"};
  const uint64_t BPhysVertexTrackBase::track_type_bit[] =
    {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40,
     0x80, 0x100, 0x200, 0x400,
     0x800, 0x1000, 0x2000, 0x4000,
     0x8000, 0x10000, 0x20000, 0x40000,
     0x80000, 0x100000, 0x200000, 0x400000,
     0x800000, 0x1000000, 0x2000000, 0x4000000,
     0x8000000, 0x10000000, 0x20000000, 0x40000000,
     0x80000000, 0x100000000};
  uint64_t BPhysVertexTrackBase::s_track_type_all_cached = 0x0;
  
  // static methods
  const std::string
  BPhysVertexTrackBase::tts(BPhysVertexTrackBase::track_type type) {
    return track_type_str[type];
  }
  
  uint64_t BPhysVertexTrackBase::ttb(BPhysVertexTrackBase::track_type type) {
    return track_type_bit[type];
  }
  
  uint64_t BPhysVertexTrackBase::ttallMin() {
    // only bits 0 - 6
    return 127;
  }

  uint64_t BPhysVertexTrackBase::ttall() {
    if ( s_track_type_all_cached == 0x0 ) {
      for (unsigned int i=0; i<n_track_types; ++i) {
        s_track_type_all_cached |= track_type_bit[i];
      }
    }
    return s_track_type_all_cached;
  }

  uint64_t BPhysVertexTrackBase::rttor(const std::vector<uint64_t> &vtypes) {
    // or of requested track types
    uint64_t ttor(0);
    for (size_t i=0; i<vtypes.size(); ++i) {
      ttor |= vtypes[i];
    }
    return ttor;
  }

  // track to string
  std::string
  BPhysVertexTrackBase::trackToString(const xAOD::TrackParticle* track) {
    std::string str;
    if (track != nullptr) {
      boost::format f("p(%10.4f,%10.4f,%10.4f)\n"
                      "d:(%10.5f,%10.5f,%10.5f,%10.5f,%10.6f)");
      f % (track->p4()).Px() % (track->p4()).Py() % (track->p4()).Pz();
      f % track->d0() % track->z0() % track->phi0() % track->theta();
      f % track->qOverP();
      str = f.str();
    } // if track
  return str;
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
      if ( !ostr.empty() ) ostr += "\n";
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
      m_tvaTool("CP::TrackVertexAssociationTool"),
      m_tvaToolHasWpLoose(false),
      m_tracks(NULL), m_tracksAux(NULL), m_nEvtsSeen(0), m_eventInfo(nullptr),
      m_trackTypesUsed(0), m_runNumber(0), m_evtNumber(0),
      m_debugTracksInThisEvent(false) {
    
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
    declareProperty("TVATool"               , m_tvaTool);
    declareProperty("PVContainerName", m_pvContainerName = "PrimaryVertices");
    declareProperty("RefPVContainerNames"   , m_refPVContainerNames);
    declareProperty("DoVertexType"          , m_doVertexType = 8);
    declareProperty("UseTrackTypes"         , m_useTrackTypes = {7});
    declareProperty("IncPrecVerticesInDecay", m_incPrecVerticesInDecay = true);
    declareProperty("MinNTracksInPV"        , m_minNTracksInPV = 0);
    declareProperty("PVTypesToConsider"     , m_pvTypesToConsider = {1,3});
    declareProperty("DebugTrackTypes"       , m_debugTrackTypes=0);
    declareProperty("DebugTracksInEvents"   , m_debugTracksInEvents = {});
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

    // TrackVertexAssociationTool
    ATH_CHECK(m_tvaTool.retrieve());
    // take note of working point
    // const std::string tvaWp("Loose");
    const std::string tvaWp =
      dynamic_cast<CP::TrackVertexAssociationTool*>(m_tvaTool.get())->getProperty("WorkingPoint").toString();
    m_tvaToolHasWpLoose = (tvaWp == "Loose");
    
    // initialize PV-to-SV association type vector
    initPvAssocTypeVec();

    // initialize track type request pattern
    m_trackTypesUsed = rttor(m_useTrackTypes);

    // initialize track type counters
    if ( m_debugTrackTypes > 0 ) {
      m_mttc = std::make_unique<TrackTypeCounter>(*this, name());    
    }

    ATH_MSG_DEBUG("BPhysVertexTrackBase::initialize() -- end");

    return initializeHook();
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::finalize() {

    ATH_MSG_DEBUG("BPhysVertexTrackBase::finalize()");

    // dump track type counters to log
    if ( m_debugTrackTypes > 0 ) {
      ATH_MSG_INFO("Track type counters:\n" << m_mttc->countsToString());
    }
    
    // everything all right
    return finalizeHook();
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysVertexTrackBase::addBranches() const {

    ATH_MSG_DEBUG("BPhysVertexTrackBase::addBranches() -- begin");

    // counter
    m_nEvtsSeen++;

    // run and event numbers
    CHECK(evtStore()->retrieve(m_eventInfo));
    m_runNumber = m_eventInfo->runNumber();
    m_evtNumber = m_eventInfo->eventNumber();

    // debug tracks in current event?
    m_debugTracksInThisEvent = (std::find(m_debugTracksInEvents.begin(),
                                          m_debugTracksInEvents.end(),
                                          m_evtNumber)
                                != m_debugTracksInEvents.end());
    
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
      const xAOD::VertexContainer*     svtxContainer    = NULL;
      const xAOD::VertexAuxContainer*  svtxAuxContainer = NULL;
      // refitted primary vertex container and its auxilliary store
      const xAOD::VertexContainer*     refPVContainer    = NULL;
      const xAOD::VertexAuxContainer*  refPVAuxContainer = NULL;

      // retrieve from StoreGate
      CHECK(evtStore()->retrieve(svtxContainer, m_vertexContainerNames[i]));
      CHECK(evtStore()->retrieve(svtxAuxContainer,
				 m_vertexContainerNames[i]+"Aux."));
      ATH_MSG_DEBUG("Found SV collection with key "
		    << m_vertexContainerNames[i]);
      CHECK(evtStore()->retrieve(refPVContainer   ,
				 m_refPVContainerNames[i]));
      CHECK(evtStore()->retrieve(refPVAuxContainer,
				 m_refPVContainerNames[i]+"Aux."));
      ATH_MSG_DEBUG("Found refitted PV collection with key "
		    << m_refPVContainerNames[i]);
      
      // vertex container depending setup in derived class
      CHECK(addBranchesVCSetupHook(i));
      
      // loop over secondary vertices
      for (xAOD::VertexContainer::const_iterator vtxItr =
	     svtxContainer->begin(); vtxItr!=svtxContainer->end();
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
                                                     const xAOD::Vertex* vtx,
                                                     bool doDCAin3D,
                                                     int chi2DefToUse
                                                     ) const {

    return getTrackLogChi2DCA(track, vtx, doDCAin3D, chi2DefToUse)[4];
  }
  //--------------------------------------------------------------------------  
  // getTrackLogChi2DCA()
  // Calculate logChi2 (= log((d0/d0e)^2+(z0/z0e)^2) contribution of a
  // track at the position closest to a position and
  // the distance of closest approach of a track w.r.t.
  // a position.  Either only in the transverse plane or in 3 dimensions.
  // Option chi2DefToUse:
  //   0 : from track perigee with uncertainties from track only
  //   1 : from track perigee with uncertainties from track and vertex
  //   2 : simple extrapolation from track parameters
  //       with uncertainties from track and vertex
  //   3 : CalcLogChi2toPV method from NtupleMaker using xAOD::TrackingHelpers.
  //      (only track uncertainties)
  //   4 : CalcLogChi2toPV method from NtupleMaker using xAOD::TrackingHelpers.
  //      (track and vertex uncertainties)
  //   5 : use TrackVertexAssociationTool
  //   6 : full 3D chi2 from track perigee with uncertainties
  //       from track and vertex (sum of 3x3 covariance matrices)
  //   7 : full 3D chi2 from track perigee with uncertainties
  //       from track and vertex (sum of 2x2 covariance matrices)
  //   8 : simple extrapolation from track parameters with uncertainties
  //       from track and vertex (sum of 3x3 covariance matrices)
  //   9   simple extrapolation from track parameters with uncertainties
  //       from track and vertex (sum of 2x2 covariance matrices)
  // Returned vector components:
  // 0: d0, 1: d0Err, 2: z0, 3: z0Err, 4: logChi2, 5: dca, 6: okFlag
  // 7: vtxErrPart2, 8: trkErrPart2, 9: phi0Used
  //--------------------------------------------------------------------------
  std::vector<double>
  BPhysVertexTrackBase::getTrackLogChi2DCA(const xAOD::TrackParticle* track,
                                           const xAOD::Vertex* vtx,
                                           bool doDCAin3D,
                                           int  chi2DefToUse) const {
    // presets
    std::vector<double> res = {-999., -99., -999., -99., -100., -100., -1.,
                               -99., -99., -999.};
    
    const Amg::Vector3D   pos = vtx->position();
    const AmgSymMatrix(3) poscov = vtx->covariancePosition();
    
    if ( track != NULL ) {
      if ( chi2DefToUse < 2 || (chi2DefToUse > 5 && chi2DefToUse < 8) ) {
        // use track perigee method
        std::unique_ptr<const Trk::Perigee>
          trkPerigee(m_trackToVertexTool->perigeeAtVertex(*track, pos));
        if ( trkPerigee != NULL  ) {
          res[0] = trkPerigee->parameters()[Trk::d0];
          res[2] = trkPerigee->parameters()[Trk::z0];
          const AmgSymMatrix(5)* locError = trkPerigee->covariance();
          if ( locError != NULL ) {
            // uncertainties from track
            res[1] = Amg::error(*locError, Trk::d0);
            res[3] = Amg::error(*locError, Trk::z0);
            if ( chi2DefToUse == 1 ) {
              // add uncertainties from vertex
              Amg::Vector3D perppt(trkPerigee->momentum().y()/trkPerigee->pT(),
                                   -trkPerigee->momentum().x()/trkPerigee->pT(),
                                   0.);
              double vtxD0Err2 = perppt.transpose()*poscov*perppt;
              res[1] = sqrt( pow(res[1], 2.) + vtxD0Err2 );
              res[3] = sqrt( pow(res[3], 2.) + poscov(2,2) );
            }
            if ( chi2DefToUse < 2 ) {
              if ( fabs(res[1]) > 0. && fabs(res[3]) > 0. ) {
                res[4] = log( pow(res[0]/res[1], 2.)
                              + pow(res[2]/res[3], 2.) );
                res[6] = 2.; // ok
              } else {
                ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                                << " d0 = " << res[0] << ", d0Err = "
                                << res[1] << ", z0 = " << res[2]
                                << ", z0Err = " << res[3]);
              }
            }
            // chi2DefToUse 6 or 7
            if ( chi2DefToUse > 5 && chi2DefToUse < 8 ) {
              double phi0 = trkPerigee->parameters()[Trk::phi0];
              double doca = sqrt(pow(res[0],2.) + pow(res[2], 2.));
              res[9] = phi0;
              if ( doca > 0. ) {
                if ( chi2DefToUse == 6 ) {
                  AmgMatrix(5,3) dmat = AmgMatrix(5,3)::Zero();
                  dmat(0,0) = -sin(phi0);
                  dmat(0,1) =  cos(phi0);
                  dmat(1,2) =  1.;
                  dmat(2,0) = -res[0]*cos(phi0);
                  dmat(2,1) = -res[0]*sin(phi0);
                  AmgSymMatrix(3) mCovTrk3D = dmat.transpose()*(*locError)*dmat;
                  Amg::Vector3D dvec(-res[0]*sin(phi0), res[0]*cos(phi0),
                                     res[2]); // (x,y,z)
                  Amg::Vector3D duvec = dvec.unit();
                  // log(chi2) = log( docavec^T * V^-1 * docavec )
                  res[4] = log( dvec.transpose() * (poscov+mCovTrk3D).inverse()
                                * dvec );
                  res[7] = duvec.transpose()*poscov*duvec;
                  res[8] = duvec.transpose()*mCovTrk3D*duvec;
                  res[6] = 3.; // ok
                }
                if ( chi2DefToUse == 7 ) {
                  AmgMatrix(3,2) dmat = AmgMatrix(3,2)::Zero();
                  dmat(0,0) = -sin(phi0);
                  dmat(1,0) =  cos(phi0);
                  dmat(2,0) =  0.;
                  dmat(0,1) =  0.;
                  dmat(1,1) =  0.;
                  dmat(2,1) =  1.;
                  AmgSymMatrix(2) mCovVtx2D = dmat.transpose()*poscov*dmat;
                  AmgSymMatrix(2) mCovTrk2D = AmgSymMatrix(2)::Zero();
                  mCovTrk2D(0,0) = (*locError)(Trk::d0,Trk::d0);
                  mCovTrk2D(0,1) = (*locError)(Trk::d0,Trk::z0);
                  mCovTrk2D(1,0) = (*locError)(Trk::d0,Trk::z0);
                  mCovTrk2D(1,1) = (*locError)(Trk::z0,Trk::z0);
                  Amg::Vector2D dvec(res[0], res[2]); // (d0, z0)
                  Amg::Vector2D duvec = dvec.unit();
                  // log(chi2) = log( (d0, z0) * V^-1 * (d0, z0)^T )
                  res[4] = log( dvec.transpose()*(mCovVtx2D+mCovTrk2D).inverse()
                                * dvec );
                  res[7] = duvec.transpose()*mCovVtx2D*duvec;
                  res[8] = duvec.transpose()*mCovTrk2D*duvec;
                  res[6] = 4.; // ok
                }
              } else {
                ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                                << " doca == 0 !");
              }
            } // if chi2DefToUse > 5 && chi2DefToUse < 8
            res[5] = doDCAin3D ?
              sqrt( pow(res[0], 2.) + pow(res[2], 2.) ) : res[0];
            res[6] += 1.; // ok
          } else {
            ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                            " locError pointer is NULL!");
          }
        } else {
          ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                          " trkPerigee pointer is NULL!");
        } // if trkPerigee

      } else if ( chi2DefToUse == 2
                  || (chi2DefToUse > 7 && chi2DefToUse < 10 )) {
        // simple extrapolation method
        // (directly taken from NtupleMaker for comparisons)

        // SV position and covariance matrix
        TVector3 SV_def(vtx->x(), vtx->y(), vtx->z());
        const AmgSymMatrix(3)& SV_cov = poscov;
        
        // chi2 track to SV
        double px      = ( track->p4() ).Px();
        double py      = ( track->p4() ).Py();
        double pt      = ( track->p4() ).Pt();
        double d0      = track->d0();
        double d0Err2  = track->definingParametersCovMatrixVec()[0];
        double z0      = track->z0();
        double z0Err2  = track->definingParametersCovMatrixVec()[2];
        double theta   = track->theta();
        double d0z0Cov = track->definingParametersCovMatrixVec()[1];
        double phi     = track->phi();

        TVector3 trk_origin(  track->vx(),  track->vy(),  track->vz() );
        TVector3 SV = SV_def - trk_origin;
        
        // calc. error in direction perpendicular to pT (still x-y plane)
        double upx        = py/pt;
        double upy        = -px/pt;
        double d0toSV     = d0 + (SV[0]*upx + SV[1]*upy);
        double d0toSVErr2 = upx*SV_cov(0, 0)*upx + 2*upx*SV_cov(1, 0)*upy
          + upy*SV_cov(1, 1)*upy + d0Err2;

        upx = px/pt;
        upy = py/pt;
        double cot_theta  = cos(theta)/sin(theta);
        double z0corr     = (SV[0]*upx + SV[1]*upy)*cot_theta;
        double z0toSV     = z0 + z0corr - SV[2];
        double z0toSVErr2 = SV_cov(2, 2) + z0Err2;

        double docaSV     = sqrt( pow(d0toSV, 2) + pow(z0toSV, 2) );
     
        double chi2testSV(999.);
        if ( chi2DefToUse == 2 ) {
          if (d0toSVErr2 !=0 && z0toSVErr2 != 0)
            chi2testSV = log(pow( d0toSV, 2)/d0toSVErr2
                             + pow( z0toSV, 2)/z0toSVErr2);
          // set results
          res = {d0toSV, sqrt(d0toSVErr2), z0toSV, sqrt(z0toSVErr2),
                 chi2testSV, (doDCAin3D ? docaSV : d0toSV), 4,
                 -99., -99., -999.};
        }
        if ( chi2DefToUse > 7 && chi2DefToUse < 10 ) {
          if ( docaSV > 0. ) {
            if ( chi2DefToUse == 8 ) {
              AmgMatrix(5,3) dmat = AmgMatrix(5,3)::Zero();
              dmat(0,0) = -sin(phi);
              dmat(0,1) =  cos(phi);
              dmat(1,2) =  1.;
              dmat(2,0) = -d0toSV*cos(phi);
              dmat(2,1) = -d0toSV*sin(phi);
              const AmgSymMatrix(5) mCovTrk5D =
                track->definingParametersCovMatrix();
              AmgSymMatrix(3) mCovTrk3D = dmat.transpose()*mCovTrk5D*dmat;
              Amg::Vector3D dvec(-d0toSV*sin(phi), d0toSV*cos(phi),
                                 z0toSV); // (x,y,z)
              Amg::Vector3D duvec = dvec.unit();
              // log(chi2) = log( docavec^T * V^-1 * docavec )
              double chi2testSV = log( dvec.transpose()
                                       * (poscov+mCovTrk3D).inverse()
                                       * dvec );
              double vtx3DErr2 = duvec.transpose()*poscov*duvec;
              double trk3DErr2 = duvec.transpose()*mCovTrk3D*duvec;
              // set results
              res = {d0toSV, sqrt(d0Err2), z0toSV, sqrt(z0Err2),
                     chi2testSV, (doDCAin3D ? docaSV : d0toSV), 5,
                     vtx3DErr2, trk3DErr2, phi};
            }
            if ( chi2DefToUse == 9 ) {
              AmgMatrix(3,2) dmat = AmgMatrix(3,2)::Zero();
              dmat(0,0) = -sin(phi);
              dmat(1,0) =  cos(phi);
              dmat(2,0) =  0.;
              dmat(0,1) =  0.;
              dmat(1,1) =  0.;
              dmat(2,1) =  1.;
              AmgSymMatrix(2) mCovVtx2D = dmat.transpose()*SV_cov*dmat;
              AmgSymMatrix(2) mCovTrk2D = AmgSymMatrix(2)::Zero();
              mCovTrk2D(0,0) = d0Err2;
              mCovTrk2D(0,1) = d0z0Cov;
              mCovTrk2D(1,0) = d0z0Cov;
              mCovTrk2D(1,1) = z0Err2;
              Amg::Vector2D dvec(d0toSV, z0toSV);
              Amg::Vector2D duvec = dvec.unit();
              // log(chi2) = log( (d0, z0) * V^-1 * (d0, z0)^T )
              chi2testSV = log( dvec.transpose()*(mCovVtx2D+mCovTrk2D).inverse()
                                * dvec );
              double vtx2DErr2 = duvec.transpose()*mCovVtx2D*duvec;
              double trk2DErr2 = duvec.transpose()*mCovTrk2D*duvec;

              if ( vtx2DErr2 < 0. || trk2DErr2 < 0. ) {
                ATH_MSG_WARNING("BPhysVertexTrackBase::"
                                "getTrackLogChi2DCA(): "
                                << "vtx2DErr2 = " << vtx2DErr2
                                << " trk2DErr2 = " << trk2DErr2
                                << " chi2testSV = " << chi2testSV);
                ATH_MSG_WARNING("dvec = " << dvec);
                ATH_MSG_WARNING("mCovVtx2D = " << mCovVtx2D);
                ATH_MSG_WARNING("mCovTrk2D = " << mCovTrk2D);
                ATH_MSG_WARNING("dmat = " << dmat);
                ATH_MSG_WARNING("SV_cov = " << SV_cov);
                ATH_MSG_WARNING("det(mCovVtx2D) = " << mCovVtx2D.determinant());
                ATH_MSG_WARNING("det(mCovTrk2D) = " << mCovTrk2D.determinant());
                ATH_MSG_WARNING("det(SV_cov) = " << SV_cov.determinant());
                ATH_MSG_WARNING("d0toSV = " << d0toSV
                                << " z0toSV = " << z0toSV
                                << " phi = " << phi
                                << " docaSV = " << docaSV);
              }
                 
              // set results
              res = {d0toSV, sqrt(d0Err2), z0toSV, sqrt(z0Err2),
                     chi2testSV, (doDCAin3D ? docaSV : d0toSV), 6,
                     vtx2DErr2, trk2DErr2, phi};
            }
          } else {
            ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                            << " docaSV == 0 !");
          }
        } // if chi2DefToUse > 7 && chi2DefToUse < 10

      } else if ( chi2DefToUse > 2 && chi2DefToUse < 5 ) {
        // CalcLogChi2toPV method using xAOD::TrackingHelpers
        // (simply taken from NtupleMaker for comparisons)
        // N.B. z0significance method of the helper doesn't include pv_z0
        // uncertainty
        double d0sign(0.);
        if (chi2DefToUse == 4) {
          d0sign =
            xAOD::TrackingHelpers::d0significance(track,
                                                  m_eventInfo->beamPosSigmaX(),
                                                  m_eventInfo->beamPosSigmaY(),
                                                  m_eventInfo->beamPosSigmaXY()
                                                  );
        } else {
          d0sign = xAOD::TrackingHelpers::d0significance( track );
        }
        // trk z0 is expressed relative to the beamspot position along z-axis
        // (trk->vz())
        // DCA always in 3D
        double z0toPV = track->z0() + track->vz() - vtx->z();
        double z0Err2 = track->definingParametersCovMatrixVec()[2];
        if (chi2DefToUse == 4) z0Err2+= vtx->covariancePosition()(2,2);
        double z0sign = z0toPV / sqrt( z0Err2 );
        double chi2 = log( pow(d0sign, 2.) + pow(z0sign, 2.) );
        // set results
        res = {-999., -99., z0toPV, sqrt(z0Err2), chi2, -100., 4, -99., -99.,
               -999.};
        
      } // if chi2DefToUse
    } else {
      ATH_MSG_WARNING("BPhysVertexTrackBase::getTrackLogChi2DCA():"
                      " track pointer is NULL!");
      res[6] = -2.;
    } // if track != NULL
    return res;
  }
  //--------------------------------------------------------------------------
  // detTrackTypes(): returns a bit pattern of the applicable
  // track types from {ASSOCPV, PVTYPE0, PVTYPE1, PVTYPE2, PVTYPE3, NONE,
  // NULLVP, CAPVXXXXXXX, ...} (or'd).
  //--------------------------------------------------------------------------
  uint64_t BPhysVertexTrackBase::detTrackTypes(const xAOD::TrackParticle* track,
                                               const xAOD::Vertex* candPV,
                                               const xAOD::Vertex* candRefPV) const {
    int bits = 0x0;

    // PVTYPE0 - PVTYPE3, NONE
    ATH_MSG_ERROR("BPhysVertexTrackBase::detTrackTypes must be adjusted due to changes in TrackParticle");

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
      //
      // CLOSEAPV
      for (unsigned int i=7; i<n_track_types; ++i) {
        if ( (track_type_bit[i] & m_trackTypesUsed) > 0x0 ) {
          bool useRefittedPvs = ( i%2 == 1 );
          bool doDCAin3D      = ( (i-7)%4 > 1 );
          int  chi2DefToUse   = (i-7)/4;
          // adjustment above bit 20
          if ( i > 20 ) {
            doDCAin3D    = true;
            chi2DefToUse = (i-13)/2;
          }
          const xAOD::Vertex* minChi2PV(nullptr);
          if ( chi2DefToUse == 5 ) {
            minChi2PV =
              findAssocPV(track, candPV, candRefPV, m_pvTypesToConsider,
                          m_minNTracksInPV, useRefittedPvs);
          } else {
            minChi2PV =
              findMinChi2PV(track, candPV, candRefPV, m_pvTypesToConsider,
                            m_minNTracksInPV, useRefittedPvs,
                            doDCAin3D, chi2DefToUse).first;
          } // if chi2DefToUse
          if ( candPV == minChi2PV
               || (candRefPV != nullptr && candRefPV == minChi2PV) ) {
            bits |= track_type_bit[i];
          }
        } // if m_trackTypesUsed
      } // for i

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

    std::string bname(buildBranchBaseName(its, ipv, itt));
    
    // tracks to be considered
    TrackBag tracks;
    for (xAOD::TrackParticleContainer::const_iterator trkItr =
	   inpTracks->begin(); trkItr != inpTracks->end(); ++trkItr) {
      const xAOD::TrackParticle* track = *trkItr;
      uint64_t trackTypesForTrack(0x0);
      // debug track types (before any cuts)
      if ( m_debugTrackTypes > 0 ) {
        trackTypesForTrack = detTrackTypes(track, candPV, candRefPV);
        m_mttc->addToCounter(trackTypesForTrack, itt, bname, "all");
      }
      // track selection check
      if ( ! m_trackSelectionTools[its]->accept(*track, candRefPV) ) continue;
      // debug track types (after track selection cuts)
      if ( m_debugTrackTypes > 0 ) {
        m_mttc->addToCounter(trackTypesForTrack, itt, bname, "ats");
      }

      // calcluation of track type bits not necessary if all bits requested
      if ( ! ((unsigned int)m_useTrackTypes[itt] == ttall() ||
              (unsigned int)m_useTrackTypes[itt] == ttallMin()) ) {
        // track type check -- determination if not in debugging mode
        // delayed for execution speed reasons
        if ( trackTypesForTrack == 0x0 ) {
          trackTypesForTrack = detTrackTypes(track, candPV, candRefPV);
        }
        if ( (trackTypesForTrack &  m_useTrackTypes[itt]) == 0x0 ) {
          continue;
        }
      }
      // debug track types (after track type cuts)
      if ( m_debugTrackTypes > 0 ) {
        m_mttc->addToCounter(trackTypesForTrack, itt, bname, "att");
      }
      // track not in list of tracks to exclude
      if ( std::find(exclTracks.begin(), exclTracks.end(), track)
           != exclTracks.end() ) continue;
      // debug track types (after all cuts)
      if ( m_debugTrackTypes > 0 ) {
        m_mttc->addToCounter(trackTypesForTrack, itt, bname, "fin");
      }
      // tracks that survived so far
      tracks.push_back(track);
    } // for

    return tracks;
  }
  //--------------------------------------------------------------------------
  // buildBranchBaseName: build branch name from track selection, primary
  // vertex association and track type qualifiers.
  //--------------------------------------------------------------------------
  std::string BPhysVertexTrackBase::buildBranchBaseName(unsigned int its,
                                                        unsigned int ipv,
                                                        unsigned int itt,
                                                        std::string preSuffix)
    const {
    
    ATH_MSG_DEBUG("BPhysVertexTrackBase::buildBranchBaseName -- begin");
    
    std::string tsName   = m_trackSelectionTools[its].name();
    std::string pvAssoc  = xAOD::BPhysHelper::pv_type_str[m_pvAssocTypes[ipv]];

    // need to get part of tsname after last underscore
    std::size_t ipos = tsName.find_last_of("_");
    if ( ipos != std::string::npos ) tsName = tsName.substr(ipos+1);

    // format it nicely
    boost::format f("T%010d_%s_%s%s%s");
    f % m_useTrackTypes[itt] % tsName % pvAssoc;
    f % (preSuffix.length() > 0 ? "_"+preSuffix : "");
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
  //
  // Find primary vertex to which a track is closest to in terms of minimum
  // chi2 to any primary vertex.  Replace primary vertex by refitted primary
  // vertex (for B candidate associated primary vertices)
  // if appropriate (and available).
  // Only consider primary vertices of specified primary vertex types and
  // with a minimum number of tracks.
  //
  //--------------------------------------------------------------------------
  std::pair<const xAOD::Vertex*, double>
  BPhysVertexTrackBase::findMinChi2PV(const xAOD::TrackParticle* track,
                                      const xAOD::Vertex* candPV,
                                      const xAOD::Vertex* candRefPV,
                                      const std::vector<uint64_t>& pvtypes,
                                      const int minNTracksInPV,
                                      const bool useRefittedPvs,
                                      const bool doDCAin3D,
                                      const int chi2DefToUse) const {

    double minChi2 = std::numeric_limits<double>::max();
    const xAOD::Vertex* minChi2PV(nullptr);
    
    for (auto pvtx: *m_pvtxContainer) {
      if ( pvtx != nullptr ) {
        if ( std::find(pvtypes.begin(),pvtypes.end(),pvtx->vertexType())
             != pvtypes.end() ) {
          const xAOD::Vertex* cvtx = pvtx;
          // replace by refitted PV if associated PV matches orignal PV
          if ( useRefittedPvs && pvtx == candPV ) {
            if ( candRefPV != nullptr ) {
              cvtx = candRefPV;
            } else {
              ATH_MSG_WARNING(" BPhysVertexTrackBase::findMinChi2PV:"
                              << " candRefPV == NULL!");
              continue;
            }
          } // if pvtx == candPV
          if ( (int)cvtx->nTrackParticles() >= minNTracksInPV ) {
            double chi2 = getTrackLogChi2DCA(track, cvtx, doDCAin3D,
                                             chi2DefToUse)[4];
            if ( chi2 < minChi2 ) {
              minChi2   = chi2;
              minChi2PV = cvtx;
            } // if chi2 < minChi2
          } // if minNTracksInPV
        } // if pvTypes in pvtypes vector
      } // if pvtx != nullptr
    } // for pvtx

    return std::make_pair(minChi2PV, minChi2);
  }  
  //--------------------------------------------------------------------------
  //
  // Find primary vertex to which a track is closest using the
  // TrackVertexAssociationTool.  Replace primary vertex by refitted primary
  // vertex (for B candidate associated primary vertices)
  // if appropriate (and available).
  // Only consider primary vertices of specified primary vertex types and
  // with a minimum number of tracks.
  //
  //--------------------------------------------------------------------------
  const xAOD::Vertex*
  BPhysVertexTrackBase::findAssocPV(const xAOD::TrackParticle* track,
                                    const xAOD::Vertex* candPV,
                                    const xAOD::Vertex* candRefPV,
                                    const std::vector<uint64_t>& pvtypes,
                                    const int minNTracksInPV,
                                    const bool useRefittedPvs) const {

    // select PVs to be considered/replace candPV by candRefPV if requested
    std::vector<const xAOD::Vertex*> vpvtx;
    for (auto pvtx: *m_pvtxContainer) {
      if ( pvtx != nullptr ) {
        if ( std::find(pvtypes.begin(),pvtypes.end(),pvtx->vertexType())
             != pvtypes.end() ) {
          const xAOD::Vertex* cvtx = pvtx;
          // replace by refitted PV if associated PV matches orignal PV
          if ( useRefittedPvs && pvtx == candPV ) {
            if ( candRefPV != nullptr ) {
              cvtx = candRefPV;
            } else {
              ATH_MSG_WARNING("BPhysVertexTrackBase::findAssocPV:"
                              << " candRefPV == NULL!");
              continue;
            }
          } // if pvtx == candPV
          if ( (int)cvtx->nTrackParticles() >= minNTracksInPV ) {
            vpvtx.push_back(cvtx);
          } // if minNTracksInPV
        } // if pvTypes in pvtypes vector
      } // if pvtx != nullptr
    } // for pvtx

    const xAOD::Vertex* assocPV(NULL);
    if ( useRefittedPvs && m_tvaToolHasWpLoose ) {
      // check whether track is in refitted PV - if so accept
      // Need to do this here as the TrackVertexAssociationTool
      // with WP 'Loose' only checks the track->vertex() pointer
      // which always points to the original PV.
      for (const auto &tp : candRefPV->trackParticleLinks()) {
        if ( *tp == track ) {
          // track is part of refitted PV -- accept it
          assocPV = candRefPV;
          break;
        }
      } // for tp
      // if not matching use the TrackVertexAssociationTool (other PVs etc)
      if ( assocPV == nullptr ) {
        assocPV = m_tvaTool->getUniqueMatchVertex(*track, vpvtx);
      }
    } else {
      assocPV = m_tvaTool->getUniqueMatchVertex(*track, vpvtx);
    } // if useRefittedPvs && m_tvaToolHasWpLoose
    if ( assocPV == nullptr ) {
      ATH_MSG_WARNING("BPhysVertexTrackBase::findAssocPV:"
                      << " assocPV == NULL for track!"
                      << " len(vpvtx) = " << vpvtx.size()
                      << " useRefittedPvs = " << useRefittedPvs
                      << " minNTracksInPV = " << minNTracksInPV);
    }
    
    return assocPV;
  }  
  //--------------------------------------------------------------------------  
}
