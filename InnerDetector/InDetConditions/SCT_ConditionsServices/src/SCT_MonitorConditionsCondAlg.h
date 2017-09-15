/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_MONITORCONDITIONSCONDALG
#define SCT_MONITORCONDITIONSCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_MonitorConditionsCondData.h"
#include "GaudiKernel/ICondSvc.h"

class SCT_MonitorConditionsCondAlg : public AthAlgorithm 
{  
 public:
  SCT_MonitorConditionsCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_MonitorConditionsCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_MonitorConditionsCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc; 
};

#endif // SCT_MONITORCONDITIONSCONDALG
