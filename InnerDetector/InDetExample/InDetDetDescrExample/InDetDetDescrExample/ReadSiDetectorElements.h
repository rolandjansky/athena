/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_ReadSiDetectorElements_h
#define InDetDetDescrExample_ReadSiDetectorElements_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "SiPropertiesSvc/ISiPropertiesTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetCondServices/ISiLorentzAngleTool.h"

#include <vector>

class Identifier;

namespace InDetDD{
  class SiDetectorManager;
  class SiDetectorElement;
}

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

  void testElement(const Identifier & id, 
                   const std::vector<InDetDD::SiCellId> & cellIdVec,
                   const std::vector<Amg::Vector2D> & positionsVec,
                   const InDetDD::SiDetectorElementCollection* elements=nullptr) const;
  std::string printElementId(const InDetDD::SiDetectorElement * element) const;
  void printAllElements(const bool accessDuringInitialization);
  void printRandomAccess(const bool accessDuringInitialization);

 private:
  // Job properties
  std::string m_managerName;
  bool m_doLoop;
  bool m_doInit;
  bool m_doExec;
  bool m_useConditionsTools;
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Silicon conditions tool"};
  ToolHandle<ISiPropertiesTool> m_siPropertiesTool{this, "SiPropertiesTool", "SiPropertiesTool", "Silicon properties tool"};
  ToolHandle<ISiLorentzAngleTool> m_siLorentzAngleTool{this, "SiLorentzAngleTool", "SiLorentzAngleTool", "Silicon Lorentz anglet tool"};

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_detEleCollKey{this, "DetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection"};

  // Other
  const InDetDD::SiDetectorManager * m_manager;
  const AtlasDetectorID * m_idHelper;
  const PixelID * m_pixelIdHelper;
  const SCT_ID * m_sctIdHelper;

  bool m_first;
  

};

#endif // InDetDetDescrExample_ReadSiDetectorElements_h
