/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGB_H
#define CONDALGS_ALGB_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class AlgB  :  public AthAlgorithm {
  
public:
    
  AlgB (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgB();
  
  bool isClonable() const override { return true; }

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "a1", "read key 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "a3", "write key 1"};
  
};
#endif
