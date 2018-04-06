/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSTEMPCONDALG
#define SCT_DCSCONDITIONSTEMPCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class SCT_DCSConditionsTempCondAlg : public AthAlgorithm 
{  
 public:
  SCT_DCSConditionsTempCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DCSConditionsTempCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_DCSFloatCondData> m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;

  BooleanProperty m_returnHVTemp;
};

#endif // SCT_DCSCONDITIONSTEMPCONDALG
