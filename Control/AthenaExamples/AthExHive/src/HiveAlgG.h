/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGG_H
#define ATHEXHIVE_ALGG_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgG  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgG (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgG();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "d1", "Read key 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "g1", "Write key 1"};

   
  
};
#endif
