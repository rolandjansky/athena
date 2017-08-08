/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 *  @file   BPhysAddMuonBasedInvMass.cxx
 *  @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 */

// includes
#include "DerivationFrameworkBPhys/BPhysAddMuonBasedInvMass.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODBPhys/BPhysHelper.h"
// for Amg::error():
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <sstream>
#include <limits>

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  BPhysAddMuonBasedInvMass::BPhysAddMuonBasedInvMass(const std::string& t,
						     const std::string& n,
						     const IInterface*  p)
    : AthAlgTool(t,n,p), m_trackToVertexTool("Reco::TrackToVertex") {

    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare branch prefix
    declareProperty("BranchPrefix", m_branchPrefix = "_NONE_");
    // Necessary containers
    declareProperty("VertexContainerName"    , m_vertexContainerName = "");
    // track mass assignment
    declareProperty("TrkMasses", m_trkMasses = std::vector<double>());
    // track-to-vertex tool
    declareProperty("TrackToVertexTool"     , m_trackToVertexTool);
    // adjust track from muon kinematics?
    declareProperty("AdjustToMuonKinematics", m_adjustToMuonKinematics = false);
    // add minChi2ToAnyPV decoration
    declareProperty("AddMinChi2ToAnyPVMode" , m_addMinChi2ToAnyPVMode = 0);
    // name of container for primary vertices
    declareProperty("PrimaryVertexContainerName", m_pvContainerName = "");
    // minimum number of tracks in PV for PV to be considered in calculation
    // of minChi2MuToAnyPV variable    
    declareProperty("MinNTracksInPV"        , m_minNTracksInPV = 0);
    // list of primary vertex types to consider
    declareProperty("PVTypesToConsider"     , m_pvTypesToConsider = {1,3});
    // PV-to-SV association types to be considered
    declareProperty("DoVertexType"          , m_doVertexType = 63);
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysAddMuonBasedInvMass::initialize() {
  
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::initialize() -- begin");

    // candidate vertices container
    if ( m_vertexContainerName == "" ) {
      ATH_MSG_ERROR("No vertex container name provided!");
    }
    
    // TrackToVertexTool
    CHECK(m_trackToVertexTool.retrieve());

    // PV container if needed
    if ( m_addMinChi2ToAnyPVMode > 0 && m_pvContainerName == "" ) {
      ATH_MSG_ERROR("No primary vertex container name provided!");
    }

    // PV type list if needed
    if ( m_addMinChi2ToAnyPVMode > 0 && m_pvTypesToConsider.size() == 0 ) {
      ATH_MSG_ERROR("No primary vertex types to be considered provided!");
    }

    // PV-to-SV association type if needed
    if ( m_addMinChi2ToAnyPVMode > 1 && m_doVertexType < 1 ) {
      ATH_MSG_ERROR("No PV-to-SV association types to be considered provided!");
    }
    
    ATH_MSG_INFO("BPhysAddMuonBasedInvMass::initialize(): "
		 << "AdjustToMuonKinematics = " << m_adjustToMuonKinematics);
    
    ATH_MSG_INFO("BPhysAddMuonBasedInvMass::initialize(): "
		 << "AddMinChi2ToAnyPVMode = " << m_addMinChi2ToAnyPVMode);

    // initialize PV-to-SV association type vector
    initPvAssocTypeVec();
    
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::initialize() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BPhysAddMuonBasedInvMass::finalize() {

    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::finalize()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode BPhysAddMuonBasedInvMass::addBranches() const {

    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::addBranches() -- begin");
    
    // vertex container and its auxilliary store
    xAOD::VertexContainer*     vtxContainer    = NULL;
    xAOD::VertexAuxContainer*  vtxAuxContainer = NULL;

    // retrieve from StoreGate
    CHECK(evtStore()->retrieve(vtxContainer   , m_vertexContainerName));
    CHECK(evtStore()->retrieve(vtxAuxContainer, m_vertexContainerName+"Aux."));

    const xAOD::VertexContainer* pvContainer = NULL;
    if ( m_addMinChi2ToAnyPVMode > 0 ) {
      CHECK(evtStore()->retrieve(pvContainer   , m_pvContainerName));
    }

    // apply the decorations
    std::string branchPrefix("");
    if ( m_branchPrefix != "" && m_branchPrefix != "_NONE_" ) {
      branchPrefix = m_branchPrefix + "_";
    }
    
    // loop over secondary vertices
    for (xAOD::VertexContainer::iterator vtxItr = vtxContainer->begin();
	 vtxItr!=vtxContainer->end(); ++vtxItr) {

      xAOD::BPhysHelper vtx(*vtxItr);
      
      SG::AuxElement::Decorator< float >
	d_mucalc_mass(branchPrefix+"MUCALC_mass");
      SG::AuxElement::Decorator< float >
	d_mucalc_massErr(branchPrefix+"MUCALC_massErr");

      // TODO: check number of muons requested!
      std::pair<double,double> MuCalcCandMass =
	getMuCalcMass(vtx, m_trkMasses, 2);

      // fill default values
      d_mucalc_mass(**vtxItr)    = MuCalcCandMass.first;
      d_mucalc_massErr(**vtxItr) = MuCalcCandMass.second;

      // add MinChi2ToAnyPV information if requested
      if ( m_addMinChi2ToAnyPVMode > 0 ) {

	if (m_addMinChi2ToAnyPVMode == 1) {
	// w.r.t. to all PVs
	  SG::AuxElement::Decorator< float >
	    d_minChi2ToAnyPV(branchPrefix+"minLogChi2ToAnyPV");
	  // fill it
	  d_minChi2ToAnyPV(**vtxItr) =
	    getMinChi2ToAnyPV(vtx, pvContainer, m_pvTypesToConsider,
			      m_minNTracksInPV, m_addMinChi2ToAnyPVMode,
			      xAOD::BPhysHelper::PV_MIN_A0); // dummy
	} else if (m_addMinChi2ToAnyPVMode > 1 && m_addMinChi2ToAnyPVMode < 4) {
	  // skip or replace associated PVs
	  for (auto pvAssocType : m_pvAssocTypes) {
	    SG::AuxElement::Decorator< float >
	      d_minChi2ToAnyPV(branchPrefix+"minLogChi2ToAnyPV_"
			       +xAOD::BPhysHelper::pv_type_str[pvAssocType]);
	    // fill it
	    d_minChi2ToAnyPV(**vtxItr) =
	      getMinChi2ToAnyPV(vtx, pvContainer, m_pvTypesToConsider,
				m_minNTracksInPV, m_addMinChi2ToAnyPVMode,
				pvAssocType);
	    
	  } // for pvAssocType
	} else {
	  ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::addBranches():"
			  << " Undefined AddMinChi2ToAnyPVMode value: "
			  << m_addMinChi2ToAnyPVMode);
	}
      } // if m_addMinChi2ToAnyPVMode
    } // end of loop over vertices
      
    // clean cache
    clearAdjTpCache();
    
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::addBranches() -- end");

    // nothing to do here
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  // Calculate invariant mass based on muon system information if available.
  //
  std::pair<double, double>
  BPhysAddMuonBasedInvMass::getMuCalcMass(xAOD::BPhysHelper& vtx,
					  std::vector<double> trkMasses,
					  int nMuRequested) const {

    std::pair<double, double> mpe(0., -1.);

    std::pair<TrackBag, int> tracksWithMu = getTracksWithMuons(vtx);

    if ( tracksWithMu.second == nMuRequested ) {
      if ( tracksWithMu.first.size() == trkMasses.size() ) {
	mpe = getInvariantMassWithError(tracksWithMu.first,
					trkMasses,
					vtx.vtx()->position());
      } else {
	ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getMuCalcMass:"
			<< " vector sizes disagree!"
			<< " tracksWithMu: " << tracksWithMu.first.size()
			<< " BtrkMasses: " << trkMasses.size());
      }
    } else {
      mpe.second = -10 - tracksWithMu.second;
      ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::getMuCalcMass:"
		    << " muon number mismatch:"
		    << " tracksWithMu: " << tracksWithMu.second
		    << " requested: " << nMuRequested);
    }
    return mpe;
  }
  //--------------------------------------------------------------------------
  // Obtain a set of ID tracks for a set of muons
  //--------------------------------------------------------------------------
  TrackBag BPhysAddMuonBasedInvMass::getIdTracksForMuons(MuonBag& muons) const {

    TrackBag muTracks;
    
    for (auto &muon : muons) {
      if ( muon != nullptr ) {
	const xAOD::TrackParticle* trk = 
	  muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	if ( trk != nullptr ) {
	  muTracks.push_back(trk);
	} else {
	  ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getIdTracksForMuon:"
			  << " no ID track for muon found.");
	}
      } else {
	ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getIdTracksForMuon:"
			<< " muon pointer is NULL!");
      }
    } // for muon
    return muTracks;
  }
  //--------------------------------------------------------------------------
  // Obtain a set of tracks with muon track information if available
  //--------------------------------------------------------------------------
  std::pair<TrackBag, int>
  BPhysAddMuonBasedInvMass::getTracksWithMuons(xAOD::BPhysHelper& vtx) const {
    
    TrackBag            tracksWithMu;
    int                 nMuFound = 0;
    std::vector<int>    vnMuFound;

    MuonBag muons = findAllMuonsInDecay(vtx);

    if ( muons.size() > 0 ) {
      for (int itrk=0; itrk<vtx.nRefTrks(); ++itrk) {
	// only charged tracks are of interest
	if ( abs(vtx.refTrkCharge(itrk)) > 0. ) {
	  const xAOD::TrackParticle* trkParticle =
	    (xAOD::TrackParticle*)vtx.refTrkOrigin(itrk);
	  for (unsigned int imu = 0; imu<muons.size(); ++imu) {
	    if ( vtx.refTrkOrigin(itrk) ==
		 muons.at(imu)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ) {
	      const xAOD::TrackParticle* trkMuon = 
		adjustTrackParticle(muons.at(imu));
	      if ( trkMuon != NULL ) {
		trkParticle    = trkMuon;
		nMuFound++;
		break;
	      }
	    }
	  } // for imu
	  tracksWithMu.push_back(trkParticle);
	  vnMuFound.push_back(nMuFound);
	} // for charged track
      } // for itrk
    } else {
      ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::getTracksWithMuons: "
		    "vertex contains no muons, but "
		    << vtx.nRefTrks() << " refitted tracks ...");
    }
    // debug output
    std::string svnMuFound    = "[";
    for (unsigned int i=0; i<vnMuFound.size(); ++i) {
      svnMuFound  += std::to_string(vnMuFound[i]) + ',';
    }
    svnMuFound.back()    = ']';
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::getTracksWithMuons: "
		  "nMuFound     = " << nMuFound
		  << "\nvnMuFound    = " << svnMuFound );
    
    return std::pair<TrackBag, int>(tracksWithMu, nMuFound);
  }
  //--------------------------------------------------------------------------
  // adjustTrackParticle: extract primary track particle from muon
  // if configured adjust pt, eta and phi of it before returning
  // a pointer to it.
  //--------------------------------------------------------------------------
  const xAOD::TrackParticle* BPhysAddMuonBasedInvMass
  ::adjustTrackParticle(const xAOD::Muon* muon) const {

    const xAOD::TrackParticle* tp  = NULL;    
    const xAOD::TrackParticle* org = muon->primaryTrackParticle();

    if ( m_adjustToMuonKinematics ) {
      if ( org != NULL ) {
	TpMap_t::iterator it = m_adjTpCache.find(org);
	if ( it != m_adjTpCache.end() ) {
	  // copy cached link
	  tp = it->second;
	  ATH_MSG_DEBUG("adjustTrackParticle(): from cache: tp = " << tp);
	} else {
	  // copy object -- this does not work because of seg fault later
	  // xAOD::TrackParticle* newTp = new xAOD::TrackParticle(*org);

	  // create new object and copy properties
	  xAOD::TrackParticle* newTp = new xAOD::TrackParticle();
	  newTp->makePrivateStore(*org);

	  // ajdust pt, eta and phi to the muon's properties
	  xAOD::IParticle::FourMom_t p4 = muon->p4();
	  float qoverp = p4.P() > 0. ? 1./p4.P() : 10.e6;
	  if ( org->qOverP() < 0. ) qoverp *= -1.;
	  newTp->setDefiningParameters(org->d0(), org->z0(),
				       p4.Phi(), p4.Theta(), qoverp);
	  // cache new TrackParticle
	  m_adjTpCache[org] = newTp;
	  tp = newTp;
	  ATH_MSG_DEBUG("adjustTrackParticle(): new tp = " << tp
			<< " org = " << org);
	} // if it != end()
      } // if != NULL
    } else {
      // copy pointer
      tp = org;
      ATH_MSG_DEBUG("adjustTrackParticle(): copy: org: " << org
		    << " -> tp: " << tp);
    }
      
    // debug output
    if ( org != NULL ) { 
      ATH_MSG_DEBUG("adjustTrackParticle(): org: " << org << " ("
		    << org->d0() << "," << org->z0() << "," << org->phi0()
		    << "," << org->theta() << "," << org->qOverP() << ") pt: "
		    << org->pt());
    } else {
      ATH_MSG_DEBUG("adjustTrackParticle(): org = NULL");
    }
    if ( org != NULL ) { 
      ATH_MSG_DEBUG("adjustTrackParticle(): tp : " << tp << " ("
		    << tp->d0() << "," << tp->z0() << "," << tp->phi0()
		    << "," << tp->theta() << "," << tp->qOverP() << ") pt: "
		    << tp->pt());
    } else {
      ATH_MSG_DEBUG("adjustTrackParticle(): tp = NULL");
    }
    return tp;
  }
  //--------------------------------------------------------------------------
  // clearAdjTpCache: clear the cache of adjusted TrackParticles
  //--------------------------------------------------------------------------
  void BPhysAddMuonBasedInvMass::clearAdjTpCache() const {

    for ( TpMap_t::iterator it = m_adjTpCache.begin(); it != m_adjTpCache.end();
	  ++it) {
      if ( it->second != NULL ) {
	const_cast<xAOD::TrackParticle*>(it->second)->releasePrivateStore();
	delete(it->second);
	it->second = NULL;
      }
      m_adjTpCache.clear();
    }
  }
  //--------------------------------------------------------------------------
  // findAllMuonsInDecay: returns a vector of xAOD::Muon objects found
  // in this vertex and subsequent decay vertices.
  // Recursively calls itself if necessary.
  //--------------------------------------------------------------------------
  MuonBag BPhysAddMuonBasedInvMass::findAllMuonsInDecay(xAOD::BPhysHelper& vtx)
    const {

    MuonBag muons = vtx.muons();

    // loop over preceeding vertices
    for (int ivtx = 0; ivtx < vtx.nPrecedingVertices(); ++ivtx) {
      xAOD::BPhysHelper precVtx(vtx.precedingVertex(ivtx));
      MuonBag muonsForVtx = findAllMuonsInDecay(precVtx);
      muons.insert(muons.end(), muonsForVtx.begin(), muonsForVtx.end());
    }
    return muons;
  }
  //--------------------------------------------------------------------------
  // getMinChi2ToAnyPV: 
  // Find minimum chi2 distance of signal muons w.r.t any primary vertex
  // of required types and with a minimum number of tracks cut.
  // It also depends on the mode w.r.t. the treatment of the associated
  // primary vertex and the type of PV-to-SV association.
  // Returns this minimum chi2.
  //--------------------------------------------------------------------------
  double
  BPhysAddMuonBasedInvMass::getMinChi2ToAnyPV(xAOD::BPhysHelper& vtx,
					      const xAOD::VertexContainer*
					      pvContainer,
					      const std::vector<int>& pvtypes,
					      const int minNTracksInPV,
					      const int mode,
					      const xAOD::BPhysHelper::pv_type&
					      pvAssocType) const {

    MuonBag  muons  = findAllMuonsInDecay(vtx);
    TrackBag tracks = getIdTracksForMuons(muons);
    const xAOD::Vertex* origPV = nullptr;
    const xAOD::Vertex* refPV  = nullptr;

    if ( mode > 1 ) {
      // need to obtain original PV
      origPV = vtx.origPv(pvAssocType);
      if ( origPV == nullptr ) {
	ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getMinChi2ToAnyPV:"
			<< " origPV == NULL for pvAssocType = "
			<< pvAssocType);
      }
      if ( mode > 2 ) {
	refPV  = vtx.pv(pvAssocType);
	if ( refPV == nullptr ) {
	  ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getMinChi2ToAnyPV:"
			  << " refPV == NULL for pvAssocType = "
			  << pvAssocType);
	}
      }
    }
    
    double minChi2 = std::numeric_limits<double>::max();

    for (auto &pvtx : *pvContainer) {
      if ( pvtx != nullptr ) {
	if ( std::find(pvtypes.begin(),pvtypes.end(),pvtx->vertexType())
	     != pvtypes.end() ) {
	  const xAOD::Vertex* cvtx = pvtx;
	  // switch if PV matches original PV and replacement is requested
	  if ( mode > 1 && pvtx == origPV ) {
	    // mode 2 -- skip
	    switch(mode) {
	    case 2: // skip current PV
	      continue;
	      break;
	    case 3: // replace by refitted PV
	      if ( refPV != nullptr ) {
		cvtx = refPV;
	      } else {
		ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getMinChi2ToAnyPV:"
				<< " refPV == NULL!");
		continue;
	      }
	      break;
	    }
	  }
	  if ( (int)cvtx->nTrackParticles() >= minNTracksInPV ) {
	    for (auto &track : tracks) {
	      const Amg::Vector3D pos = cvtx->position();
	      minChi2 = std::min(minChi2, getTrackPVChi2(*track, pos));
	    } // for track
	  } // if minNTracksInPV
	} // if pvTypes in pvtypes vector
      }  else {
	ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getMinChi2ToAnyPV:"
			<< " pvtx == NULL!");
      } // if vtx != nullptr
    } // for pvtx
    
    return minChi2;
  }
  //--------------------------------------------------------------------------
  // getTrackPVChi2:
  // Calculate the chi2 ( = log((d0/d0e)^2+(z0/z0e)^2) contribution of
  // a track at the position closest to the given PV.
  //--------------------------------------------------------------------------
  double
  BPhysAddMuonBasedInvMass::getTrackPVChi2(const xAOD::TrackParticle& track,
					   const Amg::Vector3D& pos) const {
    
  double chi2 = -100.;

  const Trk::Perigee* trkPerigee =
    m_trackToVertexTool->perigeeAtVertex(track, pos);
  if ( trkPerigee != NULL ) {
    const AmgSymMatrix(5)* locError = trkPerigee->covariance();
    if ( locError != NULL ) {
      double d0    = trkPerigee->parameters()[Trk::d0];
      double z0    = trkPerigee->parameters()[Trk::z0];
      double d0Err = Amg::error(*locError, Trk::d0);
      double z0Err = Amg::error(*locError, Trk::z0);
      if (fabs(d0Err) > 0. && fabs(z0Err) > 0.) { 
	chi2 = log( pow(d0/d0Err,2.0) + pow(z0/z0Err,2.0) );
      } else {
	ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getTrackPVChi2():"
			<< " d0 = " << d0 << ", d0Err = " << d0Err
			<< ", z0 = " << z0 << ", z0Err = " << z0Err);
      }
    } // locError != NULL
    delete trkPerigee;
    trkPerigee = nullptr;
  } else {
    ATH_MSG_WARNING("getTrackPVChi2: Could not get perigee");
  }
 
  return chi2;
}
  //--------------------------------------------------------------------------
  // getInvariantMassWithError: returns invariant mass and mass error given
  // a set of tracks, their mass hypotheses and a reference position. 
  // Each track must have a separate mass hypothesis in
  // the vector, and they must be in the same order as the tracks in the
  // track vector.  Otherwise it will go horribly wrong.
  //--------------------------------------------------------------------------
  std::pair<double,double> BPhysAddMuonBasedInvMass::
  getInvariantMassWithError(TrackBag trksIn,
			    std::vector<double> massHypotheses,
			    const Amg::Vector3D& pos) const {
    
  std::pair<double, double> mass(0.,0.);

  // ensure there is a mass hypothesis for each track
  if ( trksIn.size() == massHypotheses.size() ) {
    std::vector<const xAOD::TrackParticle*>::iterator trItr = trksIn.begin();
    std::vector<const xAOD::TrackParticle*>::iterator trItrEnd  =trksIn.end();
    std::vector<double>::iterator massHypItr = massHypotheses.begin();
    
    double pxTmp,pyTmp,pzTmp,massTmp,eTmp;
    
    std::vector<TLorentzVector> trkMom;
    TLorentzVector totMom;
    std::vector<const Trk::Perigee*> trkPer;

    for (;trItr != trItrEnd; trItr++,massHypItr++){
      const Trk::Perigee* trkPerigee = 
        m_trackToVertexTool->perigeeAtVertex(*(*trItr), pos);
      trkPer.push_back(trkPerigee);
      if ( trkPerigee != NULL ) {
        // try to get the correct momentum measurement
        pxTmp = trkPerigee->momentum()[Trk::px];
        pyTmp = trkPerigee->momentum()[Trk::py];
        pzTmp = trkPerigee->momentum()[Trk::pz];
	ATH_MSG_DEBUG("getInvariantMassWithError(): pvec = ("
		      << pxTmp << "," << pyTmp << "," << pzTmp << ")");
      } else {
	// otherwise default to this one
        pxTmp = ((*trItr)->p4()).Px();
        pyTmp = ((*trItr)->p4()).Py();
        pzTmp = ((*trItr)->p4()).Pz();
        ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getInvariantMassError: "
			"defaulting to simple momentum!");
      }
      massTmp = *massHypItr;
      eTmp    = pxTmp*pxTmp+pyTmp*pyTmp+pzTmp*pzTmp+massTmp*massTmp;
      eTmp    = eTmp > 0. ? sqrt(eTmp) : 0.; 
      TLorentzVector tmpMom(pxTmp, pyTmp, pzTmp, eTmp);
      trkMom.push_back(tmpMom);
      totMom += tmpMom;
    }
    mass.first = totMom.M();
    double mErr2 = 0.;
    // reset trItr
    trItr   = trksIn.begin();
    std::vector<TLorentzVector>::iterator      tmItr  = trkMom.begin();
    std::vector<const Trk::Perigee*>::iterator perItr = trkPer.begin();
    AmgVector(3) dMdP;
    dMdP.setZero();
    for (; tmItr != trkMom.end(); ++tmItr, ++trItr, ++perItr) {
      dMdP(0) = (totMom.E() * tmItr->Px()/tmItr->E() - totMom.Px())/totMom.M();
      dMdP(1) = (totMom.E() * tmItr->Py()/tmItr->E() - totMom.Py())/totMom.M();
      dMdP(2) = (totMom.E() * tmItr->Pz()/tmItr->E() - totMom.Pz())/totMom.M();
      if ( *perItr != NULL ) {
        mErr2 += (dMdP.transpose() * getMomentumCov(*perItr) * dMdP)(0,0);
      } else {
        mErr2 += (dMdP.transpose() * getMomentumCov(*trItr ) * dMdP)(0,0);
      }
    }
    mass.second = mErr2 > 0. ? sqrt(mErr2) : 0.;
    // clean up 
    for ( perItr = trkPer.begin(); perItr != trkPer.end(); ++perItr) {
      delete (*perItr);
    }
  } else {
    ATH_MSG_WARNING("BPhysAddMuonBasedInvMass::getInvariantMassError: "
		    "size mismatch of tracks and mass hypotheses vectors!");
  } // if size comparison

  return mass;
  }
  //--------------------------------------------------------------------------
  // 
  // Extract the 3x3 momentum covariance matrix in (x,y,z) notation
  // from the (phi, theta, qoverp) notation from a TrackParticle.
  //
  //--------------------------------------------------------------------------
  AmgSymMatrix(3) BPhysAddMuonBasedInvMass
    ::getMomentumCov(const xAOD::TrackParticle* track) const {
    
    AmgSymMatrix(3) cov;
    cov.setZero();
    
    if ( track != NULL ) {
      cov = getMomentumCov( &track->perigeeParameters() );
    }
    return cov;
  }
  //--------------------------------------------------------------------------
  // 
  // Extract the 3x3 momentum covariance matrix in (x,y,z) notation
  // from the (phi, theta, qoverp) notation from a Perigee.
  //
  //--------------------------------------------------------------------------
  AmgSymMatrix(3) BPhysAddMuonBasedInvMass
    ::getMomentumCov(const Trk::Perigee* perigee) const {
    
    AmgSymMatrix(3) cov;
    cov.setZero();

    if ( perigee != NULL ) {
      cov = getMomentumCov(perigee->parameters(), *perigee->covariance());
    }
    return cov;
  }
  //--------------------------------------------------------------------------
  // Extract the 3x3 momentum covariance matrix in (x,y,z) notation
  // from the (phi, theta, qoverp) notation from a vector of
  // track parameters and the error matrix
  //
  // Coding ideas orignally taken from
  // V0Tools::massErrorVKalVrt(...),
  // Code converted from BPhysToolBox::getMomentumCov(...).
  //--------------------------------------------------------------------------
  //
  AmgSymMatrix(3) BPhysAddMuonBasedInvMass
    ::getMomentumCov(const AmgVector(5)& pars,
		     const AmgSymMatrix(5)& cMatrix) const {
    
    AmgSymMatrix(3) cov;
    cov.setZero();
    
    AmgMatrix(3,3) der;
    der.setZero();
    
    double phi    = pars[Trk::phi];
    double theta  = pars[Trk::theta];
    double qoverp = pars[Trk::qOverP];

    if ( qoverp != 0. ) {
      AmgVector(3) p( cos(phi)*sin(theta)/fabs(qoverp),
		      sin(phi)*sin(theta)/fabs(qoverp),
		      cos(theta)/fabs(qoverp) );
      
      // d(px,py,pz)/d(phi,theta,qoverp)
      der(0,0) = - p.y();
      der(1,0) =   p.x();
      der(2,0) =   0.;
      der(0,1) =   cos(phi) * p.z();
      der(1,1) =   sin(phi) * p.z();
      der(2,1) = - sin(theta) / fabs(qoverp);
      der(0,2) = - p.x()/qoverp;
      der(1,2) = - p.y()/qoverp;
      der(2,2) = - p.z()/qoverp;

      for (unsigned int i=0; i<3; i++) {
	for (unsigned int j=0; j<3; j++) {
	  for (unsigned int k=0; k<3; k++) {
	    for (unsigned int l=0; l<3; l++) {
	      cov(i,j) += der(i,k)*cMatrix(k+2,l+2)*der(j,l);
	    }
	  }
	}
      }
      
      // debug output
      ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::getTracksWithMuons:"
		    << "\nlocalErrCov:\n" 
		    << std::setprecision(10) << cMatrix 
		    << "\ncov:\n" 
		    << std::setprecision(10) << cov 
		    << "\np: " << std::setprecision(10) << p 
		    << "\nder:\n"
		    << std::setprecision(10) << der);
    } // if qoverp
  
    return cov;
  }
  //--------------------------------------------------------------------------
  // Initialize PV-to-SV association type vector
  //--------------------------------------------------------------------------
  void BPhysAddMuonBasedInvMass::initPvAssocTypeVec() {

    m_pvAssocTypes.clear();
    for (unsigned int i=0; i<xAOD::BPhysHelper::n_pv_types; ++i) {
      if ( (m_doVertexType & (1 << i)) > 0 )
        m_pvAssocTypes.push_back((xAOD::BPhysHelper::pv_type)i);
    }
  }
  //--------------------------------------------------------------------------  
}
