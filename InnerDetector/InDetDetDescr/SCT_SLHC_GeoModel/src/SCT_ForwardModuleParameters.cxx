/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <iostream>

namespace InDetDDSLHC {

SCT_ForwardModuleParameters::SCT_ForwardModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    m_SctFwdModule(),
    m_SctFwdSensor(),
    m_SctFwdSpine()
{
  if (sctdb) { // Will be zero for old text file format.
    m_SctFwdModule  = sctdb->fwdModule();
    m_SctFwdSensor  = sctdb->fwdSensor();
    m_SctFwdSpine   = sctdb->fwdSpine();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Forward Modules Parameters =======" 
					    << endmsg;
  }
}

//
// Forward Sensor
//
int
SCT_ForwardModuleParameters::fwdSensorNumWafers(int iModuleType) const
{
  return db()->getInt(m_SctFwdSensor,"NUMWAFERS",iModuleType);
}

double 
SCT_ForwardModuleParameters::moduleInterSidesGap(int iModuleType) const
{
  return db()->getDouble(m_SctFwdModule,"INTERSIDESGAP",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorThickness(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"THICKNESS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorLength(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"LENGTH",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerWidth(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"INNERWIDTH",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterWidth(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"OUTERWIDTH",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerRadius(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"INNERRADIUS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterRadius(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"OUTERRADIUS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorMiddleRadius(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"MIDDLERADIUS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorDeltaPhi(int iModuleType) const
{
   return db()->getDouble(m_SctFwdSensor,"DELTAPHI",iModuleType) * Gaudi::Units::mm;
}

std::string 
SCT_ForwardModuleParameters::fwdSensorMaterial(int iModuleType) const
{
  return db()->getString(m_SctFwdSensor,"MATERIAL",iModuleType);
}

double 
SCT_ForwardModuleParameters::fwdSensorActiveHalfLength(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"ACTIVEHALFLENGTH",iModuleType) * Gaudi::Units::mm;
}  

double 
SCT_ForwardModuleParameters::fwdSensorAngularPitch(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSensor,"ANGULARPITCH",iModuleType) * Gaudi::Units::radian;
}

int
SCT_ForwardModuleParameters::fwdSensorNumReadoutStrips(int iModuleType) const
{
  return db()->getInt(m_SctFwdSensor,"NUMREADOUTSTRIPS",iModuleType);
}

double 
SCT_ForwardModuleParameters::fwdModuleStereoAngle(int iModuleType) const
{
  return db()->getDouble(m_SctFwdModule,"STEREOANGLE",iModuleType) * Gaudi::Units::milliradian;
}

int 
SCT_ForwardModuleParameters::fwdSensorChargeCarrier(int iModuleType) const 
{
  if (!db()->testField(m_SctFwdSensor, "CARRIER", iModuleType)) return 1;
  int chargeCarrier = db()->getDouble(m_SctFwdSensor, "CARRIER", iModuleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Fwd Charge Carrier CARRIER mod_type("<<iModuleType<<") = "<<chargeCarrier <<endmsg;
  return chargeCarrier;
}

//
// Forward Spine
//
double 
SCT_ForwardModuleParameters::fwdSpineThickness(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSpine,"THICKNESS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineLength(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSpine,"LENGTH",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineMiddleRadius(int iModuleType) const
{
  return db()->getDouble(m_SctFwdSpine,"MIDDLERADIUS",iModuleType) * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineDeltaPhi(int iModuleType) const{
  return db()->getDouble(m_SctFwdSpine,"DELTAPHI",iModuleType) * Gaudi::Units::radian;
}

std::string 
SCT_ForwardModuleParameters::fwdSpineMaterial(int iModuleType) const
{
  return db()->getString(m_SctFwdSpine,"MATERIAL",iModuleType);
}

}
