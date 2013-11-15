/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BarrelCryostatConstruction.h
 *
 * @brief Declaration of BarrelCryostatConstruction class
 *
 * $Id: BarrelCryostatConstruction.h,v 1.12 2009-02-10 16:49:47 tsulaia Exp $
 */


#ifndef __BarrelCryostatConstruction_H__
#define __BarrelCryostatConstruction_H__

#include "GeoModelKernel/GeoFullPhysVol.h"
class StoreGateSvc;

namespace LArGeo {

  /** @class LArGeo::BarrelCryostatConstruction 
      @brief Builds GeoModel description of the LAr Electromagnetic Barrel.
      Descriptions of the presampler and dad material in the crack region are
      implemented in separate classes
   */
  class BarrelCryostatConstruction 
  {
  public:
    
    BarrelCryostatConstruction(bool fullGeo);
    virtual ~BarrelCryostatConstruction();
    
    // Get the envelope containing this detector.
    virtual GeoFullPhysVol* GetEnvelope();
    

    void setBarrelSagging(bool flag)        {_barrelSagging  = flag;}
    void setBarrelCellVisLimit(int maxCell) {_barrelVisLimit = maxCell;}
    
  private:

    bool                    _barrelSagging;
    int                     _barrelVisLimit;
    GeoFullPhysVol          *cryoMotherPhysical;

    bool                    m_fullGeo;  // true->FULL, false->RECO
  };

} // namespace LArGeo

#endif // __BarrelCryostatConstruction_H__
