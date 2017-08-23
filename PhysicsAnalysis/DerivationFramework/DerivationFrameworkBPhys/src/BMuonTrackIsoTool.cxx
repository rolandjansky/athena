/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BMuonTrackIsoTool.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add muon track isolation information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
// Job options provided by this class:
// - MuonContainerName          -- name of muon container
// - IsolationConeSizes         -- List of isolation cone sizes
// - IsoTrkImpLogChi2Max        -- List of maximum log(chi2) cuts for
//                                 association of tracks to the primary
//                                 vertex picked.
//                                 (Must contain the same number of elements
//                                  as the IsolationConeSizes list.)
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BMuonTrackIsoTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "boost/format.hpp"
#include "TVector3.h"
#include <algorithm>
#include <sstream>
#include <string>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BMuonTrackIsoTool::MuIsoItem::MuIsoItem(std::string Name,
					  std::string Bname,
					  std::string Prefix) :
    BaseItem(Name, Bname, Prefix) {
  }
  
  BMuonTrackIsoTool::MuIsoItem::~MuIsoItem() {
  }
  
  void BMuonTrackIsoTool::MuIsoItem::resetVals() {
    vIsoValues.clear();
    vNTracks.clear();
    vMuons.clear();
  }

  void BMuonTrackIsoTool::MuIsoItem::copyVals(const BaseItem& item) {
    copyVals((const MuIsoItem&)item);
  }
  
  void BMuonTrackIsoTool::MuIsoItem::copyVals(const MuIsoItem& item) {
      vIsoValues = item.vIsoValues;
      vNTracks   = item.vNTracks;
      vMuons     = item.vMuons;
  }

  void BMuonTrackIsoTool::MuIsoItem::fill(double isoValue, int nTracks,
					  const xAOD::Muon* muon) {
    vIsoValues.push_back(isoValue);
    vNTracks.push_back(nTracks);
    vMuons.push_back(muon);
  }
  
  std::string BMuonTrackIsoTool::MuIsoItem::muIsoName() {
    return buildName();
  }

  std::string BMuonTrackIsoTool::MuIsoItem::nTracksName() {
    return buildName("Ntracks");
  }

  std::string BMuonTrackIsoTool::MuIsoItem::muLinkName() {
    return buildName("", "_muLink");
  }

  //--------------------------------------------------------------------------
  BMuonTrackIsoTool::BMuonTrackIsoTool(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : BPhysVertexTrackBase(t,n,p) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("MuonContainerName"     , m_muonContainerName="");
    declareProperty("IsolationConeSizes"    , m_isoConeSizes);
    declareProperty("IsoTrkImpLogChi2Max"   , m_isoTrkImpLogChi2Max);
  }
  //--------------------------------------------------------------------------
  StatusCode BMuonTrackIsoTool::initializeHook() {
  
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initializeHook() -- begin");

    // check like-sized arrays
    if ( m_isoConeSizes.size() != m_isoTrkImpLogChi2Max.size() ) {
      ATH_MSG_ERROR("Size mismatch of IsolationConeSizes ("
		    << m_isoConeSizes.size()
		    << ") and IsoTrkImpChi2Max ("
		    << m_isoTrkImpLogChi2Max.size() << ") lists!");
    }      
    // check muon container name
    if ( m_muonContainerName == "" ) {
      ATH_MSG_ERROR("No muon container name provided!");
    }

    // initialize results array
    initResults();

    ATH_MSG_DEBUG("BMuonTrackIsoTool::initializeHook() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BMuonTrackIsoTool::finalizeHook() {

    ATH_MSG_DEBUG("BMuonTrackIsoTool::finalizeHook()");

    // everything all right
    return StatusCode::SUCCESS;
  }
 //--------------------------------------------------------------------------
  StatusCode
  BMuonTrackIsoTool::addBranchesVCSetupHook(size_t ivc) const {

    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesVCLoopHook() -- begin");

    ATH_MSG_DEBUG("BMuonTrackisoTool::addBranchesVCSetupHook: "
		  << "Vertex container index " << ivc
		  << " for collection " << m_vertexContainerNames[ivc]
		  << " with prefix " << m_branchPrefixes[ivc]);
    
    setResultsPrefix(m_branchPrefixes[ivc]);
    
    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesVCSetupHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode
  BMuonTrackIsoTool::addBranchesSVLoopHook(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesSVLoopHook() -- begin");

    // retrieve muon container
    m_muons = NULL;
    if ( m_muonContainerName != "" ) {
      CHECK(evtStore()->retrieve(m_muons, m_muonContainerName));
      ATH_MSG_DEBUG("Found muon collection with key " << m_muonContainerName);
    }
    
    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesSVLoopHook(): "
		  "calculate muon track isolation ...");
    CHECK(calculateValues(vtx));

    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesSVLoopHook(): "
		  "save muon track isolation ...");
    // save the isolation values
    CHECK(saveIsolation(vtx));

    ATH_MSG_DEBUG("BMuonTrackIsoTool::addBranchesSVLoopHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Calculate track isolation variables -- faster method with caching
  //--------------------------------------------------------------------------  
  StatusCode
  BMuonTrackIsoTool::calcValuesHook(const xAOD::Vertex* vtx,
				      const unsigned int ipv,
				      const unsigned int its,
				      const unsigned int itt) const {
    
    ATH_MSG_DEBUG("calcValuesHook:  ipv: " << ipv
		  << ", its: " << its << ", itt: " << itt);
 
    // candidate tracks and momentum
    xAOD::BPhysHelper   cand(vtx);
    TVector3            candP     = cand.totalP();
    const xAOD::Vertex* candRefPV = cand.pv(m_pvAssocTypes[ipv]);

    MuonBag  muons;
    // TrackBag candMuTracks = findAllMuonIdTracksInDecay(cand, muons);
    std::vector<TVector3> candMuTracks = findMuonRefTrackMomenta(cand, muons);
    
    TrackBag tracks = selectTracks(m_tracks, cand, ipv, its, itt);

    ATH_MSG_DEBUG("calcValuesHook: found " << muons.size() <<
		  " muons and " << candMuTracks.size() <<
		  " tracks from B cand; " << tracks.size() <<
		  " tracks to check.");
    
    // loop over isolation cones (pt and deltaR)
    unsigned int nCones = m_isoConeSizes.size();
    for (unsigned int ic = 0; ic < nCones; ++ic) {
      MuIsoItem& iso = m_results[ic][its][ipv][itt];
      // reset 
      iso.resetVals();

      // loop over refitted ID tracks for muons in candidate
      unsigned int id(0);
      // for (TrackBag::const_iterator muTrkItr = candMuTracks.begin();
      // muTrkItr != candMuTracks.end(); ++muTrkItr, ++id) {
      for (id=0; id < candMuTracks.size(); ++id) {
      
	// make sure there was an ID track for the muon
	// if ( *muTrkItr != NULL ) {
	if ( candMuTracks[id].Mag() > 0. ) {
	
	  const double& coneSize   = m_isoConeSizes[ic];
	  const double& logChi2Max = m_isoTrkImpLogChi2Max[ic];

	  double nTracksInCone = 0;
	  double ptSumInCone   = 0.; 

	  double isoValue(-5.);
	  
	  // make sure candRefPV exists
	  if ( candRefPV != NULL ) {

	    for (TrackBag::const_iterator trkItr = tracks.begin();
		 trkItr != tracks.end(); ++trkItr) {
	      // double deltaR = candP.DeltaR((*trkItr)->p4().Vect());
	      double deltaR = candMuTracks[id].DeltaR((*trkItr)->p4().Vect());
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
	    // if ( ptSumInCone + candP.Pt() > 0. ) {
	    // isoValue = candP.Pt() / ( ptSumInCone + candP.Pt() );
	    // }
	    if ( ptSumInCone + candMuTracks[id].Pt() > 0. ) {
	      isoValue = candMuTracks[id].Pt()
		/ ( ptSumInCone + candMuTracks[id].Pt() );
	    }

	  } else {
	    isoValue = -10.;
	  } // if candRefPV != NULL
	    
	  const xAOD::Muon* muon = id < muons.size() ? muons.at(id) : NULL;
	  iso.fill(isoValue, nTracksInCone, muon);
	} // if *muTrkItr != NULL
      } // for muTrkItr
    } // for ic

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Fill track isolation values from cache if found
  //--------------------------------------------------------------------------  
  bool BMuonTrackIsoTool::fastFillHook(const xAOD::Vertex* vtx,
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
  StatusCode
  BMuonTrackIsoTool::saveIsolation(const xAOD::Vertex* vtx) const {

    typedef ElementLink<xAOD::MuonContainer> MuonLink_t;
    typedef std::vector<MuonLink_t>          MuonLinkVector_t;

    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    MuIsoItem result = m_results[ic][its][ipv][itt];
	    SG::AuxElement::Decorator< std::vector<float> >
	      dv_iso_values(result.muIsoName());
	    SG::AuxElement::Decorator< std::vector<int> >
	      dv_iso_ntracks(result.nTracksName());
	    dv_iso_values(*vtx)  = result.vIsoValues;
	    dv_iso_ntracks(*vtx) = result.vNTracks; 
	    ATH_MSG_DEBUG("BMuonTrackIsoTool::saveIsolation() -- isobn: "
			  << result.muIsoName() << ", ntbn: "
			  << result.nTracksName());
	    ATH_MSG_DEBUG("BMuonTrackIsoTool::saveIsolation() -- vertex: ("
			  << vtx->x() << ", "
			  << vtx->y() << ", "
			  << vtx->z() << "), N(iso): "
			  << result.vIsoValues.size() << ", N(nTracks): "
			  << result.vNTracks.size());
	    MuonLinkVector_t links;
	    for (const xAOD::Muon* muon : result.vMuons) {
	      if ( muon != NULL ) {
		MuonLink_t link(muon, *m_muons);
		links.push_back(link);
	      } else {
		ATH_MSG_WARNING("BMuonTrackIsoTool::saveIsolation(): "
				<< " *muon == NULL -- EL not saved!");
	      }
	    }
	    vtx->auxdecor<MuonLinkVector_t>(result.muLinkName()) = links;
	    ATH_MSG_DEBUG("BMuonTrackIsoTool::saveIsolation() -- muLinks: "
			  << "N_saved = " << links.size() );
	  } // for itt
	} // for ic
      } // for ipv
    } // for its

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void BMuonTrackIsoTool::setResultsPrefix(std::string prefix) const {

    ATH_MSG_DEBUG("BMuonTrackIsoTool::setResultsPrefix -- begin");

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
    
    ATH_MSG_DEBUG("BMuonTrackIsoTool::setResultsPrefix -- end");
  }
  //--------------------------------------------------------------------------
  void BMuonTrackIsoTool::initResults() {

    unsigned int nCones      = m_isoConeSizes.size();
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();

    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- begin");
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults : nCones = " << nCones);
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults : nTrackSels = "
		  << nTrackSels);
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults : nPvAssocs = "
		  << nPvAssocs);
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults : nTrackTypes = "
		  << nTrackTypes);
    m_results.resize(boost::extents[nCones][nTrackSels][nPvAssocs][nTrackTypes]);
    for (unsigned int its = 0; its < nTrackSels; ++its) {
      ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- its = " << its);
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
	ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- ipv = " << ipv);
	for (unsigned int ic = 0; ic < nCones; ++ic) {
	  ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- ic = " << ic);
	  for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
	    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- itt = " << itt);

	    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults :"
			  << m_branchBaseName << buildBranchName(ic, its,
								 ipv, itt));

	    m_results[ic][its][ipv][itt].setup(buildBranchName(ic, its,
							       ipv, itt),
					       m_branchBaseName);
	  } // for itt
	} // for ic
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BMuonTrackIsoTool::initResults -- end");
  }
  //--------------------------------------------------------------------------
  std::string BMuonTrackIsoTool::buildBranchName(unsigned int ic,
						 unsigned int its,
						 unsigned int ipv,
						 unsigned int itt) const {
    ATH_MSG_DEBUG("BMuonTrackIsoTool::buildBranchName -- begin");
    
    double      coneSize   = m_isoConeSizes[ic];
    double      logChi2Max = m_isoTrkImpLogChi2Max[ic];

    // format it nicely
    boost::format f("%02d_LC%02d_%s");
    f % (int)(coneSize*10.) % (int)(logChi2Max*10.)
      % buildBranchBaseName(its, ipv, itt);
    
    ATH_MSG_DEBUG("BMuonTrackIsoTool::buildBranchName: " << f.str());

    return f.str();
  }
  //--------------------------------------------------------------------------
}
