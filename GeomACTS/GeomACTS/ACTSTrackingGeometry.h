/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeomACTS_ACTSTrackingGeometry_h
#define GeomACTS_ACTSTrackingGeometry_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "MagFieldInterfaces/IMagFieldSvc.h"


#include <fstream>

class IGeoModelSvc;

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingVolumeBuilder;
  class CylinderVolumeHelper;
  class GeoModelDetectorElement;
  class ITrackingGeometrySvc;
  class IExtrapolationTool;
}

namespace InDetDD {
  class InDetDetectorManager;
}


class ACTSTrackingGeometry : public AthAlgorithm {
public:
  ACTSTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode buildTrackingGeometry();

  void writeTrackingGeometry(const Acts::TrackingGeometry& trackingGeometry);

private:
  bool m_firstEvent;
  
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  MagField::IMagFieldSvc* m_fieldService;
  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;


  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};
};

#endif // GeomACTS_ACTSTrackingGeometry_h
