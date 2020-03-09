/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// The G4ShiftedCone class copied from standard G4Cons and modified to:
// 1) have an arbitrary position along Z axis,
// 2) represent a twopi-cone. Sectors are not supported but the
//    corresponding code kept and just commented out.
//

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
//
// --------------------------------------------------------------------
// GEANT 4 class header file
//
// G4ShiftedCone
//
// Class description:
//
//   A G4ShiftedCone is, in the general case, a Phi segment of a cone,
//   inner and outer radii specified at z1 and z2.
//   This version does not support phi segmetation, but the code is kept
//   in case it would be necessary later.
//   The Phi segment is described by a starting fSPhi angle, and the
//   +fDPhi delta angle for the shape.
//   If the delta angle is >=2*pi, the shape is treated as continuous
//   in Phi
//
//   Member Data:
//
//  fRmin1  inside radius at  z1
//  fRmin2  inside radius at  z2
//  fRmax1  outside radius at z1
//  fRmax2  outside radius at z2
//  fDz  half length in z
//
//  fSPhi  starting angle of the segment in radians
//  fDPhi  delta angle of the segment in radians
//
//  fPhiFullCone   Boolean variable used for indicate the Phi Section
//  always true in the current version, hope compiler optimize
//
//   Note:
//      Internally fSPhi & fDPhi are adjusted so that fDPhi<=2PI,
//      and fDPhi+fSPhi<=2PI. This enables simpler comparisons to be
//      made with (say) Phi of a point.

// History:
// 03.07.2019 A. Sukharev copied from G4Cons for ATLAS EMEC needs
// --------------------------------------------------------------------
#ifndef G4ShiftedCone_HH
#define G4ShiftedCone_HH

#include <CLHEP/Units/PhysicalConstants.h>

#include "G4CSGSolid.hh"
#include "G4Polyhedron.hh"

class G4ShiftedCone : public G4CSGSolid
{
  public:  // with description

    G4ShiftedCone(const G4String& pName,
                 G4double pZ1, G4double pZ2,
                 G4double pRmin1, G4double pRmax1,
                 G4double pRmin2, G4double pRmax2);
      //         G4double pSPhi, G4double pDPhi);
      //
      // Constructs a cone with the given name and dimensions

   ~G4ShiftedCone() ;
      //
      // Destructor

    // Accessors

    inline G4double GetInnerRadiusMinusZ() const;
    inline G4double GetOuterRadiusMinusZ() const;
    inline G4double GetInnerRadiusPlusZ()  const;
    inline G4double GetOuterRadiusPlusZ()  const;
    inline G4double GetZHalfLength()       const;
    inline G4double GetZ1()                const;
    inline G4double GetZ2()                const;
    inline G4double GetStartPhiAngle()     const;
    inline G4double GetDeltaPhiAngle()     const;
    inline G4double GetSinStartPhi()       const;
    inline G4double GetCosStartPhi()       const;
    inline G4double GetSinEndPhi()         const;
    inline G4double GetCosEndPhi()         const;

    // Modifiers

    inline void SetInnerRadiusMinusZ (G4double Rmin1 );
    inline void SetOuterRadiusMinusZ (G4double Rmax1 );
    inline void SetInnerRadiusPlusZ  (G4double Rmin2 );
    inline void SetOuterRadiusPlusZ  (G4double Rmax2 );
//    inline void SetStartPhiAngle     (G4double newSPhi, G4bool trig=true);
//    inline void SetDeltaPhiAngle     (G4double newDPhi);

    // Other methods for solid

    inline G4double GetCubicVolume();
    inline G4double GetSurfaceArea();

    void ComputeDimensions(       G4VPVParameterisation* p,
                            const G4int n,
                            const G4VPhysicalVolume* pRep );

    void BoundingLimits(G4ThreeVector& pMin, G4ThreeVector& pMax) const;

    G4bool CalculateExtent( const EAxis pAxis,
                            const G4VoxelLimits& pVoxelLimit,
                            const G4AffineTransform& pTransform,
                                  G4double& pMin, G4double& pMax ) const;

    EInside Inside( const G4ThreeVector& p ) const;

    G4ThreeVector SurfaceNormal( const G4ThreeVector& p ) const;

    G4double DistanceToIn (const G4ThreeVector& p,
                           const G4ThreeVector& v) const;
    G4double DistanceToIn (const G4ThreeVector& p) const;
    G4double DistanceToOut(const G4ThreeVector& p,
                           const G4ThreeVector& v,
                           const G4bool calcNorm=G4bool(false),
                                 G4bool *validNorm=0,
                                 G4ThreeVector *n=0) const;
    G4double DistanceToOut(const G4ThreeVector& p) const;

    G4GeometryType GetEntityType() const;

    G4ThreeVector GetPointOnSurface() const;

    G4VSolid* Clone() const;

    std::ostream& StreamInfo(std::ostream& os) const;

    // Visualisation functions

    void          DescribeYourselfTo( G4VGraphicsScene& scene ) const;
    G4Polyhedron* CreatePolyhedron() const;

  public:  // without description

    G4ShiftedCone(__void__&);
      //
      // Fake default constructor for usage restricted to direct object
      // persistency for clients requiring preallocation of memory for
      // persistifiable objects.

    G4ShiftedCone(const G4ShiftedCone& rhs);
    G4ShiftedCone& operator=(const G4ShiftedCone& rhs);
      // Copy constructor and assignment operator.

    //  Old access functions

    inline G4double    GetRmin1() const;
    inline G4double    GetRmax1() const;
    inline G4double    GetRmin2() const;
    inline G4double    GetRmax2() const;
//    inline G4double    GetSPhi() const;
//    inline G4double    GetDPhi() const;

  private:

    inline void Initialize();
      //
      // Reset relevant values to zero

//    inline void CheckSPhiAngle(G4double sPhi);
//    inline void CheckDPhiAngle(G4double dPhi);
//    inline void CheckPhiAngles(G4double sPhi, G4double dPhi);
      //
      // Reset relevant flags and angle values

    inline void InitializeTrigonometry();
      //
      // Recompute relevant trigonometric values and cache them

    G4ThreeVector ApproxSurfaceNormal(const G4ThreeVector& p) const;
      //
      // Algorithm for SurfaceNormal() following the original
      // specification for points not on the surface

  private:

    // Used by distanceToOut
    //
    enum ESide {kNull,kRMin,kRMax,kSPhi,kEPhi,kPZ,kMZ};

    // used by normal
    //
    enum ENorm {kNRMin,kNRMax,kNSPhi,kNEPhi,kNZ};

    G4double kRadTolerance, kAngTolerance;
      //
      // Radial and angular tolerances

    G4double fRmin1, fRmin2, fRmax1, fRmax2;
    G4double fDz, fZshift;//, fSPhi, fDPhi;
      //
      // Radial and angular dimensions

//    G4double sinCPhi, cosCPhi, cosHDPhiOT, cosHDPhiIT,
//             sinSPhi, cosSPhi, sinEPhi, cosEPhi;
      //
      // Cached trigonometric values

//    const G4bool fPhiFullCone;
      //
      // Flag for identification of section or full cone

    G4double halfCarTolerance, halfRadTolerance, halfAngTolerance;
      //
      // Cached half tolerance values
};

#include "G4ShiftedCone.icc"

#endif
