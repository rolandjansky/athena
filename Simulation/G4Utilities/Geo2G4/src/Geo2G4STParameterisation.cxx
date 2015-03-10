/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4STParameterisation.h"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Geometry/Transform3D.h"

Geo2G4STParameterisation::Geo2G4STParameterisation(const GeoXF::Function* func,
                                                   unsigned int copies):
  function(func->clone()),
  nCopies(copies)
{
  rotation = new G4RotationMatrix();
}

Geo2G4STParameterisation::~Geo2G4STParameterisation()
{
  delete rotation;
}

void Geo2G4STParameterisation::ComputeTransformation(const G4int copyNo,
                                                     G4VPhysicalVolume* physVol) const
{
  HepGeom::Transform3D transform = (*function)(copyNo);
  G4ThreeVector translation = transform.getTranslation();
  *rotation = transform.getRotation().inverse();

  physVol->SetTranslation(translation);
  physVol->SetRotation(rotation);
}
