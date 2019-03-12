/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_ForwardParameters_H
#define SCT_GeoModel_SCT_ForwardParameters_H

#include <memory>
#include <string>
#include <vector>

class SCT_DataBase;
class FSIHelper;
class FSIDetails;

class SCT_ForwardParameters {

public:

  // Constructor and destructor 
  SCT_ForwardParameters(SCT_DataBase* rdb);
  ~SCT_ForwardParameters();

  //////////////////////////////////////////////////////////////////////////////
  // Forward SCT
  //////////////////////////////////////////////////////////////////////////////

  // Ring
  //double fwdRingModuleToDiscCenter() const;
  int    fwdRingNumModules(int iRing) const;
  double fwdRingModuleStagger(int iRing) const;
  double fwdRingPhiOfRefModule(int iRing) const;
  int    fwdRingStaggerOfRefModule(int iRing) const;
  int    fwdRingUsualRingSide(int iRing) const;
  double fwdRingDistToDiscCenter(int iRin) const;
  
  

  // Wheel
  double fwdWheelZPosition(int iWheel) const;
  int    fwdWheelStereoType(int iWheel) const;
  int    fwdWheelNumRings(int iWheel) const;
  
  int    fwdWheelRingMapIndex(int iWheel, int iRing, int ec) const;
  int    fwdWheelRingMapIndexDB(int iWheelDB, int iRing) const;
  int    fwdRingStaggerOfRefModule(int iWheel, int iRing, int ec) const;
  int    fwdWheelRingSide(int iWheel, int iRing, int ec) const;
  int    fwdWheelModuleType(int iWheel, int iRing, int ec) const;

  // DiscSupport
  double fwdDiscSupportInnerRadius() const;
  double fwdDiscSupportOuterRadius() const;
  double fwdDiscSupportThickness() const;
  std::string fwdDiscSupportMaterial() const;

  // PatchPanel
  int    fwdNumPatchPanelLocs() const;
  int    fwdPatchPanelType(int iLoc) const;
  double fwdPatchPanelLocAngle(int iLoc) const;
  bool   fwdPatchPanelRepeatQuadrant(int iLoc) const;
  int    fwdNumPatchPanelTypes() const;
  double fwdPatchPanelThickness(int iType) const;
  double fwdPatchPanelMidRadius(int iType) const;
  double fwdPatchPanelDeltaR(int iType) const;
  double fwdPatchPanelRPhi(int iType) const;
  std::string fwdPatchPanelMaterial(int iType) const;

  // PatchPanel Connector
  bool   fwdPPConnectorPresent() const;
  double fwdPPConnectorThickness() const;
  double fwdPPConnectorDeltaR() const;
  double fwdPPConnectorRPhi() const;
  std::string fwdPPConnectorMaterial() const;
  
  // PatchPanel Cooling
  bool   fwdPPCoolingPresent() const;
  double fwdPPCoolingThickness() const;
  double fwdPPCoolingDeltaR() const;
  double fwdPPCoolingRPhi() const;
  std::string fwdPPCoolingMaterial() const;

  // Cooling Block
  int    fwdCoolingBlockHiLo(int iType) const;
  int    fwdCoolingBlockMainOrSecondary(int iType) const;
  double fwdCoolingBlockDeltaR(int iType) const;
  double fwdCoolingBlockRPhi(int iType) const;
  double fwdCoolingBlockThickness(int iType) const;
  double fwdCoolingBlockOffsetFromDisc(int iType) const;
  std::string fwdCoolingBlockMaterial(int iType) const;

  // DiscPowerTape
  double fwdDiscPowerTapeInnerRadius(int iRing) const;
  double fwdDiscPowerTapeOuterRadius(int iRing) const;
  double fwdDiscPowerTapeThickness(int iRing) const;
  std::string fwdDiscPowerTapeMaterial(int iRing) const;

  // RingCooling
  double fwdRingCoolingInnerRadius(int iRing) const;
  double fwdRingCoolingOuterRadius(int iRing) const;
  double fwdRingCoolingThickness(int iRing) const;
  std::string fwdRingCoolingMaterial(int iRing) const;

  // Disc Fixation
  bool   fwdDiscFixationPresent() const;
  double fwdDiscFixationThickness() const;
  double fwdDiscFixationRadius() const;
  std::string fwdDiscFixationMaterial() const;

  // Support Frame
  double fwdSupportFrameRadialThickness() const; 
  double fwdSupportFrameInnerRadius() const; 
  double fwdSupportFrameZMin() const; 
  double fwdSupportFrameZMax() const; 
  std::string fwdSupportFrameMaterial() const;

  // CoolingPipe
  double fwdCoolingPipeRadius() const;
  std::string fwdCoolingPipeMaterial() const;
 
  // PowerTape
  double fwdPowerTapeCrossSectArea() const;
  std::string fwdPowerTapeMaterial() const;

  // FSI
  //int fwdFSINumLocTypes() const;
  //std::string fwdFSILocType(int iLocIndex) const;
  //double fwdFSILocRadius(int iLocIndex) const;
  //double fwdFSILocPhi(int iLocIndex) const;
  int  fwdFSINumGeomTypes() const;
  double fwdFSIGeomDeltaR(int iType) const;
  double fwdFSIGeomRPhi(int iType) const;
  double fwdFSIGeomThickness(int iType) const;
  std::string fwdFSIGeomMaterial(int iType) const;
  double fwdFSIGeomZOffset(int iType) const;

  //int fwdFSIWheel(int index) const;
  //std::string fwdFSILocation(int index) const;
  //int fwdFSISimType(int index) const;
  const std::vector<const FSIDetails *> & fsiVector(int iWheel) const;
  const FSIHelper & fsiHelper() const;

  // Cylinder Services
  bool   fwdCylinderServicePresent() const;
  int    fwdNumCylinderServiceLocs() const;
  std::string fwdCylinderServiceLocName(int iLoc) const;
  double fwdCylinderServiceLocAngle(int iLoc) const;
  int    fwdNumCylinderServiceTypes() const;
  std::string fwdCylinderServiceName(int iType) const;
  std::string fwdCylinderServiceMaterial(int iType) const;
  double fwdCylinderServiceDeltaR(int iType) const;
  double fwdCylinderServiceRPhi(int iType) const;

  // Thermal Shield
  int    fwdNumThermalShieldElements() const;
  std::string fwdThermalShieldMaterial(int iElement) const;
  double fwdThermalShieldInnerRadius(int iElement) const; 
  double fwdThermalShieldOuterRadius(int iElement) const; 
  double fwdThermalShieldZMin(int iElement) const; 
  double fwdThermalShieldZMax(int iElement) const; 
 
  // Forward General
  int    fwdNumModuleTypes() const;
  int    fwdNumWheels() const;
  double fwdInnerRadius() const;
  double fwdOuterRadius() const;
  double fwdZMin() const;
  double fwdZMax() const;
  double fwdTrtGapPos() const;

  // OptoHarness
  bool   fwdOptoHarnessPresent() const;
  int    fwdOptoHarnessDiscType(int itype) const;
  double fwdOptoHarnessInnerRadius(int itype) const;
  double fwdOptoHarnessOuterRadius(int itype) const;
  double fwdOptoHarnessThickness(int itype) const;
  std::string fwdOptoHarnessMaterial(int itype) const;

  SCT_ForwardParameters& operator= (const SCT_ForwardParameters& right);
  SCT_ForwardParameters(const SCT_ForwardParameters& right);

private:

  SCT_DataBase * m_rdb;

  std::unique_ptr<FSIHelper> m_fsiHelper;

};


#endif // SCT_GeoModel_SCT_ForwardParameters_H
