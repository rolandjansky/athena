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

#ifndef __BarrelDMConstruction_H__
#define __BarrelDMConstruction_H__

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

    // It is illegal to copy a BarrelDMConstruction:
    BarrelDMConstruction (const BarrelDMConstruction &);

    // It is illegal to assign a BarrelDMConstruction:
    BarrelDMConstruction & operator= (const BarrelDMConstruction &);

    bool m_activateFT;
   };



}  // namespace LArGeo

#endif // __BarrelDMConstruction_H__
