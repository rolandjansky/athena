/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSCTDBHelper/SCTForwardModuleDBHelper.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>


SCTForwardModuleDBHelper::SCTForwardModuleDBHelper(const OraclePixGeoAccessor* geoAccessor):
  m_geoAccessor(geoAccessor)
{

  m_fwdSensor = geoAccessor->getTable("SctFwdSensor");
  m_fwdHybrid = geoAccessor->getTable("SctFwdHybrid");
  m_fwdSpine = geoAccessor->getTable("SctFwdSpine");
  m_fwdModule = geoAccessor->getTable("SctFwdModule");
  m_fwdModuleConnector = geoAccessor->getTable("SctFwdModuleConnector");


}

//
// Forward Sensor
//
int
SCTForwardModuleDBHelper::fwdSensorNumWafers(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"NUMWAFERS",iModuleType);
}

double 
SCTForwardModuleDBHelper::fwdSensorThickness(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"THICKNESS",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorInnerWidthNear(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"INNERWIDTHNEAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorInnerWidthFar(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"INNERWIDTHFAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorOuterWidthNear(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"OUTERWIDTHNEAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorOuterWidthFar(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"OUTERWIDTHFAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorLengthNear(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"LENGTHNEAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorLengthFar(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"LENGTHFAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorRadiusNear(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"RADIUSNEAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorRadiusFar(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"RADIUSFAR",iModuleType) * CLHEP::mm;
}

std::string 
SCTForwardModuleDBHelper::fwdSensorMaterialNear(int iModuleType) const
{
  return m_geoAccessor->getString(m_fwdSensor,"MATERIALNEAR",iModuleType);
}

std::string 
SCTForwardModuleDBHelper::fwdSensorMaterialFar(int iModuleType) const
{
  return m_geoAccessor->getString(m_fwdSensor,"MATERIALFAR",iModuleType);
}

bool
SCTForwardModuleDBHelper::fwdSensorActiveNear(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"ACTIVENEAR",iModuleType);
}

bool
SCTForwardModuleDBHelper::fwdSensorActiveFar(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"ACTIVEFAR",iModuleType);
}

double 
SCTForwardModuleDBHelper::fwdSensorActiveHalfLengthNear(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"ACTIVEHALFLENGTHNEAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorActiveHalfLengthFar(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"ACTIVEHALFLENGTHFAR",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSensorAngularPitch(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSensor,"ANGULARPITCH",iModuleType) * CLHEP::radian;
}

int
SCTForwardModuleDBHelper::fwdSensorNumStrips(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"NUMSTRIPS",iModuleType);
}

int
SCTForwardModuleDBHelper::fwdSensorNumReadoutStrips(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"NUMREADOUTSTRIPS",iModuleType);
}

int
SCTForwardModuleDBHelper::fwdSensorStripShift(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdSensor,"STRIPSHIFT",iModuleType);
}


//
// Forward Hybrid
//
double 
SCTForwardModuleDBHelper::fwdHybridThickness() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"THICKNESS") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdHybridInnerWidth() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"INNERWIDTH") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdHybridOuterWidth() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"OUTERWIDTH") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdHybridLength() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"LENGTH") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdHybridLengthToCorner() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"LENGTHTOCORNER") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdHybridMountPointToInnerEdge() const
{
  return  m_geoAccessor->getDouble(m_fwdHybrid,"MOUNTPOINTTOINEDGE") * CLHEP::mm;
}

std::string 
SCTForwardModuleDBHelper::fwdHybridMaterial() const
{
  return m_geoAccessor->getString(m_fwdHybrid,"MATERIAL");
}

//
// Forward Spine
//
double 
SCTForwardModuleDBHelper::fwdSpineThickness(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"THICKNESS",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSpineWidth(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"WIDTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSpineEndToModuleCenter(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"ENDTOMODULECENTER",iModuleType) * CLHEP::mm;
}


double 
SCTForwardModuleDBHelper::fwdSpineEndLocatorToEndMount(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"ENDLOCATORTOENDMOUNT",iModuleType) * CLHEP::mm;
}


std::string 
SCTForwardModuleDBHelper::fwdSpineMaterial(int iModuleType) const
{
  return m_geoAccessor->getString(m_fwdSpine,"MATERIAL",iModuleType);
}

//
// Forward SubSpine
//

double 
SCTForwardModuleDBHelper::fwdSubSpineInnerWidth(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBINNERWIDTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineInnerLength(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBINNERLENGTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineInnerRefDist(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBINNERREFDIST",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineMiddleWidth(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBMIDDLEWIDTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineMiddleLength(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBMIDDLELENGTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineMiddleRefDist(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBMIDDLEREFDIST",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineOuterWidth(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBOUTERWIDTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineOuterLength(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBOUTERLENGTH",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdSubSpineOuterRefDist(int iModuleType) const
{
  return  m_geoAccessor->getDouble(m_fwdSpine,"SUBOUTERREFDIST",iModuleType) * CLHEP::mm;
}

std::string 
SCTForwardModuleDBHelper::fwdSubSpineMaterial(int iModuleType) const
{
  return m_geoAccessor->getString(m_fwdSpine,"SUBMATERIAL",iModuleType);
}



//
// Forward Module
//
int
SCTForwardModuleDBHelper::fwdModuleNumTypes() const
{
  return m_geoAccessor->getTableSize(m_fwdModule);
}  

double 
SCTForwardModuleDBHelper::fwdModuleStereoAngle(int iModuleType) const
{
  return m_geoAccessor->getDouble(m_fwdModule,"STEREOANGLE",iModuleType) * CLHEP::milliradian;
}

int
SCTForwardModuleDBHelper::fwdModuleStereoUpperSign(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdModule,"STEREOUPPERSIGN",iModuleType);
}

int 
SCTForwardModuleDBHelper::fwdModuleUpperSideNumber(int iModuleType) const
{
  // Old geometries have lower module side = 0 and upper = 1
  //  if (m_fwdModule,m_geoAccessor->isFieldNull(m_fwdModule,"SIDEUPPER",iModuleType),iModuleType) {    SES  - fix me  (isFieldNull ???)
  if (m_geoAccessor->testField(m_fwdModule,"SIDEUPPER",iModuleType)) {
    return 1;
  }
  return m_geoAccessor->getInt(m_fwdModule,"SIDEUPPER",iModuleType);
}

double
SCTForwardModuleDBHelper::fwdModuleGlueThickness(int iModuleType) const
{
  return m_geoAccessor->getDouble(m_fwdModule,"GLUETHICKNESS",iModuleType) * CLHEP::mm;
}

double
SCTForwardModuleDBHelper::fwdModuleMountPoint(int iModuleType) const
{
  return m_geoAccessor->getDouble(m_fwdModule,"MOUNTPOINT",iModuleType) * CLHEP::mm;
}

double
SCTForwardModuleDBHelper::fwdModuleDistBtwMountPoints(int iModuleType) const
{
  return m_geoAccessor->getDouble(m_fwdModule,"DISTBTWMOUNTPOINTS",iModuleType) * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdModuleHybridEdgeToSpine(int iModuleType) const
{
  return m_geoAccessor->getDouble(m_fwdModule,"HYBRIDEDGETOSPINE",iModuleType)*CLHEP::mm;
}  

bool 
SCTForwardModuleDBHelper::fwdHybridIsOnInnerEdge(int iModuleType) const
{
  return m_geoAccessor->getInt(m_fwdModule,"HYBRIDISONINNEREDGE",iModuleType); 
}

//
// Module Connectors
//
bool
SCTForwardModuleDBHelper::fwdModuleConnectorPresent() const
{
  if(m_geoAccessor->getTableSize(m_fwdModuleConnector) > 0) {
    return true;
  }
  else {
    return false;
  }
}

double 
SCTForwardModuleDBHelper::fwdModuleConnectorDeltaR() const
{
  return m_geoAccessor->getDouble(m_fwdModuleConnector,"DELTAR") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdModuleConnectorRPhi() const
{
  return m_geoAccessor->getDouble(m_fwdModuleConnector,"RPHI") * CLHEP::mm;
}

double 
SCTForwardModuleDBHelper::fwdModuleConnectorThickness() const
{
  return m_geoAccessor->getDouble(m_fwdModuleConnector,"THICKNESS") * CLHEP::mm;
}

std::string
SCTForwardModuleDBHelper::fwdModuleConnectorMaterial() const
{
  return m_geoAccessor->getString(m_fwdModuleConnector,"MATERIAL");
}


