/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/src/AuxVectorData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Manage lookup of vectors of auxiliary data.
 */


#include "AthContainers/AuxVectorData.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/error.h"
#include "CxxUtils/checker_macros.h"
#include <sstream>


namespace SG {


/// Minimum length to use for the cache vector.
size_t AuxVectorData::s_minCacheLen = 1024;


/// Empty auxid set, used for a return value when we have no associated store.
const SG::auxid_set_t AuxVectorData::s_emptySet;


/**
 * @brief Constructor.
 */
AuxVectorData::AuxVectorData()
  : m_store(0),
    m_constStore( 0 )
{
}


#if __cplusplus > 201100
/**
 * @brief Move constructor.
 * @param rhs The container from which to move.
 */
AuxVectorData::AuxVectorData (AuxVectorData&& rhs)
  : m_cache (std::move (rhs.m_cache)),
    m_constCache (std::move (rhs.m_constCache)),
    m_decorCache (std::move (rhs.m_decorCache)),
    m_store (rhs.m_store),
    m_constStore (rhs.m_constStore),
    m_constStoreLink (std::move (rhs.m_constStoreLink))
{
  rhs.m_store = 0;
  rhs.m_constStore = 0;
  rhs.m_constStoreLink.clear();
}


/**
 * @brief Move assignment.
 * @param rhs The container from which to move.
 */
AuxVectorData& AuxVectorData::operator= (AuxVectorData&& rhs)
{
  if (this != &rhs) {
    m_cache = std::move (rhs.m_cache);
    m_constCache = std::move (rhs.m_constCache);
    m_decorCache = std::move (rhs.m_decorCache);
    m_store = rhs.m_store;
    m_constStore = rhs.m_constStore;
    m_constStoreLink = rhs.m_constStoreLink;

    rhs.m_store = 0;
    rhs.m_constStore = 0;
    rhs.m_constStoreLink.clear();
  }
  return *this;
}
#endif


/**
 * @brief Destructor.
 */
AuxVectorData::~AuxVectorData()
{
}


/**
 * @brief Set the store associated with this object.
 * @param The new store.
 *
 * This will clear the non-const store pointer, and also
 * clear the cache.
 */
void AuxVectorData::setStore (const SG::IConstAuxStore* store)
{
  m_store = 0;
  m_constStore = store;
  m_constStoreLink.clear();
  clearCache();
}


/**
 * @brief Set the store associated with this object.
 * @param The new store.
 *
 * This will set both the const and non-const store pointers, and also
 * clear the cache.
 */
void AuxVectorData::setStore (SG::IAuxStore* store)
{
  m_store = store;
  m_constStore = store;
  m_constStoreLink.clear();
  clearCache();
}


/**
 * @brief Set the store associated with this object.
 * @param The new store.
 *
 * This will clear the non-const store pointer, and also
 * clear the cache.
 */
void AuxVectorData::setStore (const DataLink< SG::IConstAuxStore >& store)
{
  m_store = 0;
  m_constStore = 0;
  m_constStoreLink = store;
  clearCache();
}


/**
 * @brief Set an option for an auxiliary data variable.
 * @param id The variable for which we want to set the option.
 * @param option The option setting to make.
 *
 * The interpretation of @c option depends on the associated auxiliary store.
 * See PackedParameters.h for option settings for writing packed data.
 * Returns @c true on success, @c false otherwise.
 */
bool AuxVectorData::setOption (auxid_t id, const AuxDataOption& option)
{
  if (id == null_auxid) return false;
  SG::IAuxStore* store = getStore();
  if (!store) return false;
  return store->setOption (id, option);
}


/**
 * @brief Set an option for an auxiliary data variable.
 * @param name The name of the variable.
 * @param option The option setting to make.
 *
 * The interpretation of @c option depends on the associated auxiliary store.
 * See PackedParameters.h for option settings for writing packed data.
 * Returns @c true on success, @c false otherwise.
 */
bool AuxVectorData::setOption (const std::string& name,
                               const AuxDataOption& option)
{
  auxid_t id = SG::AuxTypeRegistry::instance().findAuxID (name);
  return setOption (id, option);
}


/**
 * @brief Set an option for an auxiliary data variable.
 * @param name The name of the variable.
 * @param clsname The name of the associated class.  May be blank.
 * @param option The option setting to make.
 *
 * The interpretation of @c option depends on the associated auxiliary store.
 * See PackedParameters.h for option settings for writing packed data.
 * Returns @c true on success, @c false otherwise.
 */
bool AuxVectorData::setOption (const std::string& name,
                               const std::string& clsname,
                               const AuxDataOption& option)
{
  auxid_t id = SG::AuxTypeRegistry::instance().findAuxID (name, clsname);
  return setOption (id, option);
}


/**
 * @brief Return a set of identifiers for existing data items
 *        in store associated with this object.
 *
 *        This will include identifiers for all items,
 *        const and non-const.  If no store is associated
 *        with this object, this will return an empty set.
 */
const SG::auxid_set_t& AuxVectorData::getAuxIDs() const
{
  if (getConstStore())
    return getConstStore()->getAuxIDs();
  return s_emptySet;
}


/**
 * @brief Return a set of identifiers for writable data items
 *        in this store.
 *
 *        This will include only non-const identifiers.
 *        If no store is associated
 *        with this object, this will return an empty set.
 */
const SG::auxid_set_t& AuxVectorData::getWritableAuxIDs() const
{
  if (m_store)
    return m_store->getWritableAuxIDs();
  return s_emptySet;
}


/**
 * @brief Out-of-line portion of isAvailable.
 * @param id The variable to test.
 */
bool AuxVectorData::isAvailableOol (auxid_t id) const
{
  const SG::IConstAuxStore* store = getConstStore();
  if (!store) return false;

  // Don't rely on getAuxIDs() --- the store can lie.
  // Explicitly try to fetch the data.
  const void* ptr = store->getData (id);
  if (ptr) {
    // We could avoid the const_cast here by having distinct const and
    // non-const Cache types, holding const void* and void*, respectively.
    // However, since this is a purely internal class that users don't
    // deal with directly, that's not worth the bother (and the extra code).
    void* vp ATLAS_THREAD_SAFE = const_cast<void*> (ptr);
    m_constCache.store (id, vp);
    return true;
  }
  return false;
}


/**
 * @brief Out-of-line portion of isAvailableWritable.
 * @param id The variable to test.
 */
bool AuxVectorData::isAvailableWritableOol (auxid_t id)
{
  const SG::IAuxStore* store = getStore();
  if (!store) return false;
  const SG::auxid_set_t& ids = store->getWritableAuxIDs();
  return ( ids.find (id) != ids.end() );
}


/**
 * @brief Out-of-line portion of isAvailableWritableAsDecoration.
 * @param id The variable to test.
 */
bool AuxVectorData::isAvailableWritableAsDecorationOol (auxid_t id) const
{
  if (!isAvailableOol (id)) return false;

  // Not nice, but not sure we can do otherwise without changing interfaces.
  // I think the case of a caught exception should be rare.
  try {
    this->getDecorationArray (id);
  }
  catch (const SG::ExcStoreLocked&) {
    return false;
  }
  return true;
}


/**
 * @brief Out-of-line portion of data access.
 * @param auxid aux data item being accessed.
 * @param allowMissing If true, then return nullptr if the variable
 *                     is missing rather than throwing an exception.
 *
 * When this function returns, the cache entry @c m_cache[auxid]
 * will be valid.  That entry is also returned.  If there's an error,
 * the function will throw  an exception rather than returning.
 */
void* AuxVectorData::getDataOol (SG::auxid_t auxid, bool allowMissing)
{
  guard_t guard (m_mutex);

  // Fetch the pointer from the store, or raise an exception if we don't
  // have a non-const store.
  void* ptr = 0;
  if (m_store)
    ptr = m_store->getData (auxid, this->size_v(), this->capacity_v());
  else if (getConstStore())
    throw SG::ExcConstAuxData ("fetch item", auxid);
  else
    throw SG::ExcNoAuxStore (auxid);

  // Check that we got a good pointer back, otherwise throw.
  if (ptr) {
    m_cache.store (auxid, ptr);

    // Set the same entry in the other caches as well.
    m_constCache.store (auxid, ptr);
    m_decorCache.store (auxid, ptr);
  }
  else if (!allowMissing)
    throw SG::ExcBadAuxVar (auxid);

  return ptr;
}


/**
 * @brief Out-of-line portion of data access (const version).
 * @param auxid aux data item being accessed.
 * @param allowMissing If true, then return nullptr if the variable
 *                     is missing rather than throwing an exception.
 *
 * When this function returns, the cache entry @c m_constCache[auxid]
 * will be valid.  That entry is also returned.  If there's an error,
 * the function will throw  an exception rather than returning.
 */
const void* AuxVectorData::getDataOol (SG::auxid_t auxid,
                                       bool allowMissing) const
{
  guard_t guard (m_mutex);

  // Fetch the pointer from the store, or raise an exception if we don't
  // have a const store.
  const void* ptr = 0;
  if (getConstStore())
    ptr = getConstStore()->getData (auxid);
  else
    throw SG::ExcNoAuxStore (auxid);

  // Check that we got a good pointer back, otherwise throw.
  if (ptr) {
    // We could avoid the const_cast here by having distinct const and
    // non-const Cache types, holding const void* and void*, respectively.
    // However, since this is a purely internal class that users don't
    // deal with directly, that's not worth the bother (and the extra code).
    void* vp ATLAS_THREAD_SAFE = const_cast<void*> (ptr);
    m_constCache.store (auxid, vp);
  }
  else if (!allowMissing)
    throw SG::ExcBadAuxVar (auxid);

  return ptr;
}


/**
 * @brief Out-of-line portion of data access (decorator version).
 * @param auxid aux data item being accessed.
 *
 * When this function returns, the cache entry @c m_cache[auxid]
 * will be valid.  That entry is also returned.  If there's an error,
 * the function will throw  an exception rather than returning.
 *
 * The difference between @c getDecorationOol and @c getDataOol is that
 * @c getDecorationOol takes a const container as input, but returns
 * a non-const pointer.  This will only succeed if either the
 * container is not locked or the item was first accessed
 * as a decoration.
 */
void* AuxVectorData::getDecorationOol (SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);

  // Fetch the pointer from the store, or raise an exception if we don't
  // have a non-const store.
  void* ptr = 0;
  if (m_store)
    ptr = m_store->getDecoration (auxid, this->size_v(), this->capacity_v());
  else if (getConstStore()) {
    // The whole point of decorations is to allow adding information to
    // something that's otherwise const.  So we have the const_cast here.
    // The store object is responsible for determining whether the
    // modification is really allowed or not.
    IConstAuxStore* store ATLAS_THREAD_SAFE =
       const_cast<IConstAuxStore*> (getConstStore());
    ptr = store->getDecoration (auxid, this->size_v(), this->capacity_v());
  }
  else
    throw SG::ExcNoAuxStore (auxid);

  // Check that we got a good pointer back, otherwise throw.
  if (!ptr)
    throw SG::ExcBadAuxVar (auxid);

  m_decorCache.store (auxid, ptr);

  // Set the same entry in the const cache as well.
  m_constCache.store (auxid, ptr);

  return ptr;
}


/**
 * @brief Cache manager constructor.
 */
AuxVectorData::Cache::Cache()
  : m_cache(),
    m_cache_len(0)
{
}


#if __cplusplus > 201100
/**
 * @brief Cache manager move constructor.
 * @param rhs The cache from which to copy.
 */
AuxVectorData::Cache::Cache (Cache&& rhs)
  : m_cache_len (rhs.m_cache_len),
    m_allcache (std::move (rhs.m_allcache))
{
  m_cache[0] = rhs.m_cache[0];
  m_cache[1] = rhs.m_cache[1];
  rhs.m_cache[0] = 0;
  rhs.m_cache[1] = 0;
  rhs.m_cache_len = 0;
}


/**
 * @brief Cache manager move assignment.
 * @param rhs The cache from which to copy.
 */
AuxVectorData::Cache& AuxVectorData::Cache::operator= (Cache&& rhs)
{
  if (this != &rhs) {
    clear();
    m_cache_len = rhs.m_cache_len;
    m_allcache = std::move (rhs.m_allcache);
    
    m_cache[0] = rhs.m_cache[0];
    m_cache[1] = rhs.m_cache[1];
    rhs.m_cache[0] = 0;
    rhs.m_cache[1] = 0;
    rhs.m_cache_len = 0;
  }
  return *this;
}
#endif


/**
 * @brief Cache manager destructor.
 */
AuxVectorData::Cache::~Cache()
{
  for (size_t i=0; i < m_allcache.size(); i++) delete [] m_allcache[i];
}


/**
 * @brief Swap this cache object with another.
 * @param other The cache object with which to swap.
 */
void AuxVectorData::Cache::swap (Cache& other)
{
  m_allcache.swap (other.m_allcache);
  std::swap (m_cache, other.m_cache);
  std::swap (m_cache_len, other.m_cache_len);
}


/**
 * @brief Clear the cache (and free any old cache vectors).
 */
void AuxVectorData::Cache::clear()
{
  if (m_cache_len > 0) {
    if (m_allcache.size() > 1) {
      for (size_t i=0; i < m_allcache.size()-1; i++)
        delete [] m_allcache[i];
      m_allcache[0] = m_allcache.back();
      m_allcache.resize(1);
}
    std::fill (m_cache[0], m_cache[0] + m_cache_len, static_cast<void*>(0));
  }
}


/**
 * @brief Store a pointer for @c auxid in the cache.
 * @param auxid The aux data item being stored.
 * @param ptr Pointer to the start of the aux vector for @c auxid.
 */
void AuxVectorData::Cache::store (SG::auxid_t auxid, void* ptr)
{
  // We must be holding the container lock m_mutex to call this.

  if (auxid >= m_cache_len) {
    // We need to expand the cache vector.   Allocate a new one.
    size_t newlen =
      std::max (static_cast<SG::auxid_t>(AuxVectorData::s_minCacheLen),
                (auxid+1)*3/2);
    void** newcache = new void*[newlen];
    m_allcache.push_back (newcache);
    void** oldcache = m_cache[0];

    // Copy old vector to the new one and clear the remainder.
    std::copy (oldcache, oldcache + m_cache_len, newcache);
    std::fill (newcache + m_cache_len, newcache + newlen,
               static_cast<void*>(0));

    // Store so that other threads can see it.
    // The stores to m_cache must happen before the store to m_cache_len;
    // we use a fence to ensure this.
    m_cache[0] = newcache;
    m_cache[1] = newcache;
    AthContainers_detail::fence_seq_cst();
    m_cache_len = newlen;
  }

  // We have room in the cache vector now.  Store the pointer.
  m_cache[0][auxid] = ptr;
}


/**
 * @brief Lock the container.
 *
 * After this, only decorations can be changed/modified.
 * If the container is already locked, this is a no-op.
 */
void AuxVectorData::lock()
{
  if (m_store) {
    m_store->lock();
    clearCache();
  }

  // No error if no store or no writable store.
}


/**
 * @brief Clear all decorations.
 *
 * Erase all decorations from the store, restoring the state to when
 * @c lock was called.
 *
 * Returns true if there were any decorations that were cleared,
 * false if the store did not contain any decorations.
 */
bool AuxVectorData::clearDecorations() const
{
  bool ret = false;
  if (m_store) {
    ret = m_store->clearDecorations();
    m_cache.clear();
    m_constCache.clear();
    m_decorCache.clear();
  }
  else if (getConstStore()) {
    // The whole point of decorations is to allow adding information to
    // something that's otherwise const.  So we have the const_cast here.
    // The store object is responsible for determining whether the
    // modification is really allowed or not.
    IConstAuxStore* store ATLAS_THREAD_SAFE =
       const_cast<IConstAuxStore*> (getConstStore());
    ret = store->clearDecorations();
  }
  else
    throw SG::ExcNoAuxStore ("lock");
  return ret;
}


} // namespace SG


#ifndef XAOD_STANDALONE


/**
 * @brief Propagate thinning.  Called after applying thinning to @c in.
 * @param in The object that was thinned.
 * @param svc The thinning service (for convenience).
 * @param filter Lists the elements to be kept.
 * @param op How to merge results: AND or OR.
 *
 * This will be called whenever a request is made to thin a @c DataVector.
 *
 * Otherwise, we pass the thinning request on to the aux store, provided
 * that it exists and is in the event store.
 */
StatusCode thinningHook (const SG::AuxVectorData* in,
                         IThinningSvc* svc,
                         const IThinningSvc::Filter_t& filter,
                         const IThinningSvc::Operator::Type op )
{
  const SG::IConstAuxStore* store = in->getConstStore();
  if (store && svc->proxy (store))
    return svc->typelessFilter (store, filter, op);
  return StatusCode::SUCCESS;
}


#endif // not XAOD_STANDALONE
