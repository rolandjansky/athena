/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "src/SeedingAlgorithm.h"

// ACTS
#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"

#include "TrkSpacePoint/SpacePointCollection.h"

namespace ActsTrk {
  
  SeedingAlgorithm::SeedingAlgorithm( const std::string &name,
					      ISvcLocator *pSvcLocator )
    : AthReentrantAlgorithm( name,pSvcLocator ) 
  {}
  
  StatusCode SeedingAlgorithm::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << " ... " );
    
    // Retrieve seed tool
    ATH_CHECK( m_seedsTool.retrieve() );
    
    // Cond
    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );
    
    // Read nd Write handles
    ATH_CHECK( m_spacePointKey.initialize() );
    ATH_CHECK( m_seedKey.initialize() );
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode SeedingAlgorithm::execute(const EventContext& ctx) const {
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
    // Beam Spot Position
    Acts::Vector3 beamPos( beamSpotData->beamPos().x() * Acts::UnitConstants::mm,
                           beamSpotData->beamPos().y() * Acts::UnitConstants::mm,
                           beamSpotData->beamPos().z() * Acts::UnitConstants::mm);
    
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
    ATLASMagneticFieldWrapper magneticField;
    Acts::MagneticFieldProvider::Cache magFieldCache = magneticField.makeCache( magFieldContext );
    Acts::Vector3 bField = *magneticField.getField( Acts::Vector3(beamPos.x(), beamPos.y(), 0),
                                                    magFieldCache );

    
    // ================================================== //
    // ===================== INPUTS ===================== // 
    // ================================================== //

    ATH_MSG_DEBUG( "Retrieving elements from " << m_spacePointKey.size() << " input collections...");
    std::vector<const ActsTrk::SpacePointContainer *> all_input_collections;
    all_input_collections.reserve(m_spacePointKey.size());

    std::size_t number_input_space_points = 0;
    for (auto& spacePointKey : m_spacePointKey) {
      ATH_MSG_DEBUG( "Retrieving from Input Collection '" << spacePointKey.key() << "' ..." );
      SG::ReadHandle< ActsTrk::SpacePointContainer > handle = SG::makeHandle( spacePointKey, ctx );
      ATH_CHECK( handle.isValid() );
      all_input_collections.push_back(handle.cptr());
      ATH_MSG_DEBUG( "    \\__ " << handle->size() << " elements!");
      number_input_space_points += handle->size();
    }

    // TODO: Write some lines to check which SPs you want to use from the input container
    // At the time being we fill a vector with all SPs available.
    std::vector<const ActsTrk::SpacePoint*> selectedSpacePoints;
    selectedSpacePoints.reserve(number_input_space_points);

    for (const auto* collection : all_input_collections) {
      for (const auto* sp : *collection) {
	selectedSpacePoints.push_back( sp );
      }
    }

    ATH_MSG_DEBUG( "    \\__ Total input space points: " << selectedSpacePoints.size());

    // ================================================== // 
    // ===================== OUTPUTS ==================== //
    // ================================================== // 
    
    SG::WriteHandle< ActsTrk::SeedContainer > seedHandle = SG::makeHandle( m_seedKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Seed Container `" << m_seedKey.key() << "` created ..." );
    std::unique_ptr< ActsTrk::SeedContainer > seedPtrs = std::make_unique< ActsTrk::SeedContainer >();
    
    // ================================================== // 
    // ===================== COMPUTATION ================ //
    // ================================================== // 

    ATH_MSG_DEBUG("Running Seed Finding ...");    
    ATH_CHECK( m_seedsTool->createSeeds( ctx, 
					 selectedSpacePoints,
					 beamPos,
					 bField,
					 *seedPtrs.get() ) );
    ATH_MSG_DEBUG("    \\__ Created " << seedPtrs->size() << " seeds");
    
    // ================================================== //   
    // ===================== STORE OUTPUT =============== //
    // ================================================== //   
    
    ATH_MSG_DEBUG("Storing Seed Collection");
    ATH_CHECK( seedHandle.record( std::move( seedPtrs ) ) );
    
    return StatusCode::SUCCESS;
  }
  
} // namespace
