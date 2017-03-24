/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_CONDALGY_H
#define CONDALGS_CONDALGY_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthExHive/CondDataObjY.h"
#include "AthExHive/IASCIICondDbSvc.h"

#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ICondSvc.h"

#include <string>

class CondAlgY  :  public AthAlgorithm {
  
public:
    
  CondAlgY (const std::string& name, ISvcLocator* pSvcLocator);
  ~CondAlgY();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::WriteCondHandleKey<CondDataObjY> m_wch1, m_wch2;
  std::string m_dbk1, m_dbk2;

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IASCIICondDbSvc> m_cds;

};

#endif
