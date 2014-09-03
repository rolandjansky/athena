///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IPROXYDICT_H
# define ATHENAKERNEL_IPROXYDICT_H

// INCLUDES
#include "AthenaKernel/sgkey_t.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ClassID.h"
#include <string>
#include <vector>

// DECLARATIONS
namespace SG {
  class DataProxy;
}

/** @class IProxyDict   
 * @brief a proxy dictionary. 
 * it inherits from INamedInterface because clients need its name (e.g. data links)
 *
 * @author Paolo Calafiura - ATLAS
 *  $Id: IProxyDict.h,v 1.5 2007-12-11 02:56:22 binet Exp $
 */

class IProxyDict : virtual public INamedInterface {
public:
  /// get proxy for a given data object address in memory,
  /// but performs a deep search among all possible 'symlinked' containers
  virtual SG::DataProxy* deep_proxy(const void* const pTransient) const=0;

  /// get proxy for a given data object address in memory
  virtual SG::DataProxy* proxy(const void* const pTransient) const=0;

  /// get default proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id) const=0;

  /// get proxy with given id and key. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const=0;

  /// Get proxy given a hashed key+clid.
  /// Find an exact match; no handling of aliases, etc.
  /// Returns 0 to flag failure.
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const = 0;

  /// Add a new proxy to the store.
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) = 0;

  /// return the list of all current proxies in store
  virtual std::vector<const SG::DataProxy*> proxies() const = 0;

  virtual ~IProxyDict() {}

  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();
};
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline
const InterfaceID& 
IProxyDict::interfaceID() {
  static const InterfaceID _IID("IProxyDict", 1, 0);
  return _IID;
}
#endif // ATHENAKERNEL_IPROXYDICT_H










