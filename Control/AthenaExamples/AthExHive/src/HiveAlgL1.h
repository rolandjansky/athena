/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGL1_H
#define ATHEXHIVE_ALGL1_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "HiveAlgBase.h"

#include <string>

class HiveAlgL1  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgL1 (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgL1();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1{this, "Key_R1", "a2", "read key"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1{this, "Key_W1", "l1", "write key"};
  
};
#endif
