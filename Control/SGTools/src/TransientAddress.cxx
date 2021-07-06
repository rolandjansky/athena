/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include <assert.h>

using SG::TransientAddress;

// Default Constructor:
TransientAddress::TransientAddress()
  : TransientAddress (CLID_NULL, "", nullptr, true, false)
{
}

// Constructor with CLID and string key:
// (typically used through a StoreGate::record()
TransientAddress::TransientAddress(CLID id, const std::string& key)
  : TransientAddress (id, key, nullptr, true, false)
{ 
}

// Constructor with CLID, string key and IOpaqueAddress:
// (typically used by a ProxyProvider)
TransientAddress::TransientAddress(CLID id, const std::string& key, 
				   IOpaqueAddress* addr, 
				   bool clearAddress)
  : TransientAddress (id, key, addr, clearAddress, true)
{
}


TransientAddress::TransientAddress(CLID id, const std::string& key, 
				   IOpaqueAddress* addr, 
				   bool clearAddress,
                                   bool consultProvider)
  : m_clid(id),
    m_sgkey(0),
    m_storeID(StoreID::UNKNOWN),
    m_clearAddress(clearAddress),
    m_consultProvider(consultProvider),
    m_address(nullptr),
    m_pAddressProvider(nullptr)
{
  if (!key.empty()) {
    m_name.store (key);
  }
  if (id != CLID_NULL)
    m_transientID.push_back(id);
  if (addr) {
    setAddress(addr);
  }
}


TransientAddress::TransientAddress (const TransientAddress& other)
  : m_clid (static_cast<CLID>(other.m_clid)),
    m_sgkey (static_cast<sgkey_t>(other.m_sgkey)),
    m_storeID (other.m_storeID),
    m_clearAddress (other.m_clearAddress),
    m_consultProvider (other.m_consultProvider),
    m_pAddressProvider (other.m_pAddressProvider),
    m_name (other.m_name),
    m_transientID (other.m_transientID),
    m_transientAlias (other.m_transientAlias)
{
  m_address = nullptr;
  setAddress (other.m_address);
}


TransientAddress::TransientAddress (TransientAddress&& other)
  : m_clid (static_cast<CLID>(other.m_clid)),
    m_sgkey (static_cast<sgkey_t>(other.m_sgkey)),
    m_storeID (other.m_storeID),
    m_clearAddress (other.m_clearAddress),
    m_consultProvider (other.m_consultProvider),
    m_pAddressProvider (other.m_pAddressProvider),
    m_name (std::move (other.m_name)),
    m_transientID (std::move (other.m_transientID)),
    m_transientAlias (std::move (other.m_transientAlias))
{
  m_address = other.m_address;
  other.m_address = nullptr;
}


// Destructor
TransientAddress::~TransientAddress() 
{ 
  setAddress(0);
}


TransientAddress& TransientAddress::operator= (const TransientAddress& other)
{
  if (this != &other) {
    m_clid = static_cast<CLID>(other.m_clid);
    m_name = other.m_name;
    m_transientID = other.m_transientID;
    m_transientAlias = other.m_transientAlias;
    m_clearAddress = other.m_clearAddress;
    m_consultProvider = other.m_consultProvider;
    m_pAddressProvider = other.m_pAddressProvider;
    m_storeID = other.m_storeID;
    m_sgkey = static_cast<sgkey_t>(other.m_sgkey);

    setAddress (other.m_address);
  }
  return *this;
}


TransientAddress& TransientAddress::operator= (TransientAddress&& other)
{
  if (this != &other) {
    m_clid = static_cast<CLID>(other.m_clid);
    m_name = std::move (other.m_name);
    m_transientID = std::move (other.m_transientID);
    m_transientAlias = std::move (other.m_transientAlias);
    m_clearAddress = other.m_clearAddress;
    m_consultProvider = other.m_consultProvider;
    m_pAddressProvider = other.m_pAddressProvider;
    m_storeID = other.m_storeID;
    m_sgkey = static_cast<sgkey_t>(other.m_sgkey);

    m_address = other.m_address;
    other.m_address = nullptr;
  }
  return *this;
}


/// set transient CLID's
void TransientAddress::setTransientID(CLID id)
{
  if (m_transientID.empty()) {
    m_transientID.push_back (id);
  }
  else if (!transientID (id)) {
    m_transientID.push_back (id);
    std::sort (m_transientID.begin(), m_transientID.end());
  }
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
  assert (m_clid == CLID_NULL && !m_name.isValid() && m_transientID.empty() &&
          m_transientAlias.empty());
  m_clid = id;
  m_name.set (key);
  if (id != CLID_NULL)
    m_transientID.push_back(id);
}

/// set IOpaqueAddress
void TransientAddress::setAddress(IOpaqueAddress* pAddress)
{ 
  if (0 != pAddress) pAddress->addRef();
  if (0 != m_address) m_address->release();
  m_address = pAddress;
}

bool TransientAddress::isValid(const EventContext* ctx,
                               bool forceUpdate /*= false*/)
{
  if (!forceUpdate && 0 != address()) return true;

  if (ctx && m_consultProvider && 0 != provider()) {
    if ((provider()->updateAddress(storeID(), this, *ctx)).isSuccess())
      return true;
  }
  return false;
}
