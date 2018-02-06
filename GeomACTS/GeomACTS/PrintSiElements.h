/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeomACTS_PrintSiElements_h
#define GeomACTS_PrintSiElements_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"


#include <fstream>

class IGeoModelSvc;

/////////////////////////////////////////////////////////////////////////////

namespace Acts {
  class TrackingGeometry;
  class ITrackingVolumeBuilder;
  class CylinderVolumeHelper;
  class GeoModelDetectorElement;
}

namespace InDetDD {
  class InDetDetectorManager;
}


class PrintSiElements : public AthAlgorithm {
public:
  PrintSiElements (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode buildTrackingGeometry();

  void writeTrackingGeometry(const Acts::TrackingGeometry& trackingGeometry);

  std::string printTransform(const Amg::Transform3D & trans) const;
  void extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const;

  std::shared_ptr<const Acts::ITrackingVolumeBuilder> 
  makeVolumeBuilder(const InDetDD::InDetDetectorManager* manager, std::shared_ptr<const Acts::CylinderVolumeHelper> cvh, bool toBeamline = false);

private:
  std::shared_ptr<std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>> m_elementStore;
  std::ofstream m_fileout;
  bool m_firstEvent;
  // Alg properties
  bool m_modulesOnly;
  bool m_expandId;
  bool m_nominal;
  bool m_aligned;
  bool m_fullRotationMatrix;
  std::string m_outputFileName;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
};

#endif // GeomACTS_PrintSiElements_h
