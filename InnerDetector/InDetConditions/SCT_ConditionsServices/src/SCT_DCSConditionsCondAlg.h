/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSCONDALG
#define SCT_DCSCONDITIONSCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSStatCondData.h"
#include "GaudiKernel/ICondSvc.h"

class SCT_DCSConditionsCondAlg : public AthAlgorithm 
{  
 public:
  SCT_DCSConditionsCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_DCSConditionsCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_DCSStatCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc; 
};

#endif // SCT_DCSCONDITIONSCONDALG
