/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::PresamplerGeometry

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step.

// 17-Aug-2004: Mikhail Leltchouk wrote the original code, Bill
// Seligman revised it for current use.

// Note: This class is intended for use in calculating identifiers in
// both active and inactive regions of the detector.  For calibration
// studies, it must work properly whether the energy is deposited in
// the liquid argon or the absorber.

#ifndef LArG4_EC_PresamplerGeometry_H
#define LArG4_EC_PresamplerGeometry_H
#include "LArG4EC/IECPresamplerGeometry.h"
#include "AthenaBaseComps/AthService.h"

// Forward declarations.
class LArG4Identifier;
class G4Step;

// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECGeometry).  This class is contained in the namespace
// LArG4::EC.

namespace LArG4 {

  namespace EC {

    class PresamplerGeometry: public AthService, virtual public IECPresamplerGeometry {

    public:
      // Constructor
      PresamplerGeometry(const std::string& name, ISvcLocator * pSvcLocator);
      StatusCode initialize() override final;
      virtual ~PresamplerGeometry();

      /** Query interface method to make athena happy */
      virtual StatusCode queryInterface(const InterfaceID&, void**) override final;

      // This is the "meat" of this class: calculate the identifier
      // given a G4Step.
      LArG4Identifier CalculateIdentifier( const G4Step* ) const override final;

    private:
      double m_zEndcapFrontFace;
      double m_halfThickness;
      // The following variables describe the endcap geometry.  Mostly these
      // were copied from the ATLAS G3 code.  FIXME Someday, they
      // should be determined from the detector database and not
      // hard-coded.
      int m_zSide; // +- 3 for inner wheel, +- 2 for outer wheel, z determines sign
      // For the presampler, the 'm_sampling' and 'm_region' are always zero.
      int m_sampling;
      int m_region;
      double m_etaScale;   // 1/deta
      double m_etaOffset;  // set so that the range of etaBin starts at zero for each compartment
      int    m_maxEta;     // the maximum value of etaBin in this compartment
      //int    m_gapsPerBin; // number of phi gaps (in LArWheelSolid) for each cell bin.
      int    m_maxPhi;     // the maximum value of phiBin in this compartment
      // Derived constants
      double m_phiScale;
      int m_maxPhi1;
      int m_halfMaxPhi1;

    };

  } // namespace EC

} // namespace LArG4

#endif // LArG4_EC_PresamplerGeometry_H
