// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthKernel/ThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Cache thinning decisions for converters.
 */


#ifndef ATHENAKERNEL_THINNINGCACHE_H
#define ATHENAKERNEL_THINNINGCACHE_H


#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/ThinningInfo.h"
#include "AthenaKernel/sgkey_t.h"
#include "CxxUtils/ConcurrentBitset.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>


class ITrigNavigationThinningSvc;


namespace SG {


class ThinningDecisionBase;
class auxid_set_t;


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
   * @param unique If true, the object must not already be listed in the cache.
   *
   * If there is already thinning information in the cache for the object
   * identified by @c key:
   *    - If @c unique is false, the new thinning request will be merged
   *      with the existing one via AND.
   *    - If @c unique is true, an exception will be thrown.
   */
  void addThinning (const std::string& key,
                    const std::vector<sgkey_t>& sgkeys,
                    const ThinningDecisionBase* thinning,
                    bool unique = false);


  /**
   * @brief Return the TrigNavigation helper for this stream, or nullptr.
   */
  const ITrigNavigationThinningSvc* trigNavigationThinningSvc() const;


  /**
   * @brief Set the TrigNavigation helper for this stream.
   * @param thinning TrigNavigation helper.
   */
  void setTrigNavigationThinningSvc (const ITrigNavigationThinningSvc* thinning);


  /**
   * @brief Lock all the @c ThinningDecisionBase objects that we own.
   *
   * This should be called after all thinning objects have been added,
   * but before the cache is installed in the EventContext.
   */
  void lockOwned();


  /**
   * @brief Set vetoed variable information one object.
   * @param key SG string key of the object being added.
   * @param vetoed Set of vetoed variables for this object.
   */
  void setVetoed (const std::string& key,
                  const CxxUtils::ConcurrentBitset& vetoed);

  /**
   * @brief Set lossy float compression information for the object.
   * @param key SG string key of the object being added.
   * @param compression Map of compression levels to variables
   *                    for this object.
   */
  void setCompression(const std::string& key,
                      const ThinningInfo::compression_map_t& compression);

  /**
   * @brief Return thinning information for @c key.
   * @param key SG key for which to return selected variables.
   *
   * Return thinning information @c key, or nullptr.
   */
  const ThinningInfo* thinningInfo (const std::string& key) const;


  /**
   * @brief Clear the cache.
   */
  void clear();



private:
  /// Mapping by string SG key.
  typedef std::unordered_map<std::string, ThinningInfo> map_t;
  map_t m_map;

  /// Mapping by hashed SG key.
  typedef std::unordered_map<sgkey_t, ThinningInfo> sgmap_t;
  sgmap_t m_sgmap;

  /// List of decision objects we've copied in order to handle merges.
  std::vector<std::unique_ptr<ThinningDecisionBase> > m_owned;

  /// Optional TrigNavigation thinning helper for this stream.
  const ITrigNavigationThinningSvc* m_trigNavigationThinningSvc = nullptr;


  /**
   * @brief Merge a new thinning request into an existing one via AND.
   * @param info @c ThinningInfo with existing decision.
   * @param sgkeys SG hashed keys of the object being added.
   * @param thinning New thinning decision.
   *
   * The new thinning decision will be combined with the old one via AND.
   */
  void merge (ThinningInfo& info,
              const std::vector<sgkey_t>& sgkeys,
              const ThinningDecisionBase& thinning);
};


} // namespace ThinningCache


#endif // not ATHENAKERNEL_THINNINGCACHE_H
