/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step in the HEC.

// Jan-2008: (M.Fincke)  To be used for new Module-Geometry 

#ifndef LArG4_HEC_HECGeometry_H
#define LArG4_HEC_HECGeometry_H

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "globals.hh"

// Forward declarations.
class LArG4Identifier;
class G4Step;
class MsgStream;

namespace LArG4 {

  namespace HEC {

    enum eHECGeometryType { kWheelActive, kWheelInactive, kWheelDead };

    class HECGeometry {

    public:

      // Standard implementation of a singleton pattern.
      static HECGeometry* GetInstance();
      virtual ~HECGeometry(){;}

      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eHECGeometryType type = kWheelActive, int *subgap=NULL);


    protected:
      HECGeometry();

    private:
      static HECGeometry *m_instance;
      
      const HECDetectorManager *m_hecManager;

      static bool m_depthHist;
      static bool m_withMother;
      static bool m_inMother;

      static int  m_g4historyDepth;
      static double s_depthSize[7];
      static double s_depthSum[7];
      static double s_minval[7][2][10];
      static double s_maxval[7][2][10];

      static double s_firstAbsThickness[2];
      static double s_wheel1;
      static double s_wheel2;
      static double s_betweenWheels;
      static double s_hecLength;
      static double s_startHec2;
      static double s_rOuter;
      static double s_rInner1;
      static double s_rInner2;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_HECGeometry_H
