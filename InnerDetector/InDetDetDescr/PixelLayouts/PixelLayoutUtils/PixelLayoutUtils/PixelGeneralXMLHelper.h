/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeneralXMLHelper_H
#define PixelGeneralXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelGeneralXMLHelper :  public GeoXMLUtils  {

 public:
  PixelGeneralXMLHelper(std::string envFileName);
  ~PixelGeneralXMLHelper();

  double getEnvelopeRMin() const;
  double getEnvelopeRMax() const;
  double getEnvelopeHalfLength() const;
  double getCommonHalfLength() const;
  double getBarrelRMin() const;
  double getBarrelRMax() const;
  double getBarrelHalfLength() const;
  bool isBarrelCylindrical() const;
  std::vector<double> getBarrelRadiusList() const;
  std::vector<double> getBarrelHalfLengthList() const;

  double getEndcapRMin() const;
  double getEndcapRMax() const;
  double getEndcapZMin() const;
  double getEndcapZMax() const;
  bool isEndcapCylindrical() const;
  std::vector<double> getEndcapRadiusList() const;
  std::vector<double> getEndcapZMinList() const;

  double getLayerRMin(int ilayer) const;
  double getLayerRMax(int ilayer) const;

  bool isBarrelPresent() const;
  bool isEndcapPresentA() const;
  bool isEndcapPresentC() const;
    
 private:

};

#endif
