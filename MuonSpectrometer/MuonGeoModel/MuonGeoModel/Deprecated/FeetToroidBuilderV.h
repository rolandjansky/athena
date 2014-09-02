/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FeetToroidBuilderV_H
#define FeetToroidBuilderV_H

class GeoPhysVol;

namespace MuonGM {

class FeetToroidBuilderV
{
 public:
 
  // Builds the Feet
  virtual void buildStandardFeet    ( GeoPhysVol* container ) = 0;
  virtual void buildExtremityFeet   ( GeoPhysVol* container ) = 0;
  virtual void buildFeetGirders     ( GeoPhysVol* container ) = 0;
  virtual void buildFeetRailSupports( GeoPhysVol* container ) = 0;
  virtual void buildFeetVoussoirs   ( GeoPhysVol* container ) = 0;
  virtual ~FeetToroidBuilderV(){};
    
};

} // namespace MuonGM

#endif
