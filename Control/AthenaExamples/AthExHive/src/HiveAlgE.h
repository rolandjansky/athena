/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGE_H
#define ATHEXHIVE_ALGE_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgE  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgE (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgE();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "c1", "Read key 1"};
  SG::ReadHandleKey<HiveDataObj>  m_rdh2 {this, "Key_R2", "b1", "Read key 2"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "e1", "Write key 1"};
  
};
#endif
