/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHHITMONTOOL_H
#define PIXELATHHITMONTOOL_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
//#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadHandleKey.h"


#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetTimeCollection.h"

#include "PixelAthMonitoringBase.h"

class PixelID;
class IPixelCablingSvc;
class PixelRDORawData;

class PixelAthHitMonTool : public PixelAthMonitoringBase {

 public:
  
  PixelAthHitMonTool( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PixelAthHitMonTool();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;

 private:

  ServiceHandle<IPixelCablingSvc> m_pixelCableSvc; //FE info
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

  const PixelID* m_pixelid;

  SG::ReadHandleKey<PixelRDO_Container> m_pixelRDOName{this, "RDOName", "PixelRDOs", "rdo data key"};

  bool m_doOnline;
  bool m_doModules;
  bool m_doLumiBlock;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doHeavyIonMon;
};
#endif
