/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H
#define ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h" // ReadCondHandleKey wants complete type
#include "ActsGeometry/ActsGeometryContext.h"

// ACTS

namespace Acts {
class TrackingGeometry;
}

class ActsGeometryContext;
class IActsTrackingGeometrySvc;

static const InterfaceID IID_ActsTrackingGeometryTool("ActsTrackingGeometryTool", 1, 0);

class ActsTrackingGeometryTool : public AthAlgTool
{

public:
  StatusCode initialize() override;

  ActsTrackingGeometryTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const;

  const ActsGeometryContext&
  getGeometryContext(const EventContext& ctx = Gaudi::Hive::currentContext()) const;

  ActsGeometryContext
  getNominalGeometryContext() const;


private:

  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  SG::ReadCondHandleKey<ActsGeometryContext> m_rchk {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};


};

#endif
