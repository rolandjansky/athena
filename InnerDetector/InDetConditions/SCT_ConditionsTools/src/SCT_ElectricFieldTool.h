// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ElectricFieldTool.h
 * @note primary author for Pixels giacinto.piacquadio@cern.ch
 * @author for SCT peter.vankov@cern.ch
 **/


#ifndef SCT_ElectricFieldTool_h
#define SCT_ElectricFieldTool_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ElectricFieldTool.h"

#include "TF1.h"

/**
 * @class Tool to provide electric field in silicon detector at a given depth in the silicon,
 * assuming a given field model (uniform or flat diode).
 **/
class SCT_ElectricFieldTool: public extends<AthAlgTool, ISCT_ElectricFieldTool>
{
 public:
  SCT_ElectricFieldTool(const std::string& t, const std::string& n, const IInterface* p);

  virtual ~SCT_ElectricFieldTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
 
  virtual double getElectricField(double positionZ,
                                  double fluence,
                                  double depletionVoltage,
                                  double sensorThickness,
                                  double biasVoltage) const override;
 private:
  enum FieldModel{UNIFORM_FIELD, FLAT_DIODE};

  IntegerProperty m_eFieldModel{this, "EFieldModel", FLAT_DIODE, "UNIFORM_FIELD=0 uniform E-field model, FLAT_DIODE=1 flat diode model"};
  TF1 m_model{"pol6", "pol6", 0., 0.03};
};

#endif // SCT_ElectricFieldTool_h
