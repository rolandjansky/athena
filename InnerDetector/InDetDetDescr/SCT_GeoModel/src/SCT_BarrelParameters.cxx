/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"

#include "SCT_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"


#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <iostream>


SCT_BarrelParameters::SCT_BarrelParameters()
{
  m_rdb = SCT_DataBase::instance();
}


//
// Barrel Ski
//
int 
SCT_BarrelParameters::skiFirstStagger() const
{
  return m_rdb->brlSki()->getInt("SKIFIRSTSTAGGER");
}

double 
SCT_BarrelParameters::skiRadialSep() const
{
  return m_rdb->brlSki()->getDouble("SKIRADIALSEP") * CLHEP::mm;
}

int
SCT_BarrelParameters::modulesPerSki() const
{
  return m_rdb->brlSkiZSize();
}

double 
SCT_BarrelParameters::skiZPosition(int index) const
{
  return m_rdb->brlSkiZ(index)->getDouble("ZPOSITION") * CLHEP::mm;
}

int 
SCT_BarrelParameters::skiModuleIdentifier(int index) const
{
  return  m_rdb->brlSkiZ(index)->getInt("MODULEID");
}

//
// Barrel Layer
//
double 
SCT_BarrelParameters::tilt(int iLayer) const
{
  return m_rdb->brlLayer(iLayer)->getDouble("TILT") * CLHEP::degree;
}

int 
SCT_BarrelParameters::layerStereoSign(int iLayer) const
{
  return m_rdb->brlLayer(iLayer)->getInt("STEREOSIGN");
}


double 
SCT_BarrelParameters::radius(int iLayer) const
{
  return m_rdb->brlLayer(iLayer)->getDouble("RADIUS") * CLHEP::mm;
}

int 
SCT_BarrelParameters::skisPerLayer(int iLayer) const
{
  return m_rdb->brlLayer(iLayer)->getInt("SKISPERLAYER");
}

double 
SCT_BarrelParameters::layerBracketPhiOffset(int iLayer) const
{
  return m_rdb->brlLayer(iLayer)->getDouble("BRACKETPHIOFFSET") * CLHEP::deg;
}

double 
SCT_BarrelParameters::layerPhiOfRefModule(int iLayer) const
{
  // For backward compatibility, if field is null return (90 - tilt) 
  // as ref module is horizontal in old versions.
  if  (m_rdb->brlLayer(iLayer)->isFieldNull("PHIOFREFMODULE")) {
    return 90*CLHEP::deg - tilt(iLayer);
  }
  return m_rdb->brlLayer(iLayer)->getDouble("PHIOFREFMODULE") * CLHEP::deg;
}


//
// Barrel Bracket
//
double
SCT_BarrelParameters::bracketThickness() const
{
  return m_rdb->brlSki()->getDouble("BRACKETTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::bracketWidth() const
{
  return m_rdb->brlSki()->getDouble("BRACKETWIDTH") * CLHEP::mm;
}
 
double
SCT_BarrelParameters::bracketLength() const
{
  return m_rdb->brlSki()->getDouble("BRACKETLENGTH") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::bracketMaterial() const
{
  return m_rdb->brlSki()->getString("BRACKETMATERIAL");
}

//
// Barrel Dogleg
//
double
SCT_BarrelParameters::doglegThickness() const
{
  return m_rdb->brlSki()->getDouble("DOGLEGTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::doglegWidth() const
{
  return m_rdb->brlSki()->getDouble("DOGLEGWIDTH") * CLHEP::mm;
}
 
double
SCT_BarrelParameters::doglegLength() const
{
  return m_rdb->brlSki()->getDouble("DOGLEGLENGTH") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::doglegMaterial() const
{
  return m_rdb->brlSki()->getString("DOGLEGMATERIAL");
}

double
SCT_BarrelParameters::doglegOffsetX() const
{
  return m_rdb->brlSki()->getDouble("DOGLEGOFFSETX") * CLHEP::mm;
}

double
SCT_BarrelParameters::doglegOffsetY() const
{
  return m_rdb->brlSki()->getDouble("DOGLEGOFFSETY") * CLHEP::mm;
}

//
// Barrel CoolingBlock
//
double
SCT_BarrelParameters::coolingBlockThickness() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKTHICK") * CLHEP::mm;
}

double
SCT_BarrelParameters::coolingBlockWidth() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKWIDTH") * CLHEP::mm;
}
 
double
SCT_BarrelParameters::coolingBlockLength() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKLENGTH") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::coolingBlockMaterial() const
{
  return m_rdb->brlSki()->getString("COOLINGBLOCKMATERIAL");
}

double
SCT_BarrelParameters::coolingBlockOffsetX() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKOFFSETX") * CLHEP::mm;
}

double
SCT_BarrelParameters::coolingBlockOffsetY() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKOFFSETY") * CLHEP::mm;
}

double
SCT_BarrelParameters::coolingBlockOffsetZ() const
{
  return m_rdb->brlSki()->getDouble("COOLINGBLOCKOFFSETZ") * CLHEP::mm;
}

//
// Barrel CoolingPipe
//
double
SCT_BarrelParameters::coolingPipeRadius() const
{
  return m_rdb->brlSki()->getDouble("COOLINGPIPERADIUS") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::coolingPipeMaterial() const
{
  return m_rdb->brlSki()->getString("COOLINGPIPEMATERIAL");
}

double
SCT_BarrelParameters::coolingPipeOffsetX() const
{
  return m_rdb->brlSki()->getDouble("COOLINGPIPEOFFSETX") * CLHEP::mm;
}

double
SCT_BarrelParameters::coolingPipeOffsetY() const
{
  return m_rdb->brlSki()->getDouble("COOLINGPIPEOFFSETY") * CLHEP::mm;
}


//
// Barrel PowerTape
//
double
SCT_BarrelParameters::powerTapeThickness() const
{
  return m_rdb->brlSki()->getDouble("POWERTAPETHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::powerTapeWidth() const
{
  return m_rdb->brlSki()->getDouble("POWERTAPEWIDTH") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::powerTapeMaterial() const
{ 
  return m_rdb->brlSki()->getString("POWERTAPEMATERIAL");
}

double
SCT_BarrelParameters::powerTapeStartPointOffset() const
{
  return m_rdb->brlSki()->getDouble("POWERTAPESTARTOFFSET") * CLHEP::mm;
}
 
//
// Barrel Harness
//
double
SCT_BarrelParameters::harnessThickness() const
{
  return m_rdb->brlSki()->getDouble("HARNESSTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::harnessWidth() const
{
  return m_rdb->brlSki()->getDouble("HARNESSWIDTH") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::harnessMaterial() const
{ 
  return m_rdb->brlSki()->getString("HARNESSMATERIAL");
}

//
// Barrel SupportCyl
//
double 
SCT_BarrelParameters::supportCylInnerRadius(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("SUPPORTCYLINNERRAD") * CLHEP::mm;
}

double 
SCT_BarrelParameters::supportCylOuterRadius(int iLayer) const
{
  return supportCylInnerRadius(iLayer) + supportCylDeltaR(iLayer);
}

double 
SCT_BarrelParameters::supportCylDeltaR(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("SUPPORTCYLDELTAR") * CLHEP::mm;
}

std::string 
SCT_BarrelParameters::supportCylMaterial(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getString("SUPPORTCYLMATERIAL");
}


//
// Barrel Flange
//
double 
SCT_BarrelParameters::flangeDeltaZ(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("FLANGEDELTAZ") * CLHEP::mm;
}

double 
SCT_BarrelParameters::flangeDeltaR(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("FLANGEDELTAR") * CLHEP::mm;
}

std::string 
SCT_BarrelParameters::flangeMaterial(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getString("FLANGEMATERIAL");
}

//
// Barrel Clamp
//
double 
SCT_BarrelParameters::clampDeltaZ(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("CLAMPDELTAZ") * CLHEP::mm;
}

double 
SCT_BarrelParameters::clampDeltaR(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("CLAMPDELTAR") * CLHEP::mm;
}

std::string 
SCT_BarrelParameters::clampMaterial(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getString("CLAMPMATERIAL");
}

//
// Barrel Cooling Inlet/outlets
//
double 
SCT_BarrelParameters::coolingEndDeltaR(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("COOLINGENDDELTAR") * CLHEP::mm;
}

std::string 
SCT_BarrelParameters::coolingEndMaterial(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getString("COOLINGENDMATERIAL");
}

//
// Barrel CloseOut
//
double 
SCT_BarrelParameters::closeOutDeltaZ(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getDouble("CLOSEOUTDELTAZ") * CLHEP::mm;
}

std::string 
SCT_BarrelParameters::closeOutMaterial(int iLayer) const
{
  return m_rdb->brlServPerLayer(iLayer)->getString("CLOSEOUTMATERIAL");
}

//
// Barrel InterLink and B6 bearing
//
double 
SCT_BarrelParameters::interLinkDeltaZ() const
{
  return m_rdb->brlServices()->getDouble("INTERLINKDELTAZ") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::interLinkInnerRadius() const
{
  return m_rdb->brlServices()->getDouble("INTERLINKINNERRADIUS") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::interLinkOuterRadius() const
{
  return m_rdb->brlServices()->getDouble("INTERLINKOUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::interLinkMaterial() const
{
  return m_rdb->brlServices()->getString("INTERLINKMATERIAL");
}

double 
SCT_BarrelParameters::interLinkDeltaPhi() const
{
  if  (m_rdb->brlServices()->isFieldNull("INTERLINKDPHI")) {
    return 360.*CLHEP::deg;
  }
  return m_rdb->brlServices()->getDouble("INTERLINKDPHI") * CLHEP::deg; 
}

double 
SCT_BarrelParameters::interLinkPhiPos() const
{
  if  (m_rdb->brlServices()->isFieldNull("INTERLINKPHIPOS")) {
    return 0.;
  }
  return m_rdb->brlServices()->getDouble("INTERLINKPHIPOS") * CLHEP::deg;
}

int
SCT_BarrelParameters::interLinkNRepeat() const
{
  if  (m_rdb->brlServices()->isFieldNull("INTERLINKNREPEAT")) {
    return 1;
  }
  return m_rdb->brlServices()->getInt("INTERLINKNREPEAT"); 
}

double 
SCT_BarrelParameters::bearingDeltaPhi() const
{
  if  (m_rdb->brlServices()->isFieldNull("BEARINGDPHI")) {
    return 0.;
  }
  return m_rdb->brlServices()->getDouble("BEARINGDPHI") * CLHEP::deg; 
}

double 
SCT_BarrelParameters::bearingPhiPos() const
{
  if  (m_rdb->brlServices()->isFieldNull("BEARINGPHIPOS")) {
    return 0.;
  }
  return m_rdb->brlServices()->getDouble("BEARINGPHIPOS") * CLHEP::deg;
}

int
SCT_BarrelParameters::bearingNRepeat() const
{
  if  (m_rdb->brlServices()->isFieldNull("BEARINGNREPEAT")) {
    return 0;
  }
  return m_rdb->brlServices()->getInt("BEARINGNREPEAT"); 
}

std::string 
SCT_BarrelParameters::bearingMaterial() const
{
  if  (m_rdb->brlServices()->isFieldNull("BEARINGMATERIAL")) {
    return " ";
  }
  return m_rdb->brlServices()->getString("BEARINGMATERIAL");
}

//
// Barrel FSI and FSI flange
//
bool 
SCT_BarrelParameters::includeFSI() const
{
  if (m_rdb->brlFSISize() > 0) {return true;}
  return false;
}

double 
SCT_BarrelParameters::fsiFlangeInnerRadius() const
{
  return m_rdb->brlFSI()->getDouble("FLANGEINNERRADIUS") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::fsiFlangeOuterRadius() const
{
  return m_rdb->brlFSI()->getDouble("FLANGEOUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::fsiFlangeMaterial() const
{
  return m_rdb->brlFSI()->getString("FLANGEMATERIAL");
}

double 
SCT_BarrelParameters::fsiFibreMaskDeltaR() const
{
  return m_rdb->brlFSI()->getDouble("FIBREMASKDELTAR") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::fsiFibreMaskMaterial() const
{
  return m_rdb->brlFSI()->getString("FIBREMASKMATERIAL");
}

double 
SCT_BarrelParameters::fsiEndJewelRadialWidth() const
{
  return m_rdb->brlFSI()->getDouble("ENDJEWELRADIALWIDTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::fsiEndJewelRPhiWidth() const
{
  return m_rdb->brlFSI()->getDouble("ENDJEWELRPHIWIDTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::fsiEndJewelLength() const
{
  return m_rdb->brlFSI()->getDouble("ENDJEWELLENGTH") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::fsiEndJewelMaterial() const
{
  return m_rdb->brlFSI()->getString("ENDJEWELMATERIAL");
}

int 
SCT_BarrelParameters::fsiEndJewelNRepeat(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getInt("ENDJEWELNREPEAT");
}

double 
SCT_BarrelParameters::fsiEndJewelPhi(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getDouble("ENDJEWELPHI") * CLHEP::degree;
}

double 
SCT_BarrelParameters::fsiEndJewelZ(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getDouble("ENDJEWELZ") * CLHEP::mm;
}

double 
SCT_BarrelParameters::fsiScorpionRadialWidth() const
{
  return m_rdb->brlFSI()->getDouble("SCORPIONRADIALWIDTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::fsiScorpionRPhiWidth() const
{
  return m_rdb->brlFSI()->getDouble("SCORPIONRPHIWIDTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::fsiScorpionLength() const
{
  return m_rdb->brlFSI()->getDouble("SCORPIONLENGTH") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::fsiScorpionMaterial() const
{
  return m_rdb->brlFSI()->getString("SCORPIONMATERIAL");
}

int 
SCT_BarrelParameters::fsiScorpionNRepeat(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getInt("SCORPIONNREPEAT");
}

double 
SCT_BarrelParameters::fsiScorpionPhi(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getDouble("SCORPIONPHI") * CLHEP::degree;
}

double 
SCT_BarrelParameters::fsiScorpionZ(int iLayer) const
{
  return m_rdb->brlFSILocation(iLayer)->getDouble("SCORPIONZ") * CLHEP::mm;
}


//
// Barrel Cooling Spider
//
double 
SCT_BarrelParameters::spiderDeltaZ() const
{
  return m_rdb->brlServices()->getDouble("SPIDERDELTAZ") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::spiderInnerRadius() const
{
  return m_rdb->brlServices()->getDouble("SPIDERINNERRADIUS") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::spiderOuterRadius() const
{
  return m_rdb->brlServices()->getDouble("SPIDEROUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::spiderMaterial() const
{
  return m_rdb->brlServices()->getString("SPIDERMATERIAL");
}

//
// Barrel Thermal Shield
//
double
SCT_BarrelParameters::thermalShieldInnerRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("INNERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldOuterRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("OUTERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldEndZMax() const
{
  return m_rdb->brlThermalShield()->getDouble("ENDZMAX") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldCylTotalThickness() const
{
  return m_rdb->brlThermalShield()->getDouble("CYLTOTALTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldCylInnerWallThickness() const
{
  return m_rdb->brlThermalShield()->getDouble("CYLINNERWALLTHICK") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldCylOuterWallThickness() const
{
  return m_rdb->brlThermalShield()->getDouble("CYLOUTERWALLTHICK") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldSpacerZWidth() const
{
  return m_rdb->brlThermalShield()->getDouble("SPACERZWIDTH") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldFirstSpacerZMin() const
{
  return m_rdb->brlThermalShield()->getDouble("FIRSTSPACERZMIN") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldEndCapCylThickness() const
{
  return m_rdb->brlThermalShield()->getDouble("ENDCAPCYLTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldEndCapThickness() const
{
  return m_rdb->brlThermalShield()->getDouble("ENDCAPTHICKNESS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldBulkheadInnerRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("BULKHEADINNERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldBulkheadOuterRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("BULKHEADOUTERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldEndPanelInnerRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("ENDPANELINNERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::thermalShieldEndPanelOuterRadius() const
{
  return m_rdb->brlThermalShield()->getDouble("ENDPANELOUTERRADIUS") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::thermalShieldMaterialSpacer() const
{
  return m_rdb->brlThermalShield()->getString("MATERIALSPACER");
}

std::string
SCT_BarrelParameters::thermalShieldMaterialCyl() const
{
  return m_rdb->brlThermalShield()->getString("MATERIALCYL");
}

std::string
SCT_BarrelParameters::thermalShieldMaterialOuterSect() const
{
  return m_rdb->brlThermalShield()->getString("MATERIALOUTERSECT");
}

std::string
SCT_BarrelParameters::thermalShieldMaterialInnerSect() const
{
  return m_rdb->brlThermalShield()->getString("MATERIALINNERSECT");
}

//
// Barrel EMI Shield
//
double
SCT_BarrelParameters::emiShieldInnerRadius() const
{
  return m_rdb->brlServices()->getDouble("EMIINNERRADIUS") * CLHEP::mm;
}

double
SCT_BarrelParameters::emiShieldDeltaR() const
{
  return m_rdb->brlServices()->getDouble("EMIDELTAR") * CLHEP::mm;
}

double
SCT_BarrelParameters::emiShieldZMax() const
{
  return m_rdb->brlServices()->getDouble("EMIZMAX") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::emiShieldMaterial() const
{
  return m_rdb->brlServices()->getString("EMIMATERIAL");
}

double
SCT_BarrelParameters::emiJointDeltaR() const
{
  return m_rdb->brlServices()->getDouble("EMIJOINTDELTAR") * CLHEP::mm;
}

double
SCT_BarrelParameters::emiJointRPhi() const
{
  return m_rdb->brlServices()->getDouble("EMIJOINTRPHI") * CLHEP::mm;
}

std::string
SCT_BarrelParameters::emiJointMaterial() const
{
  return m_rdb->brlServices()->getString("EMIJOINTMATERIAL");
}



// 
// Attachment of pixel to SCT.
//
double 
SCT_BarrelParameters::pixelAttachmentInnerRadius() const
{
  return m_rdb->brlServices()->getDouble("PIXELATTACHINNERRAD") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::pixelAttachmentOuterRadius() const
{
  return m_rdb->brlServices()->getDouble("PIXELATTACHOUTERRAD") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::pixelAttachmentZMin() const
{
  return m_rdb->brlServices()->getDouble("PIXELATTACHZMIN") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::pixelAttachmentDeltaZ() const
{
  return m_rdb->brlServices()->getDouble("PIXELATTACHDELTAZ") * CLHEP::mm; 
}

std::string 
SCT_BarrelParameters::pixelAttachmentMaterial() const
{
  return m_rdb->brlServices()->getString("PIXELATTACHMATERIAL");
}

//
// Barrel General
//
int
SCT_BarrelParameters::numLayers() const
{
  return m_rdb->brlGeneral()->getInt("NUMLAYERS"); 
}

double 
SCT_BarrelParameters::barrelInnerRadius() const
{
  return m_rdb->brlGeneral()->getDouble("INNERRADIUS") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::barrelOuterRadius() const
{
  return m_rdb->brlGeneral()->getDouble("OUTERRADIUS") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::barrelLength() const
{
  return m_rdb->brlGeneral()->getDouble("LENGTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::cylinderLength() const
{
  return m_rdb->brlGeneral()->getDouble("CYLINDERLENGTH") * CLHEP::mm; 
}

double 
SCT_BarrelParameters::activeLength() const
{
  return m_rdb->brlGeneral()->getDouble("ACTIVELENGTH") * CLHEP::mm; 
}

bool 
SCT_BarrelParameters::isOldGeometry() const
{
  if (m_rdb->brlGeneral()->isFieldNull("CYLINDERLENGTH")) {return true;}
  return false;
}

