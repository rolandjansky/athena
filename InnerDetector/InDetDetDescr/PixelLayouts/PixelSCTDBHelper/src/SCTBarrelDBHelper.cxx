/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSCTDBHelper/SCTBarrelDBHelper.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

using std::abs;

const double SCT_SAFETY = 0.01 * CLHEP::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.



SCTBarrelDBHelper::SCTBarrelDBHelper(const OraclePixGeoAccessor* geoAccessor):
  m_geoAccessor(geoAccessor)
{
  skiTable = geoAccessor->getTable("SctBrlSki");
  skiZTable = geoAccessor->getTable("SctBrlSkiZ");
  layerTable = geoAccessor->getTable("SctBrlLayer");
  srvPerLayerTable = geoAccessor->getTable("SctBrlServPerLayer");
  serviceTable = geoAccessor->getTable("SctBrlServices");
  FSITable = geoAccessor->getTable("SctBrlFSI");
  FSIlocTable = geoAccessor->getTable("SctBrlFSILocation");
  thShieldTable= geoAccessor->getTable("SctBrlThermalShield");
  generalTable= geoAccessor->getTable("SctBrlGeneral");
}

//
// General
//
double 
SCTBarrelDBHelper::safety() const
{
  return SCT_SAFETY;
}


//
// Barrel Ski
//
int 
SCTBarrelDBHelper::skiFirstStagger() const
{
  return m_geoAccessor->getInt(skiTable,"SKIFIRSTSTAGGER");
}

double 
SCTBarrelDBHelper::skiRadialSep() const
{
  return m_geoAccessor->getDouble(skiTable,"SKIRADIALSEP") * CLHEP::mm;
}

int
SCTBarrelDBHelper::modulesPerSki() const
{
  //  return m_rdb->brlSkiZSize();
  return m_geoAccessor->getTableSize(skiZTable);
}

double 
SCTBarrelDBHelper::skiZPosition(int index) const
{
  return m_geoAccessor->getDouble(skiZTable,"ZPOSITION",index) * CLHEP::mm;
}

int 
SCTBarrelDBHelper::skiModuleIdentifier(int index) const
{
  return  m_geoAccessor->getInt(skiZTable,"MODULEID",index);
}

//
// Barrel Layer
//
double 
SCTBarrelDBHelper::tilt(int iLayer) const
{
  return m_geoAccessor->getDouble(layerTable,"TILT",iLayer) * CLHEP::degree;
}

int 
SCTBarrelDBHelper::layerStereoSign(int iLayer) const
{
  return m_geoAccessor->getInt(layerTable,"STEREOSIGN",iLayer);
}


double 
SCTBarrelDBHelper::radius(int iLayer) const
{
  return m_geoAccessor->getDouble(layerTable,"RADIUS",iLayer) * CLHEP::mm;
}

int 
SCTBarrelDBHelper::skisPerLayer(int iLayer) const
{
  return m_geoAccessor->getInt(layerTable,"SKISPERLAYER",iLayer);
}

double 
SCTBarrelDBHelper::layerBracketPhiOffset(int iLayer) const
{
  return m_geoAccessor->getDouble(layerTable,"BRACKETPHIOFFSET",iLayer) * CLHEP::deg;
}

double 
SCTBarrelDBHelper::layerPhiOfRefModule(int iLayer) const
{
  // For backward compatibility, if field is null return (90 - tilt) 
//   // as ref module is horizontal in old versions.
//   if  (m_rdb->brlLayer(iLayer)->isFieldNull("PHIOFREFMODULE")) {
//     return 90*CLHEP::deg - tilt(iLayer);
//   }
  try{
  return m_geoAccessor->getDouble(layerTable,"PHIOFREFMODULE",iLayer) * CLHEP::deg;
  }
  catch(...) {
    return 90*CLHEP::deg - tilt(iLayer);
  }
}


//
// Barrel Bracket
//
double
SCTBarrelDBHelper::bracketThickness() const
{
  return m_geoAccessor->getDouble(skiTable,"BRACKETTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::bracketWidth() const
{
  return m_geoAccessor->getDouble(skiTable,"BRACKETWIDTH") * CLHEP::mm;
}
 
double
SCTBarrelDBHelper::bracketLength() const
{
  return m_geoAccessor->getDouble(skiTable,"BRACKETLENGTH") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::bracketMaterial() const
{
  return m_geoAccessor->getString(skiTable,"BRACKETMATERIAL");
}

//
// Barrel Dogleg
//
double
SCTBarrelDBHelper::doglegThickness() const
{
  return m_geoAccessor->getDouble(skiTable,"DOGLEGTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::doglegWidth() const
{
  return m_geoAccessor->getDouble(skiTable,"DOGLEGWIDTH") * CLHEP::mm;
}
 
double
SCTBarrelDBHelper::doglegLength() const
{
  return m_geoAccessor->getDouble(skiTable,"DOGLEGLENGTH") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::doglegMaterial() const
{
  return m_geoAccessor->getString(skiTable,"DOGLEGMATERIAL");
}

double
SCTBarrelDBHelper::doglegOffsetX() const
{
  return m_geoAccessor->getDouble(skiTable,"DOGLEGOFFSETX") * CLHEP::mm;
}

double
SCTBarrelDBHelper::doglegOffsetY() const
{
  return m_geoAccessor->getDouble(skiTable,"DOGLEGOFFSETY") * CLHEP::mm;
}

//
// Barrel CoolingBlock
//
double
SCTBarrelDBHelper::coolingBlockThickness() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKTHICK") * CLHEP::mm;
}

double
SCTBarrelDBHelper::coolingBlockWidth() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKWIDTH") * CLHEP::mm;
}
 
double
SCTBarrelDBHelper::coolingBlockLength() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKLENGTH") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::coolingBlockMaterial() const
{
  return m_geoAccessor->getString(skiTable,"COOLINGBLOCKMATERIAL");
}

double
SCTBarrelDBHelper::coolingBlockOffsetX() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKOFFSETX") * CLHEP::mm;
}

double
SCTBarrelDBHelper::coolingBlockOffsetY() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKOFFSETY") * CLHEP::mm;
}

double
SCTBarrelDBHelper::coolingBlockOffsetZ() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGBLOCKOFFSETZ") * CLHEP::mm;
}

//
// Barrel CoolingPipe
//
double
SCTBarrelDBHelper::coolingPipeRadius() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGPIPERADIUS") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::coolingPipeMaterial() const
{
  return m_geoAccessor->getString(skiTable,"COOLINGPIPEMATERIAL");
}

double
SCTBarrelDBHelper::coolingPipeOffsetX() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGPIPEOFFSETX") * CLHEP::mm;
}

double
SCTBarrelDBHelper::coolingPipeOffsetY() const
{
  return m_geoAccessor->getDouble(skiTable,"COOLINGPIPEOFFSETY") * CLHEP::mm;
}


//
// Barrel PowerTape
//
double
SCTBarrelDBHelper::powerTapeThickness() const
{
  return m_geoAccessor->getDouble(skiTable,"POWERTAPETHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::powerTapeWidth() const
{
  return m_geoAccessor->getDouble(skiTable,"POWERTAPEWIDTH") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::powerTapeMaterial() const
{ 
  return m_geoAccessor->getString(skiTable,"POWERTAPEMATERIAL");
}

double
SCTBarrelDBHelper::powerTapeStartPointOffset() const
{
  return m_geoAccessor->getDouble(skiTable,"POWERTAPESTARTOFFSET") * CLHEP::mm;
}
 
//
// Barrel Harness
//
double
SCTBarrelDBHelper::harnessThickness() const
{
  return m_geoAccessor->getDouble(skiTable,"HARNESSTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::harnessWidth() const
{
  return m_geoAccessor->getDouble(skiTable,"HARNESSWIDTH") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::harnessMaterial() const
{ 
  return m_geoAccessor->getString(skiTable,"HARNESSMATERIAL");
}

//
// Barrel SupportCyl
//
double 
SCTBarrelDBHelper::supportCylInnerRadius(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"SUPPORTCYLINNERRAD",iLayer) * CLHEP::mm;
}

double 
SCTBarrelDBHelper::supportCylOuterRadius(int iLayer) const
{
  return supportCylInnerRadius(iLayer) + supportCylDeltaR(iLayer);
}

double 
SCTBarrelDBHelper::supportCylDeltaR(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"SUPPORTCYLDELTAR",iLayer) * CLHEP::mm;
}

std::string 
SCTBarrelDBHelper::supportCylMaterial(int iLayer) const
{
  return m_geoAccessor->getString(srvPerLayerTable,"SUPPORTCYLMATERIAL",iLayer);
}


//
// Barrel Flange
//
double 
SCTBarrelDBHelper::flangeDeltaZ(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"FLANGEDELTAZ",iLayer) * CLHEP::mm;
}

double 
SCTBarrelDBHelper::flangeDeltaR(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"FLANGEDELTAR",iLayer) * CLHEP::mm;
}

std::string 
SCTBarrelDBHelper::flangeMaterial(int iLayer) const
{
  return m_geoAccessor->getString(srvPerLayerTable,"FLANGEMATERIAL",iLayer);
}

//
// Barrel Clamp
//
double 
SCTBarrelDBHelper::clampDeltaZ(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"CLAMPDELTAZ",iLayer) * CLHEP::mm;
}

double 
SCTBarrelDBHelper::clampDeltaR(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"CLAMPDELTAR",iLayer) * CLHEP::mm;
}

std::string 
SCTBarrelDBHelper::clampMaterial(int iLayer) const
{
  return m_geoAccessor->getString(srvPerLayerTable,"CLAMPMATERIAL",iLayer);
}

//
// Barrel Cooling Inlet/outlets
//
double 
SCTBarrelDBHelper::coolingEndDeltaR(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"COOLINGENDDELTAR",iLayer) * CLHEP::mm;
}

std::string 
SCTBarrelDBHelper::coolingEndMaterial(int iLayer) const
{
  return m_geoAccessor->getString(srvPerLayerTable,"COOLINGENDMATERIAL",iLayer);
}

//
// Barrel CloseOut
//
double 
SCTBarrelDBHelper::closeOutDeltaZ(int iLayer) const
{
  return m_geoAccessor->getDouble(srvPerLayerTable,"CLOSEOUTDELTAZ",iLayer) * CLHEP::mm;
}

std::string 
SCTBarrelDBHelper::closeOutMaterial(int iLayer) const
{
  return m_geoAccessor->getString(srvPerLayerTable,"CLOSEOUTMATERIAL",iLayer);
}

//
// Barrel InterLink and B6 bearing
//
double 
SCTBarrelDBHelper::interLinkDeltaZ() const
{
  return m_geoAccessor->getDouble(serviceTable,"INTERLINKDELTAZ") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::interLinkInnerRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"INTERLINKINNERRADIUS") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::interLinkOuterRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"INTERLINKOUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::interLinkMaterial() const
{
  return m_geoAccessor->getString(serviceTable,"INTERLINKMATERIAL");
}

double 
SCTBarrelDBHelper::interLinkDeltaPhi() const
{
//   if  (m_rdb->brlServices()->isFieldNull("INTERLINKDPHI")) {
//     return 360.*CLHEP::deg;
//   }
  try{
    return m_geoAccessor->getDouble(serviceTable,"INTERLINKDPHI") * CLHEP::deg; 
  }
  catch(...){
    return 360.*CLHEP::deg;
  }
}

double 
SCTBarrelDBHelper::interLinkPhiPos() const
{
//   if  (m_rdb->brlServices()->isFieldNull("INTERLINKPHIPOS")) {
//     return 0.;
//   }
  try{
    return m_geoAccessor->getDouble(serviceTable,"INTERLINKPHIPOS") * CLHEP::deg;
  }
  catch(...){
    return 0.*CLHEP::deg;
  }
}

int
SCTBarrelDBHelper::interLinkNRepeat() const
{
//   if  (m_rdb->brlServices()->isFieldNull("INTERLINKNREPEAT")) {
//     return 1;
//   }
  try{
  return m_geoAccessor->getInt(serviceTable,"INTERLINKNREPEAT"); 
  }
  catch(...){
    return 1;
  }
}

double 
SCTBarrelDBHelper::bearingDeltaPhi() const
{
//   if  (m_rdb->brlServices()->isFieldNull("BEARINGDPHI")) {
//     return 0.;
//   }
  try{
    return m_geoAccessor->getDouble(serviceTable,"BEARINGDPHI") * CLHEP::deg; 
  }
  catch(...){
    return 0.;
  }
}

double 
SCTBarrelDBHelper::bearingPhiPos() const
{
//   if  (m_rdb->brlServices()->isFieldNull("BEARINGPHIPOS")) {
//     return 0.;
//   }
  try{
    return m_geoAccessor->getDouble(serviceTable,"BEARINGPHIPOS") * CLHEP::deg;
  }
  catch(...){
    return 0.;
  }
}

int
SCTBarrelDBHelper::bearingNRepeat() const
{
//   if  (m_rdb->brlServices()->isFieldNull("BEARINGNREPEAT")) {
//     return 0;
//   }
  try{
    return m_geoAccessor->getInt(serviceTable,"BEARINGNREPEAT"); 
  }
  catch(...){
    return 0.;
  }
}

std::string 
SCTBarrelDBHelper::bearingMaterial() const
{
//   if  (m_rdb->brlServices()->isFieldNull("BEARINGMATERIAL")) {
//     return " ";
//   }
  try{
    return m_geoAccessor->getString(serviceTable,"BEARINGMATERIAL");
  }
  catch(...){
    return " ";
  }
}

//
// Barrel FSI and FSI flange
//
bool 
SCTBarrelDBHelper::includeFSI() const
{
  //  if (m_rdb->brlFSISize() > 0) {return true;}
  //  return false;

  if (m_geoAccessor->getTableSize(FSITable) > 0) {return true;}
  return false;
}

double 
SCTBarrelDBHelper::fsiFlangeInnerRadius() const
{
  return m_geoAccessor->getDouble(FSITable,"FLANGEINNERRADIUS") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::fsiFlangeOuterRadius() const
{
  return m_geoAccessor->getDouble(FSITable,"FLANGEOUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::fsiFlangeMaterial() const
{
  return m_geoAccessor->getString(FSITable,"FLANGEMATERIAL");
}

double 
SCTBarrelDBHelper::fsiFibreMaskDeltaR() const
{
  return m_geoAccessor->getDouble(FSITable,"FIBREMASKDELTAR") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::fsiFibreMaskMaterial() const
{
  return m_geoAccessor->getString(FSITable,"FIBREMASKMATERIAL");
}

double 
SCTBarrelDBHelper::fsiEndJewelRadialWidth() const
{
  return m_geoAccessor->getDouble(FSITable,"ENDJEWELRADIALWIDTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::fsiEndJewelRPhiWidth() const
{
  return m_geoAccessor->getDouble(FSITable,"ENDJEWELRPHIWIDTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::fsiEndJewelLength() const
{
  return m_geoAccessor->getDouble(FSITable,"ENDJEWELLENGTH") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::fsiEndJewelMaterial() const
{
  return m_geoAccessor->getString(FSITable,"ENDJEWELMATERIAL");
}

int 
SCTBarrelDBHelper::fsiEndJewelNRepeat(int iLayer) const
{
  return m_geoAccessor->getInt(FSIlocTable,"ENDJEWELNREPEAT",iLayer);
}

double 
SCTBarrelDBHelper::fsiEndJewelPhi(int iLayer) const
{
  return m_geoAccessor->getDouble(FSIlocTable,"ENDJEWELPHI",iLayer) * CLHEP::degree;
}

double 
SCTBarrelDBHelper::fsiEndJewelZ(int iLayer) const
{
  return m_geoAccessor->getDouble(FSIlocTable,"ENDJEWELZ",iLayer) * CLHEP::mm;
}

double 
SCTBarrelDBHelper::fsiScorpionRadialWidth() const
{
  return m_geoAccessor->getDouble(FSITable,"SCORPIONRADIALWIDTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::fsiScorpionRPhiWidth() const
{
  return m_geoAccessor->getDouble(FSITable,"SCORPIONRPHIWIDTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::fsiScorpionLength() const
{
  return m_geoAccessor->getDouble(FSITable,"SCORPIONLENGTH") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::fsiScorpionMaterial() const
{
  return m_geoAccessor->getString(FSITable,"SCORPIONMATERIAL");
}

int 
SCTBarrelDBHelper::fsiScorpionNRepeat(int iLayer) const
{
  return m_geoAccessor->getInt(FSIlocTable,"SCORPIONNREPEAT",iLayer);
}

double 
SCTBarrelDBHelper::fsiScorpionPhi(int iLayer) const
{
  return m_geoAccessor->getDouble(FSIlocTable,"SCORPIONPHI",iLayer) * CLHEP::degree;
}

double 
SCTBarrelDBHelper::fsiScorpionZ(int iLayer) const
{
  return m_geoAccessor->getDouble(FSIlocTable,"SCORPIONZ",iLayer) * CLHEP::mm;
}


//
// Barrel Cooling Spider
//
double 
SCTBarrelDBHelper::spiderDeltaZ() const
{
  return m_geoAccessor->getDouble(serviceTable,"SPIDERDELTAZ") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::spiderInnerRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"SPIDERINNERRADIUS") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::spiderOuterRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"SPIDEROUTERRADIUS") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::spiderMaterial() const
{
  return m_geoAccessor->getString(serviceTable,"SPIDERMATERIAL");
}

//
// Barrel Thermal Shield
//
double
SCTBarrelDBHelper::thermalShieldInnerRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"INNERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldOuterRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"OUTERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldEndZMax() const
{
  return m_geoAccessor->getDouble(thShieldTable,"ENDZMAX") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldCylTotalThickness() const
{
  return m_geoAccessor->getDouble(thShieldTable,"CYLTOTALTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldCylInnerWallThickness() const
{
  return m_geoAccessor->getDouble(thShieldTable,"CYLINNERWALLTHICK") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldCylOuterWallThickness() const
{
  return m_geoAccessor->getDouble(thShieldTable,"CYLOUTERWALLTHICK") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldSpacerZWidth() const
{
  return m_geoAccessor->getDouble(thShieldTable,"SPACERZWIDTH") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldFirstSpacerZMin() const
{
  return m_geoAccessor->getDouble(thShieldTable,"FIRSTSPACERZMIN") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldEndCapCylThickness() const
{
  return m_geoAccessor->getDouble(thShieldTable,"ENDCAPCYLTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldEndCapThickness() const
{
  return m_geoAccessor->getDouble(thShieldTable,"ENDCAPTHICKNESS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldBulkheadInnerRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"BULKHEADINNERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldBulkheadOuterRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"BULKHEADOUTERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldEndPanelInnerRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"ENDPANELINNERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::thermalShieldEndPanelOuterRadius() const
{
  return m_geoAccessor->getDouble(thShieldTable,"ENDPANELOUTERRADIUS") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::thermalShieldMaterialSpacer() const
{
  return m_geoAccessor->getString(thShieldTable,"MATERIALSPACER");
}

std::string
SCTBarrelDBHelper::thermalShieldMaterialCyl() const
{
  return m_geoAccessor->getString(thShieldTable,"MATERIALCYL");
}

std::string
SCTBarrelDBHelper::thermalShieldMaterialOuterSect() const
{
  return m_geoAccessor->getString(thShieldTable,"MATERIALOUTERSECT");
}

std::string
SCTBarrelDBHelper::thermalShieldMaterialInnerSect() const
{
  return m_geoAccessor->getString(thShieldTable,"MATERIALINNERSECT");
}

//
// Barrel EMI Shield
//
double
SCTBarrelDBHelper::emiShieldInnerRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"EMIINNERRADIUS") * CLHEP::mm;
}

double
SCTBarrelDBHelper::emiShieldDeltaR() const
{
  return m_geoAccessor->getDouble(serviceTable,"EMIDELTAR") * CLHEP::mm;
}

double
SCTBarrelDBHelper::emiShieldZMax() const
{
  return m_geoAccessor->getDouble(serviceTable,"EMIZMAX") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::emiShieldMaterial() const
{
  return m_geoAccessor->getString(serviceTable,"EMIMATERIAL");
}

double
SCTBarrelDBHelper::emiJointDeltaR() const
{
  return m_geoAccessor->getDouble(serviceTable,"EMIJOINTDELTAR") * CLHEP::mm;
}

double
SCTBarrelDBHelper::emiJointRPhi() const
{
  return m_geoAccessor->getDouble(serviceTable,"EMIJOINTRPHI") * CLHEP::mm;
}

std::string
SCTBarrelDBHelper::emiJointMaterial() const
{
  return m_geoAccessor->getString(serviceTable,"EMIJOINTMATERIAL");
}



// 
// Attachment of pixel to SCT.
//
double 
SCTBarrelDBHelper::pixelAttachmentInnerRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"PIXELATTACHINNERRAD") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::pixelAttachmentOuterRadius() const
{
  return m_geoAccessor->getDouble(serviceTable,"PIXELATTACHOUTERRAD") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::pixelAttachmentZMin() const
{
  return m_geoAccessor->getDouble(serviceTable,"PIXELATTACHZMIN") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::pixelAttachmentDeltaZ() const
{
  return m_geoAccessor->getDouble(serviceTable,"PIXELATTACHDELTAZ") * CLHEP::mm; 
}

std::string 
SCTBarrelDBHelper::pixelAttachmentMaterial() const
{
  return m_geoAccessor->getString(serviceTable,"PIXELATTACHMATERIAL");
}

//
// Barrel General
//
int
SCTBarrelDBHelper::numLayers() const
{
  return m_geoAccessor->getInt(generalTable,"NUMLAYERS"); 
}

double 
SCTBarrelDBHelper::barrelInnerRadius() const
{
  return m_geoAccessor->getDouble(generalTable,"INNERRADIUS") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::barrelOuterRadius() const
{
  return m_geoAccessor->getDouble(generalTable,"OUTERRADIUS") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::barrelLength() const
{
  return m_geoAccessor->getDouble(generalTable,"LENGTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::cylinderLength() const
{
  return m_geoAccessor->getDouble(generalTable,"CYLINDERLENGTH") * CLHEP::mm; 
}

double 
SCTBarrelDBHelper::activeLength() const
{
  return m_geoAccessor->getDouble(generalTable,"ACTIVELENGTH") * CLHEP::mm; 
}

bool 
SCTBarrelDBHelper::isOldGeometry() const
{
  try{
    if (m_geoAccessor->getDouble(generalTable,"CYLINDERLENGTH")) {return false;}
  }
  catch(...){
    return true;
  }
  return true;
}

