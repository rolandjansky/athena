/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMSERVICES_h
#define DBMSERVICES_h

#include "GeoVPixelFactory.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

/*** @class DBM_PP0
 *
 * Build the PP0 board
 *
 */

class DBM_PP0 : public GeoVPixelFactory {
  public:
    using GeoVPixelFactory::GeoVPixelFactory;
    virtual GeoVPhysVol* Build() override;
  private:
};

#endif
