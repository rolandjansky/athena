/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_ACTIVESTORESVC_H
#define STOREGATE_ACTIVESTORESVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/IProxyDict.h"

class StoreGateSvc;
class ISvcLocator;


/** @class ActiveStoreSvc
 *  @brief A service that caches a pointer to the currently active store. 
 *  It also implements the IProxyDict store interface
 *
 *  In most jobs the active store is simply the default event store 
 *  (named "StoreGateSvc"). When doing pile-up (and perhaps later on in 
 *  multi-threaded jobs, the active store changes during the event loop
 *  execution. It is important, for example, that converters always refer
 *  to the active store rather than to the default one.
 *  @author ATLAS Collaboration
 *  $Id: ActiveStoreSvc.h,v 1.10 2009-04-18 02:56:20 calaf Exp $
 **/

class ActiveStoreSvc : public IProxyDict,
                       public Service
{
public:

  ///returns pointer to the active store as StoreGateSvc
  inline StoreGateSvc* activeStore() const {
    if (p_activeStore) return p_activeStore;
    return activeStoreOOL();
  }

  ///dereference operator to access the active store
  inline StoreGateSvc* operator->() const {
    return activeStore();
  }

  /// TEMPORARY: avoid warnings
  using IProxyDict::proxy;

  ///set the active store pointer: used by the event loop mgrs
  void setStore(StoreGateSvc* s);

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy(const void* const pTransient) const override;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const override;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override;

  /// return the list of all current proxies in store
  virtual std::vector<const SG::DataProxy*> proxies() const override;
  //@}

  /// Raw addition of a proxy to the store.
  StatusCode addToStore (CLID id, SG::DataProxy* proxy) override;

  /**
   * @brief Record an object in the store.
   * @param obj The data object to store.
   * @param key The key as which it should be stored.
   * @param allowMods If false, the object will be recorded as const.
   * @param returnExisting If true, return proxy if this key already exists.
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
  virtual
  SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                               const std::string& key,
                               bool allowMods,
                               bool returnExisting) override;


  /**
   * @brief Inform HIVE that an object has been updated.
   * @param id The CLID of the object.
   * @param key The key of the object.
   */
  virtual StatusCode updatedObject (CLID id, const std::string& key) override;


  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) override;

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key) const override;

  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const override;

  /**
   * @brief Remember an additional mapping from key to string/CLID.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param clid The CLID associated with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   * Logs an error if @c key already corresponds to a different string.
   */
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid) override;


  //@{ @name Gaudi Service boilerplate
  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  //@}

  /// not really kosher: should be in IActiveStoreSvc
  static const InterfaceID& interfaceID(); 
  
  /// Standard Service Constructor. sets active store to default event store
  ActiveStoreSvc(const std::string& name, ISvcLocator* svc);

  virtual ~ActiveStoreSvc() override;

private:
  StoreGateSvc* activeStoreOOL() const;
  StoreGateSvc* p_activeStore;    
  std::string m_storeName; //< property: StoreGate instance name

};
#endif // STOREGATE_ACTIVESTORESVC_H



