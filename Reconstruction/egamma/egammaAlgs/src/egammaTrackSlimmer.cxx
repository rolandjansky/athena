/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     egammaTrackSlimmer.cxx
PACKAGE:  offline/Reconstruction/egammaRec
********************************************************************/

// INCLUDE HEADER FILES:

#include "egammaTrackSlimmer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ThinningHandle.h"

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////


// INITIALIZE METHOD:
     
StatusCode egammaTrackSlimmer::initialize()
{
  ATH_MSG_INFO("Initializing egammaTrackSlimmer");

  // initialize data handles
  ATH_CHECK(m_InputElectronContainerKey.initialize(m_doThinning));
  ATH_CHECK(m_InputPhotonContainerKey.initialize(m_doThinning));
  ATH_CHECK(m_TrackParticlesKey.initialize(m_streamName, m_doThinning));
  ATH_CHECK(m_VertexKey.initialize(m_streamName, m_doThinning));
  ATH_CHECK(m_InDetTrackParticlesKey.initialize(m_streamName, m_doThinning));

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:

StatusCode egammaTrackSlimmer::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
   
StatusCode egammaTrackSlimmer::execute (const EventContext& ctx) const {
  
  ATH_MSG_DEBUG("Executing egammaTrackSlimmer");
  if(!m_doThinning){
    ATH_MSG_DEBUG("Thinning not requested do nothing");
    return StatusCode::SUCCESS;
  }
  
  /* 
   * GSF Track Particles
   * The vector that we'll use to filter the track particles:
   */
  std::vector<bool> keptTrackParticles;
  SG::ThinningHandle<xAOD::TrackParticleContainer> trackPC(m_TrackParticlesKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if(!trackPC.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_TrackParticlesKey.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Number of TrackParticles "<< trackPC->size());
  keptTrackParticles.resize( trackPC->size(), false );

  /*
   * GSF Vertices
   * The vector that we'll use to filter the GSF vertices:
   */
  std::vector<bool> keptVertices;
  SG::ThinningHandle<xAOD::VertexContainer> vertices(m_VertexKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if(!vertices.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_VertexKey.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Number of Vertices "<< vertices->size());
  keptVertices.resize( vertices->size(), false );

  /* 
   * In Det Track Particles
   * The vector that we'll use to filter the In Det Track Particles
   */
  std::vector<bool> keptInDetTrackParticles;
  SG::ThinningHandle<xAOD::TrackParticleContainer> indetTrackPC(m_InDetTrackParticlesKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if(!indetTrackPC.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_InDetTrackParticlesKey.key());
    return StatusCode::FAILURE;
  }
  /*
   * For the InDet Track particles we set them first all to true aka keep.
   * Then we set to false ONLY the TRT-alone.
   * (05/05/21: omitting the aforementioned step for release 22 round 2 CP samples production)
   * We will reset to true whatever is to be kept due to e/gamma... 
   */
  ATH_MSG_DEBUG("Number of In Det TrackParticles "<< indetTrackPC->size());
  keptInDetTrackParticles.resize( indetTrackPC->size(), true );

  /*
   * Electron track particle Thinning
   */
  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_InputElectronContainerKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if(!electrons.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_InputElectronContainerKey.key());
    return StatusCode::FAILURE;
  }

  //Loop over electrons
  auto el_itr = electrons->begin();
  auto el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {

    auto trackParticleLinks = (*el_itr)->trackParticleLinks();
    for (const auto& link :  trackParticleLinks){
      if( ! link.isValid() ){
        continue;
      }
      ATH_MSG_DEBUG("Electrons : Keeping GSF Track Particle with index : "<< link.index() );
      keptTrackParticles[link.index() ] = true;      
  
      ATH_MSG_DEBUG("Electons : Keeping InDet Track Particle with index : "
                    << (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))->index());
      keptInDetTrackParticles[ (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))->index() ] = true;
    }
  }
  
  /*
   * Photon Vertex and track particle  Thinning
   */
  SG::ReadHandle<xAOD::PhotonContainer> photons(m_InputPhotonContainerKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if(!photons.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_InputPhotonContainerKey.key());
    return StatusCode::FAILURE;
  }
    
  //Loop over photons
  auto ph_itr = photons->begin();
  auto ph_end = photons->end();
  for( ; ph_itr != ph_end; ++ph_itr ) {

    auto vertexLinks= (*ph_itr) ->vertexLinks();
    for ( const auto& vxlink :  vertexLinks){
      if( ! vxlink.isValid() ){
        continue;
      }
      ATH_MSG_DEBUG("Photons : Keeping GSF Vertex with index : "<< vxlink.index() );
      keptVertices[vxlink.index()]=true;
      const xAOD::Vertex* vx = *(vxlink);
      if(!vx){
        continue;
      }
    
      auto trackParticleLinks = vx->trackParticleLinks();
      for ( const auto& link :  trackParticleLinks){
        if( ! link.isValid() ){
          continue;
        }
        ATH_MSG_DEBUG("Photons : Keeping GSF Track Particle with index : "<< link.index() );
        keptTrackParticles[link.index() ] = true;
        ATH_MSG_DEBUG("Photons : Keeping InDet Track Particle with index : "
                      << (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))->index());
        keptInDetTrackParticles[ (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))->index()] = true;
      }
    }
  }
  //Do the Thinning
  ATH_MSG_DEBUG("Do the Thinning");
  trackPC.keep (keptTrackParticles);
  vertices.keep (keptVertices);
  indetTrackPC.keep (keptInDetTrackParticles);
  ATH_MSG_DEBUG("completed successfully");
  
  //Return Gracefully
  return StatusCode::SUCCESS;
}

