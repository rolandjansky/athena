/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_CONDITIONSPARAMETERCONDALG
#define SCT_CONDITIONSPARAMETERCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_CondParameterData.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

#include "GaudiKernel/ICondSvc.h"

#include "GaudiKernel/Property.h"

class SCT_ConditionsParameterCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ConditionsParameterCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ConditionsParameterCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<CondAttrListVec> m_readKey;
  SG::WriteCondHandleKey<SCT_CondParameterData> m_writeKey;

  ServiceHandle<ISCT_CablingSvc> m_cablingSvc;//!< Handle on SCT cabling service
  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_CONDITIONSPARAMETERCONDALG
