/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGB_H
#define ATHEXHIVE_ALGB_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthExHive/IHiveExSvc.h"

#include <string>

class HiveAlgB : public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgB (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgB();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  void dump();

  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "b1", "WHK 1"};

  Gaudi::Hive::ContextSpecificData<int> m_di;

};
#endif
