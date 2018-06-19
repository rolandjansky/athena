/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step in the HEC.

// Jan-2008: (M.Fincke)  To be used for new Module-Geometry

#ifndef LArG4_HEC_HECGeometry_H
#define LArG4_HEC_HECGeometry_H

#include "LArG4HEC/IHECGeometry.h"
#include "AthenaBaseComps/AthService.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
//#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "globals.hh"

// Forward declarations.
class LArG4Identifier;
class G4Step;
class G4LogicalVolume;
class G4VPhysicalVolume;

namespace LArG4 {

  namespace HEC {

    class HECGeometry: public extends<AthService, IHECGeometry> {

    public:

      HECGeometry(const std::string& name, ISvcLocator * pSvcLocator);
      StatusCode initialize() override final;
      virtual ~HECGeometry(){;}

      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eHECGeometryType type = kWheelActive, int *subgap=NULL) const override final;

    private:
      bool checkDaughters(const G4LogicalVolume *possibleParent, const G4VPhysicalVolume *thePhysicalVolume, int& level) const;

      const HECDetectorManager *m_hecManager{nullptr};

      bool m_depthHist{false};
      bool m_withMother{false};

      int  m_g4historyDepth{0};
      double m_depthSize[7]{};
      double m_depthSum[7]{};
      double m_minval[7][2][10]{};
      double m_maxval[7][2][10]{};

      // Some essential numbers we'll need over and over again:
      double m_firstAbsThickness[2]{};
      double m_wheel1{};
      double m_wheel2{};
      double m_betweenWheels{40.5}; // FIXME HACK!! -- can't find this through LArReadoutGeometry, so hard-code it here
      double m_hecLength{};
      double m_startHec2{};
      double m_rOuter{};
      double m_rInner1{};
      double m_rInner2{};

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_HECGeometry_H
