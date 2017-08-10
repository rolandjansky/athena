/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4STParameterisation.h"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Geometry/Transform3D.h"

Geo2G4STParameterisation::Geo2G4STParameterisation(const GeoXF::Function* func,
                                                   unsigned int copies):
  m_function(func->clone()),
  m_nCopies(copies)
{
  m_rotation = new G4RotationMatrix();
}

Geo2G4STParameterisation::~Geo2G4STParameterisation()
{
  delete m_rotation;
}

void Geo2G4STParameterisation::ComputeTransformation(const G4int copyNo,
                                                     G4VPhysicalVolume* physVol) const
{
  HepGeom::Transform3D transform = (*m_function)(copyNo);
  G4ThreeVector translation = transform.getTranslation();
  *m_rotation = transform.getRotation().inverse();

  physVol->SetTranslation(translation);
  physVol->SetRotation(m_rotation);
}
