/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "src/SeedingTool.h"

// ACTS
#include "Acts/Seeding/SeedFilterConfig.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Definitions/Units.hpp"

namespace ActsTrk {
  
  SeedingTool::SeedingTool(const std::string& type,
				   const std::string& name,
				   const IInterface* parent) 
    : base_class(type, name, parent)  
  {}
  
  StatusCode SeedingTool::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    
    ATH_MSG_DEBUG( "Properties Summary:" );
    ATH_MSG_DEBUG( "   " << m_zBinNeighborsTop );
    ATH_MSG_DEBUG( "   " << m_zBinNeighborsBottom );

    ATH_MSG_DEBUG( " * Used by SeedfinderConfig:");
    ATH_MSG_DEBUG( "   " << m_minPt );
    ATH_MSG_DEBUG( "   " << m_cotThetaMax );
    ATH_MSG_DEBUG( "   " << m_deltaRMin );
    ATH_MSG_DEBUG( "   " << m_deltaRMax );
    ATH_MSG_DEBUG( "   " << m_deltaRMinTopSP );
    ATH_MSG_DEBUG( "   " << m_deltaRMaxTopSP );
    ATH_MSG_DEBUG( "   " << m_deltaRMinBottomSP );
    ATH_MSG_DEBUG( "   " << m_deltaRMaxBottomSP );
    ATH_MSG_DEBUG( "   " << m_useVariableMiddleSPRange );
    ATH_MSG_DEBUG( "   " << m_deltaRMiddleSPRange );
    ATH_MSG_DEBUG( "   " << m_seedConfirmation );
    ATH_MSG_DEBUG( "   " << m_enableCutsForSortedSP );
    ATH_MSG_DEBUG( "   " << m_impactMax );
    ATH_MSG_DEBUG( "   " << m_sigmaScattering );    
    ATH_MSG_DEBUG( "   " <<  m_maxPtScattering );
    ATH_MSG_DEBUG( "   " << m_maxSeedsPerSpM );
    ATH_MSG_DEBUG( "   " << m_collisionRegionMin );
    ATH_MSG_DEBUG( "   " << m_collisionRegionMax );
    ATH_MSG_DEBUG( "   " <<  m_phiMin );
    ATH_MSG_DEBUG( "   " <<  m_phiMax );
    ATH_MSG_DEBUG( "   " << m_zMin );
    ATH_MSG_DEBUG( "   " << m_zMax );
    ATH_MSG_DEBUG( "   " << m_rMax );
    ATH_MSG_DEBUG( "   " <<  m_rMin );
    ATH_MSG_DEBUG( "   " << m_radLengthPerSeed );
    ATH_MSG_DEBUG( "   " <<  m_zAlign );
    ATH_MSG_DEBUG( "   " <<  m_rAlign );
    ATH_MSG_DEBUG( "   " <<  m_sigmaError );


    ATH_MSG_DEBUG( " * Used by SeedFilterConfig:");
    ATH_MSG_DEBUG( "   " << m_deltaInvHelixDiameter );
    ATH_MSG_DEBUG( "   " << m_impactWeightFactor);
    ATH_MSG_DEBUG( "   " << m_compatSeedWeight);
    ATH_MSG_DEBUG( "   " << m_deltaRMin );
    ATH_MSG_DEBUG( "   " << m_maxSeedsPerSpM );
    ATH_MSG_DEBUG( "   " << m_compatSeedLimit);

    ATH_MSG_DEBUG( " *  Used by SpacePointGridConfig" );
    ATH_MSG_DEBUG( "   " << m_minPt );
    ATH_MSG_DEBUG( "   " << m_rMax );
    ATH_MSG_DEBUG( "   " << m_zMax );
    ATH_MSG_DEBUG( "   " << m_zMin );
    ATH_MSG_DEBUG( "   " << m_deltaRMax );
    ATH_MSG_DEBUG( "   " << m_cotThetaMax );
    ATH_MSG_DEBUG( "   " << m_impactMax );
    ATH_MSG_DEBUG( "   " << m_numPhiNeighbors );
    ATH_MSG_DEBUG( "   " << m_zBinEdges );

    if (m_zBinEdges.size() - 1 != 
	m_zBinNeighborsTop.size() and
	not m_zBinNeighborsTop.empty()) {
      ATH_MSG_ERROR("Inconsistent config zBinNeighborsTop");
      return StatusCode::FAILURE;
    }

    if (m_zBinEdges.size() - 1 !=
	m_zBinNeighborsBottom.size() and
	not m_zBinNeighborsBottom.empty()) {
      ATH_MSG_ERROR("Inconsistent config zBinNeighborsBottom");
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode
  SeedingTool::createSeeds(const EventContext& /*ctx*/,
                           const ActsTrk::SpacePointContainer& spContainer,
                           const InDet::BeamSpotData& beamSpotData,
                           const Acts::MagneticFieldContext& magFieldContext,
                           ActsTrk::SeedContainer& seedContainer ) const
  {
    // Create Seeds
    auto groupSeeds =
      createSeeds( spContainer.begin(),
		   spContainer.end(),
		   beamSpotData,
		   magFieldContext );

    // Store seeds
    seedContainer.reserve(groupSeeds.size());
    for (unsigned int i(0);i<groupSeeds.size();i++) {
      std::unique_ptr< ActsTrk::Seed > to_add = 
	std::make_unique<ActsTrk::Seed>(std::forward<ActsTrk::Seed>(groupSeeds.at(i)));
      seedContainer.push_back( std::move(to_add) );
    }

    return StatusCode::SUCCESS;
  }


  
  template<
    typename spacepoint_iterator_t,
    typename external_spacepoint_t
    >
  std::vector< Acts::Seed< external_spacepoint_t > >
  SeedingTool::createSeeds( spacepoint_iterator_t spBegin,
                            spacepoint_iterator_t spEnd,
                            const InDet::BeamSpotData& beamSpotData,
                            const Acts::MagneticFieldContext& magFieldContext ) const {
    
    using seed_t = Acts::Seed< external_spacepoint_t >;
    
    static thread_local std::vector< seed_t > seeds;
    seeds.clear();
    if ( spBegin == spEnd )
      return seeds;
    
    // Beam Spot Position
    Acts::Vector2 beamPos( beamSpotData.beamPos()[ Amg::x ] * Acts::UnitConstants::mm,
			   beamSpotData.beamPos()[ Amg::y ] * Acts::UnitConstants::mm );
    
    // Magnetic Field
    ATLASMagneticFieldWrapper magneticField;
    Acts::MagneticFieldProvider::Cache magFieldCache = magneticField.makeCache( magFieldContext );
    Acts::Vector3 bField = *magneticField.getField( Acts::Vector3(beamPos.x(), beamPos.y(), 0),
						    magFieldCache );
    
    auto Configs = prepareConfiguration< external_spacepoint_t >( beamPos,bField );
    auto gridCfg = Configs.first;
    auto finderCfg = Configs.second;
    
    
    auto extractCovariance = [=](const external_spacepoint_t& sp, float, float,
                                 float) -> std::pair<Acts::Vector3, Acts::Vector2> {
      Acts::Vector3 position(sp.x(), sp.y(), sp.z());
      Acts::Vector2 covariance(sp.varianceR(), sp.varianceZ());
      return std::make_pair(position, covariance);
    };

    std::shared_ptr< Acts::BinFinder< external_spacepoint_t > > bottomBinFinder =
      std::make_shared< Acts::BinFinder< external_spacepoint_t > >(m_zBinNeighborsBottom, m_numPhiNeighbors);
    std::shared_ptr< Acts::BinFinder< external_spacepoint_t > > topBinFinder =
      std::make_shared< Acts::BinFinder< external_spacepoint_t > >(m_zBinNeighborsTop, m_numPhiNeighbors);
    
    std::unique_ptr< Acts::SpacePointGrid< external_spacepoint_t > > grid = 
      Acts::SpacePointGridCreator::createGrid< external_spacepoint_t >(gridCfg);
    Acts::BinnedSPGroup< external_spacepoint_t > spacePointsGrouping(spBegin, spEnd, extractCovariance,
								     bottomBinFinder, topBinFinder, std::move(grid), finderCfg);
    
    Acts::Seedfinder< external_spacepoint_t > finder(finderCfg);
    
    static thread_local typename decltype(finder)::State state;

    Acts::Extent rRangeSPExtent;

    for(auto it = spBegin; it!=spEnd; ++it) {
      const auto& sp = *it;
      rRangeSPExtent.check({sp->x(), sp->y(), sp->z()}); 
    }

    auto group = spacePointsGrouping.begin();
    auto groupEnd = spacePointsGrouping.end();
    for (; group != groupEnd; ++group) {
      finder.createSeedsForGroup(state, std::back_inserter(seeds), group.bottom(),
                                 group.middle(), group.top(), rRangeSPExtent);
    }
    
    return seeds;
  }
  
  
  
  template< typename external_spacepoint_t >
  const std::pair< Acts::SpacePointGridConfig,Acts::SeedfinderConfig< external_spacepoint_t > >
  SeedingTool::prepareConfiguration( const Acts::Vector2& beamPos,
                                     const Acts::Vector3& bField ) const {
    
    // Configuration for Acts::SpacePointGrid
    // These values will not be changed during execution
    // B Field will be updated for each event (gridCfg.bFieldInZ)
    Acts::SpacePointGridConfig gridCfg;
    gridCfg.bFieldInZ = bField[2];
    gridCfg.minPt = m_minPt;
    gridCfg.rMax = m_rMax;
    gridCfg.zMax = m_zMax;
    gridCfg.zMin = m_zMin;
    gridCfg.deltaRMax = m_deltaRMax;
    gridCfg.cotThetaMax = m_cotThetaMax;
    gridCfg.impactMax = m_impactMax;
    gridCfg.numPhiNeighbors = m_numPhiNeighbors;
    gridCfg.zBinEdges = m_zBinEdges;

    // Configuration for Acts::Seedfinder
    // These values will not be changed during execution
    // B Field and Beam Spot position will be updated for each event (finderCfg.bFieldInZ and finderCfg.beamPos)
    Acts::SeedFilterConfig filterCfg;
    filterCfg.deltaInvHelixDiameter = m_deltaInvHelixDiameter;
    filterCfg.impactWeightFactor = m_impactWeightFactor;
    filterCfg.compatSeedWeight = m_compatSeedWeight;
    filterCfg.deltaRMin = m_deltaRMin;
    filterCfg.maxSeedsPerSpM = m_maxSeedsPerSpM;
    filterCfg.compatSeedLimit = m_compatSeedLimit;

    Acts::SeedfinderConfig< external_spacepoint_t > finderCfg;
    finderCfg.seedFilter = std::make_unique<Acts::SeedFilter< external_spacepoint_t > >(Acts::SeedFilter< external_spacepoint_t >(filterCfg));
    finderCfg.minPt = m_minPt;
    finderCfg.cotThetaMax = m_cotThetaMax;
    finderCfg.deltaRMin = m_deltaRMin;
    finderCfg.deltaRMax = m_deltaRMax;
    finderCfg.deltaRMaxTopSP = m_deltaRMaxTopSP;
    finderCfg.deltaRMinTopSP = m_deltaRMinTopSP;
    finderCfg.deltaRMaxBottomSP = m_deltaRMaxBottomSP;
    finderCfg.deltaRMinBottomSP = m_deltaRMinBottomSP;
    finderCfg.impactMax = m_impactMax;
    finderCfg.sigmaScattering = m_sigmaScattering;
    finderCfg.maxPtScattering = m_maxPtScattering;
    finderCfg.maxSeedsPerSpM = m_maxSeedsPerSpM;
    finderCfg.collisionRegionMin = m_collisionRegionMin;
    finderCfg.collisionRegionMax = m_collisionRegionMax;
    finderCfg.phiMin = m_phiMin;
    finderCfg.phiMax = m_phiMax;
    finderCfg.zMin = m_zMin;
    finderCfg.zMax = m_zMax;
    finderCfg.rMax = m_rMax;
    finderCfg.rMin = m_rMin;
    finderCfg.bFieldInZ = bField[2];
    finderCfg.beamPos = beamPos;
    finderCfg.radLengthPerSeed = m_radLengthPerSeed;
    finderCfg.zAlign = m_zAlign;
    finderCfg.rAlign = m_rAlign;
    finderCfg.sigmaError = m_sigmaError;

    finderCfg.rRangeMiddleSP.reserve(m_rRangeMiddleSP.size());
    for(const std::vector<double>& inner : m_rRangeMiddleSP) {
      std::vector<float> fInner;
      fInner.reserve(inner.size());
      for(double value : inner) {
        fInner.push_back(static_cast<float>(value));
      }
      finderCfg.rRangeMiddleSP.push_back(std::move(fInner));
    }

    finderCfg.useVariableMiddleSPRange = m_useVariableMiddleSPRange;
    finderCfg.deltaRMiddleSPRange = m_deltaRMiddleSPRange;

    finderCfg.seedConfirmation = m_seedConfirmation;
    finderCfg.centralSeedConfirmationRange.zMinSeedConf = m_seedConfCentralZMin;
    finderCfg.centralSeedConfirmationRange.zMaxSeedConf = m_seedConfCentralZMax;
    finderCfg.centralSeedConfirmationRange.rMaxSeedConf = m_seedConfCentralRMax;
    finderCfg.centralSeedConfirmationRange.nTopSeedConf = m_seedConfCentralNTop;
    finderCfg.centralSeedConfirmationRange.nTopForLargeR = m_seedConfCentralNTopLR;
    finderCfg.centralSeedConfirmationRange.nTopForSmallR = m_seedConfCentralNTopSR;

    finderCfg.forwardSeedConfirmationRange.zMinSeedConf = m_seedConfForwardZMin;
    finderCfg.forwardSeedConfirmationRange.zMaxSeedConf = m_seedConfForwardZMax;
    finderCfg.forwardSeedConfirmationRange.rMaxSeedConf = m_seedConfForwardRMax;
    finderCfg.forwardSeedConfirmationRange.nTopSeedConf = m_seedConfForwardNTop;
    finderCfg.forwardSeedConfirmationRange.nTopForLargeR = m_seedConfForwardNTopLR;
    finderCfg.forwardSeedConfirmationRange.nTopForSmallR = m_seedConfForwardNTopSR;

    finderCfg.zBinEdges = m_zBinEdges;
    
    finderCfg.enableCutsForSortedSP = m_enableCutsForSortedSP;

    return std::make_pair( gridCfg,finderCfg );
  }  

} // namespace ActsTrk
