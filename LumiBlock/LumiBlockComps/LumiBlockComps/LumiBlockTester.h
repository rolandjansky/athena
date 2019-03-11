/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LumiBlockTester
   @brief Algorithm to test luminosity tools
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LumiBlockTester_H
#define LUMIBLOCKCOMPS_LumiBlockTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "LumiBlockComps/ITrigLivefractionTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

class LumiBlockTester: public AthAlgorithm {
 public:
  LumiBlockTester(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode          initialize();
  StatusCode          execute();
  
 private:
  ToolHandle<ILuminosityTool> m_lumiTool;
  ToolHandle<ITrigLivefractionTool> m_liveTool;
  ToolHandle<ILumiBlockMuTool> m_muTool;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};

};

#endif

