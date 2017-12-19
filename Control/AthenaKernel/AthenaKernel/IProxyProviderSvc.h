/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IProxyProviderSvc.h 
 * @brief add on demand proxies to the address registry (SG)
 *
 * @author pcalafiura@lbl.gov - ATLAS Collaboration
 ***************************************************************************/

// $Id: IProxyProviderSvc.h,v 1.9 2007-06-23 01:12:06 calaf Exp $

#ifndef ATHENAKERNEL_IPROXYPROVIDERSVC_H
# define ATHENAKERNEL_IPROXYPROVIDERSVC_H

#include <string>
#include <list>
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/EventContext.h"

class IAddressProvider;
class IProxyRegistry; //this is the store

namespace SG {
  class DataProxy;
  class TransientAddress;
}

class IOpaqueAddress;

class IProxyProviderSvc : virtual public IService {
public:
  DeclareInterfaceID (IProxyProviderSvc,1,0);

  /// add proxies to the store before Begin Event:
  virtual StatusCode preLoadProxies(IProxyRegistry& dataStore) = 0;

  ///add new proxies to store every Event:
  virtual StatusCode loadProxies(IProxyRegistry& dataStore) = 0;

  /// Use a provider to create a proxy for ID/KEY.
  /// If successful, the new proxy will be added to DATASTORE
  /// and returned; otherwise, return null.
  virtual SG::DataProxy* retrieveProxy(const CLID& id, 
				       const std::string& key,
				       IProxyRegistry& dataStore) = 0;

  ///IAddressProvider manager functionality
  ///add a provider to the set of known ones. PROVIDER IS OWNED BY THE CLIENT
  virtual void addProvider(IAddressProvider* aProvider) = 0;

  virtual ~IProxyProviderSvc() {}
};


#endif // ATHENAKERNEL_IPROXYPROVIDERSVC_H
