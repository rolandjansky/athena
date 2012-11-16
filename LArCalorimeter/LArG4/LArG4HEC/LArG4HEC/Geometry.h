/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::HEC::Geometry

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step.

// 13-Jan-2004: Karapetian Gaiane wrote the geometry code, Bill
// Seligman structured into the LArG4 processing.

// Note: This class is intended for use in calculating identifiers in
// both active and inactive regions of the detector.  For calibration
// studies, it must work properly whether the energy is deposited in
// the liquid argon or the absorber.

#ifndef LArG4_HEC_Geometry_H
#define LArG4_HEC_Geometry_H

#include "G4ThreeVector.hh"
#include "globals.hh"

// Forward declarations.
class LArG4Identifier;
class G4Step;

// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECGeometry).  This class is contained in the namespace
// LArG4::HEC.

namespace LArG4 {

  namespace HEC {

    enum eGeometryType { kActive, kInactive, kDead };

    class Geometry {

    public:

      // Standard implementation of a singleton pattern.
      static Geometry* GetInstance();
      virtual ~Geometry();

      // This is the "meat" of this class: calculate the identifier
      // given a G4Step.
      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eGeometryType type = kActive );

    protected:
      // Constructor is protected according to the singleton pattern.
      Geometry();

    private:

      // Nominal z-shift from base.  (Note: someday, this will have to
      // be replaced with a G4Transform3D or similar description that's
      // suitable for re-alignment; for now, this will do.)
      G4float m_zShift;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_Geometry_H
