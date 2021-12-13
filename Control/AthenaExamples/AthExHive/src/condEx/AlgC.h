/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGC_H
#define CONDALGS_ALGC_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthExHive/HiveDataObj.h"
#include "AthExHive/CondDataObj.h"

#include <string>

class AlgC  :  public AthAlgorithm {
  
public:
    
  AlgC (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AlgC();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1 {this, "Key_R1", "a2", "read key 1"};
  
  SG::ReadCondHandleKey<CondDataObj> m_rch {this, "Key_CH", "X1", "cond read key"};

};
#endif
