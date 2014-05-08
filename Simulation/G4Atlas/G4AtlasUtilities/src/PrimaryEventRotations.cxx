/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasUtilities/PrimaryEventRotations.h"
#include "G4PrimaryParticle.hh"
#include "CLHEP/Geometry/Transform3D.h"
#include <utility>
using namespace std;

// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.
bool PrimaryEventRotations::EditParticle(G4PrimaryParticle *p) {
   if (!onSwitch) return true;
   HepGeom::RotateX3D rotx(xangle*CLHEP::deg);
   HepGeom::RotateY3D roty(yangle*CLHEP::deg);
   HepGeom::RotateZ3D rotz(zangle*CLHEP::deg);

   G4ThreeVector vec = p->GetMomentum();
   if (verboseLevel > 0) {
     cout << "Rotating " << vec << " by (x,y,z) angles = (" << xangle << ", " << yangle << ", " << zangle << ")" << endl;
   }

   // Orthogonal rotations so order of trf execution is unimportant
   G4ThreeVector rvec = rotz.getRotation() * roty.getRotation() * rotx.getRotation() * vec;
   p->SetMomentum(rvec.x(), rvec.y(), rvec.z());

   if (verboseLevel > 0) {
     cout << "Rotated vector = " << rvec << endl;
   }
   return true;
}
