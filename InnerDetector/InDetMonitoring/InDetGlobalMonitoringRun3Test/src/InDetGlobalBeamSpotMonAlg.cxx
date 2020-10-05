/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalBeamSpotMonAlg.cxx
 * Implementation of inner detector global beamspot monitoring tool
 *
 *@authors
 * Leonid Serkin <lserkin@cern.ch> @n
 * Per Johansson <Per.Johansson@cern.ch> @n
 *
 * based on InDetGlobalBeamSpotMonTool.cxx
 *
 ****************************************************************************/

//main header
#include "InDetGlobalBeamSpotMonAlg.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include <sstream>
#include <cmath>



InDetGlobalBeamSpotMonAlg::InDetGlobalBeamSpotMonAlg( const std::string & name, ISvcLocator* pSvcLocator) : 
  AthMonitorAlgorithm( name, pSvcLocator ),
  m_useBeamspot(true),
  m_vxContainerWithBeamConstraint(false),
  m_minTracksPerVtx(4),
  m_minTrackPt(500)
{
  declareProperty("useBeamspot", m_useBeamspot, "turn on histograms for assumed beam spot position");
  declareProperty("vxContainerWithBeamConstraint", m_vxContainerWithBeamConstraint, "turn on histograms for vertex-based beam spot monitoring" );
  declareProperty("minTracksPerVtx", m_minTracksPerVtx, "minimum number of tracks per vertex" );
  declareProperty("minTrackPt", m_minTrackPt, "minimum track pT (MeV)");  
}

InDetGlobalBeamSpotMonAlg::~InDetGlobalBeamSpotMonAlg() {}


StatusCode InDetGlobalBeamSpotMonAlg::initialize() {
  ATH_CHECK(m_beamSpotKey.initialize());  
  ATH_CHECK(m_vxContainerName.initialize());
  ATH_CHECK(m_trackContainerName.initialize());
  
  return AthMonitorAlgorithm::initialize();
}


StatusCode InDetGlobalBeamSpotMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  
  //*******************************************************************************
  //************************** Begin of filling Beam spot Histograms ******************
  //*******************************************************************************
  ATH_MSG_DEBUG("Filling InDetGlobalBeamSpotMonAlg");
  
  // For histogram naming
  auto bsGroup = getGroup("BeamSpot");
  
  // Get beamspot information, if available
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  float beamTiltX = 0.;
  float beamTiltY = 0.;
  float scaleFactor = 1.;
  if (m_useBeamspot) {
    
    
    auto beamSpotHandle = SG::ReadCondHandle(m_beamSpotKey, ctx);
    
    // check for tracks
    if ( !(beamSpotHandle.isValid()) ) {
      ATH_MSG_ERROR("InDetGlobalBeamSpotMonAlg: BeamSpot container "<< beamSpotHandle.key() << " could not be found.");
      return StatusCode::RECOVERABLE;
    } else {
      ATH_MSG_DEBUG("InDetGlobalBeamSpotMonAlg: BeamSpot container "<< beamSpotHandle.fullKey() <<" is found.");
    }
    
    
    
    const Amg::Vector3D &bpos = beamSpotHandle->beamPos();
    
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
    auto beamSpotX_m = Monitored::Scalar<float>("m_bsX", beamSpotX );
    auto beamSpotY_m = Monitored::Scalar<float>("m_bsY", beamSpotY );
    auto beamSpotZ_m = Monitored::Scalar<float>("m_bsZ", beamSpotZ );
    beamTiltX = beamSpotHandle->beamTilt(0);
    beamTiltY = beamSpotHandle->beamTilt(1);
    auto beamTiltX_m = Monitored::Scalar<float>("m_bsTiltX", 1e6*beamTiltX );
    auto beamTiltY_m = Monitored::Scalar<float>("m_bsTiltY", 1e6*beamTiltY );
    scaleFactor = 1000.;   // Use microns for some histograms when showing distance relative to beamspot PJ not used here?!
    
    fill(bsGroup,beamSpotX_m);
    fill(bsGroup,beamSpotY_m);
    fill(bsGroup,beamSpotZ_m);
    fill(bsGroup,beamTiltX_m);
    fill(bsGroup,beamTiltY_m);
    
    ATH_MSG_DEBUG("InDetGlobalBeamSpotMonAlg: Beamspot x0 = " << beamSpotX_m << ", y0 = " << beamSpotY_m << ", z0 = " << beamSpotZ_m << ", tiltX = " << beamTiltX_m << ", tiltY = " << beamTiltY_m);
  } // m_useBeamspot
  
  
  auto trackCollection = SG::makeHandle(m_trackContainerName, ctx);
  
  if(!(trackCollection.isValid())){
    ATH_MSG_DEBUG ("InDetGlobalBeamSpotMonAlg: Could not retrieve TrackParticleContainer container with key "+m_trackContainerName.key());
    return StatusCode::SUCCESS;
  }
  
  
  // Track monitoring
  int nTracks = 0;
  
  //range based for-loop
  for (const xAOD::TrackParticle* tpb: *trackCollection) {
    
    if ( !tpb )
      {
	ATH_MSG_DEBUG( "InDetGlobalBeamSpotMonAlg: NULL track pointer in collection" );
	continue;
      }
    
    
    const Trk::Perigee* perigee = &(tpb->perigeeParameters());
    
    if ( !perigee )  
      {
	ATH_MSG_DEBUG( "InDetGlobalBeamSpotMonAlg: NULL track->perigeeParameters pointer " );
	continue;
      }
    
    
    
    float theta = perigee->parameters()[Trk::theta];
    float qOverPt = perigee->parameters()[Trk::qOverP]/sin(theta);
    float charge = perigee->charge();
    float z0 = perigee->parameters()[Trk::z0];
    float phi0 = perigee->parameters()[Trk::phi0];
    float d0 = perigee->parameters()[Trk::d0];
    float pT = 0;
    
    if ( qOverPt != 0 ){
      pT = (1/qOverPt)*(charge);
      // For all tracks
      auto pT_m = Monitored::Scalar<float>("m_trkPt", pT/1000);  
      fill(bsGroup, pT_m);
      
      // Select tracks to use for remaining histograms
      if (pT<m_minTrackPt) continue;
    }
    
    nTracks++;
    
    auto trkDPhi_m = Monitored::Scalar<float>("m_trkD0Phi", phi0);
    auto trkD_m    = Monitored::Scalar<float>("m_trkD0", d0*1e3);
    fill(bsGroup, trkD_m, trkDPhi_m);
    
    // Currently we do the direct calculation of d0corr. We could
    // also use an extrapolator to calculate d0 wrt a
    // Trk::StraightLineSurface constructed along the beam line.
    if(m_useBeamspot){
      
      float trkbeamlineTiltX=tpb->beamlineTiltX();
      float trkbeamlineTiltY=tpb->beamlineTiltY();
      float trkbeamspotx=tpb->vx();
      float trkbeamspoty=tpb->vy();
      float trkbeamspotz=tpb->vz();
      
      float beamX = (beamSpotX-trkbeamspotx) + std::tan(beamTiltX-trkbeamlineTiltX) * (z0-beamSpotZ+trkbeamspotz);
      float beamY = (beamSpotY-trkbeamspoty) + std::tan(beamTiltY-trkbeamlineTiltY) * (z0-beamSpotZ+trkbeamspotz);
      float d0corr = d0 - ( -std::sin(phi0)*beamX + std::cos(phi0)*beamY );
      
      
      auto trkDPhiCorr_m  = Monitored::Scalar<float>("m_trkD0PhiCorr", phi0);
      auto trkDCorr_m     = Monitored::Scalar<float>("m_trkD0Corr", d0corr*1e3);
      fill(bsGroup, trkDPhiCorr_m, trkDCorr_m);
    }
  } // track iterator
  
  auto trkNPt_m = Monitored::Scalar<float>("m_trkNPt", nTracks);
  fill(bsGroup,trkNPt_m);
  
  // Primary vertex monitoring - only if we have a primary vertex collection determined
  // without beam constraint
  if (! m_vxContainerWithBeamConstraint) {
    ATH_MSG_DEBUG( "InDetGlobalBeamSpotMonAlg: vxContainerWithBeamConstraint is " << m_vxContainerWithBeamConstraint );
    
    // Basic primary vertex monitoring
    auto handle_vxContainer = SG::makeHandle(m_vxContainerName, ctx);  
    auto vertexContainer = handle_vxContainer.cptr();

    if (!handle_vxContainer.isValid()) {
      ATH_MSG_DEBUG ("InDetGlobalBeamSpotMonAlg: Could not retrieve primary vertex container with key "+ m_vxContainerName.key());
      return StatusCode::SUCCESS;
    }
    
    auto pvN_m = Monitored::Scalar<float>("m_pvN", vertexContainer->size()-1);  // exclude dummy vertex
    fill(bsGroup, pvN_m);
    
    int nPriVtx = 0;
    int nPileupVtx = 0;
    
    for(const auto & vtx : *vertexContainer) {
      
      if ( !vtx ) continue;
      
      // Count different types of vertices
      if (vtx->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
      if (vtx->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;
      
      // Select primary vertex
      if (vtx->vertexType() != xAOD::VxType::PriVtx) continue;
      if (vtx->numberDoF() <= 0) continue;
      
      if (vtx->nTrackParticles() < m_minTracksPerVtx) continue;
      
      // Found good VxCandidate to monitor - now fill histograms
      float x = vtx->position().x();
      float y = vtx->position().y();
      float z = vtx->position().z();
      float beamX = beamSpotX + std::tan(beamTiltX) * (z-beamSpotZ);
      float beamY = beamSpotY + std::tan(beamTiltY) * (z-beamSpotZ);
      float beamZ = beamSpotZ;
      
      auto pvXbeam_m = Monitored::Scalar<float>("m_pvXbeam", (x-beamX)*scaleFactor);
      fill(bsGroup, pvXbeam_m);
      
      auto pvYbeam_m = Monitored::Scalar<float>("m_pvYbeam", (y-beamY)*scaleFactor);
      fill(bsGroup, pvYbeam_m);
      
      auto pvZbeam_m = Monitored::Scalar<float>("m_pvZbeam", z-beamZ);
      fill(bsGroup, pvZbeam_m);
      
      auto pvErrX_m = Monitored::Scalar<float>("m_pvErrX", Amg::error( vtx->covariancePosition(), Trk::x));
      fill(bsGroup, pvErrX_m);

      auto pvErrY_m = Monitored::Scalar<float>("m_pvErrY", Amg::error( vtx->covariancePosition(), Trk::y));
      fill(bsGroup, pvErrY_m);

      auto pvErrZ_m = Monitored::Scalar<float>("m_pvErrZ", Amg::error( vtx->covariancePosition(), Trk::z));
      fill(bsGroup, pvErrZ_m);

      auto pvNTracks_m  = Monitored::Scalar<float>("m_pvNTracks", vtx->nTrackParticles());
      fill(bsGroup, pvNTracks_m);
      
      auto pvChiSqDof_m = Monitored::Scalar<float>("m_pvChiSqDof", vtx->chiSquared() / vtx->numberDoF());
      fill(bsGroup, pvChiSqDof_m);
      
      auto pvX_m = Monitored::Scalar<float>("m_pvX", x);
      auto pvY_m = Monitored::Scalar<float>("m_pvY", y);
      auto pvZ_m = Monitored::Scalar<float>("m_pvZ", z);
      fill(bsGroup, pvX_m, pvZ_m);
      fill(bsGroup, pvY_m, pvZ_m);
      fill(bsGroup, pvY_m, pvX_m);
      
      // Histograms on original tracks used for primary vertex
      for (unsigned int trkIter=0; trkIter!=vtx->nTrackParticles(); ++trkIter) {
        const xAOD::TrackParticle* tp = vtx->trackParticle(trkIter);
        if(!tp){
          ATH_MSG_DEBUG ("InDetGlobalBeamSpotMonAlg: Could not retrieve track particle.");
          continue;
        }
        const Trk::Perigee measuredPerigee = tp->perigeeParameters();
	auto pvTrackPt_m  = Monitored::Scalar<float>("m_pvTrackPt", measuredPerigee.pT()/1000.);
	auto pvTrackEta_m = Monitored::Scalar<float>("m_pvTrackEta", measuredPerigee.eta());
	fill(bsGroup, pvTrackPt_m);
	fill(bsGroup, pvTrackEta_m);
      }
    } // vertex iterator
    
    auto pvNPriVtx_m    = Monitored::Scalar<float>("m_pvNPriVtx", nPriVtx);
    auto pvNPileupVtx_m = Monitored::Scalar<float>("m_pvNPileupVtx", nPileupVtx);
    fill(bsGroup, pvNPriVtx_m);
    fill(bsGroup, pvNPileupVtx_m);
  } 
  
  return StatusCode::SUCCESS;
}
