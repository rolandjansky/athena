/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BarrelDMConstruction.h
 *
 * @brief Declaration of BarrelDMConstruction class
 *
 * $Id: BarrelDMConstruction.h,v 1.3 2007-07-08 18:50:15 tsulaia Exp $
 */

#ifndef LARGEOBARREL_BARRELDMCONSTRUCTION_H
#define LARGEOBARREL_BARRELDMCONSTRUCTION_H

// Forward declarations
class GeoFullPhysVol;

namespace LArGeo {

  /** @class LArGeo::BarrelDMConstruction
      @brief GeoModel description of dead material in the crack region
  */
  class BarrelDMConstruction
  {
  public:

    // Constructor;
    BarrelDMConstruction(bool activateFT = false);

    // Destructor:
    virtual ~BarrelDMConstruction();

    // Add passive materials to the barrel envelope
    void create(GeoFullPhysVol* envelope);

  private:
    // It is illegal to copy/assign a BarrelDMConstruction:
    BarrelDMConstruction (const BarrelDMConstruction &) = delete;
    BarrelDMConstruction & operator= (const BarrelDMConstruction &) = delete;

    bool m_activateFT;
  };



}  // namespace LArGeo

#endif // LARGEOBARREL_BARRELDMCONSTRUCTION_H
