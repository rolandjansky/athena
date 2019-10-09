// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthKernel/ThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Cache thinning decisions for converters.
 */


#ifndef ATHENAKERNEL_THINNINGCACHE_H
#define ATHENAKERNEL_THINNINGCACHE_H


#include "AthenaKernel/sgkey_t.h"
#include <unordered_map>
#include <string>
#include <vector>


namespace SG {


class ThinningDecisionBase;


/**
 * @brief Cache thinning decisions for converters.
 *
 * This class caches thinning decisions for a single stream for the use
 * of converters.  ThinningDecision objects may be looked up by SG key,
 * either as a string or as a hashed value.
 *
 * While writing is in progress, a pointer to the current cache is saved
 * in the @c EventContext.  Converters can find it there.
 */
class ThinningCache
{
public:
  /**
   * @brief Test to see if there's any thinning defined.
   */
  bool empty() const;


  /**
   * @brief Return thinning for @c key.
   * @param key SG key for which to return thinning.
   *
   * Return thinning defined for @c key.
   * Returns nullptr if that object is not thinned.
   */
  const ThinningDecisionBase* thinning (const std::string& key) const;


  /**
   * @brief Return thinning for @c sgkey.
   * @param key SG key for which to return thinning.
   *
   * Return thinning defined for @c sgkey.
   * Returns nullptr if that object is not thinned.
   */
  const ThinningDecisionBase* thinning (const sgkey_t sgkey) const;


  /**
   * @brief Add thinning information for one object.
   * @param key SG string key of the object being added.
   * @param sgkeys SG hashed keys of the object being added.
   * @param thinning Thinning information for the object.
   */
  void addThinning (const std::string& key,
                    const std::vector<sgkey_t>& sgkeys,
                    const ThinningDecisionBase* thinning);

  /**
   * @brief Clear the cache.
   */
  void clear();


private:
  /// Mapping by string SG key.
  typedef std::unordered_map<std::string, const ThinningDecisionBase*> map_t;
  map_t m_map;

  /// Mapping by hashed SG key.
  typedef std::unordered_map<sgkey_t, const ThinningDecisionBase*> sgmap_t;
  sgmap_t m_sgmap;
};


} // namespace ThinningCache


#endif // not ATHENAKERNEL_THINNINGCACHE_H
