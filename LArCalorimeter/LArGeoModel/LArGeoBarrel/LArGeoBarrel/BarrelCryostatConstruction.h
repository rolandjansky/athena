/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BarrelCryostatConstruction.h
 *
 * @brief Declaration of BarrelCryostatConstruction class
 *
 * $Id: BarrelCryostatConstruction.h,v 1.12 2009-02-10 16:49:47 tsulaia Exp $
 */


#ifndef LARG4EOBARREL_BARRELCRYOSTATCONSTRUCTION_H
#define LARG4EOBARREL_BARRELCRYOSTATCONSTRUCTION_H

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

    BarrelCryostatConstruction(bool fullGeo, bool activateFT = false);
    virtual ~BarrelCryostatConstruction();

    // Get the envelope containing this detector.
    virtual GeoFullPhysVol* GetEnvelope();


    void setBarrelSagging(bool flag)        {m_barrelSagging  = flag;}
    void setBarrelCellVisLimit(int maxCell) {m_barrelVisLimit = maxCell;}

  private:

    bool                    m_barrelSagging;
    int                     m_barrelVisLimit;
    GeoFullPhysVol          *m_cryoMotherPhysical;

    bool                    m_fullGeo;  // true->FULL, false->RECO
    bool                    m_activateFT;
  };

} // namespace LArGeo

#endif // LARG4EOBARREL_BARRELCRYOSTATCONSTRUCTION_H
