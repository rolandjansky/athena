/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_HALGR_H
#define ATHEXHIVE_HALGR_H 1

#include "AthenaBaseComps/AthReEntAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "EventInfo/EventInfo.h"

class HiveAlgR  :  public AthReEntAlgorithm {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgR (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgR ();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute_R(const EventContext&) const;
  StatusCode finalize();
  
private:

  SG::ReadHandleKey<EventInfo> m_evt;

  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  
};
#endif
