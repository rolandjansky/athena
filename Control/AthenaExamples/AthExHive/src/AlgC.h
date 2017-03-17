/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGC_H
#define CONDALGS_ALGC_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthExHive/HiveDataObj.h"
#include "AthExHive/CondDataObj.h"

#include <string>

class AlgC  :  public AthAlgorithm {
  
public:
    
  AlgC (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgC();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  
  SG::ReadCondHandleKey<CondDataObj> m_rch;

};
#endif
