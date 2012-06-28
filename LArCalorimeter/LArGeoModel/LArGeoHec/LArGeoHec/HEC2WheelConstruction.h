/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HEC2WheelConstruction.h
 *
 * @brief Declaration of HEC2WheelConstruction class
 *
 * $Id: HEC2WheelConstruction.h,v 1.2 2007-11-27 20:02:16 fincke Exp $
 */
#ifndef __HEC2WheelConstruction_H__
#define __HEC2WheelConstruction_H__

class GeoFullPhysVol;

namespace LArGeo 
{
  /** @class LArGeo::HEC2WheelConstruction
      @brief GeoModel description of LAr HEC

      The geometry is built and placed within HEC envelope which is implemented
      by LArGeoEndcap.
   */
  class HEC2WheelConstruction 
    {
    public:
      HEC2WheelConstruction();
      virtual ~HEC2WheelConstruction();
      // Get the envelope containing this detector.
      GeoFullPhysVol* GetEnvelope(bool fullGeo, bool posZSide=true);
      
    private:
      GeoFullPhysVol* m_physiHEC;
      bool	      m_posZSide;
    };
  
}
#endif // __HEC2WheelConstruction_H__
