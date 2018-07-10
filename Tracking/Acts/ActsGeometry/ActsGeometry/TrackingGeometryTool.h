#ifndef ACTS_TRACKINGGEOMETRYTOOL_H
#define ACTS_TRACKINGGEOMETRYTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/Property.h"

//#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"

namespace Acts {

class ITrackingGeometrySvc;
class TrackingGeometry;

static const InterfaceID IID_ActsTrackingGeometryTool("ActsTrackingGeometryTool", 1, 0);

class TrackingGeometryTool : public AthAlgTool
{

public:
  StatusCode initialize() override;

  TrackingGeometryTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const;

  StatusCode prepareAlignment() const;


private:

  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;

  SG::ReadCondHandleKey<GeoAlignmentStore> m_rchk {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};


};

}


#endif
