/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <iostream>
#include <cmath>

namespace InDetDDSLHC {



SCT_BarrelParameters::SCT_BarrelParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    m_SctBrlSensor(),
    m_SctBrlLayer(),
    m_SctBrlLadder(),
    m_SctBrlServPerLayer(),
    m_SctBrlSkiZ(),
    m_SctBrlGeneral()
{

  if (sctdb) { // Will be zero for old text file format.
   
    m_SctBrlSensor         = sctdb->brlSensor();
    m_SctBrlLayer          = sctdb->brlLayer();
    m_SctBrlLadder         = sctdb->brlLadder();
    m_SctBrlServPerLayer   = sctdb->brlServPerLayer();
    m_SctBrlSkiZ           = sctdb->brlSkiZ();
    m_SctBrlGeneral        = sctdb->brlGeneral();
          
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Barrel Parameters =======" 
					    << endmsg;
  

   
     
    // Setup z position and module id maps
    for (unsigned int i = 0; i < db()->getTableSize(m_SctBrlSkiZ); i++) {
   
      int type = db()->getInt(m_SctBrlSkiZ,"TYPE",i);
      if (type == 0)  {
	msg(MSG::ERROR) << "Type 0 reserved for uniformly spaced z positions. Do not use type 0 in SctBrlSkiZ table." << endmsg;
      } else {
	std::vector<double> * zposVec = m_zpositionMap[type];
	std::vector<int> * moduleIdVec = m_moduleIdMap[type];
	if (!zposVec) {
	zposVec = new std::vector<double>;
	m_zpositionMap[type] = zposVec; 
	}
	if (!moduleIdVec) {
	  moduleIdVec = new  std::vector<int>;
	  m_moduleIdMap[type] = moduleIdVec;
	}
	zposVec->push_back(db()->getDouble(m_SctBrlSkiZ,"ZPOSITION",i)*Gaudi::Units::mm);
	moduleIdVec->push_back(db()->getInt(m_SctBrlSkiZ,"MODULEID",i));
      }
    }
  }
  //print the modules id and zpositions
  //TODO
}

SCT_BarrelParameters::~SCT_BarrelParameters()
{
  for (std::map<int, std::vector<double> *>::iterator iter = m_zpositionMap.begin(); iter != m_zpositionMap.end(); ++iter) {
    delete iter->second;
  } 
  for (std::map<int, std::vector<int> *>::iterator iter = m_moduleIdMap.begin(); iter != m_moduleIdMap.end(); ++iter) {
    delete iter->second;
  } 
}


int 
SCT_BarrelParameters::layoutType() const{
  return db()->getInt(m_SctBrlGeneral,"LAYOUTTYPE");
}

//bool SCT_BarrelParameters::doEndCaps() const{
//  return (bool)(m_SCT_Parameters->find("DOENDCAPS"))->second;
//}

double
SCT_BarrelParameters::materialIncreaseFactor(int ilayer) const {
  return db()->getDouble(m_SctBrlServPerLayer,"MATSCALEFACTOR",ilayer);
}

//
// Barrel Ski
//
int 
SCT_BarrelParameters::skiFirstStagger() const{
  return -1;
}

double 
SCT_BarrelParameters::skiRadialSep(int ilayer) const{
  // return 2.8*Gaudi::Units::mm;//Gaudi::Units::mm
  int ladType = ladderType(ilayer);
  return db()->getDouble(m_SctBrlLadder,"MODULESRADIALSEP",ladType) * Gaudi::Units::mm;
}


int
SCT_BarrelParameters::moduleType(int ilayer) const{
  int ladType = ladderType(ilayer);
  return db()->getInt(m_SctBrlLadder,"MODTYPE",ladType);
}


int
SCT_BarrelParameters::ladderType(int ilayer) const{
  return db()->getInt(m_SctBrlLayer,"LADDERTYPE",ilayer);
}

int
SCT_BarrelParameters::modulesPerSki(int ilayer) const{
  int ladType = ladderType(ilayer);
  return db()->getInt(m_SctBrlLadder,"NUMPERLADDER",ladType);
}

int  
SCT_BarrelParameters::skiZPositionType(int ilayer) const{
  int ladType = ladderType(ilayer);
  return db()->getInt(m_SctBrlLadder,"ZPOSTYPE",ladType);
}


double 
SCT_BarrelParameters::skiZPosition(int ilayer, int module) const{
  int zpostype = skiZPositionType(ilayer);
  int stavelayout = staveLayout(ilayer);
  double zpos = 0;
  if (zpostype == 0) { 
    // Modules are places uniformly
    
    if(stavelayout==2){
      //SPECIAL CASE FOR STUB CYLINDERS, ASSUMES EVEN NUMBER OF MODULES
      int ladType = ladderType(ilayer);
      int break_mod = modulesPerSki(ilayer) / 2;
      double zsep = db()->getDouble(m_SctBrlLadder,"ZSEP",ladType);
      //CALCULATE NEGATIVE END POSITION FIRST, TO KEEP MODULE ORDERING THE SAME
      double first_pos = (-cylInnerZMin(ilayer) - (break_mod - 0.5)*zsep) * Gaudi::Units::mm;
      //PSOTION OF FIRST MODULE AFTER THE BREAK
      double break_pos = cylInnerZMin(ilayer) * Gaudi::Units::mm ;
      
      if(module < break_mod ) zpos = first_pos + (zsep * module) * Gaudi::Units::mm;
      else zpos = (break_pos + (zsep * (module - break_mod + 0.5))) * Gaudi::Units::mm;
    }

    else{
      int ladType = ladderType(ilayer);
      zpos = db()->getDouble(m_SctBrlLadder,"ZSEP",ladType) * (module - 0.5*(modulesPerSki(ilayer) - 1)) * Gaudi::Units::mm;
    }
    } else {
    std::map<int, std::vector<double> *>::const_iterator iter = m_zpositionMap.find(zpostype);
    if (iter ==  m_zpositionMap.end()) {
      msg(MSG::ERROR) << "Z position type : " << zpostype <<  " not found" << endmsg;
    } else {
      std::vector<double> * zposVec = iter->second;
      if (!zposVec || int(zposVec->size()) <= module) {
	msg(MSG::ERROR) << "Z position for module " << module <<  " of layer " << ilayer << " not found " << endmsg;
      } else {
	zpos = (*zposVec)[module];
      }
    }
  }

 return zpos;
}

int 
SCT_BarrelParameters::skiModuleIdentifier(int ilayer, int module) const{
  int zpostype = skiZPositionType(ilayer);
  int moduleId = 0;
  if (zpostype == 0) { 
    // Module Id assumed to be symmetric about 0.
    // Need to take into account if sensors are segmented
    int numSegments = getNumSegments(ilayer);
    int nModules = modulesPerSki(ilayer);
    moduleId = (module - nModules/2)*numSegments;
    // If even number of modules skip etaModule = 0.
    if ((moduleId >= 0) && !(nModules%2)) moduleId++; 
  } else {
    std::map<int, std::vector<int> *>::const_iterator iter = m_moduleIdMap.find(zpostype);
    if (iter ==  m_moduleIdMap.end()) {
      msg(MSG::ERROR) << "Z position type : " << zpostype <<  " not found" << endmsg;
    } else {
      std::vector<int> * moduleIdVec = iter->second;
      if (!moduleIdVec || int(moduleIdVec->size()) <= module) {
	msg(MSG::ERROR) << "Module id for module index " << module <<  " of layer " << ilayer << " not found " << endmsg;
      } else {
	moduleId = (*moduleIdVec)[module];
      }
    }
  }
  return moduleId;
}


//
// Barrel Layer
//
double 
SCT_BarrelParameters::tilt(int ilayer) const{
  double tilt = db()->getDouble(m_SctBrlLayer,"TILT",ilayer) * Gaudi::Units::degree;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 tilt layer TILT("<<ilayer<<") = "<< tilt/Gaudi::Units::degree << endmsg;
  return tilt;
}

double SCT_BarrelParameters::radius(int ilayer) const{
  double rlay = db()->getDouble(m_SctBrlLayer,"RADIUS",ilayer) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 radius layer RLAY("<<ilayer<<") = "<<rlay<<endmsg;
  return rlay;
}

double 
SCT_BarrelParameters::cylLength(int ilayer) const{
  return db()->getDouble(m_SctBrlLayer,"CYLLENGTH",ilayer) * Gaudi::Units::mm;
}

double 
SCT_BarrelParameters::cylInnerZMin(int ilayer) const{
  return db()->getDouble(m_SctBrlLayer,"CYLINNERZMIN",ilayer) * Gaudi::Units::mm;
}

bool SCT_BarrelParameters::doubleSided(int ilayer) const{
  return db()->getInt(m_SctBrlLayer,"DOUBLESIDED",ilayer);
}

int 
SCT_BarrelParameters::skisPerLayer(int ilayer) const{
  return db()->getInt(m_SctBrlLayer,"SKISPERLAYER",ilayer);
}

int 
SCT_BarrelParameters::layerStereoSign(int ilayer) const{
  int laytyp = db()->getInt(m_SctBrlLayer,"STEREOSIGN",ilayer);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 layerStereoSign layer LAYTYP("<<ilayer<<") = "<< laytyp << endmsg;
  return laytyp;
}

int 
SCT_BarrelParameters::staveLayout(int ilayer) const {
  if (m_SctBrlLayer) {
    return db()->getInt(m_SctBrlLayer,"STAVELAYOUT",ilayer);
  } else {
    return 0;
  }
}

double 
SCT_BarrelParameters::stereoOuter(int ilayer) const {
  if (m_SctBrlLayer) {
    return db()->getDouble(m_SctBrlLayer,"STEREOOUTER",ilayer) * Gaudi::Units::mrad;
  } else {
    return 0;
  }
}

double 
SCT_BarrelParameters::stereoInner(int ilayer) const{
  if (m_SctBrlLayer) {
    return db()->getDouble(m_SctBrlLayer,"STEREOINNER",ilayer) * Gaudi::Units::mrad;
  } else {
    return 0;
  }
}

double 
SCT_BarrelParameters::staveSupportWidth(int ilayer) const{
  if (m_SctBrlLayer) {
    int ladType = ladderType(ilayer);
    if (db()->testField(m_SctBrlLadder,"SUPPORTWIDTH",ladType)) {
      return db()->getDouble(m_SctBrlLadder,"SUPPORTWIDTH",ladType) * Gaudi::Units::mm;
    } 
  }
  return 0;
}


double 
SCT_BarrelParameters::staveSupportThickness(int ilayer) const{
  if (m_SctBrlLayer) {
    int ladType = ladderType(ilayer);
    if (db()->testField(m_SctBrlLadder,"SUPPORTTHICK",ladType)) {
      return db()->getDouble(m_SctBrlLadder,"SUPPORTTHICK",ladType) * Gaudi::Units::mm;
    }
  }
  return 0;
}

std::string
SCT_BarrelParameters::staveSupportMaterial(int ilayer) const{
  if (m_SctBrlLayer) {
    int ladType = ladderType(ilayer);
    if (db()->testField(m_SctBrlLadder,"SUPPORTMATERIAL",ladType)) {
      return db()->getString(m_SctBrlLadder,"SUPPORTMATERIAL",ladType);
    } 
  }
  return "";
}

double 
SCT_BarrelParameters::supportCylInnerRadius(int ilayer) const{
  double risup = db()->getDouble(m_SctBrlServPerLayer,"SUPPORTCYLINNERRAD",ilayer) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 supportCylInnerRadius RISUP("<<ilayer<<") = "<< risup <<endmsg;
  return risup;
}

double 
SCT_BarrelParameters::supportCylOuterRadius(int ilayer) const{
  double rosup = db()->getDouble(m_SctBrlServPerLayer,"SUPPORTCYLOUTERRAD",ilayer) * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 supportCylOuterRadius ROSUP("<<ilayer<<") = "<<rosup<<endmsg;
  return rosup;
}

std::string 
SCT_BarrelParameters::supportCylMaterial(int ilayer) const{
  std::string mat = db()->getString(m_SctBrlServPerLayer,"SUPPORTCYLMATERIAL",ilayer);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2  SUPPORTCYLMATERIAL_STRNG("<<ilayer<<") = "<<mat<<endmsg;
  return mat;
}
//
// Barrel General
//
//number of SCT layer (intermediate + outer)
int
SCT_BarrelParameters::numLayers() const{
  return db()->getInt(m_SctBrlGeneral,"NUMLAYERS");
}

double 
SCT_BarrelParameters::barrelInnerRadius() const{
  double rmin = db()->getDouble(m_SctBrlGeneral,"INNERRADIUS") * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 barrelInnerRadius RMIN = "<<rmin<<endmsg;
  return rmin;
}

double 
SCT_BarrelParameters::barrelIntermediateRadius() const{
  double rinter = db()->getDouble(m_SctBrlGeneral,"RINTERMEDIATE") * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 barrelIntermediateRadius RINTERMEDIATE = "<<rinter<<endmsg;
  return  rinter;
}

double 
SCT_BarrelParameters::barrelOuterRadius() const{
  double rmax = db()->getDouble(m_SctBrlGeneral,"OUTERRADIUS") * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 barrelOuterRadius RMAX = "<<rmax<<endmsg;
  return rmax;
}

double 
SCT_BarrelParameters::barrelLength() const{
  double length =  db()->getDouble(m_SctBrlGeneral,"LENGTH") * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 barrelLength B_LEN = "<<length<<endmsg;
  return length;
}

double 
SCT_BarrelParameters::barrelIntermediateLength() const{
  double interlen =  db()->getDouble(m_SctBrlGeneral,"INTERMEDIATELEN") * Gaudi::Units::mm;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"---------2 barrelIntermediateLength B_IntermediateLEN = "
	   <<interlen<<endmsg;
  return interlen;
}

double 
SCT_BarrelParameters::barrelServicesMaterialCylinderLength() const {
  return db()->getDouble(m_SctBrlGeneral,"BRLSERVMATTHICK") * Gaudi::Units::mm;
}

double 
SCT_BarrelParameters::barrelServicesMaterialIncreaseFactor() const {
  return db()->getDouble(m_SctBrlGeneral,"BRLSERVMATSCALEFACTOR");
}

std::string 
SCT_BarrelParameters::barrelServicesMaterial() const{
  return db()->getString(m_SctBrlGeneral,"BRLSERVMATERIAL");
}

//Copied from SCT_BarrelModuleParameters
int 
SCT_BarrelParameters::getNumSegments(int ilayer) const
{
  int numSegments = 1;
  numSegments = db()->getInt(m_SctBrlSensor, "NUMSEGMENTS", moduleType(ilayer));
  if (numSegments <= 0) numSegments = 1;
  return numSegments;
}

}
