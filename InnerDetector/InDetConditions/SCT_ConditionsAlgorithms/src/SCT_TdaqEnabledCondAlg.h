// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_TDAQENABLEDCONDALG
#define SCT_TDAQENABLEDCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class SCT_TdaqEnabledCondAlg : public AthReentrantAlgorithm
{  
 public:
  SCT_TdaqEnabledCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_TdaqEnabledCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  bool unfilledRun(const EventContext& ctx) const;

  static unsigned int parseChannelName(const std::string &chanNameString) ;
  static std::string inWords(const unsigned int aNumber) ;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/TDAQ/Resources/ATLAS/SCT/Robins", "Key of input (raw) conditions folder"};
  SG::WriteCondHandleKey<SCT_TdaqEnabledCondData> m_writeKey{this, "WriteKey", "SCT_TdaqEnabledCondData", "Key of output (derived) conditions data"};
  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

  SCT_OnlineId m_onlineId{};

  static const unsigned int s_NRODS;
  static const unsigned int s_modulesPerRod;
  static const unsigned int s_earliestRunForFolder;
  static const unsigned int s_earliestTimeStampForFolder;
};

#endif // SCT_TDAQENABLEDCONDALG
