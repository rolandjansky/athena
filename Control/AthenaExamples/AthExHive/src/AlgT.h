/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGT_H
#define CONDALGS_ALGT_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "AthExHive/IHiveTool.h"

#include "EventInfo/EventInfo.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

class AlgT  :  public AthAlgorithm {
  
public:
  
  AlgT (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgT ();
  
  bool isClonable() const override { return true; }

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::ReadHandleKey<EventInfo> m_evt{this, "EvtInfo", "McEventInfo", "EventInfo name"};
  SG::ReadHandleKey<HiveDataObj>  m_rdh1{this,"Key_R1","","read key"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1{this,"Key_W1","t1","write key"};


  ToolHandle<IHiveTool> m_tool1{this, "Tool1", "HiveTool", "tool 1"};
  ToolHandle<IHiveTool> m_tool2{this, "Tool2", "HiveTool", "tool 2"};
  ToolHandle<IHiveTool> m_tool3{this, "Tool3", "HiveTool", "tool 3"};
  
};
#endif
