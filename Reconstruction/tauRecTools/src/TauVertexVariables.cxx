/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

//#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
//#include "TrkParameters/TrackParameters.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "tauRecTools/TauEventData.h"
#include "TauVertexVariables.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauVertexVariables::TauVertexVariables(const std::string &name ) :
  TauRecToolBase(name),
  m_primaryVertexKey("PrimaryVertices"),
  // m_useOldSeedFinderAPI(false),
  m_fitTool("Trk::AdaptiveVertexFitter"),
  m_SeedFinder("Trk::CrossDistancesSeedFinder"),
  m_xaodConverter("Trk::VxCandidateXAODVertex"),
  m_pSecVtxContainer(0),
  m_pSecVtxAuxContainer(0){
  declareProperty("PrimaryVertexKey", m_primaryVertexKey);
  declareProperty("TrackParticleContainer", m_inputTrackParticleContainerName = "InDetTrackParticles");
  declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);
  declareProperty("VertexFitter", m_fitTool);
  declareProperty("SeedFinder", m_SeedFinder);
  declareProperty("XAODConverter",m_xaodConverter);
  declareProperty("useOldSeedFinderAPI",m_useOldSeedFinderAPI=false);
  declareProperty("runOnAOD", m_AODmode=false);//AODS are input file 
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
  // if (m_useOldSeedFinderAPI) CHECK( m_xaodConverter.retrieve() );

  // if (m_useOldSeedFinderAPI) {
  //   ATH_MSG_INFO("using AOD-style API of the AdaptiveVertexFitter");
  // }
  // else {
  //   ATH_MSG_INFO("using new xAOD-style API of the AdaptiveVertexFitter");
  // }

  return StatusCode::SUCCESS;
}

StatusCode TauVertexVariables::eventInitialize() {

  bool inTrigger = tauEventData()->inTrigger();
  
  // Only store the vertex containers if we are offline?
  if(!inTrigger)
    {
      // Secondary Vertex Container for tau decay vertex
      if(!m_AODmode){
	m_pSecVtxContainer = new xAOD::VertexContainer();
	m_pSecVtxAuxContainer = new xAOD::VertexAuxContainer();
	m_pSecVtxContainer->setStore( m_pSecVtxAuxContainer );
      
	CHECK( evtStore()->record( m_pSecVtxContainer, "TauSecondaryVertices" ) );
	CHECK( evtStore()->record( m_pSecVtxAuxContainer, "TauSecondaryVerticesAux." ) );
      }
      else {
	CHECK( evtStore()->retrieve( m_pSecVtxContainer, "TauSecondaryVertices") );
	CHECK( evtStore()->retrieve( m_pSecVtxAuxContainer, "TauSecondaryVerticesAux.") );
      }
    }
  
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
StatusCode TauVertexVariables::execute(xAOD::TauJet& pTau) {

  ATH_MSG_DEBUG("executing TauVertexVariables");

  // for tau trigger
  bool inTrigger = tauEventData()->inTrigger();
  if (inTrigger) ATH_MSG_DEBUG("We're in the Trigger");
	
  // impact parameter variables for standard tracks
  if (pTau.nTracks() > 0) {

    const Trk::ImpactParametersAndSigma* myIPandSigma(0);
    const xAOD::Vertex* vxcand = nullptr;
	  
    if (inTrigger) {
	  
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

  const xAOD::VertexContainer* vxContainer = 0;
  StatusCode sc=StatusCode::SUCCESS;
  if (sc.isSuccess() && inTrigger)   sc = tauEventData()->getObject("VxPrimaryCandidate", vxContainer);
  // retrieve vertex container, exit if not found
  else sc = evtStore()->retrieve(vxContainer, m_primaryVertexKey);

  if (sc.isFailure() || !vxContainer) {
    ATH_MSG_WARNING("No vertex container found. Skipping secondary vertex fitting.");
    return StatusCode::SUCCESS;
  }

  const xAOD::TrackParticleContainer* trackParticleCont = 0;
  if (inTrigger)   sc = tauEventData()->getObject( "TrackContainer", trackParticleCont );
  // retrieve track particle container, exit if not found
  else sc = evtStore()->retrieve(trackParticleCont, m_inputTrackParticleContainerName);
  if (sc.isFailure() || !trackParticleCont) {
    ATH_MSG_WARNING("No track particle container found. Skipping secondary vertex fitting.");
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
  //  if (!m_useOldSeedFinderAPI) { // use new xAOD API of VertexFitter
    xAODvertex = m_fitTool->fit(xaodTracks, seedPoint);
    if (xAODvertex && !inTrigger) {
      ATH_MSG_VERBOSE("using new xAOD API: Secondary Vertex found and recorded! x="<<xAODvertex->position().x()<< ", y="<<xAODvertex->position().y()<<", perp="<<xAODvertex->position().perp());
      m_pSecVtxContainer->push_back(xAODvertex);
      xAODvertex->setVertexType(xAOD::VxType::NotSpecified);
    }
    //  }
  // else { // use standard AOD-style API of VertexFitter
  //   Trk::VxCandidate*  = m_fitTool->fit(origTracks, *seedPoint);
  //   if (tmpVxCandidate) {
  //     ATH_MSG_VERBOSE("using old AOD API:Secondary Vertex found and recorded! x="<<tmpVxCandidate->recVertex().position().x()<< ", y="<<tmpVxCandidate->recVertex().position().y()<<", perp="<<tmpVxCandidate->recVertex().position().perp());

  //     //******************************************************************
  //     // convert VxCandidate to xAOD::Vertex
  //     //******************************************************************

  //     // assigning the input xAOD tracks to the fitted vertex
  //     // this means: after that procedure the Trk::VxCandidate knows already the links to the xAOD tracks (which have to be identical with the Trk:Tracks used in the VertexFitter!)
  //     // this is needed for the converting, otherwise the new xAODVertex don't have the track links
  //     if(tmpVxCandidate->vxTrackAtVertex() != 0 && tmpVxCandidate->vxTrackAtVertex()->size() !=0) {
  // 	for(unsigned int i = 0; i <xaodTracks.size(); ++i) {
  // 	  Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle;
  // 	  linkTT->setElement(xaodTracks[i]);
  // 	  linkTT->setStorableObject(*trackParticleCont);
  // 	  // vxtrackatvertex takes ownership!
  // 	  (*(tmpVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
  // 	}
  //     }

  //     xAODvertex = new xAOD::Vertex();
  //     if (!inTrigger) m_pSecVtxContainer->push_back(xAODvertex);
  //     // perform the final converting now
  //     if( m_xaodConverter->createXAODVertex(*tmpVxCandidate,xAODvertex).isFailure() ) {
  // 	ATH_MSG_ERROR("Failed to create xAODVertex for VxCandidate. Don't set any secondary vertex for tau!");
  // 	return StatusCode::SUCCESS;
  //     }
  //     delete tmpVxCandidate;
  //   }
  // }
    //  delete seedPoint;

  if (!xAODvertex) {
    ATH_MSG_WARNING("no secondary vertex found!");
    return StatusCode::SUCCESS;
  }

  // get the transverse flight path significance
  float trFlightPS = trFlightPathSig(pTau, *xAODvertex);
  pTau.setDetail(xAOD::TauJetParameters::trFlightPathSig, (float)(trFlightPS));
  ATH_MSG_VERBOSE("transverse flight path significance="<<trFlightPS);

  // Note, we only attach the 2nd vertex if at offline, otherwise, break the trigger persistency
  if  (!inTrigger) {
    pTau.setSecondaryVertex(m_pSecVtxContainer, xAODvertex); 		// set the link to the vertex
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
