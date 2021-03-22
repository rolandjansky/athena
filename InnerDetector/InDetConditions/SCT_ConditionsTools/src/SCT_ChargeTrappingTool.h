// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ChargeTrappingTool.h
 * @author Peter Vankov (peter.vankov@cern.ch)
 * @author Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#ifndef SCT_ChargeTrappingTool_h
#define SCT_ChargeTrappingTool_h

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ChargeTrappingTool.h"

#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsTools/ISCT_ElectricFieldTool.h"
#include "StoreGate/ReadCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"

// STL includes
#include <atomic>
#include <string>

//forward declarations
class IdentifierHash;

/**
 * @class SCT_ChargeTrappinTool
 * Concrete class for tool providing quantities related to radiation damage effects
 * for each detector element:
 * 1) Mean Free Path assuming exponential loss of charge - dependent on Fluence
 * 2) Electric Field dependent on Bias voltage and Fluence
 **/

class SCT_ChargeTrappingTool: public extends<AthAlgTool, ISCT_ChargeTrappingTool>
{
 public:
  SCT_ChargeTrappingTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ChargeTrappingTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, double pos, const EventContext& ctx) const override;
  virtual SCT_ChargeTrappingCondData getCondData(const IdentifierHash& elementHash, double pos) const override;
  virtual void getHoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2, const EventContext& ctx) const override;
  virtual void getHoleTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const override;
  
 private:
  SCT_ChargeTrappingCondData calculate(const IdentifierHash& elementHash, double pos, const EventContext& ctx) const;
  double induced(int istrip, double x, double y) const;
  double getPotentialValue(int& ix, int& iy) const;
  void holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const;

 private:
  // Properties
  StringProperty m_detectorName{this, "DetectorName", "SCT", "Detector name"};
  bool m_isSCT{false};

  // Temperature and voltages from job options only used if  SiConditionsServices is None or
  // if value read from database is out of range.
  DoubleProperty m_temperature{this, "Temperature", -2., "Default temperature in Celcius."};
  DoubleProperty m_temperatureMin{this, "TemperatureMin", -80., "Minimum temperature allowed in Celcius."};
  DoubleProperty m_temperatureMax{this, "TemperatureMax", 100., "Maximum temperature allowed in Celcius."};
  DoubleProperty m_deplVoltage{this, "DepletionVoltage", -30., "Default depletion voltage in Volt."};
  DoubleProperty m_biasVoltage{this, "BiasVoltage", 150., "Default  bias voltage in Volt."};

  bool m_conditionsToolValid{false};
  mutable std::atomic_bool m_conditionsToolWarning{false};
 
  // -- Radiation damage specific
  BooleanProperty m_calcHoles{this, "CalcHoles", true, "Default is to consider holes in signal formation."};

  // -- Fluence: Need to make it layer-dependent
  DoubleProperty m_fluence{this, "Fluence", 3.0E13, "Fluence received by the detector."};
  DoubleProperty m_betaElectrons{this, "BetaElectrons", 3.1E-16, "Constant for the trapping model for electrons, in [cm^2/ns] -- average value from Table 2 in ATL-INDET-2003-014"};
  DoubleProperty m_betaHoles{this, "BetaHoles", 5.1E-16, "Constant for the trapping model for holes in [cm^2/ns] -- average value from Table 2 in ATL-INDET-2003-014"};

  double m_PotentialValue[81][115]{{0.}};

  // -- Tool Handle
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "SCT silicon conditions tool"};
  ToolHandle<ISCT_ElectricFieldTool> m_electricFieldTool{this, "SCT_ElectricFieldTool", "SCT_ElectricFieldTool", "SCT electric field tool"};

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;
};

#endif // SCT_ChargeTrappingTool_h
