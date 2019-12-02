/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/src/ThinningCache.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Cache thinning decisions for converters.
 */


#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"


namespace SG {


/**
 * @brief Test to see if there's any thinning defined.
 */
bool ThinningCache::empty() const
{
  return m_map.empty();
}


/**
 * @brief Return thinning for @c key.
 * @param key SG key for which to return thinning.
 *
 * Return thinning defined for @c key.
 * Returns nullptr if that object is not thinned.
 */
const ThinningDecisionBase*
ThinningCache::thinning (const std::string& key) const
{
  auto it = m_map.find (key);
  if (it != m_map.end()) {
    return it->second;
  }
  return nullptr;
}


/**
 * @brief Return thinning for @c sgkey.
 * @param key SG key for which to return thinning.
 *
 * Return thinning defined for @c sgkey.
 * Returns nullptr if that object is not thinned.
 */
const ThinningDecisionBase*
ThinningCache::thinning (const sgkey_t sgkey) const
{
  auto it = m_sgmap.find (sgkey);
  if (it != m_sgmap.end()) {
    return it->second;
  }
  return nullptr;
}


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
void ThinningCache::addThinning (const std::string& key,
                                 const std::vector<sgkey_t>& sgkeys,
                                 const ThinningDecisionBase* thinning,
                                 bool unique /*= false*/)
{
  if (!thinning) return;

  auto pos = m_map.try_emplace (key, thinning);
  if (!pos.second) {
    if (!unique) {
      merge (pos.first, sgkeys, *thinning);
      return;
    }
    throw std::runtime_error ("Duplicated thinning definition");
  }

  for (sgkey_t sgkey : sgkeys) {
    if (!m_sgmap.try_emplace (sgkey, thinning).second) {
      throw std::runtime_error ("Duplicated thinning definition");
    }
  }
}


/**
 * @brief Lock all the @c ThinningDecisionBase objects that we own.
 *
 * This should be called after all thinning objects have been added,
 * but before the cache is installed in the EventContext.
 */
void ThinningCache::lockOwned()
{
  for (const std::unique_ptr<ThinningDecisionBase>& dec : m_owned) {
    dec->buildIndexMap();
  }
}


/**
 * @brief Clear the cache.
 */
void ThinningCache::clear()
{
  m_map.clear();
  m_sgmap.clear();
  m_owned.clear();
}


/**
 * @brief Merge a new thinning request into an existing one via AND.
 * @param oldThinningIt Iterator in @c m_map of the existing thinning decision.
 * @param sgkeys SG hashed keys of the object being added.
 * @param thinning New thinning decision.
 *
 * The new thinning decision will be combined with the old one via AND.
 */
void ThinningCache::merge (map_t::iterator oldThinningIt,
                           const std::vector<sgkey_t>& sgkeys,
                           const ThinningDecisionBase& thinning)
{
  // The existing thinning object.
  ThinningDecisionBase* oldThinning = nullptr;

  // Look for the existing object in our list of owned decision objects.
  // If we don't find it, then we'll need to make a copy of the existing
  // decision so that we can modify it.
  // We expect this to be relatively rare and the number of such objects
  // to be small, so just use a linear search.
  auto ownedIt = std::find_if (m_owned.begin(), m_owned.end(),
                               [old = oldThinningIt->second]
                               (const std::unique_ptr<SG::ThinningDecisionBase>& p)
                               { return p.get() == old; });

  if (ownedIt != m_owned.end()) {
    // We already own the existing decision.  We can just use it.
    oldThinning = ownedIt->get();
  }
  else {
    // We need to make a copy of the existing decision and enter it into the maps.
    m_owned.push_back (std::make_unique<SG::ThinningDecisionBase> (oldThinningIt->second->size()));
    oldThinning = m_owned.back().get();
    oldThinning->thin (*oldThinningIt->second);
    oldThinningIt->second = oldThinning;

    for (sgkey_t sgkey : sgkeys) {
      m_sgmap[sgkey] = oldThinning;
    }
  }

  // Merge the decisions.
  oldThinning->thin (thinning, SG::ThinningDecisionBase::Op::And);
}


} // namespace SG
