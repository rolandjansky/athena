/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaTrackSlimmer.cxx
PACKAGE:  offline/Reconstruction/egammaRec

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthenaKernel/IThinningSvc.h"

#include "egammaRec/egammaTrackSlimmer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"


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

// The following properties are specified at run-time
// (declared in jobOptions file)
                    
  // Name of the track thinning service
  declareProperty("thinSvc", 
		  m_thinningSvc,
		  "Name of the thinning service");

  // Name of the input electron container
  declareProperty("InputElectronContainerName",   
		  m_InputElectronContainerName  = "ElectronCollection",
		  "Name of the input electron container");

  // Name of the input photon container
  declareProperty("InputPhotonContainerName",   
		  m_InputPhotonContainerName  = "PhotonCollection",
		  "Name of the input photon container");


  
  // Name of the FSF track particle container
  declareProperty("TrackParticleContainerName", 
		  m_TrackParticlesName="GSFTrackParticles",
		  "Name of the Track Particle container");  

  // Name of the  GSF Vertex container
  declareProperty("VertexContainerName", 
		  m_VertexName="GSFConversionVertices",
		  "Name of the Vertex container");  

  // Bool to decide if we actually do the Thinning
  declareProperty("doThinning", 
		  m_doThinning=true,
		  "Bool to do Thinning");  


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
  
  //track Particles

  // The vector that we'll use to filter the track particles:
  IThinningSvc::VecFilter_t keptTrackParticles;
  const xAOD::TrackParticleContainer* trackPC = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_TrackParticlesName );
  if( ! trackPC ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "Couldn't retrieve TrackParticle container with key: "
      << m_TrackParticlesName;
    return StatusCode::SUCCESS;
  }
  // Check that the auxiliary store association was made successfully:
  if( ! trackPC->hasStore() ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "No auxiliary store got associated to the TrackParticle container "
      << "with key: " << m_TrackParticlesName;
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG("Number of TrackParticles "<< trackPC->size());
  if( keptTrackParticles.size() < trackPC->size() ) {
    keptTrackParticles.resize( trackPC->size(), false );
  }

  //Vertices
  
  // The vector that we'll use to filter the vertices:
  IThinningSvc::VecFilter_t keptVertices;

  const xAOD::VertexContainer* vertices = evtStore()->retrieve< const xAOD::VertexContainer >(m_VertexName );
  if( ! vertices ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "Couldn't retrieve Vertex container with key: "
      << m_VertexName;
    return StatusCode::SUCCESS;
  }
  // Check that the auxiliary store association was made successfully:
  if( ! vertices->hasStore() ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "No auxiliary store got associated to the Vertex container "
      << "with key: " << m_VertexName;
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG("Number of Vertices "<< vertices->size());
  if( keptVertices.size() < vertices->size() ) {
    keptVertices.resize( vertices->size(), false );
  }

  //electrons

  const xAOD::ElectronContainer* electrons =
    evtStore()->retrieve< const xAOD::ElectronContainer >( m_InputElectronContainerName );
  if( ! electrons ) {
    REPORT_MESSAGE( MSG::ERROR )
      << "Couldn't retrieve electron container with key: "
      << m_InputElectronContainerName;
    return StatusCode::FAILURE;
  }

  // Check that the auxiliary store association was made successfully:
  if( ! electrons->hasStore() ) {
    REPORT_MESSAGE( MSG::ERROR )
      << "No auxiliary store got associated to the electron container "
      << "with key: " << m_InputElectronContainerName;
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

  //photons

  const xAOD::PhotonContainer* photons =
    evtStore()->retrieve< const xAOD::PhotonContainer >( m_InputPhotonContainerName );
  if( ! photons ) {
    REPORT_MESSAGE( MSG::ERROR )
      << "Couldn't retrieve photon container with key: "
      << m_InputPhotonContainerName;
    return StatusCode::FAILURE;
  }

  // Check that the auxiliary store association was made successfully:
  if( ! photons->hasStore() ) {
    REPORT_MESSAGE( MSG::ERROR )
      << "No auxiliary store got associated to the photon container "
      << "with key: " << m_InputPhotonContainerName;
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

