/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMSERVICES_h
#define DBMSERVICES_h

#include "PixelGeoModel/GeoVPixelFactory.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

/*** @class DBM_PP0
 *
 * Build the PP0 board
 *
 */

class DBM_PP0 : public GeoVPixelFactory {
  public:
    GeoVPhysVol* Build();
  private:
};

#endif
