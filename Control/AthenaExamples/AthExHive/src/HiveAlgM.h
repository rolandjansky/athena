/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGM_H
#define ATHEXHIVE_ALGM_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgM  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgM (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgM();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  Gaudi::Property<int> m_off {this, "Offset", 1, "offset"};
  SG::ReadHandleKey<HiveDataObj> m_rdh1{this, "Key_R1", "a2", "read key 1"};
  SG::ReadHandleKey<HiveDataObj> m_rdh2{this, "Key_R2", "l1", "read key 2"};
  
};
#endif
