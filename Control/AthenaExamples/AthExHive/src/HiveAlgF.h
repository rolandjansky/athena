/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGF_H
#define ATHEXHIVE_ALGF_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthExHive/HiveDataObj.h"
#include "rGen.h"

#include <string>

/*

This is the HEADER file for the Algorithm SGWrite.
It illustrates how to write a data object into StoreGate

*/

class HiveAlgF  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgF (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgF();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandle<HiveDataObj> m_rdh1;
  SG::ReadHandle<HiveDataObj> m_rdh2;
  SG::ReadHandle<HiveDataObj> m_rdh3;

   
  
};
#endif
