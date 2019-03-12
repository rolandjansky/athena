/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "SCT_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


SCT_ForwardModuleParameters::SCT_ForwardModuleParameters(SCT_DataBase* rdb)
{
  m_rdb = rdb;
}

//
// Forward Sensor
//
int
SCT_ForwardModuleParameters::fwdSensorNumWafers(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("NUMWAFERS");
}

double 
SCT_ForwardModuleParameters::fwdSensorThickness(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerWidthNear(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("INNERWIDTHNEAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorInnerWidthFar(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("INNERWIDTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterWidthNear(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("OUTERWIDTHNEAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorOuterWidthFar(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("OUTERWIDTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorLengthNear(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("LENGTHNEAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorLengthFar(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("LENGTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorRadiusNear(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("RADIUSNEAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorRadiusFar(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("RADIUSFAR") * Gaudi::Units::mm;
}

std::string 
SCT_ForwardModuleParameters::fwdSensorMaterialNear(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getString("MATERIALNEAR");
}

std::string 
SCT_ForwardModuleParameters::fwdSensorMaterialFar(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getString("MATERIALFAR");
}

bool
SCT_ForwardModuleParameters::fwdSensorActiveNear(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("ACTIVENEAR");
}

bool
SCT_ForwardModuleParameters::fwdSensorActiveFar(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("ACTIVEFAR");
}

double 
SCT_ForwardModuleParameters::fwdSensorActiveHalfLengthNear(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("ACTIVEHALFLENGTHNEAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorActiveHalfLengthFar(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("ACTIVEHALFLENGTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSensorAngularPitch(int iModuleType) const
{
  return  m_rdb->fwdSensor(iModuleType)->getDouble("ANGULARPITCH") * Gaudi::Units::radian;
}

int
SCT_ForwardModuleParameters::fwdSensorNumStrips(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("NUMSTRIPS");
}

int
SCT_ForwardModuleParameters::fwdSensorNumReadoutStrips(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("NUMREADOUTSTRIPS");
}

int
SCT_ForwardModuleParameters::fwdSensorStripShift(int iModuleType) const
{
  return m_rdb->fwdSensor(iModuleType)->getInt("STRIPSHIFT");
}


//
// Forward Hybrid
//
double 
SCT_ForwardModuleParameters::fwdHybridThickness() const
{
  return  m_rdb->fwdHybrid()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdHybridInnerWidth() const
{
  return  m_rdb->fwdHybrid()->getDouble("INNERWIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdHybridOuterWidth() const
{
  return  m_rdb->fwdHybrid()->getDouble("OUTERWIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdHybridLength() const
{
  return  m_rdb->fwdHybrid()->getDouble("LENGTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdHybridLengthToCorner() const
{
  return  m_rdb->fwdHybrid()->getDouble("LENGTHTOCORNER") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdHybridMountPointToInnerEdge() const
{
  return  m_rdb->fwdHybrid()->getDouble("MOUNTPOINTTOINEDGE") * Gaudi::Units::mm;
}

std::string 
SCT_ForwardModuleParameters::fwdHybridMaterial() const
{
  return m_rdb->fwdHybrid()->getString("MATERIAL");
}

//
// Forward Spine
//
double 
SCT_ForwardModuleParameters::fwdSpineThickness(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineWidth(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("WIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSpineEndToModuleCenter(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("ENDTOMODULECENTER") * Gaudi::Units::mm;
}


double 
SCT_ForwardModuleParameters::fwdSpineEndLocatorToEndMount(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("ENDLOCATORTOENDMOUNT") * Gaudi::Units::mm;
}


std::string 
SCT_ForwardModuleParameters::fwdSpineMaterial(int iModuleType) const
{
  return m_rdb->fwdSpine(iModuleType)->getString("MATERIAL");
}

//
// Forward SubSpine
//

double 
SCT_ForwardModuleParameters::fwdSubSpineInnerWidth(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBINNERWIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineInnerLength(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBINNERLENGTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineInnerRefDist(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBINNERREFDIST") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineMiddleWidth(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBMIDDLEWIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineMiddleLength(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBMIDDLELENGTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineMiddleRefDist(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBMIDDLEREFDIST") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineOuterWidth(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBOUTERWIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineOuterLength(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBOUTERLENGTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdSubSpineOuterRefDist(int iModuleType) const
{
  return  m_rdb->fwdSpine(iModuleType)->getDouble("SUBOUTERREFDIST") * Gaudi::Units::mm;
}

std::string 
SCT_ForwardModuleParameters::fwdSubSpineMaterial(int iModuleType) const
{
  return m_rdb->fwdSpine(iModuleType)->getString("SUBMATERIAL");
}



//
// Forward Module
//
int
SCT_ForwardModuleParameters::fwdModuleNumTypes() const
{
  return m_rdb->fwdModuleSize();
}  

double 
SCT_ForwardModuleParameters::fwdModuleStereoAngle(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getDouble("STEREOANGLE") * Gaudi::Units::milliradian;
}

int
SCT_ForwardModuleParameters::fwdModuleStereoUpperSign(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getInt("STEREOUPPERSIGN");
}

int 
SCT_ForwardModuleParameters::fwdModuleUpperSideNumber(int iModuleType) const
{
  // Old geometries have lower module side = 0 and upper = 1
  if (m_rdb->fwdModule(iModuleType)->isFieldNull("SIDEUPPER")) {
    return 1;
  }
  return m_rdb->fwdModule(iModuleType)->getInt("SIDEUPPER");
}

double
SCT_ForwardModuleParameters::fwdModuleGlueThickness(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getDouble("GLUETHICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardModuleParameters::fwdModuleMountPoint(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getDouble("MOUNTPOINT") * Gaudi::Units::mm;
}

double
SCT_ForwardModuleParameters::fwdModuleDistBtwMountPoints(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getDouble("DISTBTWMOUNTPOINTS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdModuleHybridEdgeToSpine(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getDouble("HYBRIDEDGETOSPINE")*Gaudi::Units::mm;
}  

bool 
SCT_ForwardModuleParameters::fwdHybridIsOnInnerEdge(int iModuleType) const
{
  return m_rdb->fwdModule(iModuleType)->getInt("HYBRIDISONINNEREDGE"); 
}

//
// Module Connectors
//
bool
SCT_ForwardModuleParameters::fwdModuleConnectorPresent() const
{
  if(m_rdb->fwdModuleConnectorTable()->size() > 0) {
    return true;
  }
  else {
    return false;
  }
}

double 
SCT_ForwardModuleParameters::fwdModuleConnectorDeltaR() const
{
  return m_rdb->fwdModuleConnector()->getDouble("DELTAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdModuleConnectorRPhi() const
{
  return m_rdb->fwdModuleConnector()->getDouble("RPHI") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParameters::fwdModuleConnectorThickness() const
{
  return m_rdb->fwdModuleConnector()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardModuleParameters::fwdModuleConnectorMaterial() const
{
  return m_rdb->fwdModuleConnector()->getString("MATERIAL");
}


