/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "src/ActsSeedingFromAthenaAlgorithm.h"

// ACTS
#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"

#include <boost/container/static_vector.hpp>

namespace ActsTrk {
  
  ActsSeedingFromAthenaAlgorithm::ActsSeedingFromAthenaAlgorithm( const std::string &name, 
								  ISvcLocator *pSvcLocator )
    : AthReentrantAlgorithm( name,pSvcLocator ) 
  {}
  
  StatusCode ActsSeedingFromAthenaAlgorithm::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << " ... " );
    
    // Retrieve seed tool
    ATH_CHECK( m_seedsTool.retrieve() );
    
    // Cond
    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );
    
    // Read nd Write handles
    ATH_CHECK( m_spacePointKey.initialize() );
    ATH_CHECK( m_seedKey.initialize() );
    ATH_CHECK( m_actsSpacePointKey.initialize() );
    ATH_CHECK( m_actsSpacePointDataKey.initialize() );
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode ActsSeedingFromAthenaAlgorithm::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Executing " << name() <<" ... " );
    
    // ================================================== //
    // ===================== CONDS ====================== // 
    // ================================================== //
    
    // Read the Beam Spot information
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    ATH_CHECK( beamSpotHandle.isValid() );
    if (beamSpotHandle.cptr() == nullptr) {
      ATH_MSG_ERROR("Retrieved Beam Spot Handle contains a nullptr");
      return StatusCode::FAILURE;
    }
    auto beamSpotData = beamSpotHandle.cptr();
    
    // Read the b-field information
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle { m_fieldCondObjInputKey, ctx };
    ATH_CHECK( readHandle.isValid() );
    
    const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return StatusCode::FAILURE;
    }
    
    // Get the magnetic field
    // Using ACTS classes in order to be sure we are consistent
    Acts::MagneticFieldContext magFieldContext(fieldCondObj);
    
    // ================================================== //
    // ===================== INPUTS ===================== // 
    // ================================================== //

    ATH_MSG_DEBUG( "Retrieving Input Collection '" << m_spacePointKey.key() << "' ..." );
    SG::ReadHandle< ::SpacePointContainer > handle = SG::makeHandle( m_spacePointKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ::SpacePointContainer *spContainer = handle.get();
    ATH_MSG_DEBUG( "    \\__ " << spContainer->size() << " elements from input collection!" );

    // Convert to Acts objects
    std::unique_ptr<ActsTrk::SpacePointContainer> actsSpContainer =
      std::make_unique<ActsTrk::SpacePointContainer>();
    std::unique_ptr<ActsTrk::SpacePointData> actsSpData =
      std::make_unique<ActsTrk::SpacePointData>();

    // Conversion
    // Supported collections are Pixel and SCT clusters
    // The following piece of code converts Athena Clusters to Acts Space Points
    // This is a temporary solution used only for Acts validation purposes
    //
    // Counter here would represent the Acts measurement index
    // In this case it is a simple std::size_t value with no real correspondance 
    // to an index for retrieving an object in a collection
    std::size_t counter = 0;
    ATH_MSG_DEBUG("Retrieved " << spContainer->size() << " space point collections from the container");

    for ( const SpacePointCollection* sp_collection: *spContainer) {
      for ( const Trk::SpacePoint* sp: *sp_collection ) {
	
	const Acts::Vector3 globalPos = sp->globalPosition();
	
	InDet::SiSpacePointForSeed point;
	float r[3] = { 
	  static_cast<float>(globalPos[0]),
	  static_cast<float>(globalPos[1]),
	  static_cast<float>(globalPos[2])
	};
	point.set( sp, r );
	Acts::Vector2 variance(point.covr(), point.covz());

	boost::container::static_vector<std::size_t, 2> indexes({counter++});
	std::unique_ptr<ActsTrk::SpacePoint> toAdd = 
	  std::make_unique<ActsTrk::SpacePoint>( globalPos, 
						 variance, 
						 *actsSpData.get(),
						 indexes );
	
	actsSpContainer->push_back( std::move(toAdd) );    
      }
    }

    ATH_MSG_DEBUG( "    \\__ Total of " << counter << " Space Points" );
      
      // ================================================== // 
    // ===================== OUTPUTS ==================== //
    // ================================================== // 
    
    SG::WriteHandle< ActsTrk::SeedContainer > seedHandle = SG::makeHandle( m_seedKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Seed Container `" << m_seedKey.key() << "` created ..." );
    std::unique_ptr< ActsTrk::SeedContainer > seedPtrs = std::make_unique< ActsTrk::SeedContainer >();

    SG::WriteHandle< ActsTrk::SpacePointContainer > spacePointHandle = SG::makeHandle( m_actsSpacePointKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Space Point Container `" << m_actsSpacePointKey.key() << "` created ..." );

    SG::WriteHandle< ActsTrk::SpacePointData > spacePointDataHandle = SG::makeHandle( m_actsSpacePointDataKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Space Point Data `" << m_actsSpacePointDataKey.key() << "` created ..." );
    
    // ================================================== // 
    // ===================== COMPUTATION ================ //
    // ================================================== // 

    ATH_MSG_DEBUG("Running Seed Finding ...");    
    ATH_CHECK( m_seedsTool->createSeeds( ctx, 
					 *actsSpContainer.get(),
					 *beamSpotData, 
					 magFieldContext,
					 *seedPtrs.get() ) );
    ATH_MSG_DEBUG("    \\__ Created " << seedPtrs->size() << " seeds");
    
    // ================================================== //   
    // ===================== STORE OUTPUT =============== //
    // ================================================== //   
    
    ATH_MSG_DEBUG("Storing Output Collections");
    ATH_CHECK( seedHandle.record( std::move( seedPtrs ) ) );
    ATH_CHECK( spacePointHandle.record( std::move( actsSpContainer ) ) );
    ATH_CHECK( spacePointDataHandle.record( std::move( actsSpData ) ) );

    return StatusCode::SUCCESS;
  }
  
} // namespace
