/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_TestSiAlignment_h
#define InDetDetDescrExample_TestSiAlignment_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>

class Identifier;

namespace InDetDD{
  class SiDetectorManager;
  class SiDetectorElement;
}

/////////////////////////////////////////////////////////////////////////////

class TestSiAlignment : public AthAlgorithm {
public:
  TestSiAlignment (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  void printAlignmentShifts(const bool accessDuringInitialization);
  bool testIdentity(const Amg::Transform3D & transform, double errRot=0, double errTrans=0);
  void extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const; 
  const InDetDD::SiDetectorManager * m_manager;
  std::string m_managerName;
  bool m_longPrintOut;
  double m_errRot;
  double m_errTrans;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_detEleCollKey{this, "DetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection"};
};

#endif // InDetDetDescrExample_TestSiAlignment_h
