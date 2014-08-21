// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGgetDataSource.h,v 1.5 2008-09-23 20:54:36 ssnyder Exp $

/**
 * @file  AthLinks/tools/SGgetDataSource.h
 * @author scott snyder, Paolo Calafiura
 * @date Feb 2004
 * @brief Provide an interface for obtaining the `current' data store.
 *
 * When a @c DataProxyStorage is constructed, it needs to know the
 * data source (an @c IProxyDictWithPool implementation) in which it should
 * look to satisfy the reference.  This can be specified as a parameter
 * to the constructor, but more often, it is defaulted, meaning to use
 * the `current' data store.  The functions here define that notion.
 *
 * The entry point is @c getDataSourcePointer.  This takes as a parameter
 * the name of the data source (usually ``StoreGateSvc''), and returns
 * a pointer to a pointer to the data source.
 *
 * It is possible for the store to change.
 * In this case, the pointer (to the store pointer) returned by
 * @c getDataSourcePointer will stay the same, but the contents
 * of the pointer pointed to will change.  This allows one to safely
 * cache the pointer (to pointer) in static storage.
 *
 * By default, the store is found by looking for a Gaudi service
 * with the specified name.  This behavior may be changed by assigning
 * an alternate function to @c getDataSourcePointerFunc.
 */


#ifndef ATHLINKS_SGGETDATASOURCE_H
#define ATHLINKS_SGGETDATASOURCE_H


#include <string>


class IProxyDictWithPool;


namespace SG {


/**
 * @brief Return the named data store.
 * @param storeName The name of the desired data store.
 * @returns A pointer to a pointer to the store.
 *          The pointer returned by this function
 *          should always be the same.  However, the contents of the
 *          @c IProxyDictWithPool pointer to which it points may change.
 */
IProxyDictWithPool** getDataSourcePointer (const std::string& storeName); 



/**
 * @brief Return the named data store.
 * @param storeName The name of the desired data store.
 * @returns A pointer to the store.
 *          Note that the pointer returned
 *          by this function may change, so it's not safe to cache.
 */
IProxyDictWithPool* getDataSource (const std::string& storeName); 


/**
 * @brief Return the named data store, by looking for a Gaudi service.
 * @param storeName The name of the desired data store.
 * @returns A pointer to a pointer to the store.
 *          The pointer returned by this function
 *          should always be the same.  However, the contents of the
 *          @c IProxyDictWithPool pointer to which it points may change.
 */
IProxyDictWithPool**
getDataSourcePointerFromGaudi (const std::string& storeName); 


/// Hook function type.
typedef IProxyDictWithPool** getDataSourcePointerFunc_t (const std::string&);


/**
 * @brief Hook for data source function.
 *
 * When @c getDataSourcePointer is called, it simply calls through
 * this pointer to to the actual work.  This allows overriding the
 * notion of what constitutes the current data store.  By default,
 * this will be set to @c getDataSourcePointerFromGaudi.
 */
extern getDataSourcePointerFunc_t* getDataSourcePointerFunc;


} // namespace SG


#endif // not ATHLINKS_SGGETDATASOURCE_H
