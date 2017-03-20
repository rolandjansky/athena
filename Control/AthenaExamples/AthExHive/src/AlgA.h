/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGA_H
#define CONDALGS_ALGA_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include "EventInfo/EventInfo.h"

#include <string>

class AlgA  :  public AthAlgorithm {
  
public:
  
  AlgA (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgA ();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh2;
  SG::ReadHandleKey<EventInfo> m_evt;

  std::atomic<int> m_i {1};
  
};
#endif
