/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelRingSupportXMLHelper_H
#define PixelRingSupportXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelRingSupportXMLHelper :  public GeoXMLUtils  {

 public:
  PixelRingSupportXMLHelper();
  ~PixelRingSupportXMLHelper();

  int getNbSupport(int layer);
  double getRingSupportRMin(int iSupport) const;
  double getRingSupportRMax(int iSupport) const;
  double getRingSupportThickness(int iSupport) const;
  std::string getRingSupportMaterial(int iSupport) const;

  int getNbLayerSupport(int layer);
  std::vector<double> getLayerSupportRadius(int iSupport) const;
  std::vector<double> getLayerSupportZ(int iSupport) const;
  std::string getLayerSupportMaterial(int iSupport) const;
  
 private:
  int m_ringGeoIndex;
  bool m_bXMLfileExist;
};

#endif
