/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BarrelPresamplerConstruction.h
 *
 * @brief Declaration of BarrelPresamplerConstruction class
 *
 * $Id: BarrelPresamplerConstruction.h,v 1.4 2009-02-10 16:49:47 tsulaia Exp $
 */

#ifndef LARGEOBARREL_BARRELPRESAMPLERCONSTRUCTION_H
#define LARGEOBARREL_BARRELPRESAMPLERCONSTRUCTION_H

#include "LArGeoCode/VDetectorParameters.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoGenericFunctions/FunctionNoop.h"

// Forward declarations

namespace LArGeo {

  /** @class LArGeo::BarrelPresamplerConstruction
      @brief GeoModel description of the LAr Barrel Presampler
   */
  class BarrelPresamplerConstruction
  {
  public:

    // Constructor;
    BarrelPresamplerConstruction(bool fullGeo, const VDetectorParameters* params, int itb=0);

    // Destructor:
    virtual ~BarrelPresamplerConstruction();

    // Get the envelope containing this detector.
    GeoFullPhysVol* GetPositiveEnvelope();
    GeoFullPhysVol* GetNegativeEnvelope();

  private: 

    void MakeEnvelope();

    // It is illegal to copy a BarrelPresamplerConstruction:
    BarrelPresamplerConstruction (const BarrelPresamplerConstruction &);

    // It is illegal to assign a BarrelPresamplerConstruction:
    BarrelPresamplerConstruction & operator= (const BarrelPresamplerConstruction &);

    // Detector parameters ACCG, ACCA, ACMB, ACCO
    const LArGeo::VDetectorParameters* m_parameters;
    
    // volumes that are private member variables:
    GeoFullPhysVol*  m_psPhysicalPos;
    GeoFullPhysVol*  m_psPhysicalNeg;

    bool                       m_fullGeo;  // true->FULL, false->RECO    
  };

 

}  // namespace LArGeo

#endif // LARGEOBARREL_BARRELPRESAMPLERCONSTRUCTION_H
