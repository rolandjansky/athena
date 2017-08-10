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
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh2;
  
};
#endif
