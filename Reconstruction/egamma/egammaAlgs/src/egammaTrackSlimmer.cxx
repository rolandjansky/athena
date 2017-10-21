/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaTrackSlimmer.cxx
PACKAGE:  offline/Reconstruction/egammaRec

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthenaKernel/IThinningSvc.h"

#include "egammaTrackSlimmer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "StoreGate/ReadHandle.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////


//  CONSTRUCTOR:
    
egammaTrackSlimmer::egammaTrackSlimmer(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), 
  m_thinningSvc( "ThinningSvc",  name )
{
  // Name of the track thinning service
  declareProperty("thinSvc", 
		  m_thinningSvc,
		  "Name of the thinning service");
}

// DESTRUCTOR:
  
egammaTrackSlimmer::~egammaTrackSlimmer()
{  }

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:
     
StatusCode egammaTrackSlimmer::initialize()
{
  ATH_MSG_INFO("Initializing egammaTrackSlimmer");

  // Get pointer to IThinningSvc and cache it :
  if ( !m_thinningSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to IThinningSvc");
    return StatusCode::FAILURE;
  }

  // initialize data handles
  ATH_CHECK(m_InputElectronContainerKey.initialize(m_doThinning));
  ATH_CHECK(m_InputPhotonContainerKey.initialize(m_doThinning));
  ATH_CHECK(m_TrackParticlesKey.initialize(m_doThinning));
  ATH_CHECK(m_VertexKey.initialize(m_doThinning));

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:

StatusCode egammaTrackSlimmer::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
   
StatusCode egammaTrackSlimmer::execute() {
  
  ATH_MSG_DEBUG("Executing egammaTrackSlimmer");

  if(!m_doThinning){
    ATH_MSG_DEBUG("Thinning not requested do nothing");
    return StatusCode::SUCCESS;
  }
  
  //Track Particles
  // The vector that we'll use to filter the track particles:
  IThinningSvc::VecFilter_t keptTrackParticles;
  SG::ReadHandle<xAOD::TrackParticleContainer> trackPC(m_TrackParticlesKey);

  // check is only used for serial running; remove when MT scheduler used
  if(!trackPC.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_TrackParticlesKey.key());
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("Number of TrackParticles "<< trackPC->size());
  if( keptTrackParticles.size() < trackPC->size() ) {
    keptTrackParticles.resize( trackPC->size(), false );
  }

  //Vertices
  //The vector that we'll use to filter the vertices:
  IThinningSvc::VecFilter_t keptVertices;
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_VertexKey);
  
  // check is only used for serial running; remove when MT scheduler used
  if(!vertices.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_VertexKey.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Number of Vertices "<< vertices->size());
  if( keptVertices.size() < vertices->size() ) {
    keptVertices.resize( vertices->size(), false );
  }

  //Electron track particle Thinning
  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_InputElectronContainerKey);
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
    for (auto link :  trackParticleLinks){
      if( ! link.isValid() ){
	continue;
      }
      ATH_MSG_DEBUG("Electrons : Keeping GSF Track Particle with index : "<< link.index() );
      keptTrackParticles[link.index() ] = true;      
    }
  }
  
  //Photon vertex thinning
  SG::ReadHandle<xAOD::PhotonContainer> photons(m_InputPhotonContainerKey);
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
    for ( auto vxlink :  vertexLinks){
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
      for ( auto link :  trackParticleLinks){
	if( ! link.isValid() ){
	  continue;
	}
	ATH_MSG_DEBUG("Photons : Keeping GSF Track Particle with index : "<< link.index() );
	keptTrackParticles[link.index() ] = true;
      }
    }
  }
  //Do the Thinning
  ATH_MSG_DEBUG("Do the Thinning");
  CHECK( m_thinningSvc->filter( *trackPC,  keptTrackParticles ) );
  CHECK( m_thinningSvc->filter( *vertices, keptVertices ) );
  ATH_MSG_DEBUG("completed successfully");
  
  //Return Gracefully
  return StatusCode::SUCCESS;
}

