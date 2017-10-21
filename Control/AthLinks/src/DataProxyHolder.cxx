/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/src/DataProxyHolder.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2014
 * @brief Manage @c DataProxy reference in ElementLink/DataLink.
 */


#include "AthLinks/tools/DataProxyHolder.h"
#include "AthLinks/exceptions.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace {


/// Current input renaming map.
const SG::DataProxyHolder::InputRenameRCU_t* s_inputRenameMap ATLAS_THREAD_SAFE
  = nullptr;


}


namespace SG {


/**
 * @brief Set the link to an object given by a pointer.
 * @param obj Pointer to the object.
 * @param link_clid CLID of the link being set.
 * @param sg Associated store.
 * @returns The SG key for this object.
 *
 * This will try to look up the proxy for @c obj.  If that succeeds,
 * then we store the pointer to the proxy and return the SG key.
 * Otherwise, we store a pointer to the object itself, flagging
 * this case with the low bit, and return 0.
 *
 * If @c sg is 0, then we take the store from whatever the link's currently
 * set to.  If the link has no current store, then we take the global
 * default.
 */
DataProxyHolder::sgkey_t
DataProxyHolder::toStorableObject (const_pointer_t obj,
                                   CLID link_clid,
                                   IProxyDict* sg)
{
  sgkey_t key = 0;
  if (obj == 0) {
    // Setting the link to null.
    m_proxy = 0;
  }
  else {
    // Find the store to use.
    if (sg == 0)
      sg = this->source1();
    if (sg == 0)
      sg = SG::CurrentEventStore::store();

    m_proxy = sg->proxy (obj);
    if (m_proxy == 0) {
      // Didn't find a proxy for this object.
      // Store the object pointer directly, and return 0.
      storeObjpointer (obj);
    }
    else {
      // Found a proxy.  Fetch the SG key and check that the type of the object
      // is consistent with the link type.
      key = m_proxy->sgkey();
      if (link_clid != m_proxy->clID() && !m_proxy->transientID (link_clid)) {
        if (m_proxy->clID() != CLID_NULL)
          throw SG::ExcCLIDMismatch (m_proxy->clID(), link_clid);

        // Transient clid was null.
        // This can happen when reading a view vector with xAODRootAccess
        // in an athena build, where the TAD may not get a CLID set.
        // Check based on key.
        sgkey_t link_sgkey = sg->stringToKey (m_proxy->name(), link_clid);
        if (link_sgkey != m_proxy->sgkey())
          throw SG::ExcCLIDMismatch (m_proxy->clID(), link_clid);
      }
    }
  }
  return key;
}


/**
 * @brief Set the link to an object given by a string key.
 * @param dataID Key of the object.
 * @param link_clid CLID of the link being set.
 * @param sg Associated store.
 * @returns The SG key for this object.
 *
 * This will try to look up the proxy for @c dataID.  If that succeeds,
 * then we store the pointer to the proxy.  Otherwise, we create
 * a dummy proxy and add it to the store.  We return the SG key
 * in either case.
 *
 * If @c sg is 0, then we take the store from whatever the link's currently
 * set to.  If the link has no current store, then we take the global
 * default.
 */
DataProxyHolder::sgkey_t
DataProxyHolder::toIdentifiedObject (const ID_type& dataID,
                                     CLID link_clid,
                                     IProxyDict* sg)
{
  // Find the store to use.
  if (sg == 0)
    sg = this->source1();
  if (sg == 0)
    sg = SG::CurrentEventStore::store();

  if (!sg)
    return 0;

  // Look up the proxy.
  m_proxy = sg->proxy (link_clid, dataID);
  if (m_proxy == 0) {
    // Didn't find a proxy; make a dummy.
    SG::TransientAddress tad (link_clid, dataID);
    tad.setSGKey (sg->stringToKey (dataID, link_clid));
    m_proxy = new SG::DataProxy (std::move(tad), static_cast<IConverter*>(nullptr));
    if (sg->addToStore (link_clid, m_proxy).isFailure())
      std::abort();
  }

  // Return the proxy's sgkey.
  return m_proxy->sgkey();
}


/**
 * @brief Set the link to an object given by a hashed key.
 * @param key Hashed key of the object.
 * @param link_clid CLID of the link being set.
 * @param sg Associated store.
 *
 * This will try to look up the proxy for @c key.  If that succeeds,
 * then we store the pointer to the proxy.  Otherwise, we create
 * a dummy proxy and add it to the store.
 *
 * If @c sg is 0, then we take the store from whatever the link's currently
 * set to.  If the link has no current store, then we take the global
 * default.
 *
 * May throw @c ExcCLIDMismatch.
 */
void
DataProxyHolder::toIdentifiedObject (sgkey_t sgkey,
                                     CLID link_clid,
                                     IProxyDict* sg)
{
  if (!sgkey) return;

  // Find the store to use.
  if (sg == 0)
    sg = this->source1();
  if (sg == 0)
    sg = SG::CurrentEventStore::store();

  if (!sg)
    return;

  // Look up the proxy.
  m_proxy = sg->proxy_exact (sgkey);

  CLID clid = CLID_NULL;
  const std::string* key = nullptr;
  if (m_proxy == 0) {
    // Didn't find it --- have SG query proxy providers.
    // Try to turn the hashed key into a string key + clid.
    key = sg->keyToString (sgkey, clid);
    if (key) {
      if (link_clid != CLID_NULL && clid != link_clid)
        throw SG::ExcCLIDMismatch (clid, link_clid);
      m_proxy = sg->proxy (clid, *key);
    }
  }
  
  if (m_proxy == 0) {
    // Still didn't find it --- make a dummy.
    SG::TransientAddress tad;
    if (key) {
      tad = SG::TransientAddress (clid, *key);
    }
    tad.setSGKey (sgkey);
    m_proxy = new SG::DataProxy (std::move(tad), static_cast<IConverter*>(nullptr));
    if (sg->addToStore (clid, m_proxy).isFailure())
      std::abort();
  }
  else if (link_clid != CLID_NULL &&
           m_proxy->clID() != CLID_NULL &&
           !m_proxy->transientID(link_clid))
  {
    // Found a proxy, but types don't match.
    throw SG::ExcCLIDMismatch (m_proxy->clID(), link_clid);
  }
}


/**
 * @brief Return the SG key that we reference, as a string.
 *
 * Returns a null string on failure.
 */
const DataProxyHolder::ID_type&
DataProxyHolder::dataID() const
{
  SG::DataProxy* dp = proxy (true);
  if (dp)
    return dp->name();

  static const std::string dummy;
  return dummy;
}


/**
 * @brief Return a pointer to the currently-referenced object.
 * @param castfn Function to do the cast from data proxy to object.
 *               If 0, use a dynamic cast.
 * @param clid The CLID of the desired object.
 *             This is used to determine how the returned pointer
 *             is to be converted.
 * @return A pointer to an object of the type given by @a clid,
 *         or null on a failure (or if the reference is null).
 */
void* DataProxyHolder::storableBase (castfn_t* castfn, CLID clid) const
{
  // Test for null link.
  if (!m_proxy)
    return 0;

  // Test for direct pointer to an object.
  if (isObjpointer())
    return objpointer();

  // We have a proxy.  Get the object pointer from the proxy.
  // We have to take care of converting to the proper return type, though
  // (as requested by clid).
  void* obj = castfn ? castfn (m_proxy) : SG::DataProxy_cast (m_proxy, clid);
  if (obj)
    return obj;

  // We failed.  It may be that we don't have a conversion
  // between clid and the object type: it may have been stored
  // using a hard cast.  Check to see if this object has actually
  // been registered under the requested clid.
  if (m_proxy->transientID (clid)) {
    DataBucketBase* db =
      dynamic_cast<DataBucketBase*> (m_proxy->accessData());

    // Do a hard cast...
    if (db)
      return db->object();
  }

  return 0;
}


/**
 * @brief Return the data source for this reference.
 *
 * If the link is null, return 0.
 * If we're pointing at an object directly, then we return the default store
 * if the object is found in SG; otherwise, throw @c ExcPointerNotInSG.
 */
IProxyDict* DataProxyHolder::source() const
{
  SG::DataProxy* dp = proxy();
  if (!dp)
    return 0;
  return dp->store();
}


/**
 * @brief Finish initialization after link has been read.
 * @param sgkey Hashed SG key.
 * @param sg Associated store.
 *
 * This should be called after a link has been read by root
 * in order to set the proxy pointer.  It calls @c toIdentifiedObject
 * with the provided hashed key.
 *
 * If @c sg is 0, then we use the global default store.
 */
void
DataProxyHolder::toTransient (sgkey_t sgkey, IProxyDict* sg /*= 0*/)
{
  m_proxy = 0;

  // Find the store to use.
  if (sg == 0)
    sg = this->source1();
  if (sg == 0)
    sg = SG::CurrentEventStore::store();

  // Do input renaming.
  if (s_inputRenameMap) {
    Athena::RCURead<InputRenameMap_t> r (*s_inputRenameMap);
    auto it = r->find (sgkey);
    if (it != r->end())
      sgkey = it->second;
  }

  if (sgkey)
    toIdentifiedObject (sgkey, CLID_NULL, sg);
}


/**
 * @brief Finish initialization after link has been read.
 * @param dataID Key of the object.
 * @param link_clid CLID of the link being set.
 * @param sg Associated store.
 * @returns The hashed SG key for this object.
 *
 * This should be called after a link has been read by root
 * in order to set the proxy pointer.  It calls @c toIdentifiedObject
 * with the provided hashed key.
 *
 * If @c sg is 0, then we use the global default store.
 */
DataProxyHolder::sgkey_t
DataProxyHolder::toTransient (const ID_type& dataID,
                              CLID link_clid,
                              IProxyDict* sg /*= 0*/)
{
  // Find the store to use.
  if (sg == 0)
    sg = this->source1();
  if (sg == 0)
    sg = SG::CurrentEventStore::store();

  sgkey_t sgkey = sg->stringToKey (dataID, link_clid);
  toTransient (sgkey, sg);
  return sgkey;
}


/**
 * @brief Prepare this link for writing.
 * @param sgkey Reference to the hashed SG key.
 *
 * One of the @c toPersistent methods should be called before
 * trying to write the link with root.
 *
 * This takes a reference to the hashed SG key.  In the case where we're
 * referencing an object directly by pointer, the hashed key will be 0.
 * In that case, we try to look up the object in the default store.
 * If we find it, the hashed key is updated appropriately; otherwise,
 * we throw @c ExcPointerNotInSG.
 *
 * This version does not perform link remapping.
 */
void DataProxyHolder::toPersistentNoRemap (sgkey_t& sgkey)
{
  if (!sgkey && m_proxy) {
    m_proxy = proxy(); // May throw.
    sgkey = m_proxy->sgkey();
  }
}


/**
 * @brief Prepare this link for writing.
 * @param sgkey Reference to the hashed SG key.
 * @param index Index of this link.
 *
 * One of the @c toPersistent methods should be called before
 * trying to write the link with root.
 *
 * This takes a reference to the hashed SG key.  In the case where we're
 * referencing an object directly by pointer, the hashed key will be 0.
 * In that case, we try to look up the object in the default store.
 * If we find it, the hashed key is updated appropriately; otherwise,
 * we throw @c ExcPointerNotInSG.
 *
 * If the target of the link has been remapped, then the @c sgkey and @c index
 * parameters will be updated to reflect that, and @c true will be returned.
 * Otherwise, if there was no remapping, then @c false will be returned.
 *
 * This version is for the case where indices are given by @c uint64_t.
 */
bool DataProxyHolder::toPersistent (sgkey_t& sgkey, uint64_t& index)
{
  toPersistentNoRemap (sgkey);
  size_t index_s = index;
  bool ret = tryRemap (sgkey, index_s);
  if (ret)
    index = index_s;
  return ret;
}


/**
 * @brief Prepare this link for writing.
 * @param sgkey Reference to the hashed SG key.
 * @param index Index of this link.
 *
 * One of the @c toPersistent methods should be called before
 * trying to write the link with root.
 *
 * This takes a reference to the hashed SG key.  In the case where we're
 * referencing an object directly by pointer, the hashed key will be 0.
 * In that case, we try to look up the object in the default store.
 * If we find it, the hashed key is updated appropriately; otherwise,
 * we throw @c ExcPointerNotInSG.
 *
 * If the target of the link has been remapped, then the @c sgkey and @c index
 * parameters will be updated to reflect that, and @c true will be returned.
 * Otherwise, if there was no remapping, then @c false will be returned.
 *
 * This version is for the case where indices are given by @c uint32_t.
 */
bool DataProxyHolder::toPersistent (sgkey_t& sgkey, uint32_t& index)
{
  toPersistentNoRemap (sgkey);
  size_t index_s = index;
  bool ret = tryRemap (sgkey, index_s);
  if (ret)
    index = index_s;
  return ret;
}


/**
 * @brief Test to see if the link has been remapped.
 * @param sgkey Reference to the hashed SG key.
 * @param index Index of this link.
 *
 * If this link has been remapped, @c sgkey and @c index will be
 * adjusted accordingly.
 *
 * Returns @c true if the link was remapped, @c false otherwise.
 */
bool DataProxyHolder::tryRemap (sgkey_t& sgkey, size_t& index)
{
  if (sgkey) {
    // Check for remapping.
    sgkey_t sgkey_out;
    size_t index_out = 0;
    IProxyDict* sg = this->source();
    if (sg && sg->tryELRemap (sgkey, index, sgkey_out, index_out)) {
      this->toIdentifiedObject (sgkey_out, CLID_NULL, this->source());
      sgkey = sgkey_out;
      index = index_out;
      return true;
    }
  }

  return false;
}


/**
 * @brief Adjust for thinning.
 * @param sgkey Reference to the hashed SG key.
 * @param index Index of this link.
 *
 * If this link points to a container that has been thinned,
 * @c sgkey and @c index will be adjusted accordingly.
 *
 * Returns @c true if the index was changed; @c false otherwise.
 */
bool DataProxyHolder::thin (sgkey_t& sgkey, size_t& index)
{
  bool result = false;
  if (sgkey) {
    // Check if thinning is needed for this link:
    IThinningSvc* thinSvc = IThinningSvc::instance();
    if( ! thinSvc ) {
       // If the service is not available, return now. Since this just means
       // that there is no thinning defined for this stream.
       return false;
    }
    // Try to get a DataProxy for our link:
    SG::DataProxy* dp = proxy( true );
    if( ! dp ) {
       // If we were not successful, let's give up.
       REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR,
                                    "DataProxyHolder::toPersistent" )
          << "Couldn't get proxy for target object";
       return false;
    }
    // Get the updated index:
    const std::size_t persIdx = thinSvc->index( dp, index );
    // If the object was thinned away, set the persistent variables to an
    // invalid state. Otherwise update just the index variable.
    if( persIdx == IThinningSvc::RemovedIdx ) {
       sgkey = 0;
       index = 0;
       result = true;
    }
    else {
      if (index != persIdx)
        result = true;
      index = persIdx;
    }
  }

  return result;
}


/**
 * @brief Throw a @c ExcInvalidLink exception for this link.
 * @param sgkey The hashed key for this link.
 *
 * This will fill in parameters for the exception message from the proxy.
 */
void DataProxyHolder::throwInvalidLink (sgkey_t sgkey) const
{
  CLID clid = CLID_NULL;
  std::string key;
  SG::DataProxy* dp = proxy();
  if (dp) {
    clid = dp->clID();
    key = dp->name();
  }
  throw SG::ExcInvalidLink (clid, key, sgkey);
}


/**
 * @brief Helper for @c proxy(), for the case of a direct object pointer.
 * @param nothrow If true, return 0 on failure instead of throwing
 *                an exception.
 *
 * This is the out-of-line portion of @c proxy(), called if this link
 * is directly pointing at an object.  Try to look up
 * the corresponding @c DataProxy using the default store.  Return it
 * if we find it; otherwise, either throw @c ExcPointerNotInSG or
 * return 0, depending on the @c nothrow parameter.
 */
SG::DataProxy* DataProxyHolder::proxy1(bool nothrow) const
{
  const_pointer_t obj = reinterpret_cast<const_pointer_t>
    (reinterpret_cast<unsigned long> (m_proxy) & ~1UL);
  SG::DataProxy* proxy = SG::CurrentEventStore::store()->proxy (obj);
  if (proxy == 0 && !nothrow)
    throw SG::ExcPointerNotInSG (obj);
  return proxy;
}



/**
 * @brief Compare for equality.
 */
bool DataProxyHolder::operator== (const DataProxyHolder& other) const
{
  if (m_proxy == other.m_proxy) return true;

  // One could refer to an object directly by pointer, the other could
  // have a proxy.

  const_pointer_t ptr = 0;
  const DataProxy* proxy = 0;
  if (isObjpointer() && !other.isObjpointer()) {
    ptr = objpointer();
    proxy = other.m_proxy;
  }
  else if (other.isObjpointer() && !isObjpointer()) {
    ptr = other.objpointer();
    proxy = m_proxy;
  }
  else
    return false;

  if (proxy->store()->proxy(ptr) == proxy)
    return true;

  return false;
}


/**
 * @brief Set map used for performing input renaming in toTransient.
 * @param map The new map, or nullptr for no renmaing.
 */
void DataProxyHolder::setInputRenameMap ATLAS_NOT_THREAD_SAFE (const InputRenameRCU_t* map)
{
  s_inputRenameMap = map;
}


} // namespace SG
