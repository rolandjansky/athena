/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_TestTRT_Alignment_h
#define InDetDetDescrExample_TestTRT_Alignment_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"

//#include "CLHEP/Geometry/Transform3D.h"

#include <vector>

class Identifier;
//class HepGeom::Transform3D;

namespace InDetDD{
  class TRT_DetectorManager;
  class TRT_BaseElement;
}

class IGeoModelSvc;
class TRT_ID;

/////////////////////////////////////////////////////////////////////////////

class TestTRT_Alignment : public AthAlgorithm {
public:
  TestTRT_Alignment (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  /// GeoInit callback
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);
  StatusCode geoInitialize();

private:
  
  void addShiftTop(const Amg::Transform3D & transform); 
  void addShiftModule(int bec, int phiMod, int layer, const Amg::Transform3D & transform);
  void addShift(int level, const Identifier & id, const Amg::Transform3D & transform); 
  void compareStraw(const InDetDD::TRT_BaseElement * element, int straw);
  void compareEndStraws(const Identifier &id);
  void printAlignmentShifts();
  bool testIdentity(const Amg::Transform3D & transform, double errRot=0, double errTrans=0);


  const InDetDD::TRT_DetectorManager * m_manager;
  const TRT_ID * m_idHelper;

  std::string m_managerName;
  bool m_longPrintOut;
  bool m_testAllStraws;
  bool m_testAllElements;
  double m_errRot;
  double m_errTrans;
  bool m_hardwiredShifts;
  int m_precision;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
};

#endif // InDetDetDescrExample_TestTRT_Alignment_h
