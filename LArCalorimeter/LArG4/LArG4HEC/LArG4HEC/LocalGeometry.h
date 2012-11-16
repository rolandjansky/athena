/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::HEC::LocalGeometry

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step.

// 05-Apr-2006: Pavol Strizenec, adaptation from TB LocalCalculator to Atlas

// Note: This class is intended for use in calculating identifiers in
// both active and inactive regions of the detector.  For calibration
// studies, it must work properly whether the energy is deposited in
// the liquid argon or the absorber.

#ifndef LArG4_HEC_LocalGeometry_H
#define LArG4_HEC_LocalGeometry_H

#include "LArG4Code/LArVG4DetectorParameters.h"
#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"

// Forward declarations.
class LArG4Identifier;
class G4Step;
class MsgStream;

namespace LArG4 {

  namespace HEC {

    enum eLocalGeometryType { kLocActive, kLocInactive, kLocDead };

    class LocalGeometry {

    public:

      // Standard implementation of a singleton pattern.
      static LocalGeometry* GetInstance();
      virtual ~LocalGeometry(){;}

      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eLocalGeometryType type = kLocActive , int depthadd = 0, double deadzone = 4.*CLHEP::mm, double locyadd = 0.*CLHEP::mm);

      bool isX() const { return m_isX; }
      void SetX(G4bool x) { m_isX = x; }

    protected:
      LocalGeometry();

    private:
      static LocalGeometry *m_instance;
      
      MsgStream *m_log;

      // Pointer to source of detector parameters.
      LArVG4DetectorParameters* m_parameters;

      static bool m_isX; 

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_LocalGeometry_H
