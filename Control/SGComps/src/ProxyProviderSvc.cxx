/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <string>
#include <vector>

#include "SGTools/DataProxy.h"
#include "SGTools/BaseInfo.h"

#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IProxyRegistry.h"

#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/GaudiException.h"

#include "ProxyProviderSvc.h"

#include "boost/foreach.hpp"

using namespace std;

ProxyProviderSvc::ProxyProviderSvc(const std::string& name, 
                                   ISvcLocator* svcLoc): 
  AthService(name, svcLoc),
  m_pDataLoader(0)
{
  declareProperty("ProviderNames", m_providerNames,
                  "names of the services to be use as address providers");
  m_providerNames.declareUpdateHandler(&ProxyProviderSvc::providerNamesPropertyHandler, this);
}

ProxyProviderSvc::~ProxyProviderSvc() {}

StatusCode 
ProxyProviderSvc::initialize() 
{
  msg() << MSG::INFO << "Initializing " << name() 
        << " - package version " << PACKAGE_VERSION 
        << endreq;

  const bool CREATEIF(true);
  // cache pointer to Persistency Service
  if (!(service("EventPersistencySvc", m_pDataLoader, CREATEIF)).isSuccess()) {
    m_pDataLoader = 0;
    ATH_MSG_ERROR("Could not	 get pointer to Persistency Service");
    return StatusCode::FAILURE;;
  } else {
#ifdef DEBUGPPS
    ATH_MSG_VERBOSE("Got pointer to Persistency Service " << m_pDataLoader);
#endif
  }

  //get properties set;	
  if(!(AthService::initialize()).isSuccess()) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

///IProxyProvider interface
/// add proxies (before Begin Event)
StatusCode 
ProxyProviderSvc::preLoadProxies(IProxyRegistry& store)
{
  StatusCode sc(StatusCode::SUCCESS);
  pAPiterator iProvider(m_providers.begin()), iEnd(m_providers.end());
  while (sc.isSuccess() && (iProvider != iEnd)) {
    TAdList tList;
    sc = (**iProvider).preLoadAddresses(store.storeID(), tList);
    if (sc.isSuccess()) sc = addAddresses(store, *iProvider, tList);
    ++iProvider;
  }
#ifdef DEBUGPPS
  ATH_MSG_VERBOSE("preLoadProxies returns " << sc);
#endif
  return sc;
}


///IProxyProvider interface
///add proxies to the store to modify
StatusCode 
ProxyProviderSvc::loadProxies(IProxyRegistry& store)
{
  StatusCode sc(StatusCode::SUCCESS);
  pAPiterator iProvider(m_providers.begin()), iEnd(m_providers.end());
  while (sc.isSuccess() && (iProvider != iEnd)) {
    TAdList tList;
    sc = (**iProvider).loadAddresses(store.storeID(), tList);
    if (sc.isSuccess()) sc = addAddresses(store, *iProvider, tList);
    ++iProvider;
  }
#ifdef DEBUGPPS
  ATH_MSG_VERBOSE("loadProxies returns " << sc);
#endif
  return sc;
}

/// add a list of TADs to store.
StatusCode ProxyProviderSvc::addAddresses(IProxyRegistry& store, 
					  IAddressProvider* provider,
					  TAdList& tList)
{

  TAdIterator iTAD(tList.begin()), endTAD(tList.end());
  for (; iTAD != endTAD; iTAD++) {
    SG::TransientAddress* pTAD = *iTAD;

    SG::DataProxy* proxy = store.proxy_exact(pTAD->clID(), pTAD->name());
    /// if proxy exists, simply update the proxy with new TransientAddress, 
    /// else construct a new proxy
    if (0 != proxy) 
    {
      proxy->setAddress(pTAD->address());
      delete pTAD;
      pTAD = proxy->transientAddress();
      if (pTAD->provider() == 0)
        pTAD->setProvider(provider, store.storeID());
    } else {
      pTAD->setProvider(provider, store.storeID());
      if ( 0 == addAddress(store, pTAD) ) return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;

}

///create a new Proxy, overriding CLID and/or key
SG::DataProxy*
ProxyProviderSvc::addAddress(IProxyRegistry& store, 
			     SG::TransientAddress* tAddr) 
{  
  //HACK! The proxies for all object those key starts with "HLTAutoKey" will be deleted at the end of each event (resetOnly=false)
  //hence avoiding the proxy explosion observed with trigger object for releases <= 14.1.0
  bool resetOnly(tAddr->name().substr(0,10) != std::string("HLTAutoKey"));
  // std::cout << "PPS:addAdress: proxy for key " << tAddr->name() << " has resetOnly " << resetOnly << std::endl;
  SG::DataProxy* dp = new SG::DataProxy(tAddr, m_pDataLoader, true, resetOnly );
  //  store.addToStore(tAddr->clID(),dp);
  //  ATH_MSG_VERBOSE("created proxy for " << tAddr->clID() << "/" << tAddr->name() << "using " << m_pDataLoader->repSvcType());

  bool addedProxy(false);
  // loop over all the transient CLIDs:
  SG::TransientAddress::TransientClidSet tClid = tAddr->transientID();
  SG::TransientAddress::TransientClidSet::const_iterator tIter = tClid.begin();
  for (; tIter != tClid.end(); tIter++) {
    addedProxy |= (store.addToStore(*tIter, dp)).isSuccess();
  }

  if (addedProxy) {
    // loop over all alias'
    SG::TransientAddress::TransientAliasSet persAlias = tAddr->alias();
    SG::TransientAddress::TransientAliasSet::const_iterator aliasIter = persAlias.begin();
    for (; aliasIter != persAlias.end(); aliasIter++) {
      (store.addAlias(*aliasIter, dp)).ignore();
    }
    
    // Add any other allowable conversions.
    const SG::BaseInfoBase* bi = SG::BaseInfoBase::find (tAddr->clID());
    if (bi) {
      BOOST_FOREACH(CLID clid, bi->get_bases()) {
	if (tClid.find (clid) == tClid.end()) {
	  store.addToStore (clid, dp).ignore();
          tAddr->setTransientID (clid);
        }
      }

      BOOST_FOREACH(CLID clid, bi->get_copy_conversions()) {
	if (tClid.find (clid) == tClid.end()) {
	  store.addToStore (clid, dp).ignore();
          tAddr->setTransientID (clid);
        }
      }
    }
  } else {
    delete dp;
    dp=0;
  }
  return dp;
}

///get the default proxy from Provider
SG::DataProxy* 
ProxyProviderSvc::retrieveProxy(const CLID& id, const std::string& key,
				IProxyRegistry& store)
{

  SG::DataProxy *dp;
  if ( (dp=store.proxy(id,key)) != 0 ) return dp;

  if ( store.storeID() != StoreID::SIMPLE_STORE ) {
    SG::TransientAddress* pTAd = new SG::TransientAddress(id, key);
    pAPiterator iProvider(m_providers.begin()), iEnd(m_providers.end());
    for (; iProvider != iEnd; iProvider++) {
      if ( ((*iProvider)->updateAddress(store.storeID(),pTAd)).isSuccess() ) 
	{
	  pTAd->setProvider(*iProvider, store.storeID());
	  return this->addAddress(store,pTAd);
	}
    }  
    delete pTAd; pTAd=0;
  }  
  
  return 0;

}

StatusCode
ProxyProviderSvc::updateAddress(StoreID::type storeID,
				SG::TransientAddress* pTAd)
{

  pAPiterator iProvider(m_providers.begin()), iEnd(m_providers.end());
  for (; iProvider != iEnd; iProvider++) {
    if ( ((*iProvider)->updateAddress(storeID, pTAd)).isSuccess() ) 
    {
      pTAd->setProvider(*iProvider, storeID);
      return StatusCode::SUCCESS;
    }
    ++iProvider;
  }  

  return StatusCode::SUCCESS;

}

/// Gaudi Service boilerplate
/// Gaudi_cast...
// N.B. Don't forget to release the interface after use!!!
StatusCode 
ProxyProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IProxyProviderSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyProviderSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


void 
ProxyProviderSvc::providerNamesPropertyHandler( Property& /*theProp*/ ) {
  //add declared providers to the list;
  std::vector<std::string>::const_iterator iN(m_providerNames.value().begin());
  std::vector<std::string>::const_iterator iEnd(m_providerNames.value().end());
  while (iN != iEnd) {
    IService *pIS(0);
    IAddressProvider *pAP(0);
    ListItem tn(*iN);
    if (!(service(tn.type(), tn.name(), pIS)).isSuccess() ||
	0 == (pAP = dynamic_cast<IAddressProvider*>(pIS))) {
      ATH_MSG_ERROR(" getting Address Provider "<< *iN);
      throw GaudiException("Failed to locate address provider",
			   "ProxyProviderSvc::providerNamesPropertyHandle", 
			   StatusCode::FAILURE);

    } else {
      ATH_MSG_DEBUG(" added Address Provider "<< pIS->name());
    }
    addProvider(pAP);
    ++iN;
  }

}

