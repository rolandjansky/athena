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
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

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
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

// forward declarations
namespace MagField {
  class IMagFieldSvc;
}

/**
 * @class SCTSiLorentzAngleCondAlg
 * This class prepares SiLorentAngleCondData using SiPropertiesTool
 **/

class SCTSiLorentzAngleCondAlg: public AthReentrantAlgorithm
{
 public:
  SCTSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiLorentzAngleCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
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
  // YOU NEED TO USE THE SAME PROPERTIES AS USED IN SCTLorentzAngleTool!!!
  DoubleProperty           m_temperature{this, "Temperature", -7., "Default temperature in Celcius."};
  DoubleProperty           m_temperatureMin{this, "TemperatureMin", -80., "Minimum temperature allowed in Celcius."};
  DoubleProperty           m_temperatureMax{this, "TemperatureMax", 100., "Maximum temperature allowed in Celcius."};
  DoubleProperty           m_deplVoltage{this, "DepletionVoltage", 70., "Default depletion voltage in Volt."};
  DoubleProperty           m_biasVoltage{this, "BiasVoltage", 150., "Default bias voltage in Volt."};
  DoubleProperty           m_nominalField{this, "NominalField", 2.0834*Gaudi::Units::tesla};
  BooleanProperty          m_useMagFieldSvc{this, "UseMagFieldSvc", true};
  BooleanProperty          m_useMagFieldDcs{this, "UseMagFieldDcs", true};
  BooleanProperty          m_sctDefaults{this, "useSctDefaults", false};
  BooleanProperty          m_useGeoModel{this, "UseGeoModel", false};
  unsigned int             m_maxHash;

  Amg::Vector3D getMagneticField(const InDetDD::SiDetectorElement* element) const;
};

#endif // SCTSiLorentzAngleCondAlg_h
