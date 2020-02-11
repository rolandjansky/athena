/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_VolumeBuilder_H
#define GEO2G4_VolumeBuilder_H

#include "G4LogicalVolume.hh"
#include "Geo2G4SvcAccessor.h"

#include "GeoModelUtilities/GeoOpticalPhysVol.h"

#include "G4VPhysicalVolume.hh"

#include <string>
#include <iostream>
#include <map>

typedef std::map< const GeoOpticalPhysVol*, G4VPhysicalVolume*,std::less< const GeoOpticalPhysVol*> > OpticalVolumesMap;

class VolumeBuilder
{
 public:
  VolumeBuilder(std::string k): m_paramOn(false), m_key(k)
  {
    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    g->RegisterVolumeBuilder(this);
  }

  virtual ~VolumeBuilder()
  {
    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    g->UnregisterVolumeBuilder(this);
  }

  std::string GetKey() const {return m_key;}

  // flag controlling Parameterization to Parameterization translation
  void SetParam(bool flag){m_paramOn = flag;}
  bool GetParam(){return m_paramOn;}

  virtual G4LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) = 0;

 protected:
  bool m_paramOn;

 private:
  std::string m_key;
};
#endif
