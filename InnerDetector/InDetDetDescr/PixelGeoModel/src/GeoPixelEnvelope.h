/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENVELOPE_H
#define GEOPIXELENVELOPE_H

#include "GeoVPixelFactory.h"

class GeoPixelEnvelope : public GeoVPixelFactory {
 public:
  using GeoVPixelFactory::GeoVPixelFactory;
  virtual GeoVPhysVol* Build() override;
};

#endif
