/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelInclRefStaveXMLHelper_H
#define PixelInclRefStaveXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelInclRefStaveXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  PixelInclRefStaveXMLHelper(int layer, const PixelGeoBuilderBasics* basics);
  ~PixelInclRefStaveXMLHelper();

  double getClearance() const;
  double getStaggerDist() const;
  double getStaggerSign() const;
  double getCenterShift() const;
  double getBarrelModuleDZ() const;

  double getStaveSupportLength() const;
  double getStaveSupportWidth() const;
  double getStaveSupportThick() const;
  std::string getStaveSupportMaterial() const;
  double getServiceOffsetX() const;
  double getServiceECOffsetX() const;
  double getServiceOffsetY() const;
  std::string getSvcRoutingPos() const;
  
 private:
  int m_layer;
  std::vector<int> m_layerIndices;
};

#endif
