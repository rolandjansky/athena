/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgE.h
 * @brief Simple Algorithm that reads a HiveDataObj created by HiveAlgB, and
 * one by HiveAlgC, and writes one more.
 */

#ifndef ATHEXHIVE_ALGE_H
#define ATHEXHIVE_ALGE_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class HiveAlgE  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgE (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgE();
  
  // Define the initialize, execute and finalize methods:
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  // By default, this DataHandle is created by HiveAlgC
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "c1", "Read key 1"};
  // By default this DataHandle is created by HiveAlgB
  SG::ReadHandleKey<HiveDataObj>  m_rdh2 {this, "Key_R2", "b1", "Read key 2"};
  
  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "e1", "Write key 1"};
  
};
#endif
