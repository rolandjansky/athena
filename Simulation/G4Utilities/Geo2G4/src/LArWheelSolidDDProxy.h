/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolidDDProxy
// proxy for LArWheelSolid to store in DetectorDescription
// Author: D. A. Maximov

#ifndef __LArWheelSolidDDProxy_HH__
#define __LArWheelSolidDDProxy_HH__

// #include "AthenaKernel/MsgStreamMember.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "SGTools/CLASS_DEF.h"


class LArWheelSolid;

class LArWheelSolidDDProxy {
public:

  LArWheelSolidDDProxy(LArWheelSolid* plws);
  virtual ~LArWheelSolidDDProxy();

  // Mandatory for custom solid Geant4 functions
/*  EInside Inside(const G4ThreeVector&) const;
  G4double DistanceToIn(const G4ThreeVector&,
                        const G4ThreeVector&) const;
  G4double DistanceToIn(const G4ThreeVector&) const;
  G4double DistanceToOut(const G4ThreeVector&,
                         const G4ThreeVector&,
                         const G4bool calcNorm = false,
                         G4bool* validNorm = 0,
                         G4ThreeVector* n = 0) const; */
  int Inside(const CLHEP::Hep3Vector&) const;
  
  double DistanceToIn(const CLHEP::Hep3Vector&, const CLHEP::Hep3Vector&) const;
  double DistanceToIn(const CLHEP::Hep3Vector&) const;

  double DistanceToOut(const CLHEP::Hep3Vector&, const CLHEP::Hep3Vector&) const;

  double DistanceToOut(const CLHEP::Hep3Vector&) const;
  CLHEP::Hep3Vector SurfaceNormal (const CLHEP::Hep3Vector&) const;

//  G4bool CalculateExtent(const EAxis,
//                         const G4VoxelLimits&,
//                         const G4AffineTransform&,
//                         G4double&,
//                         G4double&) const;

  CLHEP::Hep3Vector GetPointOnSurface() const;
  double GetCubicVolume();
  double GetSurfaceArea();

  void SetVerbose(int v) const;

private:

  LArWheelSolid * m_plws;

};


//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(LArWheelSolidDDProxy, 900345679 , 1)

#endif // __LArWheelSolidDDProxy_HH__
