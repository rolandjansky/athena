// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_MAJORITYCONDALG
#define SCT_MAJORITYCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_ConditionsData/SCT_MajorityCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class SCT_MajorityCondAlg : public AthReentrantAlgorithm
{  
 public:
  SCT_MajorityCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_MajorityCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DCS/MAJ", "Key of input (raw) conditions folder"};
  SG::WriteCondHandleKey<SCT_MajorityCondData> m_writeKey{this, "WriteKey", "SCT_MajorityCondData", "Key of output (derived) conditions data"};
};

#endif // SCT_MAJORITYCONDALG
