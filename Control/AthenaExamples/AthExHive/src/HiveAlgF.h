/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgF.h
 * @brief Simple Algorithm that reads 6 HiveDataObjs created by HiveAlgA, 
 * HiveAlgB, HiveAlgC, HiveAlgD, and HiveAlgE
 */

#ifndef ATHEXHIVE_ALGF_H
#define ATHEXHIVE_ALGF_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class HiveAlgF  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgF (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgF();

  // Define the initialize, execute and finalize methods:
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1{this, "Key_R1", "c2", "Read key 1"};
  SG::ReadHandleKey<HiveDataObj> m_rdh2{this, "Key_R2", "e1", "Read key 2"};
  SG::ReadHandleKey<HiveDataObj> m_rdh3{this, "Key_R3", "d1", "Read key 3"};
  SG::ReadHandleKey<HiveDataObj> m_rdh4{this, "Key_R4", "C1", "Read key 4"};
  SG::ReadHandleKey<HiveDataObj> m_rdh5{this, "Key_R5", "b1", "Read key 5"};
  SG::ReadHandleKey<HiveDataObj> m_rdh6{this, "Key_R6", "a1", "Read key 6"};
  
};
#endif
