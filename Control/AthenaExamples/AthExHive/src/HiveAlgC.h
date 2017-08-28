/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGC_H
#define ATHEXHIVE_ALGC_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "HiveAlgBase.h"

#include <string>

class HiveAlgC  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgC (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgC();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "a1", "read handle key 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1{ this, "Key_W1", "c1", "write handle key 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh2{ this, "Key_W2", "c2", "write handle key 2"};
  
};
#endif
