/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEXMLHELPER_H
#define BCMPRIMEXMLHELPER_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include "PixelLayoutUtils/GeoXMLUtils.h"

class BCMPrimeXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  BCMPrimeXMLHelper(int module, const PixelGeoBuilderBasics* basics);
  ~BCMPrimeXMLHelper();

  int getNumberOfModules() const;
  double getTransX() const;
  double getTransY() const;
  double getRotX() const;
  double getRotY() const;
  double getRotZ() const;
  double getModuleLength() const;
  double getModuleWidth() const;
  double getModuleTilt() const;
  double getDiamSize() const;
  double getDiamOffsetY() const;
  double getDiamSeparZ() const;
  double getTilt() const;
  double getRingRot() const;
  double getRingOffset() const;
  double getCuThickness() const;
  double getG10Thickness() const;
  double getDiamThickness() const;

  double getSupportPosition() const;
  double getSupportRMin() const;
  double getSupportRMax() const;
  double getSupportThickness() const;
  int getSupportNSectors() const;
  double getSupportSPhi() const;
  double getSupportDPhi() const;
  std::string getSupportMaterial() const;

 private:
  int m_module;
  std::vector<int> m_moduleIndices;

};

#endif
