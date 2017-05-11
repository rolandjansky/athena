/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4STParameterisation_H
#define GEO2G4_Geo2G4STParameterisation_H

#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include "G4RotationMatrix.hh"

#include "GeoModelKernel/GeoXF.h"

class G4VPhysicalVolume;

// Dummy declarations. To avoid warnings
class G4Box;
class G4Trd;
class G4Trap;
class G4Cons;
class G4Sphere;
class G4Torus;
class G4Para;
class G4Hype;
class G4Tubs;
class G4Orb;
class G4Polyhedra;
class G4Polycone;
class G4Ellipsoid;

class Geo2G4STParameterisation : public G4VPVParameterisation
{
public:

  Geo2G4STParameterisation(const GeoXF::Function* func,
                           unsigned int copies);

  virtual ~Geo2G4STParameterisation();

  void ComputeTransformation (const G4int copyNo,
                              G4VPhysicalVolume* physVol) const;

private:
  // Declaring, but not defining private copy-constructor and
  // assignment operator, as an object of this class should never be
  // copied.
  Geo2G4STParameterisation(const Geo2G4STParameterisation&);
  Geo2G4STParameterisation& operator= (const Geo2G4STParameterisation&);

  // Dummy declarations. To avoid warnings

  void ComputeDimensions (G4Box&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Tubs&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*) const {}
  void ComputeDimensions (G4Ellipsoid&,const G4int,const G4VPhysicalVolume*) const {}

  const GeoXF::Function *m_function;
  G4RotationMatrix* m_rotation;
  unsigned int m_nCopies;
};

#endif
