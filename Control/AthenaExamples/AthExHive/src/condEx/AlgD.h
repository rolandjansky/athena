/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGD_H
#define CONDALGS_ALGD_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthExHive/HiveDataObj.h"
#include "AthExHive/CondDataObj.h"

#include <string>

class AlgD  :  public AthAlgorithm {
  
public:
    
  AlgD (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AlgD();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1 {this, "Key_R1", "a1", "read key 1"};
  
  SG::ReadCondHandleKey<CondDataObj> m_rch1 {this, "Key_CH1", "X1", "cond read key1"};
  SG::ReadCondHandleKey<CondDataObj> m_rch2 {this, "Key_CH2", "X2", "cond read key2"};

};
#endif
