/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSCTDBHelper/SCTForwardDBHelper.h"

#include "PixelSCTDBHelper/SCT_FSIHelper.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

using std::abs;

SCTForwardDBHelper::SCTForwardDBHelper(const OraclePixGeoAccessor* geoAccessor):
  m_geoAccessor(geoAccessor),
  m_fsiHelper(0)
{

  m_fwdRing = geoAccessor->getTable("SctFwdRing");
  m_fwdWheel = geoAccessor->getTable("SctFwdWheel");
  m_fwdWheelRingMap = geoAccessor->getTable("SctFwdWheelRingMap");
  m_fwdDiscSupport = geoAccessor->getTable("SctFwdDiscSupport");
  m_fwdPatchPanelLoc = geoAccessor->getTable("SctFwdPatchPanelLoc");
  m_fwdPatchPanel = geoAccessor->getTable("SctFwdPatchPanel");
  m_fwdPPConnector = geoAccessor->getTable("SctFwdPPConnector");
  m_fwdPPCooling = geoAccessor->getTable("SctFwdPPCooling");
  m_fwdCoolingBlock = geoAccessor->getTable("SctFwdCoolingBlock");
  m_fwdRingServices = geoAccessor->getTable("SctFwdRingServices");
  m_fwdServices = geoAccessor->getTable("SctFwdServices");
  m_fwdFSILocation = geoAccessor->getTable("SctFwdFSILocation");
  m_fwdFSIType = geoAccessor->getTable("SctFwdFSIType");
  m_fwdFSI = geoAccessor->getTable("SctFwdFSI");
  m_fwdThermalShield = geoAccessor->getTable("SctFwdThermalShield");
  m_fwdGeneral = geoAccessor->getTable("SctFwdGeneral");
  m_fwdOptoHarness = geoAccessor->getTable("SctFwdOptoHarness");
  m_fwdDiscFixation = geoAccessor->getTable("SctFwdDiscFixation");
  m_fwdCylServ = geoAccessor->getTable("SctFwdCylServ");
  m_fwdCylServLoc = geoAccessor->getTable("SctFwdCylServLoc");

}


//
// Forward Ring
//
int 
SCTForwardDBHelper::fwdRingNumModules(int iRing) const
{
  return m_geoAccessor->getInt(m_fwdRing,"NUMMODULES",iRing);
}

double
SCTForwardDBHelper::fwdRingModuleStagger(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRing,"MODULESTAGGER",iRing) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdRingPhiOfRefModule(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRing,"PHIOFREFMODULE",iRing) * CLHEP::deg;
}


int
SCTForwardDBHelper::fwdRingUsualRingSide(int iRing) const
{
  return  m_geoAccessor->getInt(m_fwdRing,"USUALRINGSIDE",iRing);
}

double
SCTForwardDBHelper::fwdRingDistToDiscCenter(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRing,"RINGTODISCCENTER",iRing) * CLHEP::mm;
}



//
// Forward Wheel
//
double
SCTForwardDBHelper::fwdWheelZPosition(int iWheel) const
{
  return m_geoAccessor->getDouble(m_fwdWheel,"ZPOSITION",iWheel) * CLHEP::mm;
}

// Returns +/-1 
int
SCTForwardDBHelper::fwdWheelStereoType(int iWheel) const
{
  return m_geoAccessor->getInt(m_fwdWheel,"STEREOTYPE",iWheel);
}

int
SCTForwardDBHelper::fwdWheelNumRings(int iWheel) const
{
  return m_geoAccessor->getInt(m_fwdWheel,"NUMRINGS",iWheel);
}


int 
SCTForwardDBHelper::fwdWheelRingMapIndex(int iWheel, int iRing, int ec) const
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

int SCTForwardDBHelper::fwdWheelRingMapSize()  const 
{
  return m_geoAccessor->getTableSize(m_fwdWheelRingMap);
}


int 
SCTForwardDBHelper::fwdWheelRingMapIndexDB(int iWheelDB, int iRing) const
{

  for (int i=0; i < fwdWheelRingMapSize(); i++) {
    if ((m_geoAccessor->getInt(m_fwdWheelRingMap,"WHEELNUM",i) == iWheelDB) &&
	(m_geoAccessor->getInt(m_fwdWheelRingMap,"RINGNUM",i) == iRing))
    {
      return i;
      break; 
    }
  }
  return -1;
}

int
SCTForwardDBHelper::fwdRingStaggerOfRefModule(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_geoAccessor->getInt(m_fwdWheelRingMap,"STAGGEROFREFMODULE",index);
}


int
SCTForwardDBHelper::fwdWheelRingSide(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_geoAccessor->getInt(m_fwdWheelRingMap,"SIDE",index);
}


int
SCTForwardDBHelper::fwdWheelModuleType(int iWheel, int iRing, int ec) const
{
  int index = fwdWheelRingMapIndex(iWheel, iRing, ec);
  if (index < 0) return 0;
  return m_geoAccessor->getInt(m_fwdWheelRingMap,"MODULETYPE",index);
}


//
// Forward DiscSupport
//

double
SCTForwardDBHelper::fwdDiscSupportInnerRadius() const
{
  return m_geoAccessor->getDouble(m_fwdDiscSupport,"INNERRADIUS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdDiscSupportOuterRadius() const
{
  return m_geoAccessor->getDouble(m_fwdDiscSupport,"OUTERRADIUS") * CLHEP::mm;
}
 
double
SCTForwardDBHelper::fwdDiscSupportThickness() const
{
  return m_geoAccessor->getDouble(m_fwdDiscSupport,"THICKNESS") * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdDiscSupportMaterial() const
{
  return m_geoAccessor->getString(m_fwdDiscSupport,"MATERIAL");
}

//
// Forward PatchPanel
//
int
SCTForwardDBHelper::fwdNumPatchPanelLocs() const
{
  return m_geoAccessor->getTableSize(m_fwdPatchPanelLoc);
}

int
SCTForwardDBHelper::fwdPatchPanelType(int iLoc) const
{
  return m_geoAccessor->getInt(m_fwdPatchPanelLoc,"TYPENUM",iLoc);
}

double
SCTForwardDBHelper::fwdPatchPanelLocAngle(int iLoc) const
{
  return m_geoAccessor->getDouble(m_fwdPatchPanelLoc,"LOCANGLE",iLoc) * CLHEP::degree;
}

bool
SCTForwardDBHelper::fwdPatchPanelRepeatQuadrant(int iLoc) const
{
  return m_geoAccessor->getInt(m_fwdPatchPanelLoc,"REPEATQUADRANT",iLoc);
}

int
SCTForwardDBHelper::fwdNumPatchPanelTypes() const
{
  return m_geoAccessor->getTableSize(m_fwdPatchPanel);
}

double
SCTForwardDBHelper::fwdPatchPanelThickness(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdPatchPanel,"THICKNESS",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPatchPanelMidRadius(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdPatchPanel,"MIDRADIUS",iType) * CLHEP::mm;
}

 
double
SCTForwardDBHelper::fwdPatchPanelDeltaR(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdPatchPanel,"DELTAR",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPatchPanelRPhi(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdPatchPanel,"RPHI",iType) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdPatchPanelMaterial(int iType) const
{
  return m_geoAccessor->getString(m_fwdPatchPanel,"MATERIAL",iType);
}

//
// Forward PatchPanel Connector
//
bool
SCTForwardDBHelper::fwdPPConnectorPresent() const
{
  if (m_geoAccessor->getTableSize(m_fwdPPCooling) > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCTForwardDBHelper::fwdPPConnectorThickness() const
{
  return m_geoAccessor->getDouble(m_fwdPPConnector,"THICKNESS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPPConnectorDeltaR() const
{
  return m_geoAccessor->getDouble(m_fwdPPConnector,"DELTAR") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPPConnectorRPhi() const
{
  return m_geoAccessor->getDouble(m_fwdPPConnector,"RPHI") * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdPPConnectorMaterial() const
{
 return m_geoAccessor->getString(m_fwdPPConnector,"MATERIAL");
}

//
// Forward PatchPanel Cooling
//
bool
SCTForwardDBHelper::fwdPPCoolingPresent() const
{
  if (m_geoAccessor->getTableSize(m_fwdPPCooling) > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCTForwardDBHelper::fwdPPCoolingThickness() const
{
  return m_geoAccessor->getDouble(m_fwdPPCooling,"THICKNESS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPPCoolingDeltaR() const
{
  return m_geoAccessor->getDouble(m_fwdPPCooling,"DELTAR") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdPPCoolingRPhi() const
{
  return m_geoAccessor->getDouble(m_fwdPPCooling,"RPHI") * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdPPCoolingMaterial() const
{
 return m_geoAccessor->getString(m_fwdPPCooling,"MATERIAL");
}

//
// Cooling Block
//
int
SCTForwardDBHelper::fwdCoolingBlockHiLo(int iType) const
{
  return m_geoAccessor->getInt(m_fwdCoolingBlock,"HILO",iType);
}

int
SCTForwardDBHelper::fwdCoolingBlockMainOrSecondary(int iType) const
{
  // true is main, false is secondary
  return m_geoAccessor->getInt(m_fwdCoolingBlock,"MAINSECONDARY",iType);
}

double
SCTForwardDBHelper::fwdCoolingBlockDeltaR(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCoolingBlock,"DELTAR",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdCoolingBlockRPhi(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCoolingBlock,"RPHI",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdCoolingBlockThickness(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCoolingBlock,"THICKNESS",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdCoolingBlockOffsetFromDisc(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCoolingBlock,"OFFSETFROMDISC",iType) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdCoolingBlockMaterial(int iType) const
{
  return m_geoAccessor->getString(m_fwdCoolingBlock,"MATERIAL",iType);
}


//
// Forward DiscPowerTape
//
double 
SCTForwardDBHelper::fwdDiscPowerTapeInnerRadius(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"POWERTAPEINNERRADIUS",iRing) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdDiscPowerTapeOuterRadius(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"POWERTAPEOUTERRADIUS",iRing) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdDiscPowerTapeThickness(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"POWERTAPETHICKNESS",iRing) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdDiscPowerTapeMaterial(int iRing) const
{
  return m_geoAccessor->getString(m_fwdRingServices,"POWERTAPEMATERIAL",iRing);
}



//
// Forward RingCooling
//
double 
SCTForwardDBHelper::fwdRingCoolingInnerRadius(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"COOLINGINNERRADIUS",iRing) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdRingCoolingOuterRadius(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"COOLINGOUTERRADIUS",iRing) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdRingCoolingThickness(int iRing) const
{
  return m_geoAccessor->getDouble(m_fwdRingServices,"COOLINGTHICKNESS",iRing) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdRingCoolingMaterial(int iRing) const
{
  return m_geoAccessor->getString(m_fwdRingServices,"COOLINGMATERIAL",iRing);
}

//
// Forward Disc Fixation
//
bool
SCTForwardDBHelper::fwdDiscFixationPresent() const
{
  if (m_geoAccessor->getTableSize(m_fwdDiscFixation) > 0) {
    return true;
  }
  else {
    return false;
  }
}

double
SCTForwardDBHelper::fwdDiscFixationThickness() const
{
  return m_geoAccessor->getDouble(m_fwdDiscFixation,"THICKNESS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdDiscFixationRadius() const
{
  return m_geoAccessor->getDouble(m_fwdDiscFixation,"RADIUS") * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdDiscFixationMaterial() const
{
  return m_geoAccessor->getString(m_fwdDiscFixation,"MATERIAL");
}

//
// Forward SupportFrame
//
double 
SCTForwardDBHelper::fwdSupportFrameRadialThickness() const
{
  return m_geoAccessor->getDouble(m_fwdServices,"SUPPORTFRAMEDELTAR") * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdSupportFrameInnerRadius() const
{
  return m_geoAccessor->getDouble(m_fwdServices,"SUPPORTFRAMEINNERRAD") * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdSupportFrameZMin() const 
{
  return m_geoAccessor->getDouble(m_fwdServices,"SUPPORTFRAMEZMIN") * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdSupportFrameZMax() const 
{
  return m_geoAccessor->getDouble(m_fwdServices,"SUPPORTFRAMEZMAX") * CLHEP::mm;
} 

std::string 
SCTForwardDBHelper::fwdSupportFrameMaterial() const 
{
  return m_geoAccessor->getString(m_fwdServices,"SUPPORTFRAMEMATERIAL");
} 

//
// Forward CoolingPipe
//
double
SCTForwardDBHelper::fwdCoolingPipeRadius() const
{
  return m_geoAccessor->getDouble(m_fwdServices,"COOLINGPIPERADIUS") * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdCoolingPipeMaterial() const
{
  return m_geoAccessor->getString(m_fwdServices,"COOLINGPIPEMATERIAL");
}

//
// Forward PowerTape
//
double
SCTForwardDBHelper::fwdPowerTapeCrossSectArea() const
{
  return m_geoAccessor->getDouble(m_fwdServices,"POWERTAPECROSSSECT") * CLHEP::mm2;
}


std::string
SCTForwardDBHelper::fwdPowerTapeMaterial() const
{ 
  return m_geoAccessor->getString(m_fwdServices,"POWERTAPEMATERIAL");
}

//
// FSI
//
int 
SCTForwardDBHelper::fwdFSINumGeomTypes() const
{
  return m_geoAccessor->getTableSize(m_fwdFSIType);
}

double
SCTForwardDBHelper::fwdFSIGeomDeltaR(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdFSIType,"DELTAR",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdFSIGeomRPhi(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdFSIType,"RPHI",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdFSIGeomThickness(int iType) const
{
  // Fix for SCT-DC3-03. May be removed when ATLAS-DC3-07 is obsolete.
  if (iType == 0 && m_geoAccessor->versionTag() == "SCT-DC3-03") return  26*CLHEP::mm;
  return m_geoAccessor->getDouble(m_fwdFSIType,"THICKNESS",iType) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdFSIGeomMaterial(int iType) const
{
  return m_geoAccessor->getString(m_fwdFSIType,"MATERIAL",iType);
}

double
SCTForwardDBHelper::fwdFSIGeomZOffset(int iType) const
{
  // Fix for SCT-DC3-03. May be removed when ATLAS-DC3-07 is obsolete.
  if (iType == 0 && m_geoAccessor->versionTag() == "SCT-DC3-03") return  22*CLHEP::mm;
  return m_geoAccessor->getDouble(m_fwdFSIType,"ZOFFSET",iType) * CLHEP::mm;
}


const FSIHelper & 
SCTForwardDBHelper::fsiHelper() const
{
  if (!m_fsiHelper) m_fsiHelper = new FSIHelper(m_geoAccessor);
  return *m_fsiHelper;
}


const std::vector<const FSIDetails *> &
SCTForwardDBHelper::fsiVector(int iWheel) const
{
  
  std::cout<<"FSIHelper : get vector for "<<iWheel<<std::endl;

  return fsiHelper().fsiVector(iWheel);
}


//
// Forward Cylinder Services
//
bool
SCTForwardDBHelper::fwdCylinderServicePresent() const
{
  if (m_geoAccessor->getTableSize(m_fwdCylServ) > 0) {
    return true;
  }
  else {
    return false;
  }
}

int
SCTForwardDBHelper::fwdNumCylinderServiceLocs() const
{

  return m_geoAccessor->getTableSize(m_fwdCylServLoc);
}

std::string
SCTForwardDBHelper::fwdCylinderServiceLocName(int iLoc) const
{
  return m_geoAccessor->getString(m_fwdCylServLoc,"NAME",iLoc);
}

double
SCTForwardDBHelper::fwdCylinderServiceLocAngle(int iLoc) const
{
  return m_geoAccessor->getDouble(m_fwdCylServLoc,"LOCANGLE",iLoc) * CLHEP::degree;
}

int
SCTForwardDBHelper::fwdNumCylinderServiceTypes() const
{
  return m_geoAccessor->getTableSize(m_fwdCylServ);
}

std::string
SCTForwardDBHelper::fwdCylinderServiceName(int iType) const
{
  return m_geoAccessor->getString(m_fwdCylServ,"NAME",iType);
}

std::string
SCTForwardDBHelper::fwdCylinderServiceMaterial(int iType) const
{
  return m_geoAccessor->getString(m_fwdCylServ,"MATERIAL",iType);
}

double
SCTForwardDBHelper::fwdCylinderServiceDeltaR(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCylServ,"DELTAR",iType) * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdCylinderServiceRPhi(int iType) const
{
  return m_geoAccessor->getDouble(m_fwdCylServ,"RPHI",iType) * CLHEP::mm;
}

//
// Forward ThermalShieldElement
//
int
SCTForwardDBHelper::fwdNumThermalShieldElements() const
{
  return m_geoAccessor->getTableSize(m_fwdThermalShield);
}

std::string
SCTForwardDBHelper::fwdThermalShieldMaterial(int iElement) const
{
  return m_geoAccessor->getString(m_fwdThermalShield,"MATERIAL",iElement);
}

double
SCTForwardDBHelper::fwdThermalShieldInnerRadius(int iElement) const
{
  return m_geoAccessor->getDouble(m_fwdThermalShield,"INNERRADIUS",iElement) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdThermalShieldOuterRadius(int iElement) const
{
  return m_geoAccessor->getDouble(m_fwdThermalShield,"OUTERRADIUS",iElement) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdThermalShieldZMin(int iElement) const 
{
  return m_geoAccessor->getDouble(m_fwdThermalShield,"ZMIN",iElement) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdThermalShieldZMax(int iElement) const 
{
  return m_geoAccessor->getDouble(m_fwdThermalShield,"ZMAX",iElement) * CLHEP::mm;
} 


//
// Forward General
//
int
SCTForwardDBHelper::fwdNumWheels() const
{
  return m_geoAccessor->getInt(m_fwdGeneral,"NUMWHEELS");
}

double
SCTForwardDBHelper::fwdInnerRadius() const
{
  return m_geoAccessor->getDouble(m_fwdGeneral,"INNERRADIUS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdOuterRadius() const
{
  return m_geoAccessor->getDouble(m_fwdGeneral,"OUTERRADIUS") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdZMin() const
{
  return m_geoAccessor->getDouble(m_fwdGeneral,"ZMIN") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdZMax() const
{
  return m_geoAccessor->getDouble(m_fwdGeneral,"ZMAX") * CLHEP::mm;
}

double
SCTForwardDBHelper::fwdTrtGapPos() const
{
  return m_geoAccessor->getDouble(m_fwdGeneral,"TRTGAPPOS") * CLHEP::mm;
}

//
// OptoHarness
//
bool
SCTForwardDBHelper::fwdOptoHarnessPresent() const
{
  if (m_geoAccessor->getTableSize(m_fwdOptoHarness) > 0) {
    return true;
  }
  else {
    return false;
  }
}

int 
SCTForwardDBHelper::fwdOptoHarnessDiscType(int index) const
{
  return m_geoAccessor->getInt(m_fwdOptoHarness,"DISCTYPE",index);
}

double 
SCTForwardDBHelper::fwdOptoHarnessInnerRadius(int index) const
{
  return m_geoAccessor->getDouble(m_fwdOptoHarness,"INNERRADIUS",index) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdOptoHarnessOuterRadius(int index) const
{
  return m_geoAccessor->getDouble(m_fwdOptoHarness,"OUTERRADIUS",index) * CLHEP::mm;
}

double 
SCTForwardDBHelper::fwdOptoHarnessThickness(int index) const
{
  return m_geoAccessor->getDouble(m_fwdOptoHarness,"THICKNESS",index) * CLHEP::mm;
}

std::string
SCTForwardDBHelper::fwdOptoHarnessMaterial(int index) const
{
  return m_geoAccessor->getString(m_fwdOptoHarness,"MATERIAL",index);
}



