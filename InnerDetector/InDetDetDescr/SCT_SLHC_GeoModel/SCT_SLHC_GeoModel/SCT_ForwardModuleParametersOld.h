/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_ForwardModuleParametersOld_H
#define SCT_SLHC_GeoModel_SCT_ForwardModuleParametersOld_H

#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"

#include <string>
#include <map>

namespace InDetDDSLHC {

class SCT_ForwardModuleParametersOld : public SCT_ForwardModuleParameters {

public:

  // Constructor 
  SCT_ForwardModuleParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  ~SCT_ForwardModuleParametersOld();
  //disable assignment
  SCT_ForwardModuleParametersOld & operator=(const SCT_ForwardModuleParametersOld &) = delete;
  //disable copy
  SCT_ForwardModuleParametersOld(const SCT_ForwardModuleParametersOld &) = delete;
  // Sensor
  int    fwdSensorNumWafers(int iModuleType) const;
  double fwdSensorThickness(int iModuleType) const;
  double fwdSensorLength(int iModuleType) const;
  double fwdSensorInnerWidth(int iModuleType) const;
  double fwdSensorOuterWidth(int iModuleType) const;
  double fwdSensorInnerRadius(int iModuleType) const;
  double fwdSensorOuterRadius(int iModuleType) const;
  double fwdSensorMiddleRadius(int iModuleType) const;
  double fwdSensorDeltaPhi(int iModuleType) const;
  std::string fwdSensorMaterial(int iModuleType) const;
  //bool fwdSensorActive(int iModuleType) const;
  double moduleInterSidesGap(int moduleType) const;
  // Sensor 
  // Pitch and readout strips.
  double fwdSensorActiveHalfLength(int iModuleType) const;
  double fwdSensorAngularPitch(int iModuleType) const;
  int    fwdSensorNumReadoutStrips(int iModuleType) const;

  // Spine
  double fwdSpineThickness(int iModuleType) const;
  double fwdSpineLength(int iModuleType) const;
  double fwdSpineMiddleRadius(int iModuleType) const;
  double fwdSpineDeltaPhi(int iModuleType) const;
  std::string fwdSpineMaterial(int iModuleType) const;

  // Module
  double fwdModuleStereoAngle(int iModuleType) const;
  
 private:
  std::map<std::string, float>* m_SCT_Fwd_Modules;
  std::map<std::string, std::string>* m_SCT_Fwd_Modules_Strng;
};
}
#endif //SCT_SLHC_GeoModel_SCT_ForwardModuleParametersOld_H
