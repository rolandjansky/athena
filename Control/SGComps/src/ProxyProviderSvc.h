// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ProxyProviderSvc.h
    @brief manages the address providers and add proxies on demand to the store
  * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
  * $Id: ProxyProviderSvc.h,v 1.3 2008-03-05 21:57:44 gemmeren Exp $
  */


#ifndef SGCOMPS_PROXYPROVIDERSVC_H
# define SGCOMPS_PROXYPROVIDERSVC_H 1

#include "AthenaKernel/IProxyProviderSvc.h"
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/Property.h" /*StringArrayProperty*/
#include "GaudiKernel/StatusCode.h"
#include <cassert>
#include <list>
#include <string>
#include <algorithm>
#include <vector>

// framework includes
#include "AthenaBaseComps/AthService.h"

namespace SG {
  class DataProxy;
  class TransientAddress;
}

class IAddressProvider;
class IProxyRegistry; //this is DataStore
class IConversionSvc;
class IOpaqueAddress;
class ISvcLocator;

///manages the address providers and add proxies on demand to the store
class ProxyProviderSvc : public extends<AthService, IProxyProviderSvc>
{
public:
  typedef std::list<IAddressProvider*>::iterator  pAPiterator;
  typedef std::list<SG::TransientAddress*> TAdList;
  typedef TAdList::iterator TAdIterator;

  ///IProxyProvider interface
  //@{
  ///add proxies to the store to modify (before Begin Event)
  virtual StatusCode preLoadProxies(IProxyRegistry& storeToModify) override;

  ///add proxies to the store to modify (during Begin Event)
  virtual StatusCode loadProxies(IProxyRegistry& storeToModify) override;

  /// Use a provider to create a proxy for ID/KEY.
  /// If successful, the new proxy will be added to DATASTORE
  /// and returned; otherwise, return null.
  virtual SG::DataProxy* retrieveProxy(const CLID& id, const std::string& key,
				       IProxyRegistry& storeToModify) override;

  ///create a new Proxy, overriding CLID and/or key
  SG::DataProxy* addAddress(IProxyRegistry& storeToModify, 
			    SG::TransientAddress&& tad);
  //@}


  ///IAddressProvider manager functionality
  ///add a provider to the set of known ones. PROVIDER IS OWNED BY THE CLIENT
  virtual void addProvider(IAddressProvider* aProvider) override;

  /// Service boilerplate
  //@{
  virtual StatusCode initialize() override;
  //@}

  /// Standard Service Constructor
  ProxyProviderSvc(const std::string& name, ISvcLocator* svcLoc);
  virtual ~ProxyProviderSvc();

private:
  /**
   * @brief Retrieve the EventContext saved in store DS.
   * @param ds The store from which to retrieve the context.
   *
   * If there is no context recorded in the store, return a default-initialized
   * context.
   */
   const EventContext& contextFromStore (IProxyRegistry& ds) const;


  /**
   * @brief Add lists of TADs to the store.
   * @param store Store to which to add.
   * @param tList List of TADs from all providers.
   */
  StatusCode addAddresses(IProxyRegistry& store, 
                          TAdList& tList);

  
  /// property: the services declared as providers
  StringArrayProperty m_providerNames;
  /// the handler for m_providerNames
  void providerNamesPropertyHandler( Property& theProp );
  
  StatusCode doPreLoadProxies(IProxyRegistry& storeToModify);

  /// the providers we know about. WE DON'T OWN THEM
  std::list<IAddressProvider*> m_providers; 
  /// Persistency Service
  IConversionSvc* m_pDataLoader;   

  /// List of pending stores on which to run preLoadProxies().
  std::vector<IProxyRegistry*> m_pendingLoad;
};


inline void ProxyProviderSvc::addProvider(IAddressProvider* aProvider) {
  assert(aProvider);
  if (find(m_providers.begin(), m_providers.end(), aProvider) == m_providers.end()) {
    m_providers.push_back(aProvider);
  }
}

#endif // !SGCOMPS_PROXYPROVIDERSVC_H
