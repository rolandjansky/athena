/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_BarrelModuleParameters_H
#define SCT_GeoModel_SCT_BarrelModuleParameters_H

#include <string>

class SCT_DataBase;

class SCT_BarrelModuleParameters {

public:

  // Constructor 
  SCT_BarrelModuleParameters(SCT_DataBase* rdb);

  // Sensors
  double sensorThickness() const;
  double sensorWidth() const;
  double sensorLength() const;
  int sensorNumWafers() const;
  std::string sensorMaterial() const;
  double sensorDistCenterToCenter() const;
  double sensorStripLength() const; 
  double sensorStripPitch() const; 
  int sensorNumStrips() const;
  int sensorNumReadoutStrips() const;
  int sensorStripShift() const;

  // BaseBoard
  double baseBoardThickness() const;
  double baseBoardWidth() const;
  double baseBoardLength() const;
  std::string baseBoardMaterial() const;
  double baseBoardOffsetY() const;
  double baseBoardOffsetZ() const;
 
  // Hybrid
  double hybridThickness() const;
  double hybridWidth() const;
  double hybridLength() const;
  std::string hybridMaterial() const;
  double hybridOffsetX() const;
  double hybridOffsetZ() const;
  
  // Pigtail
  double pigtailThickness() const;
  double pigtailWidth() const;
  double pigtailLength() const;
  std::string pigtailMaterial() const;

  // Module
  int    moduleStereoUpperSign() const;
  int    moduleUpperSideNumber() const;
  double moduleStereoAngle() const; 
  double moduleSensorToSensorGap() const;
  

private:
  
  SCT_DataBase * m_rdb;

};


#endif // SCT_GeoModel_SCT_BarrelModuleParameters_H
