/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_LINKMASKINGCONDALG
#define SCT_LINKMASKINGCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

class SCT_LinkMaskingCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_LinkMaskingCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_LinkMaskingCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/purple/pants", "Key of input (raw) bad wafer conditions folder"};
  // This folder can be created by InnerDetector/InDetConditions/SCT_ConditionsTools/python/createLinkMaskingSQLiteFile.py
  SG::WriteCondHandleKey<SCT_ModuleVetoCondData> m_writeKey{this, "WriteKey", "SCT_LinkMaskingCondData", "Key of output (derived) bad wafer conditions data"};

  ServiceHandle<ICondSvc> m_condSvc;
};

#endif // SCT_LINKMASKINGCONDALG
