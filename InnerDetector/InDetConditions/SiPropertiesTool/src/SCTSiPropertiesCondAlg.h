/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCTSIPROPERTIESCONDALG
#define SCTSIPROPERTIESCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "SiPropertiesTool/SiliconPropertiesVector.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class SCT_ID;

class SCTSiPropertiesCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCTSiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiPropertiesCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  DoubleProperty m_temperatureMin{this, "TemperatureMin", -80., "Minimum temperature allowed in Celcius."};
  DoubleProperty m_temperatureMax{this, "TemperatureMax", 100., "Maximum temperature allowed in Celcius."};
  DoubleProperty m_temperatureDefault{this, "TemperatureDefault", -7., "Default temperature in Celcius."};
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyTemp{this, "ReadKeyeTemp", "SCT_SiliconTempCondData", "Key of input sensor temperature conditions folder"};
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyHV{this, "ReadKeyHV", "SCT_SiliconBiasVoltCondData", "Key of input bias voltage conditions folder"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<InDet::SiliconPropertiesVector> m_writeKey{this, "WriteKey", "SCTSiliconPropertiesVector", "Key of output silicon properties conditions folder"};
  ServiceHandle<ICondSvc> m_condSvc;
  ToolHandle<ISiliconConditionsTool> m_siCondTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "SiConditionsTool to be used"};
  const SCT_ID* m_pHelper; //!< ID helper for SCT
};

#endif // SCTSIPROPERTIESCONDALG
