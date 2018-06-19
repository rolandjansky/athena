/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTSiLorentzAngleCondAlg.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef SCTSiLorentzAngleCondAlg_h
#define SCTSiLorentzAngleCondAlg_h

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "SiLorentzAngleSvc/SiLorentzAngleCondData.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

// forward declarations
namespace InDetDD {
  class SiDetectorManager;
}  

namespace MagField {
  class IMagFieldSvc;
}

/**
 * @class SCTSiLorentzAngleCondAlg
 * This class prepares SiLorentAngleCondData using SiPropertiesCHSvc
 **/

class SCTSiLorentzAngleCondAlg: public AthAlgorithm
{
 public:
  SCTSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiLorentzAngleCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyTemp;
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyHV;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor;
  SG::WriteCondHandleKey<SiLorentzAngleCondData> m_writeKey;

  // needed services
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};

  const InDetDD::SiDetectorManager* m_detManager;

  // Properties
  double                   m_temperature;
  double                   m_deplVoltage;
  double                   m_biasVoltage;
  double                   m_nominalField;
  bool                     m_useMagFieldSvc;
  bool                     m_useMagFieldDcs;
  bool                     m_sctDefaults;
  bool                     m_useGeoModel;
  unsigned int             m_maxHash;
  double                   m_temperatureMin;
  double                   m_temperatureMax;

  Amg::Vector3D getMagneticField(const IdentifierHash& elementHash) const;
};

#endif // SCTSiLorentzAngleCondAlg_h
