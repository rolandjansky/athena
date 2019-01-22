/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTSiLorentzAngleCondAlg.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef SCTSiLorentzAngleCondAlg_h
#define SCTSiLorentzAngleCondAlg_h

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "SiLorentzAngleSvc/SiLorentzAngleCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// forward declarations
namespace MagField {
  class IMagFieldSvc;
}

/**
 * @class SCTSiLorentzAngleCondAlg
 * This class prepares SiLorentAngleCondData using SiPropertiesTool
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
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyTemp{this, "ReadKeyTemp", "SCT_SiliconTempCondData", "Key of input SCT temperature"};
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_readKeyHV{this, "ReadKeyHV", "SCT_SiliconBiasVoltCondData", "Key of input SCT HV"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor{this, "ReadKeyBFieldSensor", "/EXT/DCS/MAGNETS/SENSORDATA",  "Key of input B-field sensor"};
  // The /GLOBAL/BField/Maps folder is run-lumi folder and has just one IOV. The folder is not used for IOV determination.
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<SiLorentzAngleCondData> m_writeKey{this, "WriteKey", "SCTSiLorentzAngleCondData", "Key of output SiLorentzAngleCondData"};

  // needed services
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};

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

  Amg::Vector3D getMagneticField(const InDetDD::SiDetectorElement* element) const;
};

#endif // SCTSiLorentzAngleCondAlg_h
