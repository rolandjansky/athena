/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "src/SeedingFromAthenaAlgorithm.h"

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

#include "SiSPSeededTrackFinderData/ITkSiSpacePointForSeed.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

namespace ActsTrk {
  SeedingFromAthenaAlgorithm::SeedingFromAthenaAlgorithm( const std::string &name,
								  ISvcLocator *pSvcLocator )
    : AthReentrantAlgorithm( name,pSvcLocator ) 
  {}
  
  StatusCode SeedingFromAthenaAlgorithm::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << " ... " );
    
    // Retrieve seed tool
    ATH_CHECK( m_seedsTool.retrieve() );
    ATH_CHECK( m_paramEstimationTool.retrieve() );
    ATH_CHECK( m_trackingGeometryTool.retrieve() );
    ATH_CHECK( m_ATLASConverterTool.retrieve() );

    // Cond
    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );
    ATH_CHECK( m_detEleCollKey.initialize() );

    // Read and Write handles
    ATH_CHECK( m_spacePointKey.initialize() );
    ATH_CHECK( m_seedKey.initialize() );
    ATH_CHECK( m_actsTrackParamsKey.initialize() );

    ATH_CHECK( m_pixelClusterContainerKey.initialize(m_usePixel) );
    ATH_CHECK( m_stripClusterContainerKey.initialize(not m_usePixel) );

    return StatusCode::SUCCESS;
  }
  
  StatusCode SeedingFromAthenaAlgorithm::execute(const EventContext& ctx) const {
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

    std::variant < const xAOD::PixelClusterContainer*, const xAOD::StripClusterContainer* > inputClusterContainer;
    if ( m_usePixel ) {
      SG::ReadHandle< xAOD::PixelClusterContainer > pixelClusterHandle = SG::makeHandle( m_pixelClusterContainerKey, ctx );
      ATH_CHECK( pixelClusterHandle.isValid() );
      inputClusterContainer = pixelClusterHandle.get();
    } else {
      SG::ReadHandle<xAOD::StripClusterContainer> stripClusterHandle = SG::makeHandle( m_stripClusterContainerKey, ctx );
      ATH_CHECK( stripClusterHandle.isValid() );
      inputClusterContainer = stripClusterHandle.get();
    }

    SG::ReadCondHandle< InDetDD::SiDetectorElementCollection > detEleHandle( m_detEleCollKey, ctx );
    ATH_CHECK( detEleHandle.isValid() );
    const InDetDD::SiDetectorElementCollection* detEle = detEleHandle.retrieve();
    if ( detEle == nullptr ) {
      ATH_MSG_FATAL( m_detEleCollKey.fullKey() << " is not available." );
      return StatusCode::FAILURE;
    }

    // ================================================== // 
    // ===================== OUTPUTS ==================== //
    // ================================================== // 
    
    SG::WriteHandle< ActsTrk::SeedContainer > seedHandle = SG::makeHandle( m_seedKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Seed Container `" << m_seedKey.key() << "` created ..." );
    std::unique_ptr< ActsTrk::SeedContainer > seedPtrs = std::make_unique< ActsTrk::SeedContainer >();
    
    SG::WriteHandle< ActsTrk::BoundTrackParametersContainer > boundTrackParamsHandle = SG::makeHandle( m_actsTrackParamsKey, ctx );
    ATH_MSG_DEBUG( "    \\__ Track Params Estimated `"<< m_actsTrackParamsKey.key() << "` created ..." );
    std::unique_ptr< ActsTrk::BoundTrackParametersContainer > trackParams = std::make_unique< ActsTrk::BoundTrackParametersContainer >();

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
    // ================ PARAMS ESTIMATION =============== //  
    // ================================================== //   

    ATH_MSG_DEBUG( "Estimating Track Parameters from seed ..." );

    // Estimate Track Parameters
    auto retrieveSurfaceFunction = 
      [this, &inputClusterContainer, &detEle] (const Acts::Seed<ActsTrk::SpacePoint>& seed) -> const Acts::Surface& 
      { 
	std::size_t bottom_idx = seed.sp().front()->measurementIndexes()[0];

	const InDetDD::SiDetectorElement* Element = 
	std::visit([&bottom_idx, &detEle] (const auto& collection) -> const InDetDD::SiDetectorElement* {
	    const auto cluster = collection->at(bottom_idx);
	    const InDetDD::SiDetectorElement* Element = detEle->getDetectorElement(cluster->identifierHash());
	    return Element;
	  }, inputClusterContainer);

	const Trk::Surface& atlas_surface = Element->surface();
	return this->m_ATLASConverterTool->ATLASSurfaceToActs(atlas_surface);
      };

    auto geo_context = m_trackingGeometryTool->getNominalGeometryContext();

    for (const ActsTrk::Seed* seed : *seedPtrs) {
      std::optional<Acts::BoundTrackParameters> optTrackParams =
        m_paramEstimationTool->estimateTrackParameters(ctx,
						       *seed,
						       geo_context.context(),
						       magFieldContext,
						       retrieveSurfaceFunction);

      if ( optTrackParams.has_value() ) {
	Acts::BoundTrackParameters *toAdd = 
	  new Acts::BoundTrackParameters( optTrackParams.value() );
	trackParams->push_back( toAdd );
      }
    }
    
    // ================================================== //   
    // ===================== STORE OUTPUT =============== //
    // ================================================== //   
    
    ATH_MSG_DEBUG("Storing Output Collections");
    ATH_CHECK( seedHandle.record( std::move( seedPtrs ) ) );
    ATH_CHECK( boundTrackParamsHandle.record( std::move( trackParams ) ) );

    return StatusCode::SUCCESS;
  }
  
} // namespace
