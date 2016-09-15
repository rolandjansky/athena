/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysAddMuonBasedInvMass.cxx, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add muon-information based invarient mass to secondary vertices using
// a four vector sum.
//
// For an example see BPHY8.py .
//
// Job options provided by this class:
// - BranchPrefix               -- assign the prefix of added branches
//                                 (possibly the derivation format's name)
// - VertexContainerName        -- name of container for vertices
// - TrackParticleContainerName -- name of container for TrackParticles
// - MuonContainerName          -- name of container for muons
// - TrkMasses                  -- ordered list of track masses
//                                 (Important to keep proper order:
//                                  J/psi muons go first!)
// - TrackToVertexTool          -- ToolHandle for track-to-vertex tool
//                           
//============================================================================
//

#include "DerivationFrameworkBPhys/BPhysAddMuonBasedInvMass.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODBPhys/BPhysHelper.h"

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
    declareProperty("VertexContainerName", m_vertexContainerName);
    declareProperty("TrackParticleContainerName",
		    m_trackParticleContainerName);
    declareProperty("MuonContainerName"  , m_muonContainerName);
    declareProperty("TrkMasses", m_trkMasses = std::vector<double>());
    declareProperty("TrackToVertexTool",   m_trackToVertexTool);
 }
  //--------------------------------------------------------------------------
  StatusCode BPhysAddMuonBasedInvMass::initialize() {
  
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::initialize() -- begin");

    if ( m_vertexContainerName == "" ) {
      ATH_MSG_ERROR("No vertex container name provided!");
    }
    if ( m_trackParticleContainerName == "" ) {
      ATH_MSG_ERROR("No track particle container name provided!");
    }
    if ( m_muonContainerName == "" ) {
      ATH_MSG_ERROR("No muon container name provided!");
    }
    
    if ( m_muonContainerName == "" ) {
      ATH_MSG_ERROR("No muon container name provided!");
    }

    // TrackToVertexTool
    CHECK(m_trackToVertexTool.retrieve());
    
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

    // vertex container and its auxilliary store
    xAOD::VertexContainer*     vtxContainer    = NULL;
    xAOD::VertexAuxContainer*  vtxAuxContainer = NULL;

    // retrieve from StoreGate
    CHECK(evtStore()->retrieve(vtxContainer   , m_vertexContainerName));
    CHECK(evtStore()->retrieve(vtxAuxContainer, m_vertexContainerName+"Aux."));
    
    // retrieve muon container
    const xAOD::MuonContainer* muons = NULL;
    CHECK(evtStore()->retrieve(muons, m_muonContainerName));

    // retrieve ID track container
    const xAOD::TrackParticleContainer* tracks = NULL;
    CHECK(evtStore()->retrieve(tracks, m_trackParticleContainerName));
    

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
    
    } // end of loop over vertices
    
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
  // Obtain a set of tracks with muon track information if available
  //
  std::pair<TrackBag, int>
  BPhysAddMuonBasedInvMass::getTracksWithMuons(xAOD::BPhysHelper& vtx) const {
    
    TrackBag         tracksWithMu;
    int              nMuFound = 0;
    std::vector<int> vnMuFound;
      
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
		muons.at(imu)->primaryTrackParticle();
	      if ( trkMuon != NULL ) {
		trkParticle = trkMuon;
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
    std::string svnMuFound = "[";
    for (unsigned int i=0; i<vnMuFound.size(); ++i) {
      svnMuFound += std::to_string(vnMuFound[i]) + ',';
    }
    svnMuFound.back() = ']';
    ATH_MSG_DEBUG("BPhysAddMuonBasedInvMass::getTracksWithMuons: "
		  "nMuFound = " << nMuFound
		  << "\nvnMuFound = " << svnMuFound );
    
    return std::pair<TrackBag, int>(tracksWithMu, nMuFound);
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
        // mErr2 += (dMdP.transpose() * getMomentumCov(*perItr) * dMdP)(1,1);
        mErr2 += (dMdP.transpose() * getMomentumCov(*perItr) * dMdP)(0,0);
      } else {
        // mErr2 += (dMdP.transpose() * getMomentumCov(*trItr ) * dMdP)(1,1);
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
  // from the (phi, theta, qoverp) notation from a MeasuredPerigee.
  //
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
}
