/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSTRACKINGVOLUMEBUILDER_H
#define ACTSGEOMETRYINTERFACES_IACTSTRACKINGVOLUMEBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"

#include "Acts/Geometry/GeometryContext.hpp"

#include <memory>



namespace Acts {
  class TrackingVolume;
  class VolumeBounds;
}


class IActsTrackingVolumeBuilder : virtual public IAlgTool
{
  public:

  DeclareInterfaceID(IActsTrackingVolumeBuilder, 1, 0);

  IActsTrackingVolumeBuilder() {;}

  virtual
  std::shared_ptr<Acts::TrackingVolume>
  trackingVolume(const Acts::GeometryContext& gctx,
                 std::shared_ptr<const Acts::TrackingVolume> insideVolume = nullptr,
                 std::shared_ptr<const Acts::VolumeBounds> outsideBounds = nullptr) const = 0;
};

#endif
