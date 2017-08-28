/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_TDAQENABLEDCONDALG
#define SCT_TDAQENABLEDCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"
#include "StoreGate/ReadHandleKey.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ICondSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

class SCT_TdaqEnabledCondAlg : public AthAlgorithm 
{  
 public:
  SCT_TdaqEnabledCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_TdaqEnabledCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  bool unfilledRun() const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_TdaqEnabledCondData> m_writeKey;
  SG::ReadHandleKey<EventInfo> m_eventInfoKey;
  ServiceHandle<ICondSvc> m_condSvc; 
  ServiceHandle<ISCT_CablingSvc> m_cablingSvc; //!< Handle on SCT cabling service

  unsigned int parseChannelName(const std::string &chanNameString) const;
  std::string inWords(const unsigned int aNumber) const;

  static const unsigned int s_NRODS;
  static const unsigned int s_modulesPerRod;
  static const unsigned int s_earliestRunForFolder;
  static const unsigned int s_earliestTimeStampForFolder;
};

#endif // SCT_TDAQENABLEDCONDALG
