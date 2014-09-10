/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMModule_H
#define DBMModule_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"

#include "CLHEP/Geometry/Transform3D.h"

/* @class DBM_Module
 *
 * Build the DBM Module consisting of diamond sensor, FEI4 and ceramic substrate
 *
 */

namespace InDetDD {
  class SiDetectorDesign;
  class PixelDiodeMatrix;
}

class DBM_Module : public GeoVPixelFactory {
  public:

  DBM_Module();
  GeoVPhysVol* Build();

  private:

  InDetDD::SiDetectorDesign* m_design;

  InDetDD::PixelDiodeMatrix * makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
					 int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc);
};

#endif
