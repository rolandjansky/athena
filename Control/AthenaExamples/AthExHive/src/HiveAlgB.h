/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGB_H
#define ATHEXHIVE_ALGB_H 1

#include "HiveAlgBase.h"
#include "StoreGate/WriteHandle.h"
#include "AthExHive/HiveDataObj.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthExHive/IHiveExSvc.h"

#include <string>

/*
  
  This is the HEADER file for the Algorithm SGWrite.
  It illustrates how to write a data object into StoreGate
  
*/

class HiveAlgB : public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgB (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgB();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  void dump();

  SG::WriteHandle<HiveDataObj> m_wrh1;

  Gaudi::Hive::ContextSpecificData<int> m_di;

};
#endif
