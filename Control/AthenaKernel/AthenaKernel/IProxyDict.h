///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IPROXYDICT_H
# define ATHENAKERNEL_IPROXYDICT_H

// INCLUDES
#include "AthenaKernel/sgkey_t.h"
#include "AthenaKernel/IStringPool.h"
#include "AthenaKernel/IHiveStore.h"
#include "AthenaKernel/DataObjectSharedPtr.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ClassID.h"
#include <string>
#include <vector>
#include <memory>

// DECLARATIONS
namespace SG {
  class DataProxy;
}
class DataObject;
class IResetable;
class IConverter;


/**
 * @brief A proxy dictionary. 
 *
 * This is the internal interface used by StoreGateSvc and similar.
 * It provides interfaces for taking a set of @c DataProxy objects
 * and looking them up by either CLID+key or by a pointer to the
 * transient objects.
 *
 * Objects can be identified by either a CLID+key pair or by a hash of this
 * data.  This interface also provides methods for converting between
 * the two representations.
 *
 * 
 * @author Paolo Calafiura - ATLAS
 *  $Id: IProxyDict.h,v 1.5 2007-12-11 02:56:22 binet Exp $
 */
class IProxyDict : virtual public IStringPool,
                   virtual public IHiveStore,
                   virtual public INamedInterface

{
public:
  DeclareInterfaceID (IProxyDict, 2, 0);
  virtual ~IProxyDict() override {}

  /**
   * @brief Get proxy given a hashed key+clid.
   * @param sgkey Hashed key to look up.
   *
   * Find an exact match; no handling of aliases, etc.
   * Returns 0 to flag failure.
   */
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const = 0;


  /**
   * @brief Get proxy with given id and key.
   * @param id The @c CLID of the desired object.
   * @param key The key of the desired object.
   *
   * If the key is a null string, then it is a @em default key.
   * Finding a proxy via the default key should succeed only if there
   * is exactly one object with the given @c CLID in the store.
   * Finding a proxy via a default key is considered deprecated
   * for the case of the event store.
   * 
   * Returns 0 to flag failure
   */
  virtual SG::DataProxy* proxy(const CLID& id,
                               const std::string& key) const = 0;


  /**
   * @brief Get a proxy referencing a given transient object.
   * @param pTransient The object to find.
   * 
   * Returns 0 to flag failure
   */
  virtual SG::DataProxy* proxy(const void* const pTransient) const=0;


  /**
   * @brief Return the list of all current proxies in store.
   */
  virtual std::vector<const SG::DataProxy*> proxies() const = 0;


  /**
   * @brief Add a new proxy to the store.
   * @param id CLID as which the proxy should be added.
   * @param proxy The proxy to add.
   *
   * Simple addition of a proxy to the store.  The key is taken as the
   * primary key of the proxy.  Does not handle things
   * like overwrite, history, symlinks, etc.  Should return failure
   * if there is already an entry for this clid/key.
   */
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) = 0;


  /**
   * @brief Record an object in the store.
   * @param obj The data object to store.
   * @param key The key as which it should be stored.
   * @param allowMods If false, the object will be recorded as const.
   * @param returnExisting If true, return proxy if this key already exists.
   *                       If the object has been recorded under a different
   *                       key, then make an alias.
   *                       If the object has been recorded under a different
   *                       clid, then make a link.
   *
   * Full-blown record.  @c obj should usually be something
   * deriving from @c SG::DataBucket.
   *
   * Returns the proxy for the recorded object; nullptr on failure.
   * If the requested CLID/key combination already exists in the store,
   * the behavior is controlled by @c returnExisting.  If true, then
   * the existing proxy is returned; otherwise, nullptr is returned.
   * In either case, @c obj is destroyed.
   */
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                       const std::string& key,
                                       bool allowMods,
                                       bool returnExisting) = 0;


  /**
   * @brief Inform HIVE that an object has been updated.
   * @param id The CLID of the object.
   * @param key The key of the object.
   */
  virtual StatusCode updatedObject (CLID id, const std::string& key) = 0;


  /**
   * @brief Tell the store that a handle has been bound to a proxy.
   * @param handle The handle that was bound.
   * The default implementation does nothing.
   */
  virtual void boundHandle (IResetable* handle);


  /**
   * @brief Tell the store that a handle has been unbound from a proxy.
   * @param handle The handle that was unbound.
   * The default implementation does nothing.
   */
  virtual void unboundHandle (IResetable* handle);


  /**
   * @brief Test to see if the target of an ElementLink has moved.
   * @param sgkey_in Original hashed key of the EL.
   * @param index_in Original index of the EL.
   * @param sgkey_out[out] New hashed key for the EL.
   * @param index_out[out] New index for the EL.
   * @return True if there is a remapping; false otherwise.
   *
   * The default implementation here always returns false.
   */
  virtual bool tryELRemap (sgkey_t sgkey_in, size_t index_in,
                           sgkey_t& sgkey_out, size_t& index_out);


  /**
   * @brief Call converter to create an object, possibly with locking.
   * @param cvt The converter to call.
   * @param addr Opaque address information for the object to create.
   * @param refpObject Reference to location of the pointer of the
   *                   created object.
   *
   * This calls the @c createObj method on @c cvt to create the referenced
   * transient object.
   *
   * This will also lock the store, if that is required.
   *
   * The default implementation just forwards the method call,
   * with no locking.
   */
  virtual StatusCode createObj (IConverter* cvt,
                                IOpaqueAddress* addr,
                                DataObject*& refpObject);


  virtual IProxyDict* hiveProxyDict() override
  { return this; }
};


#endif // ATHENAKERNEL_IPROXYDICT_H
