/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelRoutingServiceXMLHelper_H
#define PixelRoutingServiceXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

#include "PixelLayoutUtils/GeoXMLUtils.h"

class PixelRoutingServiceXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  PixelRoutingServiceXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics);
  ~PixelRoutingServiceXMLHelper();

  int getRouteNumber() const;
  std::vector<int> getRouteLayerList(int) const;
  bool isBarrelRoute(int) const;

  int getRouteServiceMaterialIndex(int,int) const;

  double getRouteThickness(int) const;
  std::vector<std::string> getRouteRadialPositions(int) const;
  std::vector<std::string> getRouteZPositions(int) const;

  int getEndcapServiceSetIndex(int layer) const;
  int getBarrelServiceSetIndex(int layer, int module) const;
  std::vector<std::string> getServiceSetContent(int index) const;
  std::string getServiceSetName(int index) const;
  std::string getServiceSetNameId(int index) const;
  std::string getRouteType(int index) const;
  std::vector<std::string> getTypeMaterialNames( int layer, std::string pattern);

  std::vector<double> EOScardLength(int iRoute) const;
  double getEOSsvcLength(int iRoute) const;
  double getEOSCardLength(int index) const;
  int getRouteEOSCardIndex(int) const;

  double getZGap(int iRoute) const;


 private:

};

#endif

