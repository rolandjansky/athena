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

  void extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const;

  std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
  makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline = false);

  Acts::InterpolatedBFieldMap::FieldMapper<3, 3> bfield() const;

private:
  std::shared_ptr<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>> m_elementStore;
  bool m_firstEvent;
  
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  MagField::IMagFieldSvc* m_fieldService;
  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
};

#endif // GeomACTS_ACTSTrackingGeometry_h
