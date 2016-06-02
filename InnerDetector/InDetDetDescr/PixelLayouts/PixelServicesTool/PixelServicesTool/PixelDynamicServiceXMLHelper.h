/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDynamicServiceXMLHelper_H
#define PixelDynamicServiceXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

// XML library
#include "PixelLayoutUtils/GeoXMLUtils.h"

class PixelDynamicServiceXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  PixelDynamicServiceXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics);
  ~PixelDynamicServiceXMLHelper();

  bool routeBarrelStandard() const;
  bool routeEndcapStandard() const;

  double BarrelEOSLength(int index) const;
  double BarrelEOSHalfThick(int index) const;
  double BarrelEOSTolerance(int index) const;
  
  double EndcapEOSLength(int index) const;
  double EndcapEOSRMin(int index) const;
  
  std::string EndcapDiscRoute(int index) const;
  double EndcapEOSOffset(int index) const;

 private:
  bool m_bParsed;
};

#endif

