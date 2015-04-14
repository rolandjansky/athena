/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkLinks/LinkToXAODNeutralParticle.h"

// Local include(s):
#include "VertexCnvAlg.h"

namespace xAODMaker {

  VertexCnvAlg::VertexCnvAlg( const std::string& name,
    ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ) {

    declareProperty( "AODContainerName",
      m_aodContainerName = "VxPrimaryCandidate" );
    declareProperty( "xAODContainerName",
      m_xaodContainerName = "PrimaryVertices" );
    declareProperty( "TPContainerName",
      m_TPContainerName = "InDetTrackParticles" );
    declareProperty( "NPContainerName",
      m_NPContainerName = "VertexNeutralParticles" );
  }

  StatusCode VertexCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );
    ATH_MSG_INFO( "TPContainerName = " << m_TPContainerName );
    ATH_MSG_INFO( "NPContainerName = " << m_NPContainerName );

      // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode VertexCnvAlg::execute() {
    // Retrieve the AOD vertexes:
    const VxContainer* aod = evtStore()->tryConstRetrieve<VxContainer>(m_aodContainerName);
    if (!aod) {
       ATH_MSG_DEBUG("No VxContainer with key " << m_aodContainerName << " found. Do nothing.");
       return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );

    // Create the xAOD container and its auxiliary store:
    xAOD::VertexContainer* xaod = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* aux = new xAOD::VertexAuxContainer();

    if (evtStore()->contains<xAOD::VertexContainer>(m_xaodContainerName)) {   
      CHECK( evtStore()->overwrite( aux, m_xaodContainerName + "Aux.",true,false) );
      CHECK( evtStore()->overwrite( xaod, m_xaodContainerName,true,false) );
      xaod->setStore( aux );
      ATH_MSG_DEBUG( "Overwrote Vertexes with key: " << m_xaodContainerName );
    }
    else{
      CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
      xaod->setStore( aux );
      ATH_MSG_DEBUG( "Recorded Vertexes with key: " << m_xaodContainerName );
    }

    // Create the xAOD objects:
    auto itr = aod->begin();
    auto end = aod->end();
    for( ; itr != end; ++itr ) {
      // Create the xAOD object:
      xAOD::Vertex* vertex = new xAOD::Vertex();
      xaod->push_back( vertex );
      // Get & set the Position
      const Amg::Vector3D& position = (*itr)->recVertex().position();
      vertex->setPosition(position);
      // Get & set the Covariance Position
      const AmgSymMatrix(3)& covariance = (*itr)->recVertex().covariancePosition();       
      vertex->setCovariancePosition(covariance);
      // Get & set the Fit quality
      vertex->setFitQuality((*itr)->recVertex().fitQuality().chiSquared(), static_cast<float>((*itr)->recVertex().fitQuality().doubleNumberDoF ())); 
      //Type of the vertex
      vertex->setVertexType((xAOD::VxType::VertexType)(*itr)->vertexType());
      //Set Links
      unsigned int VTAVsize = (*itr)->vxTrackAtVertex()->size();
      for (unsigned int i = 0 ; i < VTAVsize ; ++i)
          {
            Trk::VxTrackAtVertex* VTAV = (*((*itr)->vxTrackAtVertex()))[i];
            Trk::ITrackLink*      trklink = VTAV->trackOrParticleLink();
            Trk::LinkToTrackParticleBase* linkToTrackPB = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink);  
            if (linkToTrackPB)                 {
                 ElementLink<xAOD::TrackParticleContainer> newLink;
                 newLink.resetWithKeyAndIndex( m_TPContainerName, linkToTrackPB->index());
                 //Now set the newlink to the new xAOD vertex
                 vertex->addTrackAtVertex(newLink, VTAV->vtxCompatibility()); 
                } 
            else                {
	      Trk::LinkToXAODNeutralParticle* linkToTrackNP = dynamic_cast<Trk::LinkToXAODNeutralParticle*>(trklink);  
	      if (linkToTrackNP)                 {
		ElementLink<xAOD::NeutralParticleContainer> newLink;
		newLink.resetWithKeyAndIndex( m_NPContainerName, linkToTrackNP->index());
		//Now set the newlink to the new xAOD vertex
		vertex->addNeutralAtVertex(newLink, VTAV->vtxCompatibility()); 
	      } 
	      else {
		ATH_MSG_DEBUG ("Cast of element link failed, skip this VxTrack/NeutralAtVertex !!!!!");
	      }
	    } 
          }
    } 
    // Return gracefully:
    return StatusCode::SUCCESS;
  }//execute
  
} // namespace xAODMaker
