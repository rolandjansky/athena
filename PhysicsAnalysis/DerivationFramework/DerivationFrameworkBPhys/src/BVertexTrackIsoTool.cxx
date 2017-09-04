/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BVertexTrackIsoTool.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add B vertex track isolation information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
// Job options provided by this class:
// - IsolationConeSizes         -- List of isolation cone sizes
// - IsoTrkImpLogChi2Max        -- List of maximum log(chi2) cuts for
//                                 association of tracks to the primary
//                                 vertex picked.
//                                 (Must contain the same number of elements
//                                  as the IsolationConeSizes list.)
// - UseOptimizedAlgo           -- Use the speed-optimized algorithm.
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BVertexTrackIsoTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "boost/format.hpp"
#include "TVector3.h"
#include <algorithm>
#include <sstream>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BVertexTrackIsoTool::IsoItem::IsoItem(std::string Name,
					std::string Bname,
					std::string Prefix,
					double IsoValue,
					int NTracks) :
    BaseItem(Name, Bname, Prefix), isoValue(IsoValue), nTracks(NTracks) {
  }
  
  BVertexTrackIsoTool::IsoItem::~IsoItem() {
  }

  void BVertexTrackIsoTool::IsoItem::setup(std::string Name,
					   std::string Bname,
					   std::string Prefix) {
    BaseItem::setup(Name, Bname, Prefix);
    isoValue = -1.;
    nTracks  =  0;
  }
  
  void BVertexTrackIsoTool::IsoItem::setup(std::string Name,
					   std::string Bname,
					   std::string Prefix,
					   double      IsoValue,
					   int         NTracks) {
    BaseItem::setup(Name, Bname, Prefix);
    isoValue = IsoValue;
    nTracks  = NTracks;
  }

  void BVertexTrackIsoTool::IsoItem::resetVals() {
    isoValue = -2.;
    nTracks  = -1;
  }

  void BVertexTrackIsoTool::IsoItem::copyVals(const BaseItem& item) {
    copyVals((const IsoItem&)item);
  }
  
  void BVertexTrackIsoTool::IsoItem::copyVals(const IsoItem& item) {
    isoValue = item.isoValue;
    nTracks  = item.nTracks;
  }
  
  std::string BVertexTrackIsoTool::IsoItem::isoName() {
    return buildName();
  }

  std::string BVertexTrackIsoTool::IsoItem::nTracksName() {
    return buildName("Ntracks");
  }

  //--------------------------------------------------------------------------
  BVertexTrackIsoTool::BVertexTrackIsoTool(const std::string& t,
					   const std::string& n,
					   const IInterface*  p)
    : BPhysVertexTrackBase(t,n,p) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("IsolationConeSizes"    , m_isoConeSizes);
    declareProperty("IsoTrkImpLogChi2Max"   , m_isoTrkImpLogChi2Max);
    declareProperty("UseOptimizedAlgo"      , m_useOptimizedAlgo = true);
  }
  //--------------------------------------------------------------------------
  StatusCode BVertexTrackIsoTool::initializeHook() {
  
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initializeHook() -- begin");

    // check like-sized arrays
    if ( m_isoConeSizes.size() != m_isoTrkImpLogChi2Max.size() ) {
      ATH_MSG_ERROR("Size mismatch of IsolationConeSizes ("
		    << m_isoConeSizes.size()
		    << ") and IsoTrkImpChi2Max ("
		    << m_isoTrkImpLogChi2Max.size() << ") lists!");
    }      

    // initialize results array
    initResults();

    // info output
    ATH_MSG_INFO("calculateIsolation: using "
		 << (m_useOptimizedAlgo ?
		     "optimized (faster)" : "regular (slower)")
		 << "track isolation calculation methd.");

    ATH_MSG_DEBUG("BVertexTrackIsoTool::initializeHook() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BVertexTrackIsoTool::finalizeHook() {

    ATH_MSG_DEBUG("BVertexTrackIsoTool::finalizeHook()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode
  BVertexTrackIsoTool::addBranchesVCSetupHook(size_t ivc) const {

    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesVCLoopHook() -- begin");

    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesVCSetupHook: "
		  << "Vertex container index " << ivc
		  << " for collection " << m_vertexContainerNames[ivc]
		  << " with prefix " << m_branchPrefixes[ivc]);
    
    setResultsPrefix(m_branchPrefixes[ivc]);
    
    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesVCSetupHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode
  BVertexTrackIsoTool::addBranchesSVLoopHook(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesSVLoopHook() -- begin");

    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesSVLoopHook(): "
		  "calculate isolation ...");
    if ( m_useOptimizedAlgo ) {
      CHECK(calculateValues(vtx));
    } else {
      CHECK(calculateIsolation(vtx));
	}
    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesSVLoopHook(): "
		  "save isolation ...");
    // save the isolation values
    CHECK(saveIsolation(vtx));

    ATH_MSG_DEBUG("BVertexTrackIsoTool::addBranchesSVLoopHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Calculate track isolation variables -- faster method with caching
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexTrackIsoTool::calcValuesHook(const xAOD::Vertex* vtx,
				      const unsigned int ipv,
				      const unsigned int its,
				      const unsigned int itt) const {
    
    ATH_MSG_DEBUG("calcValuesHook:  ipv: " << ipv
		  << ", its: " << its << ", itt: " << itt);
 
    // candidate tracks and momentum
    xAOD::BPhysHelper   cand(vtx);
    TVector3            candP      = cand.totalP();
    const xAOD::Vertex* candRefPV  = cand.pv(m_pvAssocTypes[ipv]);

    TrackBag tracks = selectTracks(m_tracks, cand, ipv, its, itt);
    
    // loop over isolation cones (pt and deltaR)
    unsigned int nCones = m_isoConeSizes.size();
    for (unsigned int ic = 0; ic < nCones; ++ic) {
      
      IsoItem&      iso        = m_results[ic][its][ipv][itt];
      const double& coneSize   = m_isoConeSizes[ic];
      const double& logChi2Max = m_isoTrkImpLogChi2Max[ic];
	
      // presets
      iso.resetVals();

      double nTracksInCone = 0;
      double ptSumInCone   = 0.; 

      // make sure candRefPV exists
      if ( candRefPV != NULL ) {
      
	for (TrackBag::const_iterator trkItr = tracks.begin();
	     trkItr != tracks.end(); ++trkItr) {
	  double deltaR = candP.DeltaR((*trkItr)->p4().Vect());
	  if ( deltaR < coneSize ) {
	    double logChi2 =
	      abs(getTrackCandPVLogChi2(*trkItr, candRefPV->position()));
	    if ( logChi2 < logChi2Max ) {
	      nTracksInCone++;
	      ptSumInCone += (*trkItr)->pt();
	    }
	  } // deltaR
	}    
	// calculate result
	if ( ptSumInCone + candP.Pt() > 0. ) {
	  iso.isoValue = candP.Pt() / ( ptSumInCone + candP.Pt() );
	} else {
	  iso.isoValue = -5.;
	}
	
      } else {
	iso.isoValue = -10.;
      } // if candRefPV != NULL

      iso.nTracks  = nTracksInCone;
    } // for ic

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Fill track isolation values from cache if found
  //--------------------------------------------------------------------------  
  bool BVertexTrackIsoTool::fastFillHook(const xAOD::Vertex* vtx,
					 const int ipv) const {

    ATH_MSG_DEBUG("fastFillHook: ipv: " << ipv);
    
    bool found(false);
    
    StringIntMap_t::iterator itpv =
      m_pvAssocResMap.find(buildPvAssocCacheName(vtx, ipv));
    if ( itpv != m_pvAssocResMap.end() ) {
      found = true;
      unsigned int nCones      = m_isoConeSizes.size();
      unsigned int nTrackSels  = m_trackSelectionTools.size();
      unsigned int nTrackTypes = m_useTrackTypes.size();
      for (unsigned int its = 0; its < nTrackSels; ++its) {
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    m_results[ic][its][ipv][itt]
	      .copyVals(m_results[ic][its][itpv->second][itt]);
	  } // for its
	} // for ic
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
  // Track isolation calculation loops -- slower method
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexTrackIsoTool::calculateIsolation(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BVertexTrackIsoTool::calculateIsolation -- begin");
    
    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    CHECK(calcIsolation(m_results[ic][its][ipv][itt], vtx,
				m_isoConeSizes[ic], m_isoTrkImpLogChi2Max[ic],
				m_trackSelectionTools[its],
				m_pvAssocTypes[ipv], m_useTrackTypes[itt]));
	  } // for itt
	} // for ic
      } // for ipv
    } // for its

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Calculate track isolation variables -- slower method
  //--------------------------------------------------------------------------
  StatusCode BVertexTrackIsoTool::
  calcIsolation(const IsoItem& iso,
		const xAOD::Vertex* vtx,
		const double coneSize,
		const double logChi2Max,
		const ToolHandle<TrkSelTool>& tSelTool,
		const xAOD::BPhysHelper::pv_type pvAssocType,
		const int trackTypes ) const {

    // preset
    iso.nTracks =  -1;
    iso.isoValue = -2.;
    
    // candidate tracks and momentum
    xAOD::BPhysHelper   cand(vtx);
    TrackBag            candTracks = findAllTracksInDecay(cand);
    TVector3            candP      = cand.totalP();
    const xAOD::Vertex* candRefPV  = cand.pv(pvAssocType);
    const xAOD::Vertex* candPV     = cand.origPv(pvAssocType);
    
    // tracks to be considered
    TrackBag tracks;
    for (xAOD::TrackParticleContainer::const_iterator trkItr =
	   m_tracks->begin(); trkItr != m_tracks->end(); ++trkItr) {
      const xAOD::TrackParticle* track = *trkItr;
      // track selection check
      if ( ! tSelTool->accept(*track, candRefPV) ) continue;
      // track type check
      if ( ! ((unsigned int)trackTypes == ttall() ||
	      (detTrackTypes(track, candPV) & trackTypes) > 0x0) ) continue; 
      // track not in SV
      if ( std::find(candTracks.begin(), candTracks.end(), track)
	   != candTracks.end() ) continue;
      // tracks that survived so far
      tracks.push_back(track);
    }

    double nTracksInCone = 0;
    double ptSumInCone   = 0.; 
    for (TrackBag::const_iterator trkItr = tracks.begin();
	 trkItr != tracks.end(); ++trkItr) {
      double deltaR = candP.DeltaR((*trkItr)->p4().Vect());
      if ( deltaR < coneSize ) {
	double logChi2 =
	  abs(getTrackCandPVLogChi2(*trkItr, candRefPV->position()));
	if ( logChi2 < logChi2Max ) {
	  nTracksInCone++;
	  ptSumInCone += (*trkItr)->pt();
	}
      } // deltaR
    }    
    // calculate result
    if ( ptSumInCone + candP.Pt() > 0. ) {
      iso.isoValue = candP.Pt() / ( ptSumInCone + candP.Pt() );
    } else {
      iso.isoValue = -5;
    }
    iso.nTracks  = nTracksInCone;
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexTrackIsoTool::saveIsolation(const xAOD::Vertex* vtx) const {

    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    IsoItem result = m_results[ic][its][ipv][itt];
	    SG::AuxElement::Decorator< float >
	      d_iso_value(result.isoName());
	    SG::AuxElement::Decorator< int >
	      d_iso_ntracks(result.nTracksName());
	    d_iso_value(*vtx)   = result.isoValue;
	    d_iso_ntracks(*vtx) = result.nTracks; 
	    ATH_MSG_DEBUG("BVertexTrackIsoTool::saveIsolation() -- isobn: "
			  << result.isoName() << ", ntbn: "
			  << result.nTracksName());
	    ATH_MSG_DEBUG("BVertexTrackIsoTool::saveIsolation() -- vertex: ("
			  << vtx->x() << ", "
			  << vtx->y() << ", "
			  << vtx->z() << "), iso: "
			  << result.isoValue << ", nTracks: "
			  << result.nTracks);
	  } // for itt
	} // for ic
      } // for ipv
    } // for its

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void BVertexTrackIsoTool::setResultsPrefix(std::string prefix) const {

    ATH_MSG_DEBUG("BVertexTrackIsoTool::setResultsPrefix -- begin");

    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    m_results[ic][its][ipv][itt].setPrefix(prefix);
	  } // for itt
	} // for ic
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexTrackIsoTool::setResultsPrefix -- end");
  }
  //--------------------------------------------------------------------------
  void BVertexTrackIsoTool::initResults() {

    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- begin");
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults : nCones = " << nCones);
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults : nTrackSels = "
		  << nTrackSels);
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults : nPvAssocs = "
		  << nPvAssocs);
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults : nTrackTypes = "
		  << nTrackTypes);
    m_results.resize(boost::extents[nCones][nTrackSels][nPvAssocs][nTrackTypes]);
    for (unsigned int its = 0; its < nTrackSels; ++its) {
      ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- its = " << its);
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- ipv = " << ipv);
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- ic = " << ic);
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- itt = " << itt);

	    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults :"
			  << m_branchBaseName << buildBranchName(ic, its,
								 ipv, itt));

	    m_results[ic][its][ipv][itt].setup(buildBranchName(ic, its,
							       ipv, itt),
					       m_branchBaseName);
	  } // for itt
	} // for ic
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexTrackIsoTool::initResults -- end");
  }
  //--------------------------------------------------------------------------
  std::string BVertexTrackIsoTool::buildBranchName(unsigned int ic,
						   unsigned int its,
						   unsigned int ipv,
						   unsigned int itt) const {
    ATH_MSG_DEBUG("BVertexTrackIsoTool::buildBranchName -- begin");
    
    double      coneSize   = m_isoConeSizes[ic];
    double      logChi2Max = m_isoTrkImpLogChi2Max[ic];

    // format it nicely
    boost::format f("%02d_LC%02d_%s");
    f % (int)(coneSize*10.) % (int)(logChi2Max*10.)
      % buildBranchBaseName(its, ipv, itt);
    
    ATH_MSG_DEBUG("BVertexTrackIsoTool::buildBranchName: " << f.str());

    return f.str();
  }
  //--------------------------------------------------------------------------
}

