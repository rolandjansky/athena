/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_BarrelParametersOld_H
#define SCT_SLHC_GeoModel_SCT_BarrelParametersOld_H

#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"

#include <string>
#include <map>
#include <vector>

namespace InDetDDSLHC {

class SCT_BarrelParametersOld : public SCT_BarrelParameters {

public:

  // Constructor 
  SCT_BarrelParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  ~SCT_BarrelParametersOld();
  //disable copy
  SCT_BarrelParametersOld(const SCT_BarrelParametersOld & ) = delete;
  //disable assign
  SCT_BarrelParametersOld & operator=(const SCT_BarrelParametersOld & ) = delete;
  int layoutType() const;
  bool doEndCaps() const;
  // Ski
  int    skiFirstStagger() const;
  double skiRadialSep(int ilayer) const;
  //---int    modulesPerSki(int iLayer) const;
  double skiZPosition(int iLayer, int module) const;
  int    skiModuleIdentifier(int iLayer, int index) const;

  // Layer
  double materialIncreaseFactor(int ilayer) const ;
  double tilt(int iLayer) const;
  int    layerStereoSign(int iLayer) const;
  double radius(int iLayer) const;
  double cylLength(int iLayer) const;
  bool   doubleSided(int iLayer) const;
  int    moduleType(int iLayer) const;
  int    modulesPerSki(int iLayer) const;
  int    skisPerLayer(int iLayer) const;
  //double layerBracketPhiOffset(int iLayer) const;

  // SupportCyl
  double supportCylInnerRadius(int iLayer) const;
  double supportCylOuterRadius(int iLayer) const;
  std::string supportCylMaterial(int iLayer) const;

  // Barrel General
  int    numLayers() const;
  double barrelInnerRadius() const;
  double barrelIntermediateRadius() const;
  double barrelOuterRadius() const;
  double barrelLength() const;
  double barrelIntermediateLength() const;
  double barrelServicesMaterialCylinderLength() const;
  double barrelServicesMaterialIncreaseFactor() const;
  std::string barrelServicesMaterial() const;
 private:
  std::map<std::string, float>* m_SCT_Parameters;
  std::map<std::string, std::string>* m_SCT_Parameters_Strng;
  std::vector<std::vector<float>*>* m_modules_position;
  std::vector<std::vector<int>*>* m_modules_id;
};

}
#endif // SCT_SLHC_GeoModel_SCT_BarrelParametersOld_H
