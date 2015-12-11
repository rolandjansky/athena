/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_BarrelModuleParameters_H
#define SCT_SLHC_GeoModel_SCT_BarrelModuleParameters_H

#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"

#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDDSLHC {

class SCT_DataBase;

class SCT_BarrelModuleParameters : public SCT_ParametersBase{

public:

  // Constructor 
  SCT_BarrelModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);

  // Sensors
  virtual double sensorThickness(int moduleType) const;
  virtual double sensorWidth(int moduleType) const;
  virtual double sensorLength(int moduleType) const;
  virtual std::string sensorMaterial(int moduleType) const;
  virtual int chargeCarrier(int moduleType) const;

  // BaseBoard
  virtual double baseBoardThickness(int moduleType) const;
  virtual double baseBoardWidth(int moduleType) const;
  virtual double baseBoardLength(int moduleType) const;
  virtual std::string baseBoardMaterial(int moduleType) const;
  virtual double baseBoardOffsetY(int moduleType) const;
  virtual double baseBoardOffsetZ(int moduleType) const;

  // Module
  virtual double moduleStereoAngle(int moduleType) const; 
  virtual double moduleInterSidesGap(int moduleType) const;

  //////////////////////////////////////////////////////////////////////////////
  // Barrel SCT Module Side Design
  //////////////////////////////////////////////////////////////////////////////
  virtual double barrelModelSideStripPitch(int moduleType) const;
  virtual double barrelModelSideStripLength(int moduleType) const;
  virtual double barrelModelSideTotalDeadLength(int moduleType) const;
  virtual int barrelModelSideSegments(int moduleType) const;
  virtual double barrelModelSideSegmentGap(int moduleType) const;
  virtual int barrelModelSideCrystals(int moduleType) const;
  virtual int barrelModelSideDiodes(int moduleType) const;
  virtual int barrelModelSideCells(int moduleType) const;
  virtual int barrelModelSideShift(int moduleType) const;
  virtual double barrelDeadEdge(int moduleType) const;


private:
  IRDBRecordset_ptr SctBrlSensor;
  IRDBRecordset_ptr SctBrlModule; 

};

}
#endif // SCT_SLHC_GeoModel_SCT_BarrelModuleParameters_H
