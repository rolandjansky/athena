/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_TDAQENABLEDCONDALG
#define SCT_TDAQENABLEDCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "EventInfo/EventInfo.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

class SCT_TdaqEnabledCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_TdaqEnabledCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_TdaqEnabledCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  bool unfilledRun(const EventContext& ctx) const;

  unsigned int parseChannelName(const std::string &chanNameString) const;
  std::string inWords(const unsigned int aNumber) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/TDAQ/Resources/ATLAS/SCT/Robins", "Key of input (raw) conditions folder"};
  SG::WriteCondHandleKey<SCT_TdaqEnabledCondData> m_writeKey{this, "WriteKey", "SCT_TdaqEnabledCondData", "Key of output (derived) conditions data"};
  SG::ReadHandleKey<EventInfo> m_eventInfoKey{this, "EventInfoKey", "ByteStreamEventInfo", "Key of non-xAOD EventInfo"};
  ServiceHandle<ICondSvc> m_condSvc; 
  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

  static const unsigned int s_NRODS;
  static const unsigned int s_modulesPerRod;
  static const unsigned int s_earliestRunForFolder;
  static const unsigned int s_earliestTimeStampForFolder;
};

#endif // SCT_TDAQENABLEDCONDALG
