/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSHVCONDALG
#define SCT_DCSCONDITIONSHVCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class SCT_DCSConditionsHVCondAlg : public AthAlgorithm 
{  
 public:
  SCT_DCSConditionsHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DCSConditionsHVCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_DCSFloatCondData> m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;

  BooleanProperty m_returnHVTemp;
};

#endif // SCT_DCSCONDITIONSHVCONDALG
