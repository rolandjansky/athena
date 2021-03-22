/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#include "AthContainers/ConstDataVector.h"
// Local include(s):
#include "GSFTrackParticleMapCnvAlg.h"

namespace xAODMaker {

  GSFTrackParticleMapCnvAlg::GSFTrackParticleMapCnvAlg( const std::string& name,
				  ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ) {

    declareProperty( "AODContainerName", m_aodContainerName = "TrackParticleCandidate" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "InDetTrackParticles" );
    declareProperty( "AODContainerNameGSF", m_aodContainerNameGSF = "GSFTrackParticleCandidate" );
    declareProperty( "xAODContainerNameGSF", m_xaodContainerNameGSF = "GSFTrackParticles" );
    declareProperty( "TrackMapName", m_trackMapName="GSFTrackAssociation",  "Old new Track Map Name" );

  }

  StatusCode GSFTrackParticleMapCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AODContainerNameGSF = " << m_aodContainerNameGSF );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );
    ATH_MSG_INFO( "AODContainerNameGSF = " << m_aodContainerNameGSF );
    ATH_MSG_INFO( "xAODContainerNameGSF = " << m_xaodContainerNameGSF );


    // Return gracefully:
    return StatusCode::SUCCESS;
  }

    /////////////////////////////////////////////////////////////////////////////////////////

  StatusCode GSFTrackParticleMapCnvAlg::execute() {

    // Retrieve the  track particles from AOD
    const Rec::TrackParticleContainer*  aodContainer = evtStore()->tryConstRetrieve<Rec::TrackParticleContainer>(m_aodContainerName );
    if( !aodContainer ) {
      ATH_MSG_WARNING("No AOD TrackParticle container found in TES: StoreGate Key = " <<m_aodContainerName);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );    


    // Retrieve the GSF track particles from AOD
    const Rec::TrackParticleContainer*  aodContainerGSF = evtStore()->tryConstRetrieve<Rec::TrackParticleContainer>(m_aodContainerNameGSF );
    if( !aodContainerGSF ) {
      ATH_MSG_WARNING("No AOD GSFTrackParticle container found in TES: StoreGate Key = " << m_aodContainerNameGSF);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerNameGSF );

    // Retrieve the GSF track particles from xAOD
    const xAOD::TrackParticleContainer*  xaodContainerGSF = evtStore()->tryConstRetrieve<xAOD::TrackParticleContainer>(m_xaodContainerNameGSF );
    if( !xaodContainerGSF ) {
      ATH_MSG_WARNING("No xAOD GSFTrackParticle container found in TES: StoreGate Key = " << m_xaodContainerNameGSF);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_xaodContainerNameGSF );

    // Retrieve the GSF -> Trk associations
    const TrackParticleAssocs *trkassocs = nullptr;
    StatusCode sc=evtStore()->retrieve( trkassocs, m_trackMapName ) ;
   
    if( sc.isFailure()) {
      ATH_MSG_WARNING( "Could not retrieve " << m_trackMapName);
      return StatusCode::SUCCESS;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////
    
    // Loop over the GSF track particles from the AOD and the GSF ones from xAOD
    // assuming they are aligned
    Rec::TrackParticleContainer::const_iterator aod = aodContainerGSF->begin();
    Rec::TrackParticleContainer::const_iterator aodEnd = aodContainerGSF->end();
    
    xAOD::TrackParticleContainer::const_iterator xaod = xaodContainerGSF->begin();
    xAOD::TrackParticleContainer::const_iterator xaodEnd = xaodContainerGSF->end();
    
    for ( ; aod != aodEnd && xaod != xaodEnd; ++aod, ++xaod){
      ConstDataVector<Rec::TrackParticleContainer> associatedElems(SG::VIEW_ELEMENTS);    

      if ( !trkassocs->assocs(*aod, associatedElems) ){
        ATH_MSG_WARNING("Missing link from GSF track particle to original TP");
        continue;
      }
      
      // Old link to AOD container
      ElementLink<Rec::TrackParticleContainer>  oldLink(associatedElems.at(0), *aodContainer);
      
      // New link to xAOD container of non-GSF track particles
      ElementLink<xAOD::TrackParticleContainer> newLink;
      newLink.resetWithKeyAndIndex( m_xaodContainerName, oldLink.index() );
      
      // Set new link as a decoration of the new GSF track particle
      if(newLink.isValid()){
        newLink.toPersistent();
        (*xaod)->auxdecor< ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" ) = newLink;
      }
    }

   // Return gracefully:
  return StatusCode::SUCCESS;
  }

} // namespace xAODMaker

