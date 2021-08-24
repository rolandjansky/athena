/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "LArG4HEC/ILocalGeometry.h"
#include "AthenaBaseComps/AthService.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"

class LArG4Identifier;
class G4Step;

namespace LArG4 {

  namespace HEC {

    class LocalGeometry: public extends<AthService, ILocalGeometry> {

    public:

      LocalGeometry(const std::string& name, ISvcLocator * pSvcLocator);
      StatusCode initialize() override final;
      virtual ~LocalGeometry(){;}

      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eLocalGeometryType type = kLocActive,
                                           int depthadd = 0, double deadzone = 4.*CLHEP::mm, double locyadd = 0.*CLHEP::mm) const override final;

      //bool isX() const { return m_isX; } //UNUSED
      //void SetX(G4bool x) { m_isX = x; }

    private:
      int binSearch(double ly, int depth, int reg) const;
      int binSearchAll(double ly, int depth, bool regular=false) const;
      double deadZone(double locx, double locy) const;
      ServiceHandle<IGeoModelSvc> m_geoModel;
      ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc;

      bool m_isX;
      double m_depthSize[7] = {0.0};
      double m_firstAbsorber[7] = {0.0};
      double m_pads[7][15] = {}; // 15 is the number of bins in eta.
      double m_betweenWheel = 0.0;
    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_LocalGeometry_H
