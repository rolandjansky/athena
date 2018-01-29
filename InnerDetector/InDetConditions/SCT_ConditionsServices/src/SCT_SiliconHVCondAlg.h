/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_SILICONHVCONDALG
#define SCT_SILICONHVCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSStatCondData.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h"

#include "GaudiKernel/ICondSvc.h"

class SCT_ID;

class SCT_SiliconHVCondAlg : public AthAlgorithm 
{  
 public:
  SCT_SiliconHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_SiliconHVCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<SCT_DCSStatCondData> m_readKeyState;
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyHV;
  SG::WriteCondHandleKey<SCT_DCSFloatCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<ISCT_DCSConditionsSvc> m_sctDCSSvc;
  const SCT_ID* m_pHelper;//!< ID helper for SCT
};

#endif // SCT_SILICONHVCONDALG
