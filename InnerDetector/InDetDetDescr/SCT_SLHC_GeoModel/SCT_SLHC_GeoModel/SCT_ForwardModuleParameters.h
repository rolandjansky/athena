/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_ForwardModuleParameters_H
#define SCT_SLHC_GeoModel_SCT_ForwardModuleParameters_H

#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"

#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDDSLHC {

class SCT_DataBase;

class SCT_ForwardModuleParameters : public SCT_ParametersBase{

public:

  // Constructor 
  SCT_ForwardModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);

  // Sensor
  virtual int    fwdSensorNumWafers(int iModuleType) const;
  virtual double fwdSensorThickness(int iModuleType) const;
  virtual double fwdSensorLength(int iModuleType) const;
  virtual double fwdSensorInnerWidth(int iModuleType) const;
  virtual double fwdSensorOuterWidth(int iModuleType) const;
  virtual double fwdSensorInnerRadius(int iModuleType) const;
  virtual double fwdSensorOuterRadius(int iModuleType) const;
  virtual double fwdSensorMiddleRadius(int iModuleType) const;
  virtual double fwdSensorDeltaPhi(int iModuleType) const;
  virtual std::string fwdSensorMaterial(int iModuleType) const;
  //virtual bool fwdSensorActive(int iModuleType) const;
  virtual double moduleInterSidesGap(int moduleType) const;
  // Sensor 
  // Pitch and readout strips.
  virtual double fwdSensorActiveHalfLength(int iModuleType) const;
  virtual double fwdSensorAngularPitch(int iModuleType) const;
  virtual int    fwdSensorNumReadoutStrips(int iModuleType) const;
  virtual int fwdSensorChargeCarrier(int iModuleType) const;

  // Spine
  virtual double fwdSpineThickness(int iModuleType) const;
  virtual double fwdSpineLength(int iModuleType) const;
  virtual double fwdSpineMiddleRadius(int iModuleType) const;
  virtual double fwdSpineDeltaPhi(int iModuleType) const;
  virtual std::string fwdSpineMaterial(int iModuleType) const;

  // Module
  virtual double fwdModuleStereoAngle(int iModuleType) const;
  
 private:

  IRDBRecordset_ptr SctFwdModule;
  IRDBRecordset_ptr SctFwdSensor;
  IRDBRecordset_ptr SctFwdSpine; 

};
}
#endif //SCT_SLHC_GeoModel_SCT_ForwardModuleParameters_H
