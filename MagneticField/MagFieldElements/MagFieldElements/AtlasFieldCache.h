/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author R.D.Schaffer -at- cern.ch
 * @date July 2019
 * @brief Local cache for magnetic field (based on MagFieldServices/AtlasFieldSvcTLS.h)
 */

#ifndef MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H
#define MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H 1

// MagField includes
#include "MagFieldElements/AtlasFieldMap.h"
#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldCacheZR.h"
#include "MagFieldElements/BFieldCond.h"
#include "MagFieldElements/BFieldZone.h"
#include "MagFieldElements/BFieldMeshZR.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CxxUtils/restrict.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include<iostream>
#include <memory>
namespace MagField {

    
/** @class AtlasFieldCache
 *
 *  @brief Local cache for magnetic field (based on MagFieldServices/AtlasFieldSvcTLS.h)
 *
 *  @author R.D.Schaffer -at- cern.ch
 */
    class AtlasFieldCache
    // : public ::AthMessaging
    {
    public:
        AtlasFieldCache();
        // ** constructor to setup with field scale and magnetic field service for first access to field */
        AtlasFieldCache(double solFieldScale,
                        double torFieldScale,
                        const AtlasFieldMap* fieldMap,
                        bool useNewBfieldCache);
        AtlasFieldCache& operator= (AtlasFieldCache&& other)      = default;
        ~AtlasFieldCache() = default; 
 
        /// Temporary flag for switching between 'old' and 'new' magField usage
        bool useNewBfieldCache() { return m_useNewBfieldCache; }

        /** get B field value at given position */
        /** xyz[3] is in mm, bxyz[3] is in kT */
        /** if deriv[9] is given, field derivatives are returned in kT/mm */
        inline void getField  (const double* ATH_RESTRICT xyz, 
                               double* ATH_RESTRICT bxyz, 
                               double* ATH_RESTRICT deriv = nullptr);
        inline void getFieldZR(const double* ATH_RESTRICT xyz, 
                               double* ATH_RESTRICT bxyz, 
                               double* ATH_RESTRICT deriv = nullptr);
    
    private:
    
        AtlasFieldCache(const AtlasFieldCache& other)             = delete;
        AtlasFieldCache& operator= (const AtlasFieldCache& other) = delete;
        AtlasFieldCache(AtlasFieldCache&& other)                  = delete;

        inline bool fillFieldCache(double z, double r, double phi);
        inline bool fillFieldCacheZR(double z, double r);

        /// Temporary flag for switching between 'old' and 'new' magField usage
        bool m_useNewBfieldCache{false};

        /// magnetic field scales from currents
        double m_solScale{1};
        double m_torScale{1};
        double m_scaleToUse{1};
        // Solenoid zone ID number - needed to set solScale. Assumes only one Solenoid zone!
        int    m_solZoneId{-1}; 

        /// handle to the magnetic field service - not owner
        const AtlasFieldMap* m_fieldMap;

        /// Pointer to the conductors in the current field zone (to compute Biot-Savart component)
        /// Owned by AtlasFieldMap. 
        const std::vector<BFieldCond>* m_cond{nullptr};

        /// Full 3d field
        BFieldCache   m_cache3d;

        /// Fast 2d field
        BFieldCacheZR m_cacheZR;

        // fast 2d map (made of one zone)
        /// Owned by AtlasFieldMap.
        const BFieldMeshZR* m_meshZR{nullptr};
        
    };

}  // namespace MagField


/** fill given magnetic field zone */
bool
MagField::AtlasFieldCache::fillFieldCache(double z, double r, double phi) 
{
    // search for the zone
    const BFieldZone* zone = m_fieldMap->findBFieldZone( z, r, phi );
    if ( zone == nullptr ) {
        // outsize all zones
        return false;
    }

    // set scale for field
    if (zone->id() == m_solZoneId) {
        m_scaleToUse = m_solScale;
    }
    else {
        m_scaleToUse = m_torScale;
    }

    // fill the cache, pass in current scale factor
    zone->getCache( z, r, phi, m_cache3d, m_scaleToUse );

    // save pointer to the conductors in the zone
    m_cond = zone->condVector();

    return true;
}

/** fill Z-R cache for solenoid */
bool
MagField::AtlasFieldCache::fillFieldCacheZR(double z, double r) 
{
    // is it inside the solenoid zone?
    if ( m_meshZR && m_meshZR->inside( z, r ) ) {

        // fill the cache, pass in current scale factor
        m_meshZR->getCache( z, r, m_cacheZR, m_solScale );

    } else {
        // outside solenoid
        return false;
    }
    return true;
}


void
MagField::AtlasFieldCache::getField(const double* ATH_RESTRICT xyz, 
                                    double* ATH_RESTRICT bxyz, 
                                    double* ATH_RESTRICT deriv) 
{
    const double &x(xyz[0]);
    const double &y(xyz[1]);
    const double &z(xyz[2]);
    double r = std::sqrt(x * x + y * y);
    double phi = std::atan2(y, x);

    // test if initialised and the cache is valid
    if ( !m_cache3d.inside(z, r, phi) ) {
        // cache is invalid -> refresh cache
        if (!fillFieldCache(z, r, phi)) {
            // caching failed -> outside the valid map volume
            // return default field (0.1 gauss)
            const double defaultB(0.1*CLHEP::gauss);
            bxyz[0] = bxyz[1] = bxyz[2] = defaultB;
            // return zero gradient if requested
            if ( deriv ) {
                for ( int i = 0; i < 9; i++ ) {
                    deriv[i] = 0.;
                }
            }
            return;
        }
    }

    // do interpolation (cache3d has correct scale factor)
    m_cache3d.getB(xyz, r, phi, bxyz, deriv);
    // add biot savart component - must add in scale factor to avoid changing conductor SF since the
    // conductor is part of the static magnetic field model
    if (m_cond) {
        const size_t condSize = m_cond->size();
        for (size_t i = 0; i < condSize; i++) {
            (*m_cond)[i].addBiotSavart(m_scaleToUse, xyz, bxyz, deriv); 
        }
    }
} 

void
MagField::AtlasFieldCache::getFieldZR(const double* ATH_RESTRICT xyz, 
                                      double* ATH_RESTRICT bxyz, 
                                      double* ATH_RESTRICT deriv) 
{
    const double &x(xyz[0]);
    const double &y(xyz[1]);
    const double &z(xyz[2]);
    double r = sqrt(x * x + y * y);

    // test if the cache was initialized and the ZR cache is valid for current position
    if ( !m_cacheZR.inside(z, r) ) {

        // cache is invalid -> refresh cache
        if (!fillFieldCacheZR(z, r)) {

            // caching failed -> outside the valid z-r map volume
            // call the full version of getField()
            getField(xyz, bxyz, deriv);
            return;

        }
    }
    // do interpolation
    m_cacheZR.getB(xyz, r, bxyz, deriv);
}

#endif  // MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H
