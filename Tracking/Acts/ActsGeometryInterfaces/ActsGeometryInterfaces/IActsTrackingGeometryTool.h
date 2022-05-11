/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "ActsGeometryInterfaces/ActsGeometryContext.h"

namespace Acts {
  class TrackingGeometry;
}


class IActsTrackingGeometryTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IActsTrackingGeometryTool, 1, 0);

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const = 0;

  virtual
  const ActsGeometryContext&
  getGeometryContext(const EventContext& ctx = Gaudi::Hive::currentContext()) const = 0;

  virtual
  ActsGeometryContext
  getNominalGeometryContext() const = 0;
};

#endif
