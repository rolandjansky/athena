// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_SENSORSCONDALG
#define SCT_SENSORSCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_ConditionsData/SCT_SensorsCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class SCT_SensorsCondAlg : public AthReentrantAlgorithm
{  
 public:
  SCT_SensorsCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_SensorsCondAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final;
  virtual bool isReEntrant() const override final { return false; }

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/Sensors", "Key of input (raw) conditions folder"};
  SG::WriteCondHandleKey<SCT_SensorsCondData> m_writeKey{this, "WriteKey", "SCT_SensorsCondData", "Key of output (derived) conditions data"};
};

#endif // SCT_SENSORSCONDALG
