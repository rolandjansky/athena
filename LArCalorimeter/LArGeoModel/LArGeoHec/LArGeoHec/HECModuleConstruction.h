/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HECModuleConstruction.h
 *
 * @brief Declaration of HECModuleConstruction class
 *
 * $Id: HECModuleConstruction.h,v 1.1 2007-10-09 16:43:20 fincke Exp $
 */
#ifndef __HECModuleConstruction_H__
#define __HECModuleConstruction_H__

class GeoFullPhysVol;

namespace LArGeo 
{
  /** @class LArGeo::HECModuleConstruction
      @brief GeoModel description of LAr HECModule

      The geometry is built and placed within HECModule envelope which is implemented
      by LArGeoEndcap.
   */
  class HECModuleConstruction 
    {
    public:
      HECModuleConstruction(bool threeBoards=false, bool frontWheel=true, bool tb=false, int tbyear=2002);
      virtual ~HECModuleConstruction();
      // Get the envelope containing this detector.
      GeoFullPhysVol* GetEnvelope();
      
    private:
      GeoFullPhysVol* m_physiHECModule;
      bool	      m_threeBoards;
      bool     	      m_frontWheel;
      bool	      m_tb;
      int 	      m_tbyear;
    };
  
}
#endif // __HECModuleConstruction_H__
