/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELGEODETMODULEMAP_H
#define GEOPIXELGEODETMODULEMAP_H

#include "Identifier/Identifier.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;
class PixelGeoBuilderBasics;

class GeoDetModulePixelMap : public GeoXMLUtils {

 public:
  GeoDetModulePixelMap();//default constructor
  GeoDetModulePixelMap(const PixelGeoBuilderBasics*, bool bModule=true);

  void preBuild();
  int getModuleIndex(std::string moduleType) const;

 private:
  const PixelGeoBuilderBasics* m_basics;
  bool m_bModule;
  std::map<std::string, int> m_moduleMap;


};

#endif
