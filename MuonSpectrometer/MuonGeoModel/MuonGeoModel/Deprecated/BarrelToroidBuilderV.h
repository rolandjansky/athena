/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BarrelToroidBuilderV_H
#define BarrelToroidBuilderV_H

class GeoPhysVol;

namespace MuonGM {

class BarrelToroidBuilderV 
{
 public:

  // Builds the Barrel Toroid
  virtual void buildBTVacuumVessel( GeoPhysVol* container ) = 0;
  virtual void buildBTColdMass    ( GeoPhysVol* container ) = 0;
  virtual void buildBTVoussoirs   ( GeoPhysVol* container ) = 0;
  virtual void buildBTStruts      ( GeoPhysVol* container ) = 0;
  virtual void buildBTCryoring    ( GeoPhysVol* container ) = 0;
  
  // Builds the Rails on the feet
  virtual void buildRails( GeoPhysVol* container ) = 0;
  virtual ~BarrelToroidBuilderV(){};
    
    
};

} // namespace MuonGM

#endif
