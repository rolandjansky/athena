/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  virtual ~SCT_MonitorConditionsCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_MonitorConditionsCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc; 
};

#endif // SCT_MONITORCONDITIONSCONDALG
