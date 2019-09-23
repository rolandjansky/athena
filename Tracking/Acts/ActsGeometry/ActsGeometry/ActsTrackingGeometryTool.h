/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H
#define ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h" // ReadCondHandleKey wants complete type
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"

// ACTS

namespace Acts {
class TrackingGeometry;
}

class ActsGeometryContext;
class IActsTrackingGeometrySvc;


class ActsTrackingGeometryTool : public extends<AthAlgTool, IActsTrackingGeometryTool>
{

public:
  StatusCode initialize() override;

  ActsTrackingGeometryTool(const std::string &type, const std::string &name,
                           const IInterface *parent);

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const override;

  virtual
  const ActsGeometryContext&
  getGeometryContext(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

  virtual
  ActsGeometryContext
  getNominalGeometryContext() const override;

private:
  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  SG::ReadCondHandleKey<ActsGeometryContext> m_rchk{
      this, "ActsAlignmentKey", "ActsAlignment", "cond read key"};
};

#endif
