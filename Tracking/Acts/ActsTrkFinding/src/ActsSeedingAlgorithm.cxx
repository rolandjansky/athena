/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "src/ActsSeedingAlgorithm.h"

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
  
  ActsSeedingAlgorithm::ActsSeedingAlgorithm( const std::string &name, 
					      ISvcLocator *pSvcLocator )
    : AthReentrantAlgorithm( name,pSvcLocator ) 
  {}
  
  StatusCode ActsSeedingAlgorithm::initialize() {
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
  
  StatusCode ActsSeedingAlgorithm::execute(const EventContext& ctx) const {
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
    SG::ReadHandle< ActsTrk::SpacePointContainer > handle = SG::makeHandle( m_spacePointKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ActsTrk::SpacePointContainer *spContainer = handle.get();
    ATH_MSG_DEBUG( "    \\__ " << spContainer->size() << " elements from input collection!" );

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
					 *spContainer,
					 *beamSpotData, 
					 magFieldContext,
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
