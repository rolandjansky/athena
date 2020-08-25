/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMModuleCage_h
#define DBMModuleCage_h

#include "GeoVPixelFactory.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

/*** @class DBM_ModuleCage
 *
 * The module cage is the structure forming of 
 * the three aluminium plates and rods,
 * which support the DBM module.
 *
 */

class ATLAS_NOT_THREAD_SAFE DBM_ModuleCage : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
  public:
    GeoVPhysVol* Build();
  private:
};

 #endif
