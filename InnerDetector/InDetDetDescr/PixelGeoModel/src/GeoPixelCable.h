/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELCABLE_H
#define GEOPIXELCABLE_H

#include "GeoVPixelFactory.h"
#include <vector>

class ATLAS_NOT_THREAD_SAFE GeoPixelCable : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
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
