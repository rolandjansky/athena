/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveAlgR.h
 * @brief Re-entrant Algorithm that reads the EventInfo obj, and writes
 * a HiveDataObjs to the store
 */


#ifndef ATHEXHIVE_HIVEALGR_H
#define ATHEXHIVE_HIVEALGR_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "xAODEventInfo/EventInfo.h"

// to make an Algorithm Re-entrant, you must inherit from AthReentrantAlgorithm
class HiveAlgR  :  public AthReentrantAlgorithm {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgR (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgR ();
  
  // Define the initialize, execute and finalize methods: 
  StatusCode initialize();
  StatusCode finalize();

  // Re-entrant version of execute is const and takes an EventContext param
  StatusCode execute(const EventContext&) const;
  
private:

  SG::ReadHandleKey<xAOD::EventInfo> m_evt{this, "EvtInfo", "EventInfo", "event info key"};

  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "ar1", "write handle key"};
  
};
#endif
