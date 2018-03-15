/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoPixelXMLMaterial_H
#define GeoPixelXMLMaterial_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "GeoModelKernel/GeoPhysVol.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class GeoPixelXMLMaterial : public GeoXMLUtils, public PixelGeoBuilder {

public:  
  GeoPixelXMLMaterial(const PixelGeoBuilderBasics* basics, std::string fileName);
  virtual GeoVPhysVol* Build(std::string prefix="none");

private:
  
  std::string m_xmlFileName;

};

#endif

