/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ShieldBuilderV_H
#define ShieldBuilderV_H

class GeoPhysVol;

namespace MuonGM {

class ShieldBuilderV 
{

 public:

  // Builds the JD, JF, JT Shielding
  virtual void buildDiskShielding   ( GeoPhysVol* container ) = 0;
  virtual void buildToroidShielding ( GeoPhysVol* container ) = 0;
  virtual void buildForwardShielding( GeoPhysVol* container ) = 0;
    virtual ~ShieldBuilderV(){};
    
    

};

} // namespace MuonGM

#endif
