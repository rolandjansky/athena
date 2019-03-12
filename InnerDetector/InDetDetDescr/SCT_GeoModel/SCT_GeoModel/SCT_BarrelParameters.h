/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_BarrelParameters_H
#define SCT_GeoModel_SCT_BarrelParameters_H

#include <string>

class SCT_DataBase;

class SCT_BarrelParameters {

public:

  // Constructor 
  SCT_BarrelParameters(SCT_DataBase* rdb);

  // Ski
  int    skiFirstStagger() const;
  double skiRadialSep() const;
  int    modulesPerSki() const;
  double skiZPosition(int index) const;
  int    skiModuleIdentifier(int index) const;

  // Layer
  double tilt(int iLayer) const;
  int    layerStereoSign(int iLayer) const;
  double radius(int iLayer) const;
  int    skisPerLayer(int iLayer) const;
  double layerBracketPhiOffset(int iLayer) const;
  double layerPhiOfRefModule(int iLayer) const;

  // Bracket
  double bracketThickness() const;
  double bracketWidth() const;
  double bracketLength() const;
  std::string bracketMaterial() const;

  // Dogleg
  double doglegThickness() const;
  double doglegWidth() const;
  double doglegLength() const;
  std::string doglegMaterial() const;
  double doglegOffsetX() const;
  double doglegOffsetY() const;

  // CoolingBlock
  double coolingBlockThickness() const;
  double coolingBlockWidth() const;
  double coolingBlockLength() const;
  std::string coolingBlockMaterial() const;
  double coolingBlockOffsetX() const;
  double coolingBlockOffsetY() const;
  double coolingBlockOffsetZ() const;

  // CoolingPipe
  double coolingPipeRadius() const;
  std::string coolingPipeMaterial() const;
  double coolingPipeOffsetX() const;
  double coolingPipeOffsetY() const;

  // PowerTape
  double powerTapeThickness() const;
  double powerTapeWidth() const;
  std::string powerTapeMaterial() const;
  double powerTapeStartPointOffset() const;

  // Harness
  double harnessThickness() const;
  double harnessWidth() const;
  std::string harnessMaterial() const;

  // SupportCyl
  double supportCylInnerRadius(int iLayer) const;
  double supportCylOuterRadius(int iLayer) const;
  double supportCylDeltaR(int iLayer) const;
  std::string supportCylMaterial(int iLayer) const;

  // Flange
  double flangeDeltaZ(int iLayer) const;
  double flangeDeltaR(int iLayer) const;
  std::string flangeMaterial(int iLayer) const;

  // Clamp
  double clampDeltaZ(int iLayer) const;
  double clampDeltaR(int iLayer) const;
  std::string clampMaterial(int iLayer) const;

  // Cooling Inlet/outlets
  double coolingEndDeltaR(int iLayer) const;
  std::string coolingEndMaterial(int iLayer) const;

  // CloseOut
  double closeOutDeltaZ(int iLayer) const;
  std::string closeOutMaterial(int iLayer) const;

  // InterLink and B6 bearing
  double interLinkDeltaZ() const;
  double interLinkInnerRadius() const;
  double interLinkOuterRadius() const;
  std::string interLinkMaterial() const;
  double interLinkDeltaPhi() const;
  double interLinkPhiPos() const;
  int interLinkNRepeat() const;
  std::string bearingMaterial() const;
  double bearingDeltaPhi() const;
  double bearingPhiPos() const;
  int bearingNRepeat() const;

  // FSI and FSI flange
  bool includeFSI() const;
  double fsiFlangeInnerRadius() const;
  double fsiFlangeOuterRadius() const;
  std::string fsiFlangeMaterial() const;
  double fsiFibreMaskDeltaR() const;
  std::string fsiFibreMaskMaterial() const;
  double fsiEndJewelRadialWidth() const;
  double fsiEndJewelRPhiWidth() const;
  double fsiEndJewelLength() const;
  std::string fsiEndJewelMaterial() const;
  int fsiEndJewelNRepeat(int iLayer) const;
  double fsiEndJewelPhi(int iLayer) const;
  double fsiEndJewelZ(int iLayer) const;
  double fsiScorpionRadialWidth() const;
  double fsiScorpionRPhiWidth() const;
  double fsiScorpionLength() const;
  std::string fsiScorpionMaterial() const;
  int fsiScorpionNRepeat(int iLayer) const;
  double fsiScorpionPhi(int iLayer) const;
  double fsiScorpionZ(int iLayer) const;

  // Spider
  double spiderDeltaZ() const;
  double spiderInnerRadius() const;
  double spiderOuterRadius() const;
  std::string spiderMaterial() const;

  // Thermal Shield
  double thermalShieldInnerRadius() const;
  double thermalShieldOuterRadius() const;
  double thermalShieldEndZMax() const;
  double thermalShieldCylTotalThickness() const;
  double thermalShieldCylInnerWallThickness() const;
  double thermalShieldCylOuterWallThickness() const;
  double thermalShieldSpacerZWidth() const;
  double thermalShieldFirstSpacerZMin() const;
  double thermalShieldEndCapCylThickness() const;
  double thermalShieldEndCapThickness() const;
  double thermalShieldBulkheadInnerRadius() const;
  double thermalShieldBulkheadOuterRadius() const;
  double thermalShieldEndPanelInnerRadius() const;
  double thermalShieldEndPanelOuterRadius() const;
  std::string thermalShieldMaterialSpacer() const;
  std::string thermalShieldMaterialCyl() const;
  std::string thermalShieldMaterialOuterSect() const;
  std::string thermalShieldMaterialInnerSect() const;

  // EMI Shield (Inner Thermal Shield)
  double emiShieldInnerRadius() const;
  double emiShieldDeltaR() const;
  double emiShieldZMax() const;
  std::string emiShieldMaterial() const;
  double emiJointDeltaR() const;
  double emiJointRPhi() const;
  std::string emiJointMaterial() const;

  // Attachment of pixel to SCT.
  double pixelAttachmentInnerRadius() const;
  double pixelAttachmentOuterRadius() const;
  double pixelAttachmentZMin() const;
  double pixelAttachmentDeltaZ() const;
  std::string pixelAttachmentMaterial() const;

  // Barrel General
  int    numLayers() const;
  double barrelInnerRadius() const;
  double barrelOuterRadius() const;
  double barrelLength() const;
  double cylinderLength() const;
  double activeLength() const;
  bool   isOldGeometry() const;

 private:
  SCT_DataBase * m_rdb;

};


#endif // SCT_GeoModel_SCT_BarrelParameters_H
