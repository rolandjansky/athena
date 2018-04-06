/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  virtual ~SCT_ConditionsParameterCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListVec> m_readKey;
  SG::WriteCondHandleKey<SCT_CondParameterData> m_writeKey;

  ServiceHandle<ISCT_CablingSvc> m_cablingSvc;//!< Handle on SCT cabling service
  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_CONDITIONSPARAMETERCONDALG
