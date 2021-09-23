/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    ATH_MSG_INFO( "Initializing TrackCollectionCnvTool" );
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

  StatusCode TrackCollectionCnvTool::convertAndAugment( const TrackCollection* aod,
					      xAOD::TrackParticleContainer* xaod, const ObservedTracksMap* trk_map ) const {
        
    ATH_MSG_DEBUG( "convertAndAugment: Sizes of containers before conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );
    ATH_MSG_DEBUG( "convertAndAugment: Size of track map: " << trk_map->size() );
    
    TrackCollection::const_iterator itr = aod->begin();
    TrackCollection::const_iterator end = aod->end();
    ObservedTracksMap::const_iterator itrMap = trk_map->begin();

    // Check size of track collection matches size of observed tracks map
    if(aod->size() != trk_map->size()){
      ATH_MSG_ERROR("convertAndAugment: Number of tracks different in collection to convert vs. observed tracks map: "<<aod->size()<<" vs. "<<trk_map->size());
      return StatusCode::FAILURE;
    }

    for( ;itr!=end;++itr ) {
      // Create the xAOD object:
      if (!(*itr)) {
        ATH_MSG_WARNING("convertAndAugment: WTaF? Empty element in container!");
        continue;
      }
      xAOD::TrackParticle* particle = createParticle(*xaod, *aod, **itr);
      if(!particle){
        ATH_MSG_WARNING("convertAndAugment: Failed to create a TrackParticle");
        itrMap++;
        continue;
      }
      // Augment xAOD object with information from track map
			particle->auxdecor<long int>("Id")                = (*itrMap).first;
			particle->auxdecor<double>("score")               = std::get<1>((*itrMap).second);
			particle->auxdecor<int>("rejectStep")             = std::get<2>((*itrMap).second);
			particle->auxdecor<int>("rejectReason")           = std::get<3>((*itrMap).second);
			particle->auxdecor<long int>("parentId")          = std::get<4>((*itrMap).second);
			particle->auxdecor<int>("numPixelHoles")          = std::get<5>((*itrMap).second);
			particle->auxdecor<int>("numSCTHoles")            = std::get<6>((*itrMap).second);
			particle->auxdecor<int>("numSplitSharedPixel")    = std::get<7>((*itrMap).second);
			particle->auxdecor<int>("numSplitSharedSCT")      = std::get<8>((*itrMap).second);
			particle->auxdecor<int>("numSharedOrSplit")       = std::get<9>((*itrMap).second);
			particle->auxdecor<int>("numSharedOrSplitPixels") = std::get<10>((*itrMap).second);
			particle->auxdecor<int>("numShared")              = std::get<11>((*itrMap).second);
			particle->auxdecor<int>("isPatternTrack")         = std::get<12>((*itrMap).second);
			particle->auxdecor<int>("totalSiHits")            = std::get<13>((*itrMap).second);
			particle->auxdecor<int>("inROI")                  = std::get<14>((*itrMap).second);
			particle->auxdecor<int>("thishasblayer")          = std::get<15>((*itrMap).second);
			particle->auxdecor<int>("hassharedblayer")        = std::get<16>((*itrMap).second);
			particle->auxdecor<int>("hassharedpixel")         = std::get<17>((*itrMap).second);
			particle->auxdecor<int>("firstisshared")          = std::get<18>((*itrMap).second);
			particle->auxdecor<int>("numPixelDeadSensor")     = std::get<19>((*itrMap).second);
			particle->auxdecor<int>("numSCTDeadSensor")       = std::get<20>((*itrMap).second);
			particle->auxdecor<int>("numPixelHits")           = std::get<21>((*itrMap).second);
			particle->auxdecor<int>("numSCTHits")             = std::get<22>((*itrMap).second);
			particle->auxdecor<int>("numUnused")              = std::get<23>((*itrMap).second);
			particle->auxdecor<int>("numTRT_Unused")          = std::get<24>((*itrMap).second);
			particle->auxdecor<int>("numSCT_Unused")          = std::get<25>((*itrMap).second);
			particle->auxdecor<int>("numPseudo")              = std::get<26>((*itrMap).second);
			particle->auxdecor<float>("averageSplit1")        = std::get<27>((*itrMap).second);
			particle->auxdecor<float>("averageSplit2")        = std::get<28>((*itrMap).second);
			particle->auxdecor<int>("numWeightedShared")      = std::get<29>((*itrMap).second);
			ATH_MSG_DEBUG("convertAndAugment: Augmenting TrackParticle with id "<<particle->auxdata<long int>("Id")<<" and rejectReason "<<particle->auxdata<int>("rejectReason")<<" (has chi2 = "<<particle->chiSquared()<<")");
      itrMap++;
    }
    ATH_MSG_DEBUG( "convertAndAugment: Sizes of containers after conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );
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

