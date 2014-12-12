/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_BarrelModuleParametersOld_H
#define SCT_SLHC_GeoModel_SCT_BarrelModuleParametersOld_H

#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"

#include <string>
#include <map>

namespace InDetDDSLHC {

class SCT_BarrelModuleParametersOld : public SCT_BarrelModuleParameters{

public:

  /// default constructor 
  SCT_BarrelModuleParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  ~SCT_BarrelModuleParametersOld();
  
  // Sensors
  double sensorThickness(int moduleType) const;
  double sensorWidth(int moduleType) const;
  double sensorLength(int moduleType) const;
  std::string sensorMaterial(int moduleType) const;

  // BaseBoard
  double baseBoardThickness(int moduleType) const;
  double baseBoardWidth(int moduleType) const;
  double baseBoardLength(int moduleType) const;
  std::string baseBoardMaterial(int moduleType) const;
  double baseBoardOffsetY(int moduleType) const;
  double baseBoardOffsetZ(int moduleType) const;

  // Module
  double moduleStereoAngle(int moduleType) const; 
  double moduleInterSidesGap(int moduleType) const;

  //////////////////////////////////////////////////////////////////////////////
  // Barrel SCT Module Side Design
  //////////////////////////////////////////////////////////////////////////////
  double barrelModelSideStripPitch(int moduleType) const;
  double barrelModelSideStripLength(int moduleType) const;
  double barrelModelSideTotalDeadLength(int moduleType) const;
  int barrelModelSideCrystals(int moduleType) const;
  int barrelModelSideDiodes(int moduleType) const;
  int barrelModelSideCells(int moduleType) const;
  int barrelModelSideShift(int moduleType) const;
  double barrelDeadEdge(int moduleType) const;


private:
  //disallow copy and assignment
  /// assignment
  SCT_BarrelModuleParametersOld & operator=(const SCT_BarrelModuleParametersOld & );
  /// copy constructor
  SCT_BarrelModuleParametersOld(const SCT_BarrelModuleParametersOld & );
  std::map<std::string, float>* m_SCT_Modules;
  std::map<std::string, std::string>* m_SCT_Modules_Strng;
};

}
#endif // SCT_SLHC_GeoModel_SCT_BarrelModuleParametersOld_H
