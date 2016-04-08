/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>


using std::abs;

namespace InDetDDSLHC {


SCT_BarrelModuleParameters::SCT_BarrelModuleParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    SctBrlSensor(),
    SctBrlModule()
{
  if (sctdb) { // Will be zero for old text file format.
    SctBrlSensor = sctdb->brlSensor();
    SctBrlModule = sctdb->brlModule();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Barrel Modules Parameters =======" 
					    << endreq;
  }
}

//
// Barrel Sensors
//
double 
SCT_BarrelModuleParameters::sensorThickness(int moduleType) const 
{
  double thickness = db()->getDouble(SctBrlSensor, "THICKNESS", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "-----------2 sensorThickness mod_typ("<<moduleType<<") = "<< thickness << endreq;
  return thickness;
}

double 
SCT_BarrelModuleParameters::sensorWidth(int moduleType) const 
{
  double width = db()->getDouble(SctBrlSensor, "WIDTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 DXYZ2 sensorWidth mod_typ("<<moduleType<<") = "<< width <<endreq;
  return width;
}

double 
SCT_BarrelModuleParameters::sensorLength(int moduleType) const 
{
  double sensorLen = db()->getDouble(SctBrlSensor, "LENGTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 SensorLength DXYZ3 mod_typ("<<moduleType<<") = "<<sensorLen <<endreq;
  return sensorLen;
}

std::string 
SCT_BarrelModuleParameters::sensorMaterial(int moduleType) const 
{
  std::string sensorMaterial = db()->getString(SctBrlSensor, "MATERIAL", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel SENSORMATERIAL_STRNG("<<moduleType<<") = "<<sensorMaterial <<endreq;
  return sensorMaterial;
}

int 
SCT_BarrelModuleParameters::chargeCarrier(int moduleType) const 
{
  if (!db()->testField(SctBrlSensor, "CARRIER", moduleType)) return 1;
  int chargeCarrier = db()->getDouble(SctBrlSensor, "CARRIER", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel Carge Carrier CARRIER mod_type("<<moduleType<<") = "<<chargeCarrier <<endreq;
  return chargeCarrier;
}

//
// Barrel BaseBoard
//
double 
SCT_BarrelModuleParameters::baseBoardThickness(int moduleType) const 
{
  //sprintf(paraName, "BRL_M%d_BBTHICK", moduleType);
  double bbthick = db()->getDouble(SctBrlModule, "BASEBOARDTHICKNESS", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardThickness BBTHICK mod_typ("<<moduleType<<") = "<< bbthick <<endreq;
  return bbthick;
}

double 
SCT_BarrelModuleParameters::baseBoardWidth(int moduleType) const 
{
  double bbwidth = db()->getDouble(SctBrlModule, "BASEBOARDWIDTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardWidth BBWID mod_typ("<<moduleType<<") = "<< bbwidth <<endreq;
  return bbwidth;
}

double SCT_BarrelModuleParameters::baseBoardLength(int moduleType) const{
  double bblength = db()->getDouble(SctBrlModule, "BASEBOARDLENGTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 baseBoardLength BBLEN mod_typ("<<moduleType<<") = "<<bblength <<endreq;
  return bblength;
}

std::string 
SCT_BarrelModuleParameters::baseBoardMaterial(int moduleType) const 
{
  std::string baseboardMaterial = db()->getString(SctBrlModule, "BASEBOARDMATERIAL", moduleType); 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 Barrel BASEBOARDMATERIAL_STRNG("<<moduleType<<") = "<<baseboardMaterial <<endreq;
  return baseboardMaterial;
}
double SCT_BarrelModuleParameters::baseBoardOffsetY(int /*moduleType*/) const{
  //if(moduleType == 1)
    return -5.7*CLHEP::mm;
  //else
  //  return -5.7*CLHEP::mm;
}

double SCT_BarrelModuleParameters::baseBoardOffsetZ(int moduleType) const{
  if(moduleType == 1)
    return -7.1*CLHEP::mm;
  else
    return -1.9*CLHEP::mm;
}

//
// Barrel Module
//
double 
SCT_BarrelModuleParameters::moduleStereoAngle(int moduleType) const
{
  return db()->getDouble(SctBrlModule, "STEREOANGLE", moduleType) * CLHEP::mrad;
}

double 
SCT_BarrelModuleParameters::moduleInterSidesGap(int moduleType) const
{
  return db()->getDouble(SctBrlModule, "INTERSIDESGAP", moduleType) * CLHEP::mm;
}

// Barrel Module Side Design

double 
SCT_BarrelModuleParameters::barrelModelSideStripPitch(int moduleType) const
{
  double pitch = db()->getDouble(SctBrlSensor, "STRIPPITCH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideStripPitch PITCH mod_typ("<<moduleType<<") = "<<pitch <<endreq;
  return pitch;
}

double
SCT_BarrelModuleParameters::barrelModelSideStripLength(int moduleType) const
{
  double stripLen =  db()->getDouble(SctBrlSensor, "STRIPLENGTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideStripLength STRIPLEN mod_typ("<<moduleType<<") = "<<stripLen <<endreq;
  return stripLen;
}

double 
SCT_BarrelModuleParameters::barrelModelSideTotalDeadLength(int moduleType) const
{
  double stripdeadLen = db()->getDouble(SctBrlSensor, "STRIPDEADLENGTH", moduleType) * CLHEP::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideTotalDeadLength STRIPDEADLEN mod_typ("<<moduleType<<") = "<<stripdeadLen<<endreq;
  return stripdeadLen;
}

int 
SCT_BarrelModuleParameters::barrelModelSideSegments(int moduleType) const
{
  int numSegments = 1;
  if (SctBrlSensor) {
    numSegments = db()->getInt(SctBrlSensor, "NUMSEGMENTS", moduleType);
  } 
  if (numSegments <= 0) numSegments = 1;
  return numSegments;
}

double
SCT_BarrelModuleParameters::barrelModelSideSegmentGap(int moduleType) const
{
  if (SctBrlSensor) {
    return db()->getDouble(SctBrlSensor, "SEGMENTGAP", moduleType) * CLHEP::mm;
  } else { 
    return 0;
  }
}

int 
SCT_BarrelModuleParameters::barrelModelSideCrystals(int moduleType) const
{
  return db()->getInt(SctBrlSensor, "NUMBEROFWAFERS", moduleType);
}

int 
SCT_BarrelModuleParameters::barrelModelSideDiodes(int moduleType) const
{
  return barrelModelSideCells(moduleType) + 2*barrelModelSideShift(moduleType);
}

int 
SCT_BarrelModuleParameters::barrelModelSideCells(int moduleType) const
{
  int cells =  db()->getInt(SctBrlSensor, "NUMREADOUTSTRIPS", moduleType);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelModelSideCells NRO mod_typ("<<moduleType<<") = "<< cells <<endreq;
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
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"-----------2 barrelDeadEdge DEADED mod_typ("<<moduleType<<") = "<<deadEdge<<endreq;
  return deadEdge;
}

}
