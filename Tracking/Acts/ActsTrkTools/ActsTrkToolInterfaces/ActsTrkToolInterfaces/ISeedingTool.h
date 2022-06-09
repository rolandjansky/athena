/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKTOOLINTERFACES_ISEEDINGTOOL_H
#define ACTSTRKTOOLINTERFACES_ISEEDINGTOOL_H 1

// Athena 
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

// ACTS EDM
#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkEvent/Seed.h"


namespace ActsTrk {
  class ISeedingTool
    : virtual public IAlgTool {
  public:
    DeclareInterfaceID(ISeedingTool, 1, 0);
    
    virtual 
      StatusCode
      createSeeds(const EventContext& ctx,
                  const std::vector<const ActsTrk::SpacePoint*>& spContainer,
                  const Acts::Vector3& beamSpotPos,
                  const Acts::Vector3& bField,
                  ActsTrk::SeedContainer& seedContainer ) const = 0;
  };
  
} // namespace 

#endif 

