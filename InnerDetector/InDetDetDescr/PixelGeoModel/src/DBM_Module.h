/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMModule_H
#define DBMModule_H

#include "GeoVPixelFactory.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"

#include <memory>

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

  DBM_Module(InDetDD::PixelDetectorManager* ddmgr,
             PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;

  private:

  InDetDD::SiDetectorDesign* m_design;

  std::shared_ptr<const InDetDD::PixelDiodeMatrix> makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
					 int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc);
};

#endif
