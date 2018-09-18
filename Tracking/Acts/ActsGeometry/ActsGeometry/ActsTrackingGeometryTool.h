/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H
#define ACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H

// ATHENA
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// PACKAGE
#include "ActsGeometry/IActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"

namespace Acts {
class TrackingGeometry;
}

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

  SG::ReadCondHandleKey<GeoAlignmentStore> m_rchk {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};


};

#endif
