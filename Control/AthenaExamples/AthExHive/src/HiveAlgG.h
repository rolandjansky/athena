/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgG.h
 * @brief Simple Algorithm that reads a HiveDataObjs created by HiveAlgD
 * and writes another one
 */

#ifndef ATHEXHIVE_ALGG_H
#define ATHEXHIVE_ALGG_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class HiveAlgG  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgG (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgG();

  // Define the initialize, execute and finalize methods:
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  // By default this DataHandle is created by HiveAlgD
  SG::ReadHandleKey<HiveDataObj>  m_rdh1 {this, "Key_R1", "d1", "Read key 1"};
  
  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "g1", "Write key 1"};
  
};
#endif
