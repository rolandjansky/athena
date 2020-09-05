/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// AtlasFieldCache.cxx
//
// Local cache for magnetic field (based on MagFieldServices/AtlasFieldSvcTLS.h)
//
// R.D.Schaffer -at- cern.ch
//
#include "MagFieldElements/AtlasFieldCache.h"

void
MagField::AtlasFieldCache::getField(const double* ATH_RESTRICT xyz,
                                    double* ATH_RESTRICT bxyz,
                                    double* ATH_RESTRICT deriv)
{
  // Allow for the case of no map for testing
  if (m_fieldMap == nullptr) {
    // return 0 bfield if map is missing
    bxyz[0] = bxyz[1] = bxyz[2] = 0;
    if (deriv) {
      for (int i = 0; i < 9; i++) {
        deriv[i] = 0.;
      }
    }
    return;
  }

  const double x = xyz[0];
  const double y = xyz[1];
  const double z = xyz[2];
  const double r = std::sqrt(x * x + y * y);
  const double phi = std::atan2(y, x);

  // test if initialised and the cache is valid
  if (!m_cache3d.inside(z, r, phi)) {
    // cache is invalid -> refresh cache
    if (!fillFieldCache(z, r, phi)) {
      // caching failed -> outside the valid map volume
      // return default field (0.1 gauss)
      const double defaultB(0.1 * Gaudi::Units::gauss);
      bxyz[0] = bxyz[1] = bxyz[2] = defaultB;
      // return zero gradient if requested
      if (deriv) {
        for (int i = 0; i < 9; i++) {
          deriv[i] = 0.;
        }
      }
      return;
    }
  }

  // do interpolation (cache3d has correct scale factor)
  m_cache3d.getB(xyz, r, phi, bxyz, deriv);

  if (!m_cond) {
    return;
  }
  // add biot savart component - must add in scale factor to avoid changing
  // conductor SF since the conductor is part of the static magnetic field model
  const size_t condSize = m_cond->size();
  for (size_t i = 0; i < condSize; i++) {
    (*m_cond)[i].addBiotSavart(m_scaleToUse, xyz, bxyz, deriv);
  }
}

void
MagField::AtlasFieldCache::getFieldZR(const double* ATH_RESTRICT xyz,
                                      double* ATH_RESTRICT bxyz,
                                      double* ATH_RESTRICT deriv)
{

  // Allow for the case of no map for testing
  if (m_fieldMap == nullptr) {
    // constant ATLAS magnetic field if no map has been set - for testing
    constexpr double TEST_BFIELD = 1.997;
    bxyz[0] = bxyz[1] = 0;
    bxyz[2] = TEST_BFIELD;
    if (deriv) {
      for (int i = 0; i < 9; i++) {
        deriv[i] = 0.;
      }
    }
    return;
  }

  const double x = xyz[0];
  const double y = xyz[1];
  const double z = xyz[2];
  const double r = sqrt(x * x + y * y);

  // test if the cache was initialized and the ZR cache is valid for current
  // position
  if (!m_cacheZR.inside(z, r)) {

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

