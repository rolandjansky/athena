/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDiscSupportXMLHelper_H
#define PixelDiscSupportXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelDiscSupportXMLHelper :  public GeoXMLUtils  {

 public:
  PixelDiscSupportXMLHelper();
  ~PixelDiscSupportXMLHelper();

  int getNbSupport(int layer);
  double getRingSupportRMin(int iSupport) const;
  double getRingSupportRMax(int iSupport) const;
  double getRingSupportThickness(int iSupport) const;
  std::string getRingSupportMaterial(int iSupport) const;
  
 private:
  int m_ringGeoIndex;
  bool m_bXMLfileExist;
};

#endif
