// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelAthenaPool/getThinningCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helper to retrieve the current thinning cache from the event context.
 */


#ifndef DATAMODELATHENAPOOL_GETTHINNINGCACHE_H
#define DATAMODELATHENAPOOL_GETTHINNINGCACHE_H


namespace SG {
  class ThinningCache;
}


namespace DataModelAthenaPool {


/**
 * @brief Retrieve the current thinning cache from the event context.
 *        Returns nullptr if there is no current thinning cache.
 */
const SG::ThinningCache* getThinningCache();


} // namespace DataModelAthenaPool


#endif // not DATAMODELATHENAPOOL_GETTHINNINGCACHE_H
