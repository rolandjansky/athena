/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EndCapToroidBuilderV_H
#define EndCapToroidBuilderV_H

class GeoPhysVol;

namespace MuonGM {
class EndCapToroidBuilderV
{
 public:
 
   // Builds the Endcap Toroid
  virtual void buildECTVacuumVessel( GeoPhysVol* container ) = 0;
  virtual void buildECTConductorBox( GeoPhysVol* container ) = 0;
  virtual void buildECTKeystoneBox ( GeoPhysVol* container ) = 0;
  virtual void buildECTServiceTower( GeoPhysVol* container ) = 0;
  virtual ~EndCapToroidBuilderV(){};
    

};

} // namespace MuonGM

#endif
