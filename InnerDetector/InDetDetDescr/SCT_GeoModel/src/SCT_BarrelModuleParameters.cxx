/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"

#include "SCT_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

using std::abs;

//
// A few hard wired numbers (some of which should go in NOVA)
//
/*
const double SCT_MODULE_HYBRID_OFFSET = 5.0 * Gaudi::Units::mm; // Planar distance from center of sensor to edge of hybrid.
const int STEREO_UPPER_SIGN = -1; // Sign of stereo rotation of upper sensor with axis going
                                  // from lower to upper  

const double PITCH = 80 * micrometer;
const double HALF_ACTIVE_STRIP_LENGTH = 31 * Gaudi::Units::mm;
const double NOMINAL_WAFER_LENGTH = 63.960 * Gaudi::Units::mm;
const double REF_DISTANCE_BETWEEN_FIDUCIALS = 2.19 * Gaudi::Units::mm; 
const double DISTANCE_CORNER_MARK_TO_CENTER = 31.750 * Gaudi::Units::mm; 
const double DISTANCE_CORNER_MARK_TO_FIDUCIAL = 0.8 * Gaudi::Units::mm; 
const double DISTANCE_CENTER_TO_CENTER = 2 * (DISTANCE_CORNER_MARK_TO_CENTER - 
                                              DISTANCE_CORNER_MARK_TO_FIDUCIAL)
                                         + REF_DISTANCE_BETWEEN_FIDUCIALS;
*/

SCT_BarrelModuleParameters::SCT_BarrelModuleParameters()
{
  m_rdb = SCT_DataBase::instance();
}

//
// Barrel Sensors
//
double 
SCT_BarrelModuleParameters::sensorThickness() const 
{
  return m_rdb->brlSensor()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::sensorWidth() const 
{
  return m_rdb->brlSensor()->getDouble("WIDTH") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::sensorLength() const 
{
  return m_rdb->brlSensor()->getDouble("WAFERLENGTH") * Gaudi::Units::mm;
}

int
SCT_BarrelModuleParameters::sensorNumWafers() const 
{
  return m_rdb->brlSensor()->getInt("NUMWAFERS");
}

std::string
SCT_BarrelModuleParameters::sensorMaterial() const 
{
  return m_rdb->brlSensor()->getString("MATERIAL");
}

double
SCT_BarrelModuleParameters::sensorDistCenterToCenter() const 
{
  return 2 * m_rdb->brlSensor()->getDouble("CENTERTOFIDUCIAL") * Gaudi::Units::mm 
    + m_rdb->brlSensor()->getDouble("FIDUCIALSEP") * Gaudi::Units::mm;
}

double
SCT_BarrelModuleParameters::sensorStripLength() const
{
  return 2 * m_rdb->brlSensor()->getDouble("ACTIVEHALFLENGTH") * Gaudi::Units::mm;
}

double
SCT_BarrelModuleParameters::sensorStripPitch() const
{
  return m_rdb->brlSensor()->getDouble("STRIPPITCH") * Gaudi::Units::mm;
}

int 
SCT_BarrelModuleParameters::sensorNumStrips() const
{
   return m_rdb->brlSensor()->getInt("NUMSTRIPS");
}


int
SCT_BarrelModuleParameters::sensorNumReadoutStrips() const
{
  return m_rdb->brlSensor()->getInt("NUMREADOUTSTRIPS");
}

int
SCT_BarrelModuleParameters::sensorStripShift() const
{
  return m_rdb->brlSensor()->getInt("STRIPSHIFT");
}



//
// Barrel BaseBoard
//
double 
SCT_BarrelModuleParameters::baseBoardThickness() const 
{
  return m_rdb->brlModule()->getDouble("BASEBOARDTHICKNESS") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::baseBoardWidth() const 
{
  return m_rdb->brlModule()->getDouble("BASEBOARDWIDTH") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::baseBoardLength() const 
{
  return m_rdb->brlModule()->getDouble("BASEBOARDLENGTH") * Gaudi::Units::mm;
}

std::string
SCT_BarrelModuleParameters::baseBoardMaterial() const 
{
  return m_rdb->brlModule()->getString("BASEBOARDMATERIAL");
}

double 
SCT_BarrelModuleParameters::baseBoardOffsetY() const 
{
  return m_rdb->brlModule()->getDouble("BASEBOARDOFFSETY") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::baseBoardOffsetZ() const 
{
  return m_rdb->brlModule()->getDouble("BASEBOARDOFFSETZ") * Gaudi::Units::mm;
}

//
// Barrel Hybrid
//
double 
SCT_BarrelModuleParameters::hybridThickness() const 
{
  return m_rdb->brlModule()->getDouble("HYBRIDTHICKNESS") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::hybridWidth() const 
{
  return m_rdb->brlModule()->getDouble("HYBRIDWIDTH") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::hybridLength() const 
{
  return m_rdb->brlModule()->getDouble("HYBRIDLENGTH") * Gaudi::Units::mm;
}

std::string
SCT_BarrelModuleParameters::hybridMaterial() const 
{
  return m_rdb->brlModule()->getString("HYBRIDMATERIAL");
}

double 
SCT_BarrelModuleParameters::hybridOffsetX() const 
{
  return m_rdb->brlModule()->getDouble("HYBRIDOFFSETX") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::hybridOffsetZ() const 
{
  return m_rdb->brlModule()->getDouble("HYBRIDOFFSETZ") * Gaudi::Units::mm;
}

//
// Barrel Pigtail
//
double 
SCT_BarrelModuleParameters::pigtailThickness() const 
{
  return m_rdb->brlModule()->getDouble("PIGTAILTHICKNESS") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::pigtailWidth() const 
{
  return m_rdb->brlModule()->getDouble("PIGTAILWIDTH") * Gaudi::Units::mm;
}

double 
SCT_BarrelModuleParameters::pigtailLength() const 
{
  return m_rdb->brlModule()->getDouble("PIGTAILLENGTH") * Gaudi::Units::mm;
}

std::string
SCT_BarrelModuleParameters::pigtailMaterial() const 
{
  return m_rdb->brlModule()->getString("PIGTAILMATERIAL");
}



//
// Barrel Module
//
int 
SCT_BarrelModuleParameters::moduleStereoUpperSign() const
{
  return m_rdb->brlModule()->getInt("STEREOUPPERSIGN");
}

int 
SCT_BarrelModuleParameters::moduleUpperSideNumber() const
{
  // Old geometries have lower module side = 0 and upper = 1
  if (m_rdb->brlModule()->isFieldNull("SIDEUPPER")) {
    return 1;
  }
  return m_rdb->brlModule()->getInt("SIDEUPPER");
}


double 
SCT_BarrelModuleParameters::moduleStereoAngle() const
{
  return m_rdb->brlModule()->getDouble("STEREOANGLE") * Gaudi::Units::milliradian;
}


double 
SCT_BarrelModuleParameters::moduleSensorToSensorGap() const
{
  return m_rdb->brlModule()->getDouble("SENSORTOSENSORGAP") * Gaudi::Units::mm;
}




