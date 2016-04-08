/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_TestSiAlignment_h
#define InDetDetDescrExample_TestSiAlignment_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "GeoPrimitives/GeoPrimitives.h"
//#include "CLHEP/Geometry/Transform3D.h"

#include <vector>

class Identifier;
//class HepGeom::Transform3D;

namespace InDetDD{
  class SiDetectorManager;
  class SiDetectorElement;
}

class IGeoModelSvc;

/////////////////////////////////////////////////////////////////////////////

class TestSiAlignment : public AthAlgorithm {
public:
  TestSiAlignment (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  /// GeoInit callback
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);
  StatusCode geoInitialize();

private:
  void printAlignmentShifts();
  bool testIdentity(const Amg::Transform3D & transform, double errRot=0, double errTrans=0);
  void extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const; 
  const InDetDD::SiDetectorManager * m_manager;
  std::string m_managerName;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
  bool m_longPrintOut;
  double m_errRot;
  double m_errTrans;
};

#endif // InDetDetDescrExample_TestSiAlignment_h
