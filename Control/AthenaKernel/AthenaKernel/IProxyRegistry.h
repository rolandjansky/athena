/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IProxyRegistry   
 * @brief a proxy registry (a read/write dictionary)
 *
 * @author Paolo Calafiura - ATLAS
 *  $Id: IProxyRegistry.h,v 1.11 2006-08-29 18:17:09 srini Exp $
 ***************************************************************************/

#ifndef ATHENAKERNEL_IPROXYREGISTRY_H
# define ATHENAKERNEL_IPROXYREGISTRY_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/StoreID.h"
#include <string>

//<<<<<< CLASS FORWARD DECLARATIONS                                      >>>>>>
namespace SG {
  class DataProxy;
  class TransientAddress;
}

class IProxyRegistry {
public:
  /// add proxy to store.
  virtual StatusCode addToStore(const CLID& id, SG::DataProxy* proxy) = 0;

  virtual StatusCode addAlias(const std::string& key, SG::DataProxy* proxy) = 0;

  virtual StoreID::type storeID() const = 0;  //FIXME this should not be here

  /// locate a proxy for a given TransientAddress
  virtual SG::DataProxy* proxy(const SG::TransientAddress* tAD) const = 0;

  /// get proxy with given id. Returns 0 to flag failure
  virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const=0;

  /// get proxy with given id. Returns 0 to flag failure
  /// the key must match exactly (no wildcarding for the default key)
  virtual SG::DataProxy* proxy_exact(const CLID& id,
                                     const std::string& key) const = 0;

  virtual ~IProxyRegistry() {}
};
#endif // ATHENAKERNEL_IPROXYREGISTRY_H










