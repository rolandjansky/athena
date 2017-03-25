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
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  SG::ReadHandleKey<HiveDataObj>  m_rdh1;
  SG::ReadHandleKey<EventInfo> m_evt;


  ToolHandle<IHiveTool> m_tool1, m_tool2, m_tool3;
  
};
#endif
