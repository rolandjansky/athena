/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGD_H
#define ATHEXHIVE_ALGD_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

/*

This is the HEADER file for the Algorithm SGWrite.
It illustrates how to write a data object into StoreGate

*/

class HiveAlgD  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgD (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgD();

  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandle<HiveDataObj> m_rdh1;
  SG::WriteHandle<HiveDataObj> m_wrh1;

   
  
};
#endif
