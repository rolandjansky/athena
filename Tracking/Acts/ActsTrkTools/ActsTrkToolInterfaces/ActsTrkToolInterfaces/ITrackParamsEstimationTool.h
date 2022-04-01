/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKTOOLINTERFACES_ITRACKPARAMESTIMATIONTOOL_H
#define ACTSTRKTOOLINTERFACES_ITRACKPARAMESTIMATIONTOOL_H 1

// Athena 
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "ActsTrkEvent/Seed.h"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/TrackParameters.hpp"

// Others
#include <functional>

namespace ActsTrk {
  class ITrackParamsEstimationTool
    : virtual public IAlgTool {
  public:
    DeclareInterfaceID(ITrackParamsEstimationTool, 1, 0);
    
    virtual 
      std::optional<Acts::BoundTrackParameters>
      estimateTrackParameters(const EventContext& ctx,
			      const ActsTrk::Seed& seed,
			      const Acts::GeometryContext& geoContext,
			      const Acts::MagneticFieldContext& magFieldContext,
			      std::function<const Acts::Surface&(const ActsTrk::Seed&)> retrieveSurface) const = 0;

    virtual
      std::optional<Acts::BoundTrackParameters>
      estimateTrackParameters(const EventContext& ctx,
                              const ActsTrk::Seed& seed,
                              const Acts::GeometryContext& geoContext,
                              const Acts::Surface& surface,
                              const Acts::BoundSymMatrix& covariance,
                              const Acts::Vector3& bField,
                              double bFieldMin) const = 0;
  };
  
} // namespace 

#endif 

