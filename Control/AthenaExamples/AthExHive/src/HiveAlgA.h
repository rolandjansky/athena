/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgA.h
 * @brief Simple Algorithm that reads the EventInfo obj, and writes
 * two HiveDataObjs to the store
 */

#ifndef ATHEXHIVE_ALGA_H
#define ATHEXHIVE_ALGA_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

class HiveAlgA  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgA (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgA ();
  
  // Define the initialize, execute and finalize methods:
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  SG::ReadHandleKey<xAOD::EventInfo> m_evt {this,"EvtInfo", "EventInfo", "xAOD EventInfo name"};

  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "a1", "WHK 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh2 {this, "Key_W2", "a2", "WHK 2"};
  
};
#endif
