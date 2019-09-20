/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelAthenaPool/src/getThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helper to retrieve the current thinning cache from the event context.
 */


#include "DataModelAthenaPool/getThinningCache.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"


namespace DataModelAthenaPool {


/**
 * @brief Retrieve the current thinning cache from the event context.
 *        Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (ctx.hasExtension()) {
    return ctx.getExtension<Atlas::ExtendedEventContext>().thinningCache();
  }
  return nullptr;
}


} // namespace DataModelAthenaPool


