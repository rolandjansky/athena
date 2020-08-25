/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file AthenaKernel/src/getThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helpers to retrieve the current thinning cache from the event context.
 */


#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/ThinningCache.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"


namespace SG {


/**
 * @brief Retrieve the current thinning cache from the event context.
 * @brief ctx Event context.
 *
 * Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache (const EventContext& ctx)
{
  if (ctx.hasExtension()) {
    return Atlas::getExtendedEventContext(ctx).thinningCache();
  }
  return nullptr;
}


/**
 * @brief Retrieve the current thinning cache from the current event context.
 *        Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache()
{
  return getThinningCache (Gaudi::Hive::currentContext());
}


/**
 * @brief Retrieve the current thinning information for @c key.
 * @brief ctx Event context.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningInfo* getThinningInfo (const EventContext& ctx,
                                                 const std::string& key)
{
  if (const SG::ThinningCache* cache = getThinningCache (ctx)) {
    return cache->thinningInfo (key);
  }
  return nullptr;
}


/**
 * @brief Retrieve the current thinning information for @c key.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningInfo* getThinningInfo (const std::string& key)
{
  return getThinningInfo (Gaudi::Hive::currentContext(), key);
}


/**
 * @brief Retrieve the current thinning decision for @c key.
 * @brief ctx Event context.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningDecisionBase* getThinningDecision (const EventContext& ctx,
                                                     const std::string& key)
{
  if (const SG::ThinningCache* cache = getThinningCache (ctx)) {
    return cache->thinning (key);
  }
  return nullptr;
}


/**
 * @brief Retrieve the current thinning decision for @c key.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningDecisionBase* getThinningDecision (const std::string& key)
{
  return getThinningDecision (Gaudi::Hive::currentContext(), key);
}


/**
 * @brief Retrieve the current thinning decision for @c sgkey.
 * @brief ctx Event context.
 * @param sgkey SG hashed key to test.
 *
 * Returns nullptr if there is no thinning for @c sgkey.
 */
const SG::ThinningDecisionBase* getThinningDecision (const EventContext& ctx,
                                                     const sgkey_t sgkey)
{
  if (const SG::ThinningCache* cache = getThinningCache (ctx)) {
    return cache->thinning (sgkey);
  }
  return nullptr;
}


/**
 * @brief Retrieve the current thinning decision for @c sgkey.
 * @param sgkey SG hashed key to test.
 *
 * Returns nullptr if there is no thinning for @c sgkey.
 */
const SG::ThinningDecisionBase* getThinningDecision (const sgkey_t sgkey)
{
  return getThinningDecision (Gaudi::Hive::currentContext(), sgkey);
}


} // namespace SG

