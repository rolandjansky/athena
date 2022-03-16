/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// HECWheelConstruction.h
//
// Construct an Atlas HEC Wheel (Front or Rear)
// 
// Oct 2007 M. Fincke
//

#ifndef LARGEOHEC_HECWHEELCONSTRUCTION_H
#define LARGEOHEC_HECWHEELCONSTRUCTION_H

#include <string>

class GeoFullPhysVol;

namespace LArGeo 
{

  class HECWheelConstruction 
    {
    public:
      HECWheelConstruction(bool fullGeo, const std::string& wheelType="front", bool threeBoards=false, bool posZSide=true);
      virtual ~HECWheelConstruction();

      // Get the envelope containing this detector.
      GeoFullPhysVol* GetEnvelope();
      
    private:
      GeoFullPhysVol* m_physiHECWheel;
      bool	      m_posZSide;
      //bool	      m_rail;
      //bool	      m_left;
      bool	      m_threeBoards;
      bool	      m_frontWheel;
      std::string     m_wheelType;
      //double          m_clampLength;
      //double          m_clampAngle;
      bool            m_fullGeo;  // true->FULL, false->RECO
    };
  
}
#endif // LARGEOHEC_HECWHEELCONSTRUCTION_H
