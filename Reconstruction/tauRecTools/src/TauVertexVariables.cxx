/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "tauRecTools/TauEventData.h"
#include "TauVertexVariables.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauVertexVariables::TauVertexVariables(const std::string &name ) :
  TauRecToolBase(name),
  m_fitTool("Trk::AdaptiveVertexFitter"),
  m_SeedFinder("Trk::CrossDistancesSeedFinder")
{
  declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);
  declareProperty("VertexFitter", m_fitTool);
  declareProperty("SeedFinder", m_SeedFinder);
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
  CHECK( m_trackToVertexIPEstimator.retrieve() );
  CHECK( m_fitTool.retrieve() );
  CHECK( m_SeedFinder.retrieve() );

  ATH_CHECK(m_vertexInputContainer.initialize(!m_vertexInputContainer.key().empty()) );
  ATH_CHECK(m_trackPartInputContainer.initialize(!m_trackPartInputContainer.key().empty()) );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauVertexVariables::finalize() {
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauVertexVariables::executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& pSecVtxContainer) {

  ATH_MSG_DEBUG("executing TauVertexVariables");
	
  // impact parameter variables for standard tracks
  if (pTau.nTracks() > 0) {

    const Trk::ImpactParametersAndSigma* myIPandSigma(0);
    const xAOD::Vertex* vxcand = nullptr;
	  
    if (m_in_trigger) {
	  
      StatusCode scBeam = StatusCode::FAILURE;
	  
      if (tauEventData()->hasObject("Beamspot")) scBeam = tauEventData()->getObject("Beamspot", vxcand);

      if(scBeam){
	myIPandSigma = m_trackToVertexIPEstimator->estimate(pTau.track(0)->track(), vxcand);
      } else {
	ATH_MSG_DEBUG("No Beamspot object in tau candidate");
      }
	  
    }  else if (pTau.vertexLink()) {

      vxcand = *(pTau.vertexLink()) ;

      //check if vertex has a valid type (skip if vertex has type NoVtx)
      if (vxcand->vertexType() > 0){
	myIPandSigma = m_trackToVertexIPEstimator->estimate(pTau.track(0)->track(), vxcand);
      }

    }
	  
    if (myIPandSigma != 0) {
      pTau.setDetail(xAOD::TauJetParameters::ipSigLeadTrk, (float)( myIPandSigma->IPd0 / myIPandSigma->sigmad0 ));
      pTau.setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, (float)( myIPandSigma->IPz0SinTheta / myIPandSigma->sigmaz0SinTheta ));
    } else {
      ATH_MSG_DEBUG("trackToVertexIPestimator failed for a standard track!");
      pTau.setDetail(xAOD::TauJetParameters::ipSigLeadTrk, (float)(-999.));
      pTau.setDetail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, (float)(-999.));
    }
	  
    delete myIPandSigma;

  } else {

    ATH_MSG_DEBUG("Tau has no tracks");

  }


  float ipSigLeadTrk;
  float ipZ0SinThetaSigLeadTrk;

  if (pTau.detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk))
    ATH_MSG_VERBOSE("IP significance lead track " << ipSigLeadTrk);
  if (pTau.detail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, ipZ0SinThetaSigLeadTrk))
    ATH_MSG_VERBOSE("IP Z0 significance lead track " << ipZ0SinThetaSigLeadTrk);

  //try to find secondary vertex
  //look for secondary vertex if more than 1 track
  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(-1111.));
  if (pTau.nTracks() < 2) {
    return StatusCode::SUCCESS;
  }

  if (m_vertexInputContainer.key().empty()){
    const xAOD::VertexContainer* vxContainer = 0;
    StatusCode sc=StatusCode::SUCCESS;
    sc = tauEventData()->getObject("VxPrimaryCandidate", vxContainer);
    
    if (sc.isFailure() || !vxContainer) {
      ATH_MSG_WARNING("No vertex container found. Skipping secondary vertex fitting.");
      return StatusCode::SUCCESS;
    }
  }
  else{
    // retrieve vertex container, exit if not found
    SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexInputContainer );
    if (!vertexInHandle.isValid()) {
      ATH_MSG_WARNING("No vertex container found. Skipping secondary vertex fitting.");
      return StatusCode::SUCCESS;
    }
    // retrieve track particle container, exit if not found 
    if(!m_trackPartInputContainer.key().empty()){
      SG::ReadHandle<xAOD::TrackParticleContainer> trackPartInHandle( m_trackPartInputContainer );
      if (!trackPartInHandle.isValid()) {
        ATH_MSG_WARNING("No track particle container found. Skipping secondary vertex fitting.");
        return StatusCode::SUCCESS;
      }
    }
  }

  // get xAOD TrackParticles and Trk::Tracks
  std::vector<const xAOD::TrackParticle*> xaodTracks;
  std::vector<const Trk::Track*> origTracks;
  for (unsigned i = 0; i < pTau.nTracks(); ++i) {
    xaodTracks.push_back(pTau.track(i)->track());
    ATH_MSG_VERBOSE("xAOD::TrackParticle " <<i<<": "<< pTau.track(i)->pt() << " "  << pTau.track(i)->eta()  << " "  << pTau.track(i)->phi());
    if (pTau.track(i)->track()) {
      origTracks.push_back(pTau.track(i)->track()->track());

      // for debugging
      /*
	ATH_MSG_DEBUG("Trk::Track " <<i<<": "<< (*pTau.track(i)->track())->pt() << " "  << (*pTau.track(i)->track())->eta()  << " "  << (*pTau.track(i)->track())->phi());
	const Trk::TrackParameters * tmpMeasPer = (*pTau.track(i)->track())->perigeeParameters();
	const AmgSymMatrix(5)* cov = tmpMeasPer->covariance();
	ATH_MSG_DEBUG("   TrackParameters: pT="<< tmpMeasPer->pT() << ", eta="  << tmpMeasPer->eta()  << ", x="  << tmpMeasPer->position().x() << ", y="<< tmpMeasPer->position().y() <<", z="<< tmpMeasPer->position().z());
	ATH_MSG_DEBUG("   covariance ="<< *cov);
      */
      // for debugging
    }
    else {
      ATH_MSG_WARNING("no Trk::Track for xAOD::TrackParticle");
    }
  }

  // get the starting point for the fit using Trk::Tracks
  //  Trk::Vertex* seedPoint = new Trk::Vertex(m_SeedFinder->findSeed(origTracks));
  Amg::Vector3D seedPoint = m_SeedFinder->findSeed(origTracks);
  ATH_MSG_VERBOSE("seedPoint x/y/perp=" << seedPoint.x() <<  " " << seedPoint.y() << " "<< seedPoint.z() << " " << TMath::Sqrt(seedPoint.x()*seedPoint.x()+seedPoint.y()+seedPoint.y()));
  // if (!seedPoint) {
  //   ATH_MSG_WARNING("no seedPoint: Can not calculate secondary vertex!");
  //   return StatusCode::SUCCESS;
  // }

  // fitting the vertex itself
  xAOD::Vertex* xAODvertex(0);
    xAODvertex = m_fitTool->fit(xaodTracks, seedPoint);
    if (xAODvertex && !m_in_trigger) {
      ATH_MSG_VERBOSE("using new xAOD API: Secondary Vertex found and recorded! x="<<xAODvertex->position().x()<< ", y="<<xAODvertex->position().y()<<", perp="<<xAODvertex->position().perp());
      pSecVtxContainer.push_back(xAODvertex);
      xAODvertex->setVertexType(xAOD::VxType::NotSpecified);
    }

  if (!xAODvertex) {
    ATH_MSG_WARNING("no secondary vertex found!");
    return StatusCode::SUCCESS;
  }

  // get the transverse flight path significance
  float trFlightPS = trFlightPathSig(pTau, *xAODvertex);
  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(trFlightPS));
  ATH_MSG_VERBOSE("transverse flight path significance="<<trFlightPS);

  // Note, we only attach the 2nd vertex if at offline, otherwise, break the trigger persistency
  if  (!m_in_trigger) {
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
double TauVertexVariables::trFlightPathSig(const xAOD::TauJet& pTau, const xAOD::Vertex& secVertex) {

  const xAOD::Vertex* pVertex = 0;
  if (pTau.vertexLink()) pVertex = *pTau.vertexLink();
  if (!pVertex) {
    ATH_MSG_WARNING("No primary vertex information for calculation of transverse flight path significance");
    return -11111.;
  }

  double fpx = secVertex.position().x() - pVertex->position().x();
  double fpy = secVertex.position().y() - pVertex->position().y();
  double fpt = (secVertex.position() - pVertex->position()).perp();

  if (fpt == 0) {
    ATH_MSG_WARNING("delta pt of (secVtx - priVtx) is 0!");
    return -11111.;
  }

  double sigma_fpt2 = (fpx * fpx * secVertex.covariancePosition()(Trk::x, Trk::x) +
		       fpx * fpy * secVertex.covariancePosition()(Trk::x, Trk::y) +
		       fpy * fpx * secVertex.covariancePosition()(Trk::y, Trk::x) +
		       fpy * fpy * secVertex.covariancePosition()(Trk::y, Trk::y)) / (fpt * fpt);

  if (sigma_fpt2 <= 0) {
    ATH_MSG_WARNING("sigma delta pt of (secVtx - priVtx) is 0!");
    return -11111.;
  }

  double sigma_fpt = sqrt(sigma_fpt2);
  double sign = 0;

  if (fpx * pTau.p4().Px() + fpy * pTau.p4().Py() > 0.) sign = 1.;
  else sign = -1.;

  //ATH_MSG_INFO(sign << " " <<fpt << " " << sigma_fpt << " " << sign * fpt / sigma_fpt);
  return sign * fpt / sigma_fpt;
}

#endif
