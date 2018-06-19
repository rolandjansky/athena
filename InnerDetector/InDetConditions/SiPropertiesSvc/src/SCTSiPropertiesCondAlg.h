/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCTSIPROPERTIESCONDALG
#define SCTSIPROPERTIESCONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SiPropertiesSvc/SiliconPropertiesVector.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

#include "GaudiKernel/ICondSvc.h"

class SCT_ID;
namespace InDetDD {
  class SiDetectorManager;
}

class SCTSiPropertiesCondAlg : public AthAlgorithm 
{  
 public:
  SCTSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiPropertiesCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  double m_temperatureMin;
  double m_temperatureMax;
  double m_temperatureDefault;
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyTemp;
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyHV;
  SG::WriteCondHandleKey<InDet::SiliconPropertiesVector> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  ToolHandle<ISiliconConditionsTool> m_siCondTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "SiConditionsTool to be used"};
  const SCT_ID* m_pHelper; //!< ID helper for SCT
  const InDetDD::SiDetectorManager* m_detManager;
};

#endif // SCTSIPROPERTIESCONDALG
