/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHERRORMONTOOL_H
#define PIXELATHERRORMONTOOL_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsTools/IPixelByteStreamErrorsTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "StoreGate/ReadHandleKey.h"

#include "PixelAthMonitoringBase.h"

class PixelID;
class IPixelByteStreamErrorsSvc;

class PixelAthErrorMonAlg : public PixelAthMonitoringBase {

 public:
  
  PixelAthErrorMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PixelAthErrorMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;

 private:

  ToolHandle<IPixelByteStreamErrorsTool> m_pixelErrorTool{this, "PixelByteStreamErrorsTool", "PixelByteStreamErrorsTool", "Tool for PixelByteStreamErrors"};
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

  const PixelID* m_pixelid;

  bool m_doOnline;
  bool m_doModules;
  bool m_doLumiBlock;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doHeavyIonMon;
};
#endif
