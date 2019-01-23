/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


using std::abs;

namespace InDetDDSLHC {


SCT_BarrelModuleParameters::SCT_BarrelModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    m_SctBrlSensor(),
    m_SctBrlModule()
{
  if (sctdb) { // Will be zero for old text file format.
    m_SctBrlSensor = sctdb->brlSensor();
    m_SctBrlModule = sctdb->brlModule();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Barrel Modules Parameters =======" 
					    << endmsg;
  }
}

//
// Barrel Sensors
//
double 
SCT_BarrelModuleParameters::sensorThickness(int moduleType) const 
{
  double thickness = db()->getDouble(m_SctBrlSensor, "THICKNESS", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "-----------2 sensorThickness mod_typ("<<moduleType<<") = "<< thickness << endmsg;
  return thickness;
}

double 
SCT_BarrelModuleParameters::sensorWidth(int moduleType) const 
{
  double width = db()->getDouble(m_SctBrlSensor, "WIDTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 DXYZ2 sensorWidth mod_typ("<<moduleType<<") = "<< width <<endmsg;
  return width;
}

double 
SCT_BarrelModuleParameters::sensorLength(int moduleType) const 
{
  double sensorLen = db()->getDouble(m_SctBrlSensor, "LENGTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 SensorLength DXYZ3 mod_typ("<<moduleType<<") = "<<sensorLen <<endmsg;
  return sensorLen;
}

std::string 
SCT_BarrelModuleParameters::sensorMaterial(int moduleType) const 
{
  std::string sensorMaterial = db()->getString(m_SctBrlSensor, "MATERIAL", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel SENSORMATERIAL_STRNG("<<moduleType<<") = "<<sensorMaterial <<endmsg;
  return sensorMaterial;
}

int 
SCT_BarrelModuleParameters::chargeCarrier(int moduleType) const 
{
  if (!db()->testField(m_SctBrlSensor, "CARRIER", moduleType)) return 1;
  int chargeCarrier = db()->getDouble(m_SctBrlSensor, "CARRIER", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel Carge Carrier CARRIER mod_type("<<moduleType<<") = "<<chargeCarrier <<endmsg;
  return chargeCarrier;
}

//
// Barrel BaseBoard
//
double 
SCT_BarrelModuleParameters::baseBoardThickness(int moduleType) const 
{
  //sprintf(paraName, "BRL_M%d_BBTHICK", moduleType);
  double bbthick = db()->getDouble(m_SctBrlModule, "BASEBOARDTHICKNESS", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardThickness BBTHICK mod_typ("<<moduleType<<") = "<< bbthick <<endmsg;
  return bbthick;
}

double 
SCT_BarrelModuleParameters::baseBoardWidth(int moduleType) const 
{
  double bbwidth = db()->getDouble(m_SctBrlModule, "BASEBOARDWIDTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardWidth BBWID mod_typ("<<moduleType<<") = "<< bbwidth <<endmsg;
  return bbwidth;
}

double SCT_BarrelModuleParameters::baseBoardLength(int moduleType) const{
  double bblength = db()->getDouble(m_SctBrlModule, "BASEBOARDLENGTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardLength BBLEN mod_typ("<<moduleType<<") = "<<bblength <<endmsg;
  return bblength;
}

std::string 
SCT_BarrelModuleParameters::baseBoardMaterial(int moduleType) const 
{
  std::string baseboardMaterial = db()->getString(m_SctBrlModule, "BASEBOARDMATERIAL", moduleType); 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel BASEBOARDMATERIAL_STRNG("<<moduleType<<") = "<<baseboardMaterial <<endmsg;
  return baseboardMaterial;
}
double SCT_BarrelModuleParameters::baseBoardOffsetY(int /*moduleType*/) const{
  //if(moduleType == 1)
    return -5.7*Gaudi::Units::mm;
  //else
  //  return -5.7*Gaudi::Units::mm;
}

double SCT_BarrelModuleParameters::baseBoardOffsetZ(int moduleType) const{
  if(moduleType == 1)
    return -7.1*Gaudi::Units::mm;
  else
    return -1.9*Gaudi::Units::mm;
}

//
// Barrel Module
//
double 
SCT_BarrelModuleParameters::moduleStereoAngle(int moduleType) const
{
  return db()->getDouble(m_SctBrlModule, "STEREOANGLE", moduleType) * Gaudi::Units::mrad;
}

double 
SCT_BarrelModuleParameters::moduleInterSidesGap(int moduleType) const
{
  return db()->getDouble(m_SctBrlModule, "INTERSIDESGAP", moduleType) * Gaudi::Units::mm;
}

// Barrel Module Side Design

double 
SCT_BarrelModuleParameters::barrelModelSideStripPitch(int moduleType) const
{
  double pitch = db()->getDouble(m_SctBrlSensor, "STRIPPITCH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideStripPitch PITCH mod_typ("<<moduleType<<") = "<<pitch <<endmsg;
  return pitch;
}

double
SCT_BarrelModuleParameters::barrelModelSideStripLength(int moduleType) const
{
  double stripLen =  db()->getDouble(m_SctBrlSensor, "STRIPLENGTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideStripLength STRIPLEN mod_typ("<<moduleType<<") = "<<stripLen <<endmsg;
  return stripLen;
}

double 
SCT_BarrelModuleParameters::barrelModelSideTotalDeadLength(int moduleType) const
{
  double stripdeadLen = db()->getDouble(m_SctBrlSensor, "STRIPDEADLENGTH", moduleType) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideTotalDeadLength STRIPDEADLEN mod_typ("<<moduleType<<") = "<<stripdeadLen<<endmsg;
  return stripdeadLen;
}

int 
SCT_BarrelModuleParameters::barrelModelSideSegments(int moduleType) const
{
  int numSegments = 1;
  if (m_SctBrlSensor) {
    numSegments = db()->getInt(m_SctBrlSensor, "NUMSEGMENTS", moduleType);
  } 
  if (numSegments <= 0) numSegments = 1;
  return numSegments;
}

double
SCT_BarrelModuleParameters::barrelModelSideSegmentGap(int moduleType) const
{
  if (m_SctBrlSensor) {
    return db()->getDouble(m_SctBrlSensor, "SEGMENTGAP", moduleType) * Gaudi::Units::mm;
  } else { 
    return 0;
  }
}

int 
SCT_BarrelModuleParameters::barrelModelSideCrystals(int moduleType) const
{
  return db()->getInt(m_SctBrlSensor, "NUMBEROFWAFERS", moduleType);
}

int 
SCT_BarrelModuleParameters::barrelModelSideDiodes(int moduleType) const
{
  return barrelModelSideCells(moduleType) + 2*barrelModelSideShift(moduleType);
}

int 
SCT_BarrelModuleParameters::barrelModelSideCells(int moduleType) const
{
  int cells =  db()->getInt(m_SctBrlSensor, "NUMREADOUTSTRIPS", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideCells NRO mod_typ("<<moduleType<<") = "<< cells <<endmsg;
  return cells;
}

int
SCT_BarrelModuleParameters::barrelModelSideShift(int /*moduleType*/) const
{
  return 0;
}

double 
SCT_BarrelModuleParameters::barrelDeadEdge(int moduleType) const
{
  double deadEdge = 0.5*(sensorLength(moduleType) - barrelModelSideStripLength(moduleType));
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelDeadEdge DEADED mod_typ("<<moduleType<<") = "<<deadEdge<<endmsg;
  return deadEdge;
}

}
