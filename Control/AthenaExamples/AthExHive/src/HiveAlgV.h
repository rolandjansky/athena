/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgV.h
 * @brief Simple Algorithm that reads an array of HiveDataObjs, and then
 * writes an array of them
 */

#ifndef ATHEXHIVE_ALGF_V
#define ATHEXHIVE_ALGF_V 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class HiveAlgV  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgV (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgV();

  // Define the initialize, execute and finalize methods:
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  
  Gaudi::Property<bool> m_writeFirst {this, "WriteBeforeRead", true,
      "do write before the read" };

  SG::ReadHandleKeyArray<HiveDataObj> m_rhv {
    this, "Key_RV", {"a1","a2","d1","e1","c1"},
    "Array of ReadHandleKey<HiveDataObj>" };

  SG::WriteHandleKeyArray<HiveDataObj> m_whv {
    this, "Key_WV", {}, "Array of WriteHandleKey<HiveDataObj>" };

  // do the actual reading of the ReadHandleArray
  StatusCode read() const;

  // do the actual writing of the ReadHandleArray
  StatusCode write();
   
};
#endif
