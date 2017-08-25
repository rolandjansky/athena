/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/EventContextClid.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/EventContext.h"

#include <assert.h>

using SG::TransientAddress;

// Default Constructor:
TransientAddress::TransientAddress()
  : m_clid(0), m_name(""), m_address(0), m_clearAddress(true),
    m_consultProvider(false), m_pAddressProvider(0), m_storeID(StoreID::UNKNOWN),
    m_sgkey(0)
{ }

// Constructor with CLID and string key:
// (typically used through a StoreGate::record()
TransientAddress::TransientAddress(const CLID& id, const std::string& key)
  : m_clid(id), m_name(key), m_address(0), m_clearAddress(true),
    m_consultProvider(false), m_pAddressProvider(0), m_storeID(StoreID::UNKNOWN),
    m_sgkey(0)
{ 
  if (id != CLID_NULL)
    m_transientID.insert(id);
}

// Constructor with CLID, string key and IOpaqueAddress:
// (typically used by a ProxyProvider)
TransientAddress::TransientAddress(const CLID& id, const std::string& key, 
				   IOpaqueAddress* addr, 
				   bool clearAddress)
  : m_clid(id), m_name(key), m_address(0), m_clearAddress(clearAddress),
    m_consultProvider(true), m_pAddressProvider(0), m_storeID(StoreID::UNKNOWN),
    m_sgkey(0)
{ 
  if (id != CLID_NULL)
    m_transientID.insert(id);
  setAddress(addr);
}

// Destructor
TransientAddress::~TransientAddress() 
{ 
  setAddress(0);
}


/**
 * @brief Set the CLID / key.
 * @param id The new CLID.
 * @param key The new StoreGate key.
 *
 * This will only succeed if the clid/key are currently clear.
 */
void TransientAddress::setID (CLID id, const std::string& key)
{
  assert (m_clid == CLID_NULL && m_name.empty() && m_transientID.empty() &&
          m_transientAlias.empty());
  m_clid = id;
  m_name = key;
  if (id != CLID_NULL)
    m_transientID.insert(id);
}

/// set IOpaqueAddress
void TransientAddress::setAddress(IOpaqueAddress* pAddress)
{ 
  if (0 != pAddress) pAddress->addRef();
  if (0 != m_address) m_address->release();
  m_address = pAddress;
}

bool TransientAddress::isValid(IProxyDict* store)
{
  if (0 != address()) return true;

  // FIXME CGL
//    if (!m_consultProvider) {
//      if ( m_clid != 0 && m_name != "" ) { return true; }
//    }
  if (m_consultProvider && 0 != provider()) {
    if ((provider()->updateAddress(storeID(), this,
                                   contextFromStore (store))).isSuccess())
      return true;
  }
  return false;
}


/**
 * @brief Retrieve the EventContext saved in store STORE.
 * @param store The store from which to retrieve the context, or nullptr.
 *
 * If there is no context recorded in the store, return a default-initialized
 * context.
 */
const EventContext& TransientAddress::contextFromStore (IProxyDict* store) const
{
  if (store) {
    SG::DataProxy* proxy = store->proxy (ClassID_traits<EventContext>::ID(),
                                         "EventContext");
    if (proxy) {
      EventContext* ctx = SG::DataProxy_cast<EventContext> (proxy);
      if (ctx) return *ctx;
    }
  }
  static const EventContext emptyContext;
  return emptyContext;
}
