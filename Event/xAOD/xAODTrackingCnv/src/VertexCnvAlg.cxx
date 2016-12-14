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
#include "CxxUtils/make_unique.h"
// Local include(s):
#include "VertexCnvAlg.h"

namespace xAODMaker {

  VertexCnvAlg::VertexCnvAlg( const std::string& name,
    ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
  m_aod("VxPrimaryCandidate"),
  m_xaodout("PrimaryVertices")  
   {

    declareProperty( "AODContainerName",
      m_aod );
    declareProperty( "xAODContainerName",
      m_xaodout );
    declareProperty( "TPContainerName",
      m_TPContainerName = "InDetTrackParticles" );
    declareProperty( "NPContainerName",
      m_NPContainerName = "VertexNeutralParticles" );
  }

  StatusCode VertexCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AODContainerName  = " << m_aod.name() );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodout.name() );
    ATH_MSG_INFO( "TPContainerName = " << m_TPContainerName );
    ATH_MSG_INFO( "NPContainerName = " << m_NPContainerName );

      // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode VertexCnvAlg::execute() {
    // Retrieve the AOD vertexes:

    if (!m_aod.isValid()) {
       ATH_MSG_DEBUG("No VxContainer with key " << m_aod.name() << " found. Do nothing.");
       return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aod.name() );

    // Create the xAOD container and its auxiliary store:

    m_xaodout = CxxUtils::make_unique<xAOD::VertexContainer>();
    m_xauxout = CxxUtils::make_unique<xAOD::VertexAuxContainer>();

    if(!m_xaodout.isValid() || !m_xauxout.isValid()){
       ATH_MSG_ERROR( "Problem creating " << m_xaodout.name() );
    }

    m_xaodout->setStore( *m_xauxout );


    // Create the xAOD objects:
    auto itr = m_aod->cbegin();
    auto end = m_aod->cend();
    m_xaodout->reserve(m_aod->size());
    
    for( ; itr != end; ++itr ) {
      // Create the xAOD object:
      xAOD::Vertex* vertex = new xAOD::Vertex();
      m_xaodout->push_back( vertex );
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
