// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file AthenaKernel/getThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helpers to retrieve the current thinning cache from the event context.
 */


#ifndef ATHENAKERNEL_GETTHINNINGCACHE_H
#define ATHENAKERNEL_GETTHINNINGCACHE_H


#include "AthenaKernel/sgkey_t.h"
#include <string>
class EventContext;


namespace SG {
  class ThinningCache;
  class ThinningInfo;
  class ThinningDecisionBase;


/**
 * @brief Retrieve the current thinning cache from the event context.
 * @brief ctx Event context.
 *
 * Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache (const EventContext& ctx);


/**
 * @brief Retrieve the current thinning cache from the current event context.
 *        Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache();


/**
 * @brief Retrieve the current thinning information for @c key.
 * @brief ctx Event context.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningInfo* getThinningInfo (const EventContext& ctx,
                                         const std::string& key);


/**
 * @brief Retrieve the current thinning information for @c key.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningInfo* getThinningInfo (const std::string& key);


/**
 * @brief Retrieve the current thinning decision for @c key.
 * @brief ctx Event context.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningDecisionBase* getThinningDecision (const EventContext& ctx,
                                                     const std::string& key);


/**
 * @brief Retrieve the current thinning decision for @c key.
 * @param key SG key to test.
 *
 * Returns nullptr if there is no thinning for @c key.
 */
const SG::ThinningDecisionBase* getThinningDecision (const std::string& key);


/**
 * @brief Retrieve the current thinning decision for @c sgkey.
 * @brief ctx Event context.
 * @param sgkey SG hashed key to test.
 *
 * Returns nullptr if there is no thinning for @c sgkey.
 */
const SG::ThinningDecisionBase* getThinningDecision (const EventContext& ctx,
                                                     const sgkey_t sgkey);


/**
 * @brief Retrieve the current thinning decision for @c sgkey.
 * @param key SG key to test.
 * @param sgkey SG hashed key to test.
 *
 * Returns nullptr if there is no thinning for @c sgkey.
 */
const SG::ThinningDecisionBase* getThinningDecision (const sgkey_t sgkey);


} // namespace SG


#endif // not ATHENAKERNEL_GETTHINNINGCACHE_H
