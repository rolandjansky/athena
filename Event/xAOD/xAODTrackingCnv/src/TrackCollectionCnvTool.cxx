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
    declareProperty("TrackParticleCreator", m_particleCreator, "creator of xAOD::TrackParticles");
  }
  
  StatusCode TrackCollectionCnvTool::initialize() {
    // Greet the user:
    ATH_MSG_INFO( "Initializing TrackCollectionCnvTool with " << m_particleCreator.name() );
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
					      xAOD::TrackParticleContainer* xaod, const ObservedTrackMap* trk_map ) const {
        
    ATH_MSG_DEBUG( "convertAndAugment: Sizes of containers before conversion: aod, xaod: " << aod->size() << ", " << xaod->size() );
    ATH_MSG_DEBUG( "convertAndAugment: Size of track map: " << trk_map->size() );
    
    TrackCollection::const_iterator itr = aod->begin();
    TrackCollection::const_iterator end = aod->end();
    ObservedTrackMap::const_iterator itrMap = trk_map->begin();

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
			particle->auxdecor<double>("score")               = std::get<xAOD::ObserverToolIndex::score>((*itrMap).second);
			particle->auxdecor<int>("rejectStep")             = std::get<xAOD::ObserverToolIndex::rejectStep>((*itrMap).second);
			particle->auxdecor<int>("rejectReason")           = std::get<xAOD::ObserverToolIndex::rejectReason>((*itrMap).second);
			particle->auxdecor<long int>("parentId")          = std::get<xAOD::ObserverToolIndex::parentId>((*itrMap).second);
			particle->auxdecor<int>("numPixelHoles")          = std::get<xAOD::ObserverToolIndex::numPixelHoles>((*itrMap).second);
			particle->auxdecor<int>("numSCTHoles")            = std::get<xAOD::ObserverToolIndex::numSCTHoles>((*itrMap).second);
			particle->auxdecor<int>("numSplitSharedPixel")    = std::get<xAOD::ObserverToolIndex::numSplitSharedPixel>((*itrMap).second);
			particle->auxdecor<int>("numSplitSharedSCT")      = std::get<xAOD::ObserverToolIndex::numSplitSharedSCT>((*itrMap).second);
			particle->auxdecor<int>("numSharedOrSplit")       = std::get<xAOD::ObserverToolIndex::numSharedOrSplit>((*itrMap).second);
			particle->auxdecor<int>("numSharedOrSplitPixels") = std::get<xAOD::ObserverToolIndex::numSharedOrSplitPixels>((*itrMap).second);
			particle->auxdecor<int>("numShared")              = std::get<xAOD::ObserverToolIndex::numShared>((*itrMap).second);
			particle->auxdecor<int>("isPatternTrack")         = std::get<xAOD::ObserverToolIndex::isPatternTrack>((*itrMap).second);
			particle->auxdecor<int>("totalSiHits")            = std::get<xAOD::ObserverToolIndex::totalSiHits>((*itrMap).second);
			particle->auxdecor<int>("inROI")                  = std::get<xAOD::ObserverToolIndex::inROI>((*itrMap).second);
			particle->auxdecor<int>("thishasblayer")          = std::get<xAOD::ObserverToolIndex::hasIBLHit>((*itrMap).second);
			particle->auxdecor<int>("hassharedblayer")        = std::get<xAOD::ObserverToolIndex::hasSharedIBLHit>((*itrMap).second);
			particle->auxdecor<int>("hassharedpixel")         = std::get<xAOD::ObserverToolIndex::hasSharedPixel>((*itrMap).second);
			particle->auxdecor<int>("firstisshared")          = std::get<xAOD::ObserverToolIndex::firstPixIsShared>((*itrMap).second);
			particle->auxdecor<int>("numPixelDeadSensor")     = std::get<xAOD::ObserverToolIndex::numPixelDeadSensor>((*itrMap).second);
			particle->auxdecor<int>("numSCTDeadSensor")       = std::get<xAOD::ObserverToolIndex::numSCTDeadSensor>((*itrMap).second);
			particle->auxdecor<int>("numPixelHits")           = std::get<xAOD::ObserverToolIndex::numPixelHits>((*itrMap).second);
			particle->auxdecor<int>("numSCTHits")             = std::get<xAOD::ObserverToolIndex::numSCTHits>((*itrMap).second);
			particle->auxdecor<int>("numUnused")              = std::get<xAOD::ObserverToolIndex::numUnused>((*itrMap).second);
			particle->auxdecor<int>("numTRT_Unused")          = std::get<xAOD::ObserverToolIndex::numTRT_Unused>((*itrMap).second);
			particle->auxdecor<int>("numSCT_Unused")          = std::get<xAOD::ObserverToolIndex::numSCT_Unused>((*itrMap).second);
			particle->auxdecor<int>("numPseudo")              = std::get<xAOD::ObserverToolIndex::numPseudo>((*itrMap).second);
			particle->auxdecor<float>("averageSplit1")        = std::get<xAOD::ObserverToolIndex::averageSplit1>((*itrMap).second);
			particle->auxdecor<float>("averageSplit2")        = std::get<xAOD::ObserverToolIndex::averageSplit2>((*itrMap).second);
			particle->auxdecor<int>("numWeightedShared")      = std::get<xAOD::ObserverToolIndex::numWeightedShared>((*itrMap).second);
      std::vector<int> v_rejectStep(std::get<xAOD::ObserverToolIndex::rejectStep_full>((*itrMap).second).begin(),std::get<xAOD::ObserverToolIndex::rejectStep_full>((*itrMap).second).end());
      std::vector<int> v_rejectReason(std::get<xAOD::ObserverToolIndex::rejectReason_full>((*itrMap).second).begin(),std::get<xAOD::ObserverToolIndex::rejectReason_full>((*itrMap).second).end());
      particle->auxdecor<std::vector<int>>("rejectStep_full") = v_rejectStep;
      particle->auxdecor<std::vector<int>>("rejectReason_full") = v_rejectReason;
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

