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

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h" // ReadCondHandleKey wants complete type

// ACTS

namespace Acts {
class TrackingGeometry;
}

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

  StatusCode prepareAlignment() const;

private:

  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  SG::ReadCondHandleKey<ActsAlignmentStore> m_rchk {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};


};

#endif
