/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "TauVertexVariables.h"
#include <memory>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauVertexVariables::TauVertexVariables(const std::string &name) :
  TauRecToolBase(name) {
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauVertexVariables::~TauVertexVariables() {
}


//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauVertexVariables::initialize() {
  
  ATH_CHECK( m_trackToVertexIPEstimator.retrieve() );
  ATH_CHECK( m_fitTool.retrieve() );
  ATH_CHECK( m_SeedFinder.retrieve() );

  if (inTrigger()) {
    ATH_CHECK(m_beamSpotKey.initialize());
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauVertexVariables::executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& pSecVtxContainer) const {

  // impact parameter variables for standard tracks
  if (pTau.nTracks() > 0) {

    std::unique_ptr<const Trk::ImpactParametersAndSigma> myIPandSigma;
    const xAOD::Vertex* vxcand = nullptr;

    xAOD::Vertex theBeamspot;
    theBeamspot.makePrivateStore();

    if (inTrigger()) { // online: use beamspot
      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
      if(beamSpotHandle.isValid()){
        theBeamspot.setPosition(beamSpotHandle->beamPos());
        const auto& cov = beamSpotHandle->beamVtx().covariancePosition();
        theBeamspot.setCovariancePosition(cov);
        vxcand = &theBeamspot;

	myIPandSigma = std::unique_ptr<const Trk::ImpactParametersAndSigma>(m_trackToVertexIPEstimator->estimate(pTau.track(0)->track(), vxcand));
      }
      else {
        ATH_MSG_DEBUG("No Beamspot object in tau candidate");
      }
    }
    else if (pTau.vertexLink()) { // offline: obtain tau vertex by link
      vxcand = pTau.vertex() ;
      //check if vertex has a valid type (skip if vertex has type NoVtx)
      if (vxcand->vertexType() != xAOD::VxType::NoVtx) {
	myIPandSigma = std::unique_ptr<const Trk::ImpactParametersAndSigma>(m_trackToVertexIPEstimator->estimate(pTau.track(0)->track(), vxcand));
      }
    }

    if (myIPandSigma) {
      // kept for now, but will be discontinued as we now store these for all tracks, not only the leading one
      pTau.setDetail(xAOD::TauJetParameters::ipSigLeadTrk, (float)( myIPandSigma->IPd0 / myIPandSigma->sigmad0 ));
      pTau.setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, (float)( myIPandSigma->IPz0SinTheta / myIPandSigma->sigmaz0SinTheta ));
    }
    else {
      ATH_MSG_DEBUG("trackToVertexIPestimator failed for a standard track!");
      pTau.setDetail(xAOD::TauJetParameters::ipSigLeadTrk, (float)(-999.));
      pTau.setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, (float)(-999.));
    }

    // in the trigger, z0sintheta IP and corresponding significance are meaningless if we use the beamspot
    if(vxcand && (inTrigger() || (!inTrigger() && vxcand->vertexType() != xAOD::VxType::NoVtx))) {
      static const SG::AuxElement::Decorator<float> dec_d0IPsig("d0IPsig");
      static const SG::AuxElement::Decorator<float> dec_z0sinthetaIPsig("z0sinthetaIPsig");

      for(auto track : pTau.allTracks()) {
	myIPandSigma = std::unique_ptr<const Trk::ImpactParametersAndSigma>(m_trackToVertexIPEstimator->estimate(track->track(), vxcand));
	if(myIPandSigma) {
	  dec_d0IPsig(*track) = (myIPandSigma->sigmad0 != 0.) ? (float)( myIPandSigma->IPd0 / myIPandSigma->sigmad0 ) : -999.;
	  dec_z0sinthetaIPsig(*track) = (myIPandSigma->sigmaz0SinTheta != 0.) ? (float)( myIPandSigma->IPz0SinTheta / myIPandSigma->sigmaz0SinTheta ) : -999.;
	}
	else {
	  dec_d0IPsig(*track) = -999.;
	  dec_z0sinthetaIPsig(*track) = -999.;
	}
      }
    }
  }
  else {
    ATH_MSG_DEBUG("Tau has no tracks");
  }

  float ipSigLeadTrk;
  float ipZ0SinThetaSigLeadTrk;

  if (pTau.detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk))
    ATH_MSG_VERBOSE("IP significance lead track " << ipSigLeadTrk);
  if (pTau.detail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, ipZ0SinThetaSigLeadTrk))
    ATH_MSG_VERBOSE("IP Z0 significance lead track " << ipZ0SinThetaSigLeadTrk);

  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(-1111.));
  
  //try to find secondary vertex if more than 1 track and the tau vertex is available
  if ( pTau.nTracks() < 2 ||  !pTau.vertexLink() ) {
    return StatusCode::SUCCESS;
  }

  // get xAOD TrackParticles and Trk::Tracks
  std::vector<const xAOD::TrackParticle*> xaodTracks;
  std::vector<const Trk::Track*> origTracks;
  for (unsigned i = 0; i < pTau.nTracks(); ++i) {
    xaodTracks.push_back(pTau.track(i)->track());
    ATH_MSG_VERBOSE("xAOD::TrackParticle " <<i<<": "<< pTau.track(i)->pt() << " "  << pTau.track(i)->eta()  << " "  << pTau.track(i)->phi());
    if (pTau.track(i)->track()) {
      origTracks.push_back(pTau.track(i)->track()->track());
    }
    else {
      ATH_MSG_WARNING("no Trk::Track for xAOD::TrackParticle");
    }
  }

  // get the starting point for the fit using Trk::Tracks
  const Amg::Vector3D& seedPoint = m_SeedFinder->findSeed(origTracks);
  ATH_MSG_VERBOSE("seedPoint x/y/perp=" << seedPoint.x() <<  " " << seedPoint.y() << " "<< seedPoint.z() << " " << TMath::Sqrt(seedPoint.x()*seedPoint.x()+seedPoint.y()+seedPoint.y()));

  // fitting the vertex itself
  xAOD::Vertex* xAODvertex = m_fitTool->fit(xaodTracks, seedPoint);
  if (!xAODvertex) {
    ATH_MSG_WARNING("no secondary vertex found!");
    return StatusCode::SUCCESS;
  }

  // get the transverse flight path significance
  double trFlightPS = trFlightPathSig(pTau, *xAODvertex);
  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(trFlightPS));
  ATH_MSG_VERBOSE("transverse flight path significance="<<trFlightPS);

  // Note, we only attach the 2nd vertex if at offline, otherwise, break the trigger persistency
  if (!inTrigger()) {
    ATH_MSG_VERBOSE("secondary vertex recorded! x="<<xAODvertex->position().x()<< ", y="<<xAODvertex->position().y()<<", perp="<<xAODvertex->position().perp());
    pSecVtxContainer.push_back(xAODvertex);
    xAODvertex->setVertexType(xAOD::VxType::NotSpecified);
    pTau.setSecondaryVertex(&pSecVtxContainer, xAODvertex); 		// set the link to the vertex
  }
  else {
    delete xAODvertex; // delete the vertex when in trigger mode, because we can not save it
  }

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// calculate the transverse flight path significance
//-------------------------------------------------------------------------
double TauVertexVariables::trFlightPathSig(const xAOD::TauJet& pTau, const xAOD::Vertex& secVertex) const {

  const xAOD::Vertex* pVertex = nullptr;
  if (pTau.vertexLink()) pVertex = pTau.vertex();
  if (!pVertex) {
    ATH_MSG_WARNING("No primary vertex information for calculation of transverse flight path significance");
    return -11111.;
  }

  double fpx = secVertex.position().x() - pVertex->position().x();
  double fpy = secVertex.position().y() - pVertex->position().y();
  double fpt = (secVertex.position() - pVertex->position()).perp();

  if (fpt == 0.) {
    ATH_MSG_WARNING("delta pt of (secVtx - priVtx) is 0!");
    return -11111.;
  }

  double sigma_fpt2 = (fpx * fpx * secVertex.covariancePosition()(Trk::x, Trk::x) +
		       fpx * fpy * secVertex.covariancePosition()(Trk::x, Trk::y) +
		       fpy * fpx * secVertex.covariancePosition()(Trk::y, Trk::x) +
		       fpy * fpy * secVertex.covariancePosition()(Trk::y, Trk::y)) / (fpt * fpt);

  if (sigma_fpt2 <= 0.) {
    ATH_MSG_WARNING("sigma delta pt of (secVtx - priVtx) is 0!");
    return -11111.;
  }

  double sigma_fpt = std::sqrt(sigma_fpt2);
  double sign = 0.;

  if (fpx * pTau.p4().Px() + fpy * pTau.p4().Py() > 0.) sign = 1.;
  else sign = -1.;

  return sign * fpt / sigma_fpt;
}

#endif
