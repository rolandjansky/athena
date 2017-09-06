/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
// EDM include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "Particle/TrackParticleContainer.h"

// Local include(s):
#include "RecTrackParticleContainerCnvTool.h"



namespace xAODMaker {

  RecTrackParticleContainerCnvTool::RecTrackParticleContainerCnvTool( const std::string& type, 
								      const std::string& name,
								      const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool")
  {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< IRecTrackParticleContainerCnvTool >( this );
    declareProperty( "TrackParticleCreator" , m_particleCreator );
  }
  
  StatusCode RecTrackParticleContainerCnvTool::initialize() {
    // Greet the user:
    ATH_MSG_DEBUG( "Initializing RecTrackParticleContainerCnvTool"  );
    ATH_CHECK( m_particleCreator.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode RecTrackParticleContainerCnvTool::convert( const Rec::TrackParticleContainer* aod,
							xAOD::TrackParticleContainer* xaod ) const {
    
    ATH_MSG_DEBUG( "Sizes of containers before conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );
    Rec::TrackParticleContainer::const_iterator itr = aod->begin();
    Rec::TrackParticleContainer::const_iterator end = aod->end();

        for( ;itr!=end;++itr ) {
      // Create the xAOD object:
      if (!(*itr)) {
        ATH_MSG_WARNING("WTaF? Empty element in container!");
        continue;
      }
      xAOD::TrackParticle* particle = createParticle(xaod, aod, *itr);

      if(!particle){
      	ATH_MSG_WARNING("Failed to create a TrackParticle");
      	continue;
      }      
    }

    ATH_MSG_DEBUG( "Sizes of containers after conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );

    return StatusCode::SUCCESS;    
  }


  xAOD::TrackParticle* RecTrackParticleContainerCnvTool::createParticle( xAOD::TrackParticleContainer* xaod, 
									 const Rec::TrackParticleContainer* /**container*/, 
									 const Rec::TrackParticle* tp) const
  {
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    return m_particleCreator->createParticle( *tp, xaod );
  } // createParticleAndTruth

  StatusCode RecTrackParticleContainerCnvTool::setParticleCreatorTool(ToolHandle<Trk::ITrackParticleCreatorTool> *tool)
  {
    ATH_MSG_DEBUG( "In setParticleCreatorTool" );
    m_particleCreator = *tool;

    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker
