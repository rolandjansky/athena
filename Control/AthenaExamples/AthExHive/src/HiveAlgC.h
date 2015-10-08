/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGC_H
#define ATHEXHIVE_ALGC_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthExHive/HiveDataObj.h"
#include "HiveAlgBase.h"

#include <string>

/*

This is the HEADER file for the Algorithm SGWrite.
It illustrates how to write a data object into StoreGate

*/

class HiveAlgC  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgC (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgC();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandle<HiveDataObj> m_rdh1;
  SG::WriteHandle<HiveDataObj> m_wrh1;
  SG::WriteHandle<HiveDataObj> m_wrh2;
  
};
#endif
