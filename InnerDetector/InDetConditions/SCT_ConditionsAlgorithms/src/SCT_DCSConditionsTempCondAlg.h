/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSTEMPCONDALG
#define SCT_DCSCONDITIONSTEMPCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

class SCT_DCSConditionsTempCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_DCSConditionsTempCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DCSConditionsTempCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DCS/MODTEMP", "Key of input (raw) temperature conditions folder"};
  SG::WriteCondHandleKey<SCT_DCSFloatCondData> m_writeKey{this, "WriteKey", "SCT_DCSTemp0CondData", "Key of output (derived) temperature conditions data"};

  ServiceHandle<ICondSvc> m_condSvc;

  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
};

#endif // SCT_DCSCONDITIONSTEMPCONDALG
