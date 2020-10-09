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
  
  ATH_CHECK( m_fitTool.retrieve() );
  ATH_CHECK( m_SeedFinder.retrieve() );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauVertexVariables::executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& pSecVtxContainer) const {

  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(-1111.));
  
  // try to find secondary vertex if more than 1 track and the tau vertex is available
  if ( pTau.nTracks() < 2 ||  !pTau.vertexLink().isValid() ) {
    return StatusCode::SUCCESS;
  }

  // xAOD TrackParticles
  std::vector<const xAOD::TrackParticle*> xaodTracks;
  // standard reconstruction uses Trk::Tracks in SeedFinder
  std::vector<const Trk::Track*> origTracks;
  // reconstruction from xAOD uses Trk::TrackParameters (Trk::Track not available)
  std::vector<const Trk::TrackParameters*> origTrackParameters;

  for (const xAOD::TauTrack* track : pTau.tracks()) {
    xaodTracks.push_back(track->track());

    if(track->track()->track()) {
      origTracks.push_back(track->track()->track());
    }
    else {
      const Trk::Perigee& perigee = track->track()->perigeeParameters();
      origTrackParameters.push_back(static_cast<const Trk::TrackParameters*>(&perigee));
    }
  }

  // origTrackParameters should be empty in standard reconstruction, origTracks should be empty when re-running from xAOD
  if(origTracks.size()>0 && origTrackParameters.size()>0) {
    ATH_MSG_ERROR("Inconsistent mix of Trk::Track and Trk::TrackParameter");
    return StatusCode::FAILURE;
  }

  xAOD::Vertex* xAODvertex = nullptr;

  if(origTracks.size() > 0) {
    // get the starting point for the fit using Trk::Tracks
    const Amg::Vector3D& seedPoint = m_SeedFinder->findSeed(origTracks);
    // fitting the vertex
    xAODvertex = m_fitTool->fit(xaodTracks, seedPoint);
  }
  else if (origTrackParameters.size() > 0) {
    // get the starting point for the fit using Trk::TrackParameters
    const Amg::Vector3D& seedPoint = m_SeedFinder->findSeed(origTrackParameters);
    // fitting the vertex
    xAODvertex = m_fitTool->fit(xaodTracks, seedPoint);
  }

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
    pTau.setSecondaryVertex(&pSecVtxContainer, xAODvertex);
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

  if (! pTau.vertexLink().isValid()) {
    ATH_MSG_WARNING("No primary vertex information for calculation of transverse flight path significance");
    return -11111.;
  }
  const xAOD::Vertex* pVertex = pTau.vertex();

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
