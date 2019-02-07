/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGA_H
#define ATHEXHIVE_ALGA_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

class HiveAlgA  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgA (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgA ();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::ReadHandleKey<xAOD::EventInfo> m_evt {this,"EvtInfo", "EventInfo", "xAOD EventInfo name"};

  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "a1", "WHK 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh2 {this, "Key_W2", "a2", "WHK 2"};

  std::atomic<int> m_i;
  
};
#endif
