/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef PixelRoutingServiceXMLHelper_H
#define PixelRoutingServiceXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

#include "PixelLayoutUtils/GeoXMLUtils.h"

class PixelRoutingServiceXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {
  
 public:
  PixelRoutingServiceXMLHelper(const std::string& envName, const PixelGeoBuilderBasics* basics);
  ~PixelRoutingServiceXMLHelper();
  
  int getRouteNumber() const;
  std::vector<int> getRouteLayerList(int) const;
  bool isBarrelRoute(int) const;

  int getRouteServiceMaterialIndex(int,int) const;

  double getRouteThickness(int) const;
  std::vector<std::string> getRouteRadialPositions(int) const;
  std::vector<std::string> getRouteZPositions(int) const;

  bool isPhiRouting(int index) const;
  bool isPhiRouting(const std::string& ctype, int layer) const;
  int getServiceSetIndex(const std::string& ctype, int layer, int module=0) const;
  std::vector<std::string> getServiceSetContent(int index) const;
  std::string getServiceSetName(int index) const;
  std::string getRouteType(int index) const;
  std::vector<std::string> getTypeMaterialNames( int layer, const std::string& pattern) const;

  std::vector<double> EOScardLength(int iRoute) const;
  double getEOSsvcLength(int iRoute) const;
  double getEOSCardLength(int index) const;
  int getRouteEOSCardIndex(int) const;

  double getZGap(int iRoute) const;
  double getRGap(int iRoute) const;

  double getMaterialFudgeModuleSvc(int iLayer) const;
  double getMaterialFudgeSvcEc(int iLayer) const;
  double getMaterialFudgeGeneric(const std::string& Layer, const std::string& node) const;

  int       getNumSectors(int index) const;
  double getPhiRefFirstSector(int index) const;
  double getSectorVolumeWidth(int index) const;
  bool     splitLayersInPhi(int index) const;
  int   phiSplitStepInSectors(int index) const;
 
 private:

};
#endif
