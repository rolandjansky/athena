/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_MODULEVETOCONDALG
#define SCT_MODULEVETOCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"

#include "GaudiKernel/ICondSvc.h"

#include "GaudiKernel/Property.h"

class SCT_ModuleVetoCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ModuleVetoCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ModuleVetoCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey;
  SG::WriteCondHandleKey<SCT_ModuleVetoCondData> m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_MODULEVETOCONDALG
