/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author R.D.Schaffer -at- cern.ch
 * @date July 2019
 * @brief Local cache for magnetic field (based on
 * MagFieldServices/AtlasFieldSvcTLS.h)
 */

#ifndef MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H
#define MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H 1

// MagField includes
#include "CxxUtils/restrict.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MagFieldElements/AtlasFieldMap.h"
#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldCacheZR.h"
#include "MagFieldElements/BFieldCond.h"
#include "MagFieldElements/BFieldMeshZR.h"
#include "MagFieldElements/BFieldZone.h"

#include <iostream>
#include <memory>
namespace MagField {

/** @class AtlasFieldCache
 *
 *  @brief Local cache for magnetic field (based on
 * MagFieldServices/AtlasFieldSvcTLS.h)
 *
 *  @author R.D.Schaffer -at- cern.ch
 */
class AtlasFieldCache
{
public:
  AtlasFieldCache() = default;
  /** constructor to setup with field scale
   * and magnetic field service for
   * first access to field */
  AtlasFieldCache(double solFieldScale,
                  double torFieldScale,
                  const AtlasFieldMap* fieldMap);

  /**
   * Move-able but not copy-able
   */
  AtlasFieldCache& operator=(AtlasFieldCache&& other) = default;
  AtlasFieldCache(AtlasFieldCache&& other) = default;
  ~AtlasFieldCache() = default;

  /// Temporary flag for switching between 'old' and 'new' magField usage
  bool useNewBfieldCache() { return true; }

  /** get B field value at given position
   * xyz[3] is in mm, bxyz[3] is in kT
   * if deriv[9] is given, field derivatives are returned in kT/mm
   * */
  void getField(const double* ATH_RESTRICT xyz,
                double* ATH_RESTRICT bxyz,
                double* ATH_RESTRICT deriv = nullptr);

  void getFieldZR(const double* ATH_RESTRICT xyz,
                  double* ATH_RESTRICT bxyz,
                  double* ATH_RESTRICT deriv = nullptr);

  /** status of the magnets */
  bool solenoidOn() const;
  bool toroidOn() const;

private:
  AtlasFieldCache(const AtlasFieldCache& other) = delete;
  AtlasFieldCache& operator=(const AtlasFieldCache& other) = delete;

  /// fill given magnetic field zone */
  bool fillFieldCache(double z, double r, double phi);
  /// fill Z-R cache for solenoid */
  bool fillFieldCacheZR(double z, double r);

  /// Full 3d field
  BFieldCache m_cache3d;

  /// Fast 2d field
  BFieldCacheZR m_cacheZR;

  /// magnetic field scales from currents
  double m_solScale{ 1 };
  double m_torScale{ 1 };
  double m_scaleToUse{ 1 };

  /// handle to the magnetic field service - not owner
  const AtlasFieldMap* m_fieldMap{ nullptr };

  /// Pointer to the conductors in the current field zone (to compute
  /// Biot-Savart component) Owned by AtlasFieldMap.
  const std::vector<BFieldCond>* m_cond{ nullptr };

  // fast 2d map (made of one zone)
  /// Owned by AtlasFieldMap.
  const BFieldMeshZR* m_meshZR{ nullptr };

  // Solenoid zone ID number - needed to set solScale. Assumes only one Solenoid
  // zone!
  int m_solZoneId{ -1 };
};

} // namespace MagField
#include "AtlasFieldCache.icc"
#endif // MAGFIELDCONDITIONS_ATLASMTFIELDCACHE_H
