/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGF_V
#define ATHEXHIVE_ALGF_V 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

class HiveAlgV  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgV (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgV();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  bool m_writeFirst { true };

  SG::ReadHandleKeyArray<HiveDataObj> m_rhv {
    this, "Key_RV", {"a1","a2","d1","e1","c1"},
    "Array of ReadHandleKey<HiveDataObj>" };
  SG::WriteHandleKeyArray<HiveDataObj> m_whv {
    this, "Key_WV", {}, "Array of WriteHandleKey<HiveDataObj>" };

  StatusCode read() const;
  void write();
   
};
#endif
