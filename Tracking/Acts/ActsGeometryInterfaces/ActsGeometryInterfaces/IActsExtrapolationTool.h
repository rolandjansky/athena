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

#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {

  class ISurfaceMaterial;
  class IVolumeMaterial;

  using SurfaceMaterialMap
      = std::map<GeometryID, std::shared_ptr<const ISurfaceMaterial>>;

  using VolumeMaterialMap
      = std::map<GeometryID, std::shared_ptr<const IVolumeMaterial>>;

  using DetectorMaterialMaps = std::pair<SurfaceMaterialMap, VolumeMaterialMap>;

}

class IActsTrackingGeometryTool;


class IActsExtrapolationTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsExtrapolationTool, 1, 0);

  virtual
  std::vector<Acts::detail::Step>
  propagate(const EventContext& ctx,
            const Acts::BoundParameters& startParameters,
            double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;
};

#endif
