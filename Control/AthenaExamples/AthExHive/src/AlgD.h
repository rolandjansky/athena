/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGD_H
#define CONDALGS_ALGD_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthExHive/HiveDataObj.h"
#include "AthExHive/CondDataObj.h"

#include <string>

class AlgD  :  public AthAlgorithm {
  
public:
    
  AlgD (const std::string& name, ISvcLocator* pSvcLocator);
  ~AlgD();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  
  SG::ReadCondHandleKey<CondDataObj> m_rch1;
  SG::ReadCondHandleKey<CondDataObj> m_rch2;

};
#endif
