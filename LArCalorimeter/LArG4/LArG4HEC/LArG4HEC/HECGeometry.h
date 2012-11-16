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
      
      MsgStream *m_log;

      const HECDetectorManager *hecManager;

      // Pointer to source of detector parameters.
      LArVG4DetectorParameters* m_parameters;

      static bool m_depthHist;
      static bool m_withMother;
      static bool m_inMother;

      static int  m_g4historyDepth;
      static double depthSize[7];
      static double depthSum[7];
      static double minval[7][2][10];
      static double maxval[7][2][10];

      static double firstAbsThickness[2];
      static double wheel1;
      static double wheel2;
      static double betweenWheels;
      static double hecLength;
      static double startHec2;
      static double rOuter;
      static double rInner1;
      static double rInner2;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_HECGeometry_H
