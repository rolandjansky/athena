/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BVertexClosestTrackTool.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add B vertex closest track information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
// Job options provided by this class:
// - IsolationConeSizes         -- List of isolation cone sizes
// - IsoTrkImpChi2Max           -- List of maximum chi2 cuts for association of
//                                 tracks to the primary vertex picked.
//                                 (Must contain the same number of elements
//                                  as the IsolationConeSizes list.)
// - UseOptimizedAlgo           -- Use the speed-optimized algorithm.
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BVertexClosestTrackTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "DataModel/ElementLink.h"

#include "boost/format.hpp"
#include "TVector3.h"
#include <algorithm>
#include <sstream>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BVertexClosestTrackTool::CtItem::CtItem(std::string Name, std::string Prefix,
					  std::string Bname,
					  double Dca, double DcaErr,
					  double Zca, double ZcaErr,
					  int    NTrksChi2,
					  xAOD::TrackParticle* CloseTrack)
    : BaseItem(Name, Bname, Prefix), dca(Dca), dcaErr(DcaErr),
      zca(Zca), zcaErr(ZcaErr), nTrksChi2(NTrksChi2), closeTrack(CloseTrack) {
  }

  BVertexClosestTrackTool::CtItem::~CtItem() {
  }
  
  void BVertexClosestTrackTool::CtItem::setup(std::string Name,
					      std::string Bname,
					      std::string Prefix) {
    BaseItem::setup(Name, Bname, Prefix);
    dca        = -999.;
    dcaErr     = -99.;
    zca        = -999.;
    zcaErr     = -99.;
    nTrksChi2  = 0;
    closeTrack = NULL;
  }
  
  void BVertexClosestTrackTool::CtItem::setup(std::string Name,
					      std::string Bname,
					      std::string Prefix, 
					      double Dca, double DcaErr,
					      double Zca, double ZcaErr,
					      int    NTrksChi2,
					      xAOD::TrackParticle*
					      CloseTrack) {
    BaseItem::setup(Name, Bname, Prefix);
    dca        = Dca;
    dcaErr     = DcaErr;
    zca        = Zca;
    zcaErr     = ZcaErr;
    nTrksChi2  = NTrksChi2;
    closeTrack = CloseTrack;
  }

  void BVertexClosestTrackTool::CtItem::resetVals() {
    dca        = -999.;
    dcaErr     =  -99.;
    zca        = -999.;
    zcaErr     =  -99.;
    nTrksChi2  =    0;
    closeTrack = NULL;
  }

  void BVertexClosestTrackTool::CtItem::copyVals(const BaseItem& item) {
    copyVals((const CtItem&)item);
  }
    
  void BVertexClosestTrackTool::CtItem::copyVals(const CtItem& item) {
    dca        = item.dca;
    dcaErr     = item.dcaErr;
    zca        = item.zca;
    zcaErr     = item.zcaErr;
    nTrksChi2  = item.nTrksChi2;
    closeTrack = item.closeTrack;
  }
  
  std::string BVertexClosestTrackTool::CtItem::dcaName() {
    return buildName("DCA");
  }
  
  std::string BVertexClosestTrackTool::CtItem::dcaErrName() {
    return buildName("DCAError");
  }
  
  std::string BVertexClosestTrackTool::CtItem::zcaName() {
    return buildName("ZCA");
  }
  
  std::string BVertexClosestTrackTool::CtItem::zcaErrName() {
    return buildName("ZCAError");
  }
  
  std::string BVertexClosestTrackTool::CtItem::nTrksChi2Name() {
    return buildName("NTracksChi2");
  }

  std::string BVertexClosestTrackTool::CtItem::closeTrackName() {
    return buildName("CloseTrack", "_Link");
  }

  //--------------------------------------------------------------------------
  BVertexClosestTrackTool::BVertexClosestTrackTool(const std::string& t,
					   const std::string& n,
					   const IInterface*  p)
    : BPhysVertexTrackBase(t,n,p) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("CloseTrackMinDCAin3D" , m_minDCAin3D            = true);
    declareProperty("CloseTrackMaxLogChi2" , m_closeTrackMaxLogChi2  = -1.);
    declareProperty("NCloseTrackMaxLogChi2", m_nCloseTrackMaxLogChi2 = -1.);
  }
  //--------------------------------------------------------------------------
  StatusCode BVertexClosestTrackTool::initializeHook() {
  
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initializeHook() -- begin");

    // initialize results array
    initResults();

    ATH_MSG_DEBUG("BVertexClosestTrackTool::initializeHook() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BVertexClosestTrackTool::finalizeHook() {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::finalizeHook()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode
  BVertexClosestTrackTool::addBranchesVCSetupHook(size_t ivc) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCLoopHook() -- begin");

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCSetupHook: "
		  << "Vertex container index " << ivc
		  << " for collection " << m_vertexContainerNames[ivc]
		  << " with prefix " << m_branchPrefixes[ivc]);
    
    setResultsPrefix(m_branchPrefixes[ivc]);
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCSetupHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  

  //--------------------------------------------------------------------------
  StatusCode
  BVertexClosestTrackTool::addBranchesSVLoopHook(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook() -- begin");

    // calculate closest track values
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook(): "
		  "calculate closest track ...");
    CHECK(calculateValues(vtx));
      
   // save the closest track values
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook(): "
		  "save closest track ...");
    CHECK(saveClosestTrack(vtx));

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Calculate closest track variables -- method called from caching loop
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexClosestTrackTool::calcValuesHook(const xAOD::Vertex* vtx,
					  const unsigned int ipv,
					  const unsigned int its,
					  const unsigned int itt) const {
    
    ATH_MSG_DEBUG("calcValuesHook:  ipv: " << ipv
		  << ", its: " << its << ", itt: " << itt);
 
    // candidate tracks and momentum
    xAOD::BPhysHelper   cand(vtx);

    // tracks to be considered
    TrackBag tracks = selectTracks(m_tracks, cand, ipv, its, itt);

    CtItem& cti = m_results[its][ipv][itt];
    
    // presets
    cti.resetVals();

    double closestTrkDCA = 9999.;
    for (TrackBag::const_iterator trkItr = tracks.begin();
	 trkItr != tracks.end(); ++trkItr) {
      
      // Returned vector components:
      // 0: d0, 1: d0Err, 2: z0, 3: z0Err, 4: logChi2, 5: dca, 6: okFlag
      std::vector<double> vtap =
	getTrackLogChi2DCA(*trkItr, cand.vtx()->position(), m_minDCAin3D);

      ATH_MSG_DEBUG("calcValuesHook: track: " << *trkItr
		    << ", logChi2: " << vtap[4] << ", dca: " << vtap[5]);
 
      // track values at perigee found?
      if ( vtap[6] >= 0. ) {
	ATH_MSG_DEBUG("calcValuesHook: checking track count for "
		      "m_nCloseTrackMaxLogChi2 = " << m_nCloseTrackMaxLogChi2);
	// count tracks
	if ( vtap[4] < m_nCloseTrackMaxLogChi2 ) {
	  cti.nTrksChi2++;
	  ATH_MSG_DEBUG("calcValuesHook: nTrksChi2++ for track " << *trkItr);
	}
	// find closest track
	ATH_MSG_DEBUG("calcValuesHook: log(chi2) check: "
		      "m_closeTrackMaxLogChi2: " << m_closeTrackMaxLogChi2
		      << ", logChi2: " << vtap[4]
		      << ", closestTrkDCA: " << closestTrkDCA
		      << ", dca: " << fabs(vtap[5]));	
	if ( fabs(vtap[5]) < closestTrkDCA &&
	     vtap[4] < m_closeTrackMaxLogChi2 ) {
	  closestTrkDCA  = fabs(vtap[5]);
	  cti.dca        = vtap[0];
	  cti.dcaErr     = vtap[1];
	  cti.zca        = vtap[2];
	  cti.zcaErr     = vtap[3];
	  cti.closeTrack = *trkItr;
	  ATH_MSG_DEBUG("calcValuesHook: closestTrkDCA now: " << closestTrkDCA
			<< " for track " << *trkItr);
	}
      } // if ok
    } // for tracks

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Fill closest track values from cache if found
  //--------------------------------------------------------------------------  
  bool BVertexClosestTrackTool::fastFillHook(const xAOD::Vertex* vtx,
					     const int ipv) const {

    ATH_MSG_DEBUG("fastFillHook: ipv: " << ipv);
    
    bool found(false);
    
    StringIntMap_t::iterator itpv =
      m_pvAssocResMap.find(buildPvAssocCacheName(vtx, ipv));
    if ( itpv != m_pvAssocResMap.end() ) {
      found = true;
      unsigned int nTrackSels  = m_trackSelectionTools.size();
      unsigned int nTrackTypes = m_useTrackTypes.size();
      for (unsigned int its = 0; its < nTrackSels; ++its) {
	for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	  m_results[its][ipv][itt]
	    .copyVals(m_results[its][itpv->second][itt]);
	} // for its
      } // for itt
    } // if found

    ATH_MSG_DEBUG("fastFillHook: cache index: "
		  << buildPvAssocCacheName(vtx, ipv)
		  << ", found ? " << found
		  << ", ipv_ref: "
		  << (found ? itpv->second : -1));

    return found;
  }
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexClosestTrackTool::saveClosestTrack(const xAOD::Vertex* vtx) const {

    typedef ElementLink< xAOD::TrackParticleContainer > TrackParticleLink_t;
    
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    
    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	  CtItem result = m_results[its][ipv][itt];
	  SG::AuxElement::Decorator< float >
	    d_dca_value(result.dcaName());
	  SG::AuxElement::Decorator< float >
	    d_dcaErr_value(result.dcaErrName());
	  SG::AuxElement::Decorator< float >
	    d_zca_value(result.zcaName());
	  SG::AuxElement::Decorator< float >
	    d_zcaErr_value(result.zcaErrName());
	  SG::AuxElement::Decorator< int >
	    d_nTrksChi2_value(result.nTrksChi2Name());
	  d_dca_value(*vtx)       = result.dca;
	  d_dcaErr_value(*vtx)    = result.dcaErr;
	  d_zca_value(*vtx)       = result.zca;
	  d_zcaErr_value(*vtx)    = result.zcaErr;
	  d_nTrksChi2_value(*vtx) = result.nTrksChi2; 
	  ATH_MSG_DEBUG("BVertexClosestTrackTool::saveClosestTrack() "
			<< "-- dca: " << result.dcaName()
			<< ", dcaErr: " << result.dcaErrName()
			<< ", zca: " << result.zcaName()
			<< ", zcaErr: " << result.zcaErrName()
			<< ", nTrksChi2: " << result.nTrksChi2Name());
	  ATH_MSG_DEBUG("BVertexClosestTrackTool::saveClosestTrack() "
			<< "-- vertex: ("
			<< vtx->x() << ", "
			<< vtx->y() << ", "
			<< vtx->z() << ")"
			<< ", dca: " << result.dca
			<< ", dcaErr: " << result.dcaErr
			<< ", zca: " << result.zca
			<< ", zcaErr: " << result.zcaErr
			<< ", nTrksChi2: " << result.nTrksChi2);
	  // add ElementLink to closest track
	  std::string linkName = result.closeTrackName();
	  SG::AuxElement::Decorator<TrackParticleLink_t>
	    tpLinkDecor(linkName);
	  TrackParticleLink_t tpLink;
	  if ( result.closeTrack != NULL ) {
	    tpLink.toContainedElement( *m_tracks, result.closeTrack );
	  }
	  ATH_MSG_DEBUG("saveClosestTrack: Decorate vtx "
			<< vtx << " with " << linkName
			<< ", closeTrkPtr: " << result.closeTrack);
	  tpLinkDecor(*vtx) = tpLink;
	  if ( tpLink.isValid() ) {
	    ATH_MSG_DEBUG("saveClosestTrack: Decorated vtx "
			  << vtx << " with " << linkName
			  << ", closeTrkPtr: " << result.closeTrack);
	  } else {
	    ATH_MSG_VERBOSE("saveClosestTrack: Failed to decorate vtx "
			    << vtx << " with " << linkName
			    << ", closeTrkPtr: " << result.closeTrack << " !");
	  }
	  // if valid
	} // for itt
      } // for ipv
    } // for its
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void BVertexClosestTrackTool::setResultsPrefix(std::string prefix) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::setResultsPrefix -- begin");

    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	  m_results[its][ipv][itt].setPrefix(prefix);
	} // for itt
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::setResultsPrefix -- end");
  }
  //--------------------------------------------------------------------------
  void BVertexClosestTrackTool::initResults() {
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- begin");

    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nTrackSels = "
		  << nTrackSels);
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nPvAssocs = "
		  << nPvAssocs);
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nTrackTypes = "
		  << nTrackTypes);
    m_results.resize(boost::extents[nTrackSels][nPvAssocs][nTrackTypes]);
    for (unsigned int its = 0; its < nTrackSels; ++its) {
      ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- its = " << its);
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- ipv = " << ipv);
	for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	  ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- itt = "
			<< itt);
	  ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults :"
			<< m_branchBaseName
			<< buildBranchBaseName(its, ipv, itt));
	  
	  m_results[its][ipv][itt].setup(buildBranchBaseName(its, ipv, itt),
					 m_branchBaseName);
	} // for itt
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- end");
  }
  //--------------------------------------------------------------------------
}
