/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSEXTRAPOLATIONTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSEXTRAPOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

#include "Acts/Propagator/MaterialInteractor.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/EventData/TrackParameters.hpp"

/// Using some short hands for Recorded Material
using ActsRecordedMaterial = Acts::MaterialInteractor::result_type;
/// Finally the output of the propagation test
using ActsPropagationOutput =
        std::pair<std::vector<Acts::detail::Step>, ActsRecordedMaterial>;


class IActsTrackingGeometryTool;


class IActsExtrapolationTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsExtrapolationTool, 1, 0);

  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  std::unique_ptr<const Acts::CurvilinearTrackParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            Acts::NavigationDirection navDir = Acts::forward,
            double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   const Acts::Surface& target,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  std::unique_ptr<const Acts::BoundTrackParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            const Acts::Surface& target,
            Acts::NavigationDirection navDir = Acts::forward,
            double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;

  virtual 
  Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const = 0;
};

#endif
