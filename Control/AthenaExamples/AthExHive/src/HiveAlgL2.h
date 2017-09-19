/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGL2_H
#define ATHEXHIVE_ALGL2_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "HiveAlgBase.h"

#include <string>

class HiveAlgL2  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgL2 (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgL2();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1{this, "Key_R1", "l1", "read key"};

  SG::UpdateHandleKey<HiveDataObj> m_udh1;

  
};
#endif
