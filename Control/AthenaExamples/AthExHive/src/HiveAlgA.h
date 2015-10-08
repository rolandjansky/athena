/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGA_H
#define ATHEXHIVE_ALGA_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandle.h"
#include "AthExHive/HiveDataObj.h"

#include "EventInfo/EventInfo.h"

#include <string>

/*

This is the HEADER file for the Algorithm SGWrite.
It illustrates how to write a data object into StoreGate

*/

class HiveAlgA  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgA (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgA ();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::ReadHandle<EventInfo> m_evt;

  SG::WriteHandle<HiveDataObj> m_wrh1;
  SG::WriteHandle<HiveDataObj> m_wrh2;

  std::atomic<int> m_i;
  
};
#endif
