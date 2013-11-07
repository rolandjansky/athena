/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_BarrelParameters_H
#define SCT_SLHC_GeoModel_SCT_BarrelParameters_H

#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"

#include <string>
#include <map>
#include <vector>

#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDDSLHC {

class SCT_DataBase;

class SCT_BarrelParameters : public SCT_ParametersBase  {

public:

  // Constructor 
  SCT_BarrelParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  virtual ~SCT_BarrelParameters();

  virtual int layoutType() const;
  // Ski
  virtual int    skiFirstStagger() const;
  virtual double skiRadialSep(int iLayer) const;
  //---int    modulesPerSki(int iLayer) const;
  virtual double skiZPosition(int iLayer, int module) const;
  virtual int    skiModuleIdentifier(int iLayer, int index) const;

  // Layer
  virtual double materialIncreaseFactor(int iLayer) const ;
  virtual double tilt(int iLayer) const;
  virtual int    layerStereoSign(int iLayer) const;
  virtual double radius(int iLayer) const;
  virtual double cylLength(int iLayer) const;
  virtual double cylInnerZMin(int iLayer) const;
  virtual bool   doubleSided(int iLayer) const;
  virtual int    moduleType(int iLayer) const;
  virtual int    modulesPerSki(int iLayer) const;
  virtual int    skisPerLayer(int iLayer) const;
  virtual int   staveLayout(int iLayer) const;
  virtual double stereoOuter(int iLayer) const;
  virtual double stereoInner(int iLayer) const;
  virtual double staveSupportWidth(int ilayer) const;
  virtual double staveSupportThickness(int ilayer) const;
  virtual std::string staveSupportMaterial(int ilayer) const;
  //virtual double layerBracketPhiOffset(int iLayer) const;
  
  // SupportCyl
  virtual double supportCylInnerRadius(int iLayer) const;
  virtual double supportCylOuterRadius(int iLayer) const;
  virtual std::string supportCylMaterial(int iLayer) const;
  
  // Barrel General
  virtual int    numLayers() const;
  virtual double barrelInnerRadius() const;
  virtual double barrelIntermediateRadius() const;
  virtual double barrelOuterRadius() const;
  virtual double barrelLength() const;
  virtual double barrelIntermediateLength() const;
  virtual double barrelServicesMaterialCylinderLength() const;
  virtual double barrelServicesMaterialIncreaseFactor() const;
  virtual std::string barrelServicesMaterial() const;
 private:
  int    skiZPositionType(int iLayer) const;
  int    ladderType(int iLayer) const;
  int    getNumSegments(int iLayer) const;

  std::map<int, std::vector<double> *> m_zpositionMap;
  std::map<int, std::vector<int> *> m_moduleIdMap;

  IRDBRecordset_ptr SctBrlSensor;
  IRDBRecordset_ptr SctBrlLayer;
  //IRDBRecordset_ptr SctBrlSki; 
  IRDBRecordset_ptr SctBrlLadder; 
  IRDBRecordset_ptr SctBrlServPerLayer; 
  IRDBRecordset_ptr SctBrlSkiZ;
  IRDBRecordset_ptr SctBrlGeneral;

  
};

}
#endif // SCT_SLHC_GeoModel_SCT_BarrelParameters_H
