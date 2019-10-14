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
 */
void ThinningCache::addThinning (const std::string& key,
                                 const std::vector<sgkey_t>& sgkeys,
                                 const ThinningDecisionBase* thinning)
{
  if (!thinning) return;

  if (!m_map.try_emplace (key, thinning).second) {
    throw std::runtime_error ("Duplicated thinning definition");
  }

  for (sgkey_t sgkey : sgkeys) {
    if (!m_sgmap.try_emplace (sgkey, thinning).second) {
      throw std::runtime_error ("Duplicated thinning definition");
    }
  }
}


/**
 * @brief Clear the cache.
 */
void ThinningCache::clear()
{
  m_map.clear();
  m_sgmap.clear();
}


} // namespace SG
