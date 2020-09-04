/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4LogicalVolumeFactory.h"
#include "Geo2G4SolidFactory.h"
#include "GeoMaterial2G4/Geo2G4MaterialFactory.h"

#include "GeoModelKernel/GeoLogVol.h"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "SimHelpers/ServiceAccessor.h"

Geo2G4LogicalVolumeFactory::Geo2G4LogicalVolumeFactory()
{
}

G4LogicalVolume *Geo2G4LogicalVolumeFactory::Build(const GeoLogVol* theLog) const
{
  static Geo2G4SolidFactory theSolidFactory;
  static Geo2G4MaterialFactory theMaterialFactory;
  //
  // Get Material from GeoModel
  //
  G4LogicalVolume *theG4Log{};
  G4Material* theG4Mat=theMaterialFactory.Build(theLog->getMaterial());

  G4VSolid * theG4Solid = theSolidFactory.Build(theLog->getShape(),theLog->getName());

  theG4Log = new G4LogicalVolume(theG4Solid,
                                 theG4Mat,
                                 theLog->getName(),
                                 0,0,0); ;

  return theG4Log;
}
