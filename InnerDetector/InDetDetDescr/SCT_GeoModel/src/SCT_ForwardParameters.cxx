/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "SCT_GeoModel/SCT_FSIHelper.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>
#include <cmath>

SCT_ForwardParameters::SCT_ForwardParameters(SCT_DataBase* rdb)
{
  m_rdb = rdb;
  m_fsiHelper = std::make_unique<FSIHelper>(m_rdb);
}

SCT_ForwardParameters::~SCT_ForwardParameters()
{
}

//
// Forward Ring
//
int 
SCT_ForwardParameters::fwdRingNumModules(int iRing) const
{
  return m_rdb->fwdRing(iRing)->getInt("NUMMODULES");
}

double
SCT_ForwardParameters::fwdRingModuleStagger(int iRing) const
{
  return m_rdb->fwdRing(iRing)->getDouble("MODULESTAGGER") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingPhiOfRefModule(int iRing) const
{
  return m_rdb->fwdRing(iRing)->getDouble("PHIOFREFMODULE") * Gaudi::Units::deg;
}


int
SCT_ForwardParameters::fwdRingUsualRingSide(int iRing) const
{
  return  m_rdb->fwdRing(iRing)->getInt("USUALRINGSIDE");
}

double
SCT_ForwardParameters::fwdRingDistToDiscCenter(int iRing) const
{
  return m_rdb->fwdRing(iRing)->getDouble("RINGTODISCCENTER") * Gaudi::Units::mm;
}



//
// Forward Wheel
//
double
SCT_ForwardParameters::fwdWheelZPosition(int iWheel) const
{
  return m_rdb->fwdWheel(iWheel)->getDouble("ZPOSITION") * Gaudi::Units::mm;
}

// Returns +/-1 
int
SCT_ForwardParameters::fwdWheelStereoType(int iWheel) const
{
  return m_rdb->fwdWheel(iWheel)->getInt("STEREOTYPE");
}

int
SCT_ForwardParameters::fwdWheelNumRings(int iWheel) const
{
  return m_rdb->fwdWheel(iWheel)->getInt("NUMRINGS");
}


int 
SCT_ForwardParameters::fwdWheelRingMapIndex(int iWheel, int iRing, int ec) const
{
  // In SCT_GeoModel the wheel number is generally 0-8. 
  // In the table "SctFwdWheelRingMap" the wheel number is 1 to 9.

  // The postive and negative endcap are not identical.  If the negative
  // endcap is different from the positive endcap there will be in an entry
  // with the wheel number negative. For the negative endcap we first look for
  // an entry with -ve wheel number. If no entry wee try the positive endcap.

  int iWheelDB = iWheel + 1;

  // If it is in the -ve endcap we first look for an entry with -ve wheel number 
  int index = -1;
  if (ec < 0) {
    index = fwdWheelRingMapIndexDB(-iWheelDB, iRing);
    if (index >= 0) return index;
  }
  // Look for it in the positive endcap
  index = fwdWheelRingMapIndexDB(iWheelDB, iRing);
  if (index < 0) {
    std::cout << "Wheel (numbering from 1)" << iWheelDB << ", Ring " << iRing 
              << " not in table SctFwdWheelRingMap" << std::endl;
  }
  return index;
}

// Get index in SctFwdWheelRing table for wheel/ring combination
// It just loops through the table until it finds a match. 
// This is not particularly efficient but its fine for the purpose.
// Returns -1 if not found.

int 
SCT_ForwardParameters::fwdWheelRingMapIndexDB(int iWheelDB, int iRing) const
{

  for (int i=0; i < m_rdb->fwdWheelRingMapSize(); i++) {
    if ((m_rdb->fwdWheelRingMap(i)->getInt("WHEELNUM") == iWheelDB) &&
      (m_rdb->fwdWheelRingMap(i)->getInt("RINGNUM") == iRing))
    {
      return i;
      break; 
    }
  }
  return -1;
}

int
SCT_ForwardParameters::fwdRingStaggerOfRefModule(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_rdb->fwdWheelRingMap(index)->getInt("STAGGEROFREFMODULE");
}


int
SCT_ForwardParameters::fwdWheelRingSide(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_rdb->fwdWheelRingMap(index)->getInt("SIDE");
}


int
SCT_ForwardParameters::fwdWheelModuleType(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_rdb->fwdWheelRingMap(index)->getInt("MODULETYPE");
}


//
// Forward DiscSupport
//

double
SCT_ForwardParameters::fwdDiscSupportInnerRadius() const
{
  return m_rdb->fwdDiscSupport()->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdDiscSupportOuterRadius() const
{
  return m_rdb->fwdDiscSupport()->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}
 
double
SCT_ForwardParameters::fwdDiscSupportThickness() const
{
  return m_rdb->fwdDiscSupport()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdDiscSupportMaterial() const
{
  return m_rdb->fwdDiscSupport()->getString("MATERIAL");
}

//
// Forward PatchPanel
//
int
SCT_ForwardParameters::fwdNumPatchPanelLocs() const
{
  return m_rdb->fwdPatchPanelLocSize();
}

int
SCT_ForwardParameters::fwdPatchPanelType(int iLoc) const
{
  return m_rdb->fwdPatchPanelLoc(iLoc)->getInt("TYPENUM");
}

double
SCT_ForwardParameters::fwdPatchPanelLocAngle(int iLoc) const
{
  return m_rdb->fwdPatchPanelLoc(iLoc)->getDouble("LOCANGLE") * Gaudi::Units::degree;
}

bool
SCT_ForwardParameters::fwdPatchPanelRepeatQuadrant(int iLoc) const
{
  return m_rdb->fwdPatchPanelLoc(iLoc)->getInt("REPEATQUADRANT");
}

int
SCT_ForwardParameters::fwdNumPatchPanelTypes() const
{
  return m_rdb->fwdPatchPanelSize();
}

double
SCT_ForwardParameters::fwdPatchPanelThickness(int iType) const
{
  return m_rdb->fwdPatchPanel(iType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPatchPanelMidRadius(int iType) const
{
  return m_rdb->fwdPatchPanel(iType)->getDouble("MIDRADIUS") * Gaudi::Units::mm;
}

 
double
SCT_ForwardParameters::fwdPatchPanelDeltaR(int iType) const
{
  return m_rdb->fwdPatchPanel(iType)->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPatchPanelRPhi(int iType) const
{
  return m_rdb->fwdPatchPanel(iType)->getDouble("RPHI") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdPatchPanelMaterial(int iType) const
{
  return m_rdb->fwdPatchPanel(iType)->getString("MATERIAL");
}

//
// Forward PatchPanel Connector
//
bool
SCT_ForwardParameters::fwdPPConnectorPresent() const
{
  if (m_rdb->fwdPPCoolingSize() > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCT_ForwardParameters::fwdPPConnectorThickness() const
{
  return m_rdb->fwdPPConnector()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPPConnectorDeltaR() const
{
  return m_rdb->fwdPPConnector()->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPPConnectorRPhi() const
{
  return m_rdb->fwdPPConnector()->getDouble("RPHI") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdPPConnectorMaterial() const
{
 return m_rdb->fwdPPConnector()->getString("MATERIAL");
}

//
// Forward PatchPanel Cooling
//
bool
SCT_ForwardParameters::fwdPPCoolingPresent() const
{
  if (m_rdb->fwdPPCoolingSize() > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCT_ForwardParameters::fwdPPCoolingThickness() const
{
  return m_rdb->fwdPPCooling()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPPCoolingDeltaR() const
{
  return m_rdb->fwdPPCooling()->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdPPCoolingRPhi() const
{
  return m_rdb->fwdPPCooling()->getDouble("RPHI") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdPPCoolingMaterial() const
{
 return m_rdb->fwdPPCooling()->getString("MATERIAL");
}

//
// Cooling Block
//
int
SCT_ForwardParameters::fwdCoolingBlockHiLo(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getInt("HILO");
}

int
SCT_ForwardParameters::fwdCoolingBlockMainOrSecondary(int iType) const
{
  // true is main, false is secondary
  return m_rdb->fwdCoolingBlock(iType)->getInt("MAINSECONDARY");
}

double
SCT_ForwardParameters::fwdCoolingBlockDeltaR(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdCoolingBlockRPhi(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getDouble("RPHI") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdCoolingBlockThickness(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdCoolingBlockOffsetFromDisc(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getDouble("OFFSETFROMDISC") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdCoolingBlockMaterial(int iType) const
{
  return m_rdb->fwdCoolingBlock(iType)->getString("MATERIAL");
}


//
// Forward DiscPowerTape
//
double 
SCT_ForwardParameters::fwdDiscPowerTapeInnerRadius(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("POWERTAPEINNERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdDiscPowerTapeOuterRadius(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("POWERTAPEOUTERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdDiscPowerTapeThickness(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("POWERTAPETHICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdDiscPowerTapeMaterial(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getString("POWERTAPEMATERIAL");
}



//
// Forward RingCooling
//
double 
SCT_ForwardParameters::fwdRingCoolingInnerRadius(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("COOLINGINNERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdRingCoolingOuterRadius(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("COOLINGOUTERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdRingCoolingThickness(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getDouble("COOLINGTHICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdRingCoolingMaterial(int iRing) const
{
  return m_rdb->fwdRingServices(iRing)->getString("COOLINGMATERIAL");
}

//
// Forward Disc Fixation
//
bool
SCT_ForwardParameters::fwdDiscFixationPresent() const
{
  if (m_rdb->fwdDiscFixationTable()->size() > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCT_ForwardParameters::fwdDiscFixationThickness() const
{
  return m_rdb->fwdDiscFixation()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdDiscFixationRadius() const
{
  return m_rdb->fwdDiscFixation()->getDouble("RADIUS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdDiscFixationMaterial() const
{
  return m_rdb->fwdDiscFixation()->getString("MATERIAL");
}

//
// Forward SupportFrame
//
double 
SCT_ForwardParameters::fwdSupportFrameRadialThickness() const
{
  return m_rdb->fwdServices()->getDouble("SUPPORTFRAMEDELTAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdSupportFrameInnerRadius() const
{
  return m_rdb->fwdServices()->getDouble("SUPPORTFRAMEINNERRAD") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdSupportFrameZMin() const 
{
  return m_rdb->fwdServices()->getDouble("SUPPORTFRAMEZMIN") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdSupportFrameZMax() const 
{
  return m_rdb->fwdServices()->getDouble("SUPPORTFRAMEZMAX") * Gaudi::Units::mm;
} 

std::string 
SCT_ForwardParameters::fwdSupportFrameMaterial() const 
{
  return m_rdb->fwdServices()->getString("SUPPORTFRAMEMATERIAL");
} 

//
// Forward CoolingPipe
//
double
SCT_ForwardParameters::fwdCoolingPipeRadius() const
{
  return m_rdb->fwdServices()->getDouble("COOLINGPIPERADIUS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdCoolingPipeMaterial() const
{
  return m_rdb->fwdServices()->getString("COOLINGPIPEMATERIAL");
}

//
// Forward PowerTape
//
double
SCT_ForwardParameters::fwdPowerTapeCrossSectArea() const
{
  return m_rdb->fwdServices()->getDouble("POWERTAPECROSSSECT") * Gaudi::Units::mm2;
}


std::string
SCT_ForwardParameters::fwdPowerTapeMaterial() const
{ 
  return m_rdb->fwdServices()->getString("POWERTAPEMATERIAL");
}

//
// FSI
//
int 
SCT_ForwardParameters::fwdFSINumGeomTypes() const
{
  return m_rdb->fwdFSITypeSize();
}

double
SCT_ForwardParameters::fwdFSIGeomDeltaR(int iType) const
{
  return m_rdb->fwdFSIType(iType)->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdFSIGeomRPhi(int iType) const
{
  return m_rdb->fwdFSIType(iType)->getDouble("RPHI") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdFSIGeomThickness(int iType) const
{
  // Fix for SCT-DC3-03. May be removed when ATLAS-DC3-07 is obsolete.
  if (iType == 0 && m_rdb->versionTag() == "SCT-DC3-03") return  26*Gaudi::Units::mm;
  return m_rdb->fwdFSIType(iType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdFSIGeomMaterial(int iType) const
{
  return m_rdb->fwdFSIType(iType)->getString("MATERIAL");
}

double
SCT_ForwardParameters::fwdFSIGeomZOffset(int iType) const
{
  // Fix for SCT-DC3-03. May be removed when ATLAS-DC3-07 is obsolete.
  if (iType == 0 && m_rdb->versionTag() == "SCT-DC3-03") return  22*Gaudi::Units::mm;
  return m_rdb->fwdFSIType(iType)->getDouble("ZOFFSET") * Gaudi::Units::mm;
}


const FSIHelper & 
SCT_ForwardParameters::fsiHelper() const
{
  return *m_fsiHelper;
}


const std::vector<const FSIDetails *> &
SCT_ForwardParameters::fsiVector(int iWheel) const
{
  return fsiHelper().fsiVector(iWheel);
}


//
// Forward Cylinder Services
//
bool
SCT_ForwardParameters::fwdCylinderServicePresent() const
{
  if (m_rdb->fwdCylServSize() > 0) {
    return true;
  }
  else {
    return false;
  }
}

int
SCT_ForwardParameters::fwdNumCylinderServiceLocs() const
{

  return m_rdb->fwdCylServLocSize();
}

std::string
SCT_ForwardParameters::fwdCylinderServiceLocName(int iLoc) const
{
  return m_rdb->fwdCylServLoc(iLoc)->getString("NAME");
}

double
SCT_ForwardParameters::fwdCylinderServiceLocAngle(int iLoc) const
{
  return m_rdb->fwdCylServLoc(iLoc)->getDouble("LOCANGLE") * Gaudi::Units::degree;
}

int
SCT_ForwardParameters::fwdNumCylinderServiceTypes() const
{
  return m_rdb->fwdCylServSize();
}

std::string
SCT_ForwardParameters::fwdCylinderServiceName(int iType) const
{
  return m_rdb->fwdCylServ(iType)->getString("NAME");
}

std::string
SCT_ForwardParameters::fwdCylinderServiceMaterial(int iType) const
{
  return m_rdb->fwdCylServ(iType)->getString("MATERIAL");
}

double
SCT_ForwardParameters::fwdCylinderServiceDeltaR(int iType) const
{
  return m_rdb->fwdCylServ(iType)->getDouble("DELTAR") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdCylinderServiceRPhi(int iType) const
{
  return m_rdb->fwdCylServ(iType)->getDouble("RPHI") * Gaudi::Units::mm;
}

//
// Forward ThermalShieldElement
//
int
SCT_ForwardParameters::fwdNumThermalShieldElements() const
{
  return m_rdb->fwdThermalShieldSize();
}

std::string
SCT_ForwardParameters::fwdThermalShieldMaterial(int iElement) const
{
  return m_rdb->fwdThermalShield(iElement)->getString("MATERIAL");
}

double
SCT_ForwardParameters::fwdThermalShieldInnerRadius(int iElement) const
{
  return m_rdb->fwdThermalShield(iElement)->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdThermalShieldOuterRadius(int iElement) const
{
  return m_rdb->fwdThermalShield(iElement)->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdThermalShieldZMin(int iElement) const 
{
  return m_rdb->fwdThermalShield(iElement)->getDouble("ZMIN") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdThermalShieldZMax(int iElement) const 
{
  return m_rdb->fwdThermalShield(iElement)->getDouble("ZMAX") * Gaudi::Units::mm;
} 


//
// Forward General
//
int
SCT_ForwardParameters::fwdNumWheels() const
{
  return m_rdb->fwdGeneral()->getInt("NUMWHEELS");
}

double
SCT_ForwardParameters::fwdInnerRadius() const
{
  return m_rdb->fwdGeneral()->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdOuterRadius() const
{
  return m_rdb->fwdGeneral()->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdZMin() const
{
  return m_rdb->fwdGeneral()->getDouble("ZMIN") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdZMax() const
{
  return m_rdb->fwdGeneral()->getDouble("ZMAX") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdTrtGapPos() const
{
  return m_rdb->fwdGeneral()->getDouble("TRTGAPPOS") * Gaudi::Units::mm;
}

//
// OptoHarness
//
bool
SCT_ForwardParameters::fwdOptoHarnessPresent() const
{
  if (m_rdb->fwdOptoHarnessTable()->size() > 0) {
    return true;
  }
  else {
    return false;
  }
}

int 
SCT_ForwardParameters::fwdOptoHarnessDiscType(int index) const
{
  return m_rdb->fwdOptoHarness(index)->getInt("DISCTYPE");
}

double 
SCT_ForwardParameters::fwdOptoHarnessInnerRadius(int index) const
{
  return m_rdb->fwdOptoHarness(index)->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdOptoHarnessOuterRadius(int index) const
{
  return m_rdb->fwdOptoHarness(index)->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}

double 
SCT_ForwardParameters::fwdOptoHarnessThickness(int index) const
{
  return m_rdb->fwdOptoHarness(index)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParameters::fwdOptoHarnessMaterial(int index) const
{
  return m_rdb->fwdOptoHarness(index)->getString("MATERIAL");
}

SCT_ForwardParameters&
SCT_ForwardParameters::operator=(const SCT_ForwardParameters& right) {
  if (this != &right) {
    m_rdb = right.m_rdb;
    m_fsiHelper.reset(new FSIHelper(m_rdb));
  }
  return *this;
}

SCT_ForwardParameters::SCT_ForwardParameters(const SCT_ForwardParameters& right) {
  m_rdb = right.m_rdb;
  m_fsiHelper.reset(new FSIHelper(m_rdb));
}
