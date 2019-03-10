/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_ForwardModuleParameters_H
#define SCT_GeoModel_SCT_ForwardModuleParameters_H

#include <string>

class SCT_DataBase;

class SCT_ForwardModuleParameters {

public:

  // Constructor 
  SCT_ForwardModuleParameters(SCT_DataBase* rdb);

  // Sensor
  // For Inner module there is only one sensor,
  // the "Far" sensor is only defined in that case.

  int    fwdSensorNumWafers(int iModuleType) const;
  double fwdSensorThickness(int iModuleType) const;
  double fwdSensorInnerWidthNear(int iModuleType) const;
  double fwdSensorInnerWidthFar(int iModuleType) const;
  double fwdSensorOuterWidthNear(int iModuleType) const;
  double fwdSensorOuterWidthFar(int iModuleType) const;
  double fwdSensorLengthNear(int iModuleType) const;
  double fwdSensorLengthFar(int iModuleType) const;
  double fwdSensorRadiusNear(int iModuleType) const;
  double fwdSensorRadiusFar(int iModuleType) const;
  std::string fwdSensorMaterialNear(int iModuleType) const;
  std::string fwdSensorMaterialFar(int iModuleType) const;
  bool fwdSensorActiveFar(int iModuleType) const;
  bool fwdSensorActiveNear(int iModuleType) const;

  // Sensor 
  // Pitch and readout strips.
  double fwdSensorActiveHalfLengthNear(int iModuleType) const;
  double fwdSensorActiveHalfLengthFar(int iModuleType) const;
  double fwdSensorAngularPitch(int iModuleType) const;
  int    fwdSensorNumStrips(int iModuleType) const;
  int    fwdSensorNumReadoutStrips(int iModuleType) const;
  int    fwdSensorStripShift(int iModuleType) const;

  // Hybrid
  double fwdHybridThickness() const;
  double fwdHybridInnerWidth() const;
  double fwdHybridOuterWidth() const;
  double fwdHybridLength() const;
  double fwdHybridLengthToCorner() const;
  double fwdHybridMountPointToInnerEdge() const;
  std::string fwdHybridMaterial() const;

  // Spine
  double fwdSpineThickness(int iModuleType) const;
  double fwdSpineWidth(int iModuleType) const;
  double fwdSpineEndToModuleCenter(int iModuleType) const;
  double fwdSpineEndLocatorToEndMount(int iModuleType) const;
  std::string fwdSpineMaterial(int iModuleType) const;

  // SubSpine
  double fwdSubSpineInnerWidth(int iModuleType) const;
  double fwdSubSpineInnerLength(int iModuleType) const;
  double fwdSubSpineInnerRefDist(int iModuleType) const;
  double fwdSubSpineMiddleWidth(int iModuleType) const;
  double fwdSubSpineMiddleLength(int iModuleType) const;
  double fwdSubSpineMiddleRefDist(int iModuleType) const;
  double fwdSubSpineOuterWidth(int iModuleType) const;
  double fwdSubSpineOuterLength(int iModuleType) const;
  double fwdSubSpineOuterRefDist(int iModuleType) const;
  std::string fwdSubSpineMaterial(int iModuleType) const;


  // Module
  int    fwdModuleNumTypes() const;
  double fwdModuleStereoAngle(int iModuleType) const;
  int    fwdModuleStereoUpperSign(int iModuleType) const;
  int    fwdModuleUpperSideNumber(int iModuleType) const;
  double fwdModuleGlueThickness(int iModuleType) const;
  double fwdModuleMountPoint(int iModuleType) const;
  double fwdModuleDistBtwMountPoints(int iModuleType) const;
  double fwdModuleHybridEdgeToSpine(int iModuleType) const;
  bool   fwdHybridIsOnInnerEdge(int iModuleType) const;

  // Module Connectors
  bool   fwdModuleConnectorPresent() const;
  double fwdModuleConnectorDeltaR() const;
  double fwdModuleConnectorRPhi() const;
  double fwdModuleConnectorThickness() const;
  std::string fwdModuleConnectorMaterial() const;

private:

  SCT_DataBase * m_rdb;

};


#endif // SCT_GeoModel_SCT_ForwardModuleParameters_H
