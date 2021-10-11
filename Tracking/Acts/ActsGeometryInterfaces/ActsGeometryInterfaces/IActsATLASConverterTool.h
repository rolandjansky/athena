/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSATLASCONVERTERTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSATLASCONVERTERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include "Acts/EventData/TrackParameters.hpp"

namespace Trk {
  class Surface;
  class Track;
  class MeasurementBase;
}

namespace Acts {
  class Surface;
}

class IActsTrackingGeometryTool;
class ATLASSourceLink;

class IActsATLASConverterTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsATLASConverterTool, 1, 0);

  virtual 
  const Trk::Surface&
  ActsSurfaceToATLAS(const Acts::Surface &actsSurface) const = 0;

  virtual
  const Acts::Surface&
  ATLASSurfaceToActs(const Trk::Surface &atlasSurface) const = 0;

  virtual 
  const ATLASSourceLink
  ATLASMeasurementToSourceLink(const Trk::MeasurementBase *measurement) const = 0;

  virtual
  const std::vector<ATLASSourceLink>
  ATLASTrackToSourceLink(const Trk::Track &track) const = 0;

  virtual
  const Acts::BoundTrackParameters
  ATLASTrackParameterToActs(const Trk::TrackParameters *atlasParameter) const = 0;
  
  virtual
  const Trk::TrackParameters*
  ActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const = 0;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;
};

#endif
