/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DBMTelescope_H
#define DBMTelescope_H

#include "GeoVPixelFactory.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

/*** @class DBM_Telescope
 *
 * Diamond Beam Monitor telescope builder
 *
 */

class DBM_Telescope : public GeoVPixelFactory {
 public:
  using GeoVPixelFactory::GeoVPixelFactory;
  virtual GeoVPhysVol* Build() override;

  
 private:
  
};

#endif
