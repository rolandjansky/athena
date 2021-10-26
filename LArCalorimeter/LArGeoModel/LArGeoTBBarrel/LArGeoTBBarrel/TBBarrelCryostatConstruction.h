/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TBBarrelCryostatConstruction
// returns an envelope that contains the H8 barrel cryostat
//  and places inside the calorimeter module and the presampler

#ifndef LARGEOTBBARREL_TBBARRELCRYOSTATCONSTRUCTION_H
#define LARGEOTBBARREL_TBBARRELCRYOSTATCONSTRUCTION_H

#include "GeoModelKernel/GeoFullPhysVol.h"
class StoreGateSvc;

namespace LArGeo {
  class VDetectorParameters;

  class TBBarrelCryostatConstruction
  {
  public:
   TBBarrelCryostatConstruction();
   virtual ~TBBarrelCryostatConstruction();

   // get the envelope containing this detector
   virtual GeoFullPhysVol* GetEnvelope(const VDetectorParameters* params);

   void setBarrelSagging(bool flag) {_barrelSagging=flag;}
   void setBarrelCellVisLimit(int maxCell) {_barrelVisLimit = maxCell;}

   private:
    bool                  _barrelSagging;
    int                   _barrelVisLimit;
    GeoFullPhysVol*       cryoMotherPhysical;
  };

} // namespace LArGeo

#endif
