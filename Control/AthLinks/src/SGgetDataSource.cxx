/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGgetDataSource.cxx 567807 2013-10-30 09:30:16Z krasznaa $
/**
 * @file  AthLinks/src/SGgetDataSource.cxx
 * @author scott snyder, Paolo Calafiura
 * @date Feb 2004
 * @brief Provide an interface for obtaining the `current' data store.
 */


#include "AthLinks/tools/SGgetDataSource.h"
#include "GaudiKernel/Bootstrap.h"  /*Gaudi singleton*/
#include "GaudiKernel/ISvcLocator.h"
#include "SGTools/IProxyDictWithPool.h"
#include <stdexcept>


namespace SG {


/**
 * @brief Return the named data store.
 * @param storeName The name of the desired data store.
 * @returns A pointer to a pointer to the store.
 *          The pointer returned by this function
 *          should always be the same.  However, the contents of the
 *          @c IProxyDictWithPool pointer to which it points may change.
 */
IProxyDictWithPool**
getDataSourcePointer (const std::string& storeName)
{
  return getDataSourcePointerFunc (storeName);
}


/**
 * @brief Return the named data store.
 * @param storeName The name of the desired data store.
 * @returns A pointer to the store.
 *          Note that the pointer returned
 *          by this function may change, so it's not safe to cache.
 */
IProxyDictWithPool* 
getDataSource (const std::string& storeName)
{
  return *getDataSourcePointerFunc (storeName);
}



/**
 * @brief Return the named data store, by looking for a Gaudi service.
 * @param storeName The name of the desired data store.
 * @returns A pointer to a pointer to the store.
 *          The pointer returned by this function
 *          should always be the same.  However, the contents of the
 *          @c IProxyDictWithPool pointer to which it points may change.
 */
IProxyDictWithPool** 
getDataSourcePointerFromGaudi(const std::string& storeName)
{
  static IProxyDictWithPool* pStore = 0;
  if (!pStore) {
    const bool CREATEIF = true;
    if (! Gaudi::svcLocator()->service(storeName, pStore,
				       CREATEIF).isSuccess()) {
      throw std::runtime_error("Can't get store " + storeName);
    }
  }
  return &pStore;
}


/// Default implementation.
getDataSourcePointerFunc_t* getDataSourcePointerFunc =
  getDataSourcePointerFromGaudi;


} // namespace SG
