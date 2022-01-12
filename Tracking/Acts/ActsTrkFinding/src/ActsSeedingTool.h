/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRACKFINDING_ACTSSEEDINGTOOL_H
#define ACTSTRACKFINDING_ACTSSEEDINGTOOL_H 1 

// ATHENA
#include "ActsTrkFinding/IActsSeedingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS CORE
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Seeding/SpacePointGrid.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"
#include "Acts/Seeding/SeedFilterConfig.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Seeding/Seed.hpp"


namespace ActsTrk {
  
  class ActsSeedingTool :  
    public extends<AthAlgTool, ActsTrk::IActsSeedingTool> {
    
  public:
    ActsSeedingTool(const std::string& type, const std::string& name,
		    const IInterface* parent);
    virtual ~ActsSeedingTool() = default;
    
    virtual StatusCode initialize() override;
    
    // Interface
    virtual StatusCode
      createSeeds(const EventContext& ctx,
		  const ActsTrk::SpacePointContainer& spContainer,
		  const InDet::BeamSpotData& beamSpotData,
		  const Acts::MagneticFieldContext& magFieldContext,
		  ActsTrk::SeedContainer& seedContainer ) const override;
    
  private:    
    template< 
      typename spacepoint_iterator_t,
      typename external_spacepoint_t = typename std::conditional< 
                       std::is_pointer< typename spacepoint_iterator_t::value_type >::value,  
                       typename std::remove_const< typename std::remove_pointer< typename spacepoint_iterator_t::value_type >::type >::type,
                       typename std::remove_const< typename spacepoint_iterator_t::value_type >::type
                       >::type
      >
      std::vector< Acts::Seed< external_spacepoint_t > >
      createSeeds( spacepoint_iterator_t spBegin,
		   spacepoint_iterator_t spEnd,
		   const InDet::BeamSpotData& beamSpotData,
		   const Acts::MagneticFieldContext& magFieldContext ) const;
    


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

    // Properties
    Gaudi::Property< float > m_rMax {this,"rMax",200. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_deltaRMin {this,"deltaRMin",1. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_deltaRMax {this,"deltaRMax",60. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_collisionRegionMin {this,"collisionRegionMin",-250. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_collisionRegionMax {this,"collisionRegionMax",250. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_zMin {this,"zMin",-2000. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_zMax {this,"zMax",2000. * Acts::UnitConstants::mm,""};
    Gaudi::Property< float > m_maxSeedsPerSpM {this,"maxSeedsPerSpM",1,""};
    Gaudi::Property< float > m_cotThetaMax {this,"cotThetaMax",7.40627,""}; // 2.7 eta
    Gaudi::Property< float > m_sigmaScattering {this,"sigmaScattering",5,""};
    Gaudi::Property< float > m_radLengthPerSeed {this,"radLengthPerSeed",0.5,""};
    Gaudi::Property< float > m_minPt {this,"minPt",500. * Acts::UnitConstants::MeV,""};
    Gaudi::Property< float > m_impactMax {this,"impactMax",3. * Acts::UnitConstants::mm,""};
    Gaudi::Property< int > m_numPhiNeighbors {this, "numPhiNeighbors", 1, ""};
    Gaudi::Property< std::vector<float> > m_zBinEdges {this, "zBinEdges", {} , ""};
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsTop{this, "zBinNeighborsTop", {}, ""};
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsBottom{this, "zBinNeighborsBottom", {}, ""};
  };
  
} // namespace

#endif

