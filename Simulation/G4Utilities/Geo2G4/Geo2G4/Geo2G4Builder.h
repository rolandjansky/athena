/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4Builder_H
#define Geo2G4Builder_H

// main builder to create/position all volumes described in a GeoModel Tree

#include <string>
#include <vector>

// GeoVPhysVol
#include "Geo2G4/VolumeBuilder.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "Geo2G4/LogicalVolume.h"
#include "Geo2G4/GenericVolumeBuilder.h"

#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

class GeoMaterial;
class StoreGateSvc;

class Geo2G4Builder {

public:

  // Constructor:
  Geo2G4Builder(std::string detectorName);

  // Build method - geometry
  LogicalVolume*        BuildTree();

  // Build method - optical surfaces
  void BuildOpticalSurfaces(const GeoBorderSurfaceContainer* surface_container,
			    const OpticalVolumesMap* optical_volumes);

  // Access volume builder:
  VolumeBuilder* 	GetVolumeBuilder(std::string);
  
  HepGeom::Transform3D& GetDetectorTransform() {return motherTransform;}

private:

  // GeoVDetectorManager* theDetectorElement;
  std::string m_detectorName;
  HepGeom::Transform3D motherTransform;
  std::vector<PVConstLink> m_treeTops;
  VolumeBuilder *theBuilder;

  // std::Air in the case when top boolean envelope has to be built
  GeoMaterial* m_matAir;
  StoreGateSvc* m_pDetStore;
};

#endif
