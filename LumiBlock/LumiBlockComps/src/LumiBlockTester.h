/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LumiBlockTester
   @brief Algorithm to test luminosity tools
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LumiBlockTester_H
#define LUMIBLOCKCOMPS_LumiBlockTester_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockData/TrigLiveFractionCondData.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

class LumiBlockTester: public AthReentrantAlgorithm {
 public:
  LumiBlockTester(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode          initialize() override;
  virtual StatusCode          execute (const EventContext& ctx) const override;


private:
  ToolHandle<ILumiBlockMuTool> m_muTool;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};

  SG::ReadCondHandleKey<LuminosityCondData> m_luminosityCondDataKey
  {this, "LuminosityCondDataKey", "LuminosityCondData", ""};
  SG::ReadCondHandleKey<TrigLiveFractionCondData> m_trigLiveFractionCondDataKey
  {this, "TrigLiveFractionCondDataKey", "TrigLiveFractionCondData", ""};

};

#endif

