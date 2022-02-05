/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSINTERFACES_IACTSSEEDINGTOOL_U
#define ACTSINTERFACES_IACTSSEEDINGTOOL_U 1

// Athena 
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

// ACTS EDM
#include "ActsEDM/ActsSpacePoint.h" 
#include "ActsEDM/ActsSeed.h"


namespace ActsTrk {
  class IActsSeedingTool 
    : virtual public IAlgTool {
  public:
    DeclareInterfaceID(IActsSeedingTool, 1, 0);
    
    virtual 
      StatusCode
      createSeeds(const EventContext& ctx,
		  const ActsTrk::SpacePointContainer& spContainer,
		  const InDet::BeamSpotData& beamSpotData,
		  const Acts::MagneticFieldContext& magFieldContext,
		  ActsTrk::SeedContainer& seedContainer ) const = 0;
  };
  
} // namespace 

#endif 
