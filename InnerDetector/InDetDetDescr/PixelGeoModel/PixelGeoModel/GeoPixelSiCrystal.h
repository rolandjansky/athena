/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSICRYSTAL_H
#define GEOPIXELSICRYSTAL_H

#include "Identifier/Identifier.h"
#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

namespace InDetDD {
  class SiDetectorDesign;
  class PixelDiodeMatrix;
}

class GeoPixelSiCrystal : public GeoVPixelFactory {
 public:
  GeoPixelSiCrystal(bool isBLayer, bool isModule3D=false);
  virtual GeoVPhysVol* Build();
  inline Identifier getID();

  bool GetModule3DFlag() { return m_isModule3D; };

 private:
  InDetDD::PixelDiodeMatrix * makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
					 int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc);
  Identifier _id;
  InDetDD::SiDetectorDesign* m_design;
  bool m_isBLayer;
  bool m_isModule3D;
  GeoLogVol* m_logVolume;
};
//
// Add this method to store the ID in the factory. This is used by the
// module factory to pass it to the alignement class.
//
Identifier GeoPixelSiCrystal::getID() {return _id;}
#endif
