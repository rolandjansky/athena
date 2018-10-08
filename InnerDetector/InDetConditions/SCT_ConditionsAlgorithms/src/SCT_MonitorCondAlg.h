/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_MONITORCONDALG
#define SCT_MONITORCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_MonitorCondData.h"
#include "GaudiKernel/ICondSvc.h"

class SCT_ID;

class SCT_MonitorCondAlg : public AthAlgorithm 
{  
 public:
  SCT_MonitorCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_MonitorCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/Derived/Monitoring", "Key of input (raw) noisy strip conditions folder"};
  SG::WriteCondHandleKey<SCT_MonitorCondData> m_writeKey{this, "WriteKey", "SCT_MonitorCondData", "Key of output (derived) noisy strip conditions data"};
  const SCT_ID* m_helper;
  ServiceHandle<ICondSvc> m_condSvc; 
};

#endif // SCT_MONITORCONDALG
