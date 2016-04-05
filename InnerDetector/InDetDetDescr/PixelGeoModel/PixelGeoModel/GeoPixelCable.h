/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELCABLE_H
#define GEOPIXELCABLE_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include <vector>

class GeoPixelCable : public GeoVPixelFactory {
 public:
  GeoPixelCable();
  virtual GeoVPhysVol* Build();
  int numElements() const {return m_elements.size();}
  void setElement(int i) {m_currentElement = i;}
  double getStackOffset();
  double zpos();

 private:
  int m_currentElement;
  std::vector<int> m_elements;
  void fillElements(int layer, int phiSector);

};



#endif
