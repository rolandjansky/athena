/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DBMTelescope_H
#define DBMTelescope_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

/*** @class DBM_Telescope
 *
 * Diamond Beam Monitor telescope builder
 *
 */

class DBM_Telescope : public GeoVPixelFactory {
 public:
  GeoVPhysVol* Build();

  
 private:
  
};

#endif
