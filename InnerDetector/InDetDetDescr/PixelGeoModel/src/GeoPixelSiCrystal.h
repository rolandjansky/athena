/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSICRYSTAL_H
#define GEOPIXELSICRYSTAL_H

#include "Identifier/Identifier.h"
#include "GeoVPixelFactory.h"

#include <memory>

class GeoLogVol;

namespace InDetDD {
  class SiDetectorDesign;
  class PixelDiodeMatrix;
}

class ATLAS_NOT_THREAD_SAFE GeoPixelSiCrystal : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelSiCrystal(bool isBLayer, bool isModule3D=false);
  virtual GeoVPhysVol* Build();
  inline Identifier getID();

  bool GetModule3DFlag() { return m_isModule3D; };

 private:
  std::shared_ptr<const InDetDD::PixelDiodeMatrix> makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
					 int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc);
  Identifier m_id;
  InDetDD::SiDetectorDesign* m_design;
  bool m_isBLayer;
  bool m_isModule3D;
};
//
// Add this method to store the ID in the factory. This is used by the
// module factory to pass it to the alignment class.
//
Identifier GeoPixelSiCrystal::getID() {return m_id;}
#endif
