/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_ReadSiDetectorElements_h
#define InDetDetDescrExample_ReadSiDetectorElements_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <vector>

class Identifier;

namespace InDetDD{
  class SiDetectorManager;
  class SiDetectorElement;
}

class IGeoModelSvc;
class ISiLorentzAngleSvc;
class ISiliconConditionsSvc;
class ISiPropertiesSvc;
class AtlasDetectorID;
class AtlasDetectorID;
class PixelID;
class SCT_ID;

/////////////////////////////////////////////////////////////////////////////

class ReadSiDetectorElements : public AthAlgorithm {
public:
  ReadSiDetectorElements (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  /// GeoInit callback
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);
  StatusCode geoInitialize();

  void testElement(const Identifier & id, 
       const std::vector<InDetDD::SiCellId> & cellIdVec, 
       const std::vector<Amg::Vector2D> & positionsVec) const;
  std::string printElementId(const InDetDD::SiDetectorElement * element) const;
  void printAllElements();
  void printRandomAccess();

 private:
  // Job properties
  std::string m_managerName;
  bool m_doLoop;
  bool m_doInit;
  bool m_doExec;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
  ServiceHandle<ISiLorentzAngleSvc> m_siLorentzAngleSvc;
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;

  // Other
  const InDetDD::SiDetectorManager * m_manager;
  const AtlasDetectorID * m_idHelper;
  const PixelID * m_pixelIdHelper;
  const SCT_ID * m_sctIdHelper;

  bool m_first;
  

};

#endif // InDetDetDescrExample_ReadSiDetectorElements_h
