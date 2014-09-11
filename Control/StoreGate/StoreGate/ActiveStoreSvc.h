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

template <class TYPE> class SvcFactory;

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

class ActiveStoreSvc : virtual public IProxyDict,
                       public Service {
public:
  friend class SvcFactory<ActiveStoreSvc>;

  ///returns pointer to the active store as StoreGateSvc
  inline StoreGateSvc* activeStore() const {
    return p_activeStore;
  }

  ///dereference operator to access the active store
  inline StoreGateSvc* operator->() const {
    return p_activeStore;
  }

  ///set the active store pointer: used by the event loop mgrs
  inline void setStore(StoreGateSvc* s) { p_activeStore = s; }

  //@{ @name IProxyDict implementation
  /// get proxy for a given data object address in memory,
  /// but performs a deep search among all possible 'symlinked' containers
  virtual SG::DataProxy* deep_proxy(const void* const pTransient) const;

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy(const void* const pTransient) const;

  /// get default proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id) const;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const;

  /// return the list of all current proxies in store
  virtual std::vector<const SG::DataProxy*> proxies() const;
  //@}

  /// Raw addition of a proxy to the store.
  StatusCode addToStore (CLID id, SG::DataProxy* proxy);

  //@{ @name Gaudi Service boilerplate
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}

  /// not really kosher: should be in IActiveStoreSvc
  static const InterfaceID& interfaceID(); 


private:
  StoreGateSvc* p_activeStore;    

protected:
    
  /// Standard Service Constructor. sets active store to default event store
  ActiveStoreSvc(const std::string& name, ISvcLocator* svc);

  virtual ~ActiveStoreSvc();

};
#endif // STOREGATE_ACTIVESTORESVC_H



