/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4OpticalSurfaceFactory_h
#define Geo2G4OpticalSurfaceFactory_h

#include <map>

class GeoOpticalSurface;
class G4OpticalSurface;

typedef std::map<const GeoOpticalSurface* , G4OpticalSurface*, std::less<const GeoOpticalSurface*> > Geo2G4OptSurfaceMap;

class Geo2G4OpticalSurfaceFactory
{
 public:
  Geo2G4OpticalSurfaceFactory();

  G4OpticalSurface* Build(const GeoOpticalSurface*);
 private:
  Geo2G4OptSurfaceMap definedOptSurfaces;
};

#endif
