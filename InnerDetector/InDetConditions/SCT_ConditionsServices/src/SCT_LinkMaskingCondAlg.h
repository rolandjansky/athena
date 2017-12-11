/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_LINKMASKINGCONDALG
#define SCT_LINKMASKINGCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"

#include "GaudiKernel/ICondSvc.h"

#include "GaudiKernel/Property.h"

class SCT_LinkMaskingCondAlg : public AthAlgorithm 
{  
 public:
  SCT_LinkMaskingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_LinkMaskingCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_ModuleVetoCondData> m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_LINKMASKINGCONDALG
