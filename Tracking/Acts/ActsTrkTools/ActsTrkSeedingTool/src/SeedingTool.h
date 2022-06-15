/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSEEDINGTOOL_SEEDINGTOOL_H
#define ACTSTRKSEEDINGTOOL_SEEDINGTOOL_H 1

// ATHENA
#include "ActsTrkToolInterfaces/ISeedingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

// ACTS CORE
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Seeding/SpacePointGrid.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"
#include "Acts/Seeding/SeedFilterConfig.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Seeding/Seed.hpp"

#include <cmath> //for M_PI

namespace ActsTrk {
  
  class SeedingTool :
    public extends<AthAlgTool, ActsTrk::ISeedingTool> {
    
  public:
    SeedingTool(const std::string& type, const std::string& name,
		    const IInterface* parent);
    virtual ~SeedingTool() = default;
    
    virtual StatusCode initialize() override;
    
    // Interface
    virtual StatusCode
      createSeeds(const EventContext& ctx,
		  const std::vector<const ActsTrk::SpacePoint*>& spContainer,
		  const Acts::Vector3& beamSpotPos,
		  const Acts::Vector3& bField,
		  ActsTrk::SeedContainer& seedContainer ) const override;
    
  private:        
    // metafunction to obtain correct type in iterated container given the iterator type
    template<typename spacepoint_iterator_t>
    struct external_spacepoint {
      using type = typename std::conditional< 
                       std::is_pointer< typename spacepoint_iterator_t::value_type >::value,  
                       typename std::remove_const< typename std::remove_pointer< typename spacepoint_iterator_t::value_type >::type >::type,
                       typename std::remove_const< typename spacepoint_iterator_t::value_type >::type
                       >::type;
    };

    template< typename spacepoint_iterator_t >
      StatusCode
      createSeeds( spacepoint_iterator_t spBegin,
		   spacepoint_iterator_t spEnd,
		   const Acts::Vector3& beamSpotPos,
		   const Acts::Vector3& bField,
       std::vector< Acts::Seed< typename external_spacepoint<spacepoint_iterator_t>::type > >&  seeds) const;
    
    template< typename external_spacepoint_t >
      const std::pair< 
                   Acts::SpacePointGridConfig, 
                   Acts::SeedfinderConfig< external_spacepoint_t > 
      > 
      prepareConfiguration( const Acts::Vector2& beamPos, 
			    const Acts::Vector3& bField ) const;
    
    // *********************************************************************
    // *********************************************************************
  private:

    // Properties to set SpacePointGridConfig
    Gaudi::Property< float > m_minPt {this, "minPt", 900. * Acts::UnitConstants::MeV,
      "lower pT cutoff for seeds"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_cotThetaMax {this, "cotThetaMax", 27.2899,
      "cot of maximum theta angle"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_zMin {this, "zMin", -3000. * Acts::UnitConstants::mm,
      "limiting location of measurements"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_zMax {this, "zMax", 3000. * Acts::UnitConstants::mm,
      "limiting location of measurements"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_deltaRMax {this, "deltaRMax",  280. * Acts::UnitConstants::mm,
      "maximum distance in r between two measurements within one seed"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_impactMax {this, "impactMax", 2. * Acts::UnitConstants::mm,
      "maximum impact parameter"}; // Used in SeedfinderConfig as well
    Gaudi::Property< std::vector< float > > m_zBinEdges {this, "zBinEdges",
      {-3000., -2500., -1400., -925., -450., -250.,  250., 450., 925.,   1400.,  2500.,  3000.},
      "enable non equidistant binning in z"}; // Used in SeedfinderConfig as well
    Gaudi::Property< float > m_gridRMax {this, "gridRMax", 320. * Acts::UnitConstants::mm,
      "radial extension of subdetector to be used in grid building"};
    Gaudi::Property< float > m_gridPhiMin {this, "gridPhiMin", 0.,
      "phi min for space point grid formation"};
    Gaudi::Property< float > m_gridPhiMax {this, "gridPhiMax", 2*M_PI,
      "phi max for space point grid formation"};
    Gaudi::Property< float > m_bFieldInZ {this, "bFieldInZ", 2. * Acts::UnitConstants::T,
      "z component of magnetic field to be used in grid building"};
    Gaudi::Property< int > m_phiBinDeflectionCoverage {this, "phiBinDeflectionCoverage", 3,
      "sets of consecutive phi bins to cover full deflection of minimum pT particle"};

    // Properties to set SeedfinderConfig
    Gaudi::Property< float > m_rMax {this, "rMax", 320. * Acts::UnitConstants::mm,
      "limiting location of measurements"};
    Gaudi::Property< float > m_binSizeR {this, "binSizeR", 2. * Acts::UnitConstants::mm,
      "defining radial bin for space point sorting"};
    Gaudi::Property< bool > m_forceRadialSorting {this, "forceRadialSorting", true,
      "enable radial sorting in space point grid"};
    Gaudi::Property< float > m_deltaRMin {this, "deltaRMin", 20. * Acts::UnitConstants::mm,
      "minimum distance in r between two measurements within one seed"}; // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_deltaRMinTopSP {this, "deltaRMinTopSP", 6. * Acts::UnitConstants::mm,
      "minimum distance in r between middle and top SP"};
    Gaudi::Property< float > m_deltaRMaxTopSP {this, "deltaRMaxTopSP", 280. * Acts::UnitConstants::mm,
      "maximum distance in r between middle and top SP"};
    Gaudi::Property< float > m_deltaRMinBottomSP {this, "deltaRMinBottomSP", 6. * Acts::UnitConstants::mm,
      "minimum distance in r between middle and top SP"};
    Gaudi::Property< float > m_deltaRMaxBottomSP {this, "deltaRMaxBottomSP", 120. * Acts::UnitConstants::mm,
      "maximum distance in r between middle and top SP"};
    Gaudi::Property< float > m_deltaZMax {this, "deltaZMax",  10e6,
      "maximum distance in z between two measurements within one seed"};
    Gaudi::Property< float > m_collisionRegionMin {this, "collisionRegionMin", -200. * Acts::UnitConstants::mm,
      "limiting location of collision region in z"};
    Gaudi::Property< float > m_collisionRegionMax {this, "collisionRegionMax", 200. * Acts::UnitConstants::mm,
      "limiting location of collision region in z"};
    Gaudi::Property< float > m_sigmaScattering {this, "sigmaScattering", 2.,
      "how many sigmas of scattering angle should be considered"};
    Gaudi::Property< float > m_maxPtScattering {this, "maxPtScattering", 10e6,
      "Upper pt limit for scattering calculation"};
    Gaudi::Property< float > m_radLengthPerSeed {this, "radLengthPerSeed", 0.09804522341059585,
      "average radiation lengths of material on the length of a seed. used for scattering"};
    Gaudi::Property< int > m_maxSeedsPerSpM {this, "maxSeedsPerSpM", 4,
      "In dense environments many seeds may be found per middle space point. Only seeds with the highest weight will be kept if this limit is reached."}; // Used in SeedFilterConfig as well
    Gaudi::Property< bool > m_interactionPointCut {this, "interactionPointCut", true,
      "Enable cut on the compatibility between interaction point and SPs"};
    Gaudi::Property< bool > m_arithmeticAverageCotTheta {this, "arithmeticAverageCotTheta", false,
      "Use arithmetic or geometric average in the calculation of the squared error"};
    Gaudi::Property< bool > m_skipPreviousTopSP {this, "skipPreviousTopSP", true,
      "Additional cut to skip top SPs when producing triplets"};
    Gaudi::Property< std::vector< size_t > > m_zBinsCustomLooping {this, "zBinsCustomLooping",
      {1, 2, 3, 4, 11, 10, 9, 8, 6, 5, 7} , "defines order of z bins for looping"};
    Gaudi::Property< bool > m_useVariableMiddleSPRange {this, "useVariableMiddleSPRange", true,
      "Enable variable range to search for middle SPs"};
    Gaudi::Property< std::vector<std::vector<double>> > m_rRangeMiddleSP {this, "rRangeMiddleSP", {}, "radial range for middle SP"};
    Gaudi::Property< float > m_deltaRMiddleMinSPRange {this, "deltaRMiddleMinSPRange", 10.,
      "delta R for middle SP range (min)"};
    Gaudi::Property< float > m_deltaRMiddleMaxSPRange {this, "deltaRMiddleMaxSPRange", 10.,
      "delta R for middle SP range (max)"};
    Gaudi::Property< bool > m_seedConfirmation {this, "seedConfirmation", true,
      "run seed confirmation"};
    Gaudi::Property< float > m_seedConfCentralZMin {this, "seedConfCentralZMin", -250. * Acts::UnitConstants::mm,
      "minimum z for central seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_seedConfCentralZMax {this, "seedConfCentralZMax", 250. * Acts::UnitConstants::mm,
      "maximum z for central seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_seedConfCentralRMax {this, "seedConfCentralRMax", 140. * Acts::UnitConstants::mm,
      "maximum r for central seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< size_t > m_seedConfCentralNTopLargeR {this, "seedConfCentralNTopLargeR", 1,
      "nTop for large R central seed confirmation"};
      // Used in SeedFilterConfig as well
    Gaudi::Property< size_t > m_seedConfCentralNTopSmallR {this, "seedConfCentralNTopSmallR", 2,
      "nTop for small R central seed confirmation"};
      // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_seedConfForwardZMin {this, "seedConfForwardZMin", -3000. * Acts::UnitConstants::mm,
      "minimum z for forward seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_seedConfForwardZMax {this, "seedConfForwardZMax", 3000. * Acts::UnitConstants::mm,
      "maximum z for forward seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< float > m_seedConfForwardRMax {this, "seedConfForwardRMax", 140. * Acts::UnitConstants::mm,
      "maximum r for forward seed confirmation "};
      // Used in SeedFilterConfig as well
    Gaudi::Property< size_t > m_seedConfForwardNTopLargeR {this, "seedConfForwardNTopLargeR", 1,
      "nTop for large R forward seed confirmation"};
      // Used in SeedFilterConfig as well
    Gaudi::Property< size_t > m_seedConfForwardNTopSmallR {this, "seedConfForwardNTopSmallR", 2,
      "nTop for small R forward seed confirmation"};
      // Used in SeedFilterConfig as well
    Gaudi::Property< bool > m_useDetailedDoubleMeasurementInfo {this, "useDetailedDoubleMeasurementInfo", false,
      "enable use of double measurement details"};

    // Properties to set SeedFilterConfig
    Gaudi::Property< float > m_impactWeightFactor {this, "impactWeightFactor", 100.,
      "the impact parameters (d0) is multiplied by this factor and subtracted from weight"};
    Gaudi::Property< float > m_compatSeedWeight {this, "compatSeedWeight", 100.,
      "seed weight increased by this value if a compatible seed has been found"};
    Gaudi::Property< std::size_t > m_compatSeedLimit {this, "compatSeedLimit", 3,
      "how often do you want to increase the weight of a seed for finding a compatible seed"};
    Gaudi::Property< float > m_seedWeightIncrement {this, "seedWeightIncrement", 0.,
      "increment in seed weight if needed"};
    Gaudi::Property< float > m_numSeedIncrement {this, "numSeedIncrement", 10e6,
      "increment in seed weight is applied if the number of compatible seeds is larger than numSeedIncrement"};
    Gaudi::Property< bool > m_curvatureSortingInFilter {this, "curvatureSortingInFilter", true,
      "sort seed vectors by curvature"};
    Gaudi::Property< bool > m_seedConfirmationInFilter {this, "seedConfirmationInFilter", true,
      "run seed confirmation"};
    Gaudi::Property< int > m_maxSeedsPerSpMConf {this, "maxSeedsPerSpMConf", 5,
      "Maximum number of lower quality seeds in seed confirmation."};
    Gaudi::Property< int > m_maxQualitySeedsPerSpMConf {this, "maxQualitySeedsPerSpMConf", 5,
      "Maximum number of quality seeds for each middle-bottom SP-duplet in seed confirmation."};
    Gaudi::Property< bool > m_useDeltaRorTopRadius {this, "useDeltaRorTopRadius", true,
      "use deltaR (top radius - middle radius) instead of top radius"};

    // Properties to set other objects used in
    // seeding algorithm
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsTop{this,
      "zBinNeighborsTop",
      {{0, 0},  {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 1}, {0, 1},  {0, 1}, {0, 1},  {0, 1},  {0, 0}},
      "vector containing the map of z bins in the top layers"};
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsBottom{this, "zBinNeighborsBottom",
      {{0, 1},  {0, 1},  {0, 1},  {0, 1}, {0, 1},  {0, 0},  {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}},
      "vector containing the map of z bins in the top layers"};
    Gaudi::Property< int > m_numPhiNeighbors {this, "numPhiNeighbors", 1,
      "number of phi bin neighbors at each side of the current bin that will be used to search for SPs"};

  };
  
} // namespace

#endif

