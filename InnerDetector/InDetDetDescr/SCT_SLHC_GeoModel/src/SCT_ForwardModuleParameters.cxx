/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <iostream>

namespace InDetDDSLHC {

SCT_ForwardModuleParameters::SCT_ForwardModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    SctFwdModule(),
    SctFwdSensor(),
    SctFwdSpine()
{
  if (sctdb) { // Will be zero for old text file format.
    SctFwdModule  = sctdb->fwdModule();
    SctFwdSensor  = sctdb->fwdSensor();
    SctFwdSpine   = sctdb->fwdSpine();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Forward Modules Parameters =======" 
					    << endreq;
  }
}

//
// Forward Sensor
//
int
SCT_ForwardModuleParameters::fwdSensorNumWafers(int iModuleType) const
{
  return db()->getInt(SctFwdSensor,"NUMWAFERS",iModuleType);
}

double 
SCT_ForwardModuleParameters::moduleInterSidesGap(int iModuleType) const
{
  return db()->getDouble(SctFwdModule,"INTERSIDESGAP",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorThickness(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"THICKNESS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorLength(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"LENGTH",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerWidth(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"INNERWIDTH",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterWidth(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"OUTERWIDTH",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerRadius(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"INNERRADIUS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterRadius(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"OUTERRADIUS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorMiddleRadius(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"MIDDLERADIUS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorDeltaPhi(int iModuleType) const
{
   return db()->getDouble(SctFwdSensor,"DELTAPHI",iModuleType) * CLHEP::mm;
}

std::string 
SCT_ForwardModuleParameters::fwdSensorMaterial(int iModuleType) const
{
  return db()->getString(SctFwdSensor,"MATERIAL",iModuleType);
}

double 
SCT_ForwardModuleParameters::fwdSensorActiveHalfLength(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"ACTIVEHALFLENGTH",iModuleType) * CLHEP::mm;
}  

double 
SCT_ForwardModuleParameters::fwdSensorAngularPitch(int iModuleType) const
{
  return db()->getDouble(SctFwdSensor,"ANGULARPITCH",iModuleType) * CLHEP::radian;
}

int
SCT_ForwardModuleParameters::fwdSensorNumReadoutStrips(int iModuleType) const
{
  return db()->getInt(SctFwdSensor,"NUMREADOUTSTRIPS",iModuleType);
}

double 
SCT_ForwardModuleParameters::fwdModuleStereoAngle(int iModuleType) const
{
  return db()->getDouble(SctFwdModule,"STEREOANGLE",iModuleType) * CLHEP::milliradian;
}

int 
SCT_ForwardModuleParameters::fwdSensorChargeCarrier(int iModuleType) const 
{
  if (!db()->testField(SctFwdSensor, "CARRIER", iModuleType)) return 1;
  int chargeCarrier = db()->getDouble(SctFwdSensor, "CARRIER", iModuleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Fwd Charge Carrier CARRIER mod_type("<<iModuleType<<") = "<<chargeCarrier <<endreq;
  return chargeCarrier;
}

//
// Forward Spine
//
double 
SCT_ForwardModuleParameters::fwdSpineThickness(int iModuleType) const
{
  return db()->getDouble(SctFwdSpine,"THICKNESS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineLength(int iModuleType) const
{
  return db()->getDouble(SctFwdSpine,"LENGTH",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineMiddleRadius(int iModuleType) const
{
  return db()->getDouble(SctFwdSpine,"MIDDLERADIUS",iModuleType) * CLHEP::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineDeltaPhi(int iModuleType) const{
  return db()->getDouble(SctFwdSpine,"DELTAPHI",iModuleType) * CLHEP::radian;
}

std::string 
SCT_ForwardModuleParameters::fwdSpineMaterial(int iModuleType) const
{
  return db()->getString(SctFwdSpine,"MATERIAL",iModuleType);
}

}
