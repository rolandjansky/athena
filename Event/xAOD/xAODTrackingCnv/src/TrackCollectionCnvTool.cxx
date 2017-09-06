/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
// $Id$
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"

// Local include(s):
#include "TrackCollectionCnvTool.h"

namespace xAODMaker {

  TrackCollectionCnvTool::TrackCollectionCnvTool( const std::string& type, 
					      const std::string& name,
					      const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool")
  {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrackCollectionCnvTool >( this );
    declareProperty( "TrackParticleCreator", m_particleCreator );
  }
  
  StatusCode TrackCollectionCnvTool::initialize() {
    // Greet the user:
    ATH_MSG_INFO( "Initializing TrackCollectionCnvTool - Package version: " << PACKAGE_VERSION );
    ATH_CHECK( m_particleCreator.retrieve() );
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrackCollectionCnvTool::convert( const TrackCollection* aod,
					      xAOD::TrackParticleContainer* xaod ) const {
    
    ATH_MSG_DEBUG( "Sizes of containers before conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );
    
    TrackCollection::const_iterator itr = aod->begin();
    TrackCollection::const_iterator end = aod->end();

    for( ;itr!=end;++itr ) {
      // Create the xAOD object:
      if (!(*itr)) {
        ATH_MSG_WARNING("WTaF? Empty element in container!");
        continue;
      }
      xAOD::TrackParticle* particle = createParticle(*xaod, *aod, **itr);

      if(!particle){
	ATH_MSG_WARNING("Failed to create a TrackParticle");
	continue;
      }      
    }

    ATH_MSG_DEBUG( "Sizes of containers after conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );

    return StatusCode::SUCCESS;    
  }

  xAOD::TrackParticle* TrackCollectionCnvTool::createParticle(xAOD::TrackParticleContainer& xaod,
							      const TrackCollection& container,
							      const Trk::Track& tp) const {
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    ElementLink<TrackCollection> trackLink( &tp, container );
    return m_particleCreator->createParticle( trackLink, &xaod );
    //no!    return m_particleCreator->createParticle( tp, &xaod );
  }

  StatusCode TrackCollectionCnvTool::setParticleCreatorTool(ToolHandle<Trk::ITrackParticleCreatorTool> *tool)
  {
    ATH_MSG_DEBUG( "In setParticleCreatorTool" );
    m_particleCreator = *tool;

    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker

