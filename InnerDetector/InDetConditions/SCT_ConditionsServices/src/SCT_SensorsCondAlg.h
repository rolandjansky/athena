/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_SENSORSCONDALG
#define SCT_SENSORSCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_SensorsCondData.h"
#include "GaudiKernel/ICondSvc.h"

class SCT_SensorsCondAlg : public AthAlgorithm 
{  
 public:
  SCT_SensorsCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_SensorsCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_SensorsCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc; 
};

#endif // SCT_SENSORSCONDALG
