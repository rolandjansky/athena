/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_TRANSIENTADDRESS_H
#define SGTOOLS_TRANSIENTADDRESS_H

///< includes:
#include <string>
#include <set>


///< Gaudi includes:
#include "AthenaKernel/IStringPool.h"
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/ClassID.h"

///< forward declarations:
class IOpaqueAddress;
class IAddressProvider;
class IProxyDict;
class EventContext;

namespace SG {

  class TransientAddress
  {

  public:

    typedef std::set<CLID> TransientClidSet;
    typedef std::set<std::string> TransientAliasSet;
    typedef IStringPool::sgkey_t sgkey_t;

    ///< Default Constructor
    TransientAddress();

    ///< Construct from clid and string key:
    TransientAddress(const CLID& id, const std::string& key);

    ///< Construct from clid, key and IOpaqueAddress
    TransientAddress(const CLID& id, const std::string& key, 
		     IOpaqueAddress* addr, bool clearAddress = true);

    ///< Destructor
    virtual ~TransientAddress();

    /// Set the CLID / key.
    /// This will only succeed if the clid/key are currently clear.
    void setID (CLID id, const std::string& key);

    ///< Reset
    void reset();

    ///< Retrieve IOpaqueAddress
    virtual IOpaqueAddress* address() const; 

    ///< set IOpaqueAddress
    virtual void setAddress(IOpaqueAddress* pAddress);

    ///< Retrieve primary clid
    CLID clID() const;

    ///< Retrieve string key:
    const std::string& name() const;

    ///< Get the primary (hashed) SG key.
    sgkey_t sgkey() const;

    ///< Set the primary (hashed) SG key.
    void setSGKey (sgkey_t sgkey);

    ///< check if it is a transient ID (primary or symLinked):
    bool transientID (const CLID& id) const;

    ///< set transient CLID's
    void setTransientID(const CLID& id);

    ///< get transient CLID's
    const TransientClidSet& transientID() const;

    ///< set alias'
    void setAlias(const std::string& key);

    ///< set alias'
    void setAlias(const std::set<std::string>& keys);

    /// remove alias from proxy
    bool removeAlias(const std::string& key);

    ///< get transient alias
    const TransientAliasSet& alias() const;

    ///< set the clearAddress flag: IOA will not be deleted in proxy
    void clearAddress(const bool& flag);

    ///< Return the clearAddress flag.
    bool clearAddress() const;

    ///< this sets the flag whether to consult the provider to update
    /// this transient address if the IOA is not valid.
    void consultProvider(const bool& flag);

    ///< Check the validity of the Transient Address.
    bool isValid (IProxyDict* store);

    ///< cache the pointer to the Address provider which can update
    ///< this transient address
    IAddressProvider* provider() const;
    StoreID::type storeID() const;
    void setProvider(IAddressProvider* provider, StoreID::type storeID);

  private:
    /**
     * @brief Retrieve the EventContext saved in store STORE.
     * @param store The store from which to retrieve the context, or nullptr.
     *
     * If there is no context recorded in the store, return a default-initialized
     * context.
     */
    const EventContext& contextFromStore (IProxyDict* store) const;

    
    ///< clid of the concrete class (persistent clid)
    CLID m_clid;

    ///< string key of this object
    std::string m_name;

    ///< all transient clids. They come from symlinks
    TransientClidSet m_transientID; 

    ///< all alias names for a DataObject. They come from setAlias
    TransientAliasSet m_transientAlias;

    ///< IOpaqueAddress:
    IOpaqueAddress* m_address;

    ///< Controls if IOpaqueAddress should be deleted:
    bool m_clearAddress;

    ///< Control whether the Address Provider must be consulted
    bool m_consultProvider;

    ///< AddressProvider
    IAddressProvider* m_pAddressProvider;

    ///< Store type, needed by updateAddress
    StoreID::type m_storeID;

    ///< (hashed) SG key for primary clid / key.
    sgkey_t m_sgkey;
  };
  /////////////////////////////////////////////////////////////////////
  // inlined code:
  /////////////////////////////////////////////////////////////////////

  // Reset the TransientAddress
  inline
  void TransientAddress::reset()
  {
    if (m_clearAddress) setAddress(0);
  }

  /// Retrieve IOpaqueAddress
  inline
  IOpaqueAddress* TransientAddress::address() const 
  { 
    return m_address; 
  }

  /// Retrieve clid
  inline
  CLID TransientAddress::clID() const 
  { 
    return m_clid; 
  }

  /// Return StoreGate key
  inline
  const std::string& TransientAddress::name() const
  {
    return m_name;
  }

  /// Get the primary (hashed) SG key.
  inline
  TransientAddress::sgkey_t TransientAddress::sgkey() const
  {
    return m_sgkey;
  }

  /// Set the primary (hashed) SG key.
  inline
  void TransientAddress::setSGKey (sgkey_t sgkey)
  {
    m_sgkey = sgkey;
  }

  /// check if it is a transient ID:
  inline
  bool TransientAddress::transientID(const CLID& id) const
  { 
    return 0 != m_transientID.count(id);
  }

  /// set transient CLID's
  inline
  void TransientAddress::setTransientID(const CLID& id)
  { 
    m_transientID.insert(id);
  }

  /// get transient CLID's
  inline
  const TransientAddress::TransientClidSet& TransientAddress::transientID() const 
  { 
    return m_transientID; 
  }

  /// set transient Alias'
  inline 
  void TransientAddress::setAlias(const std::string& key)
  {
    m_transientAlias.insert(key);
  } 

  /// set transient Alias'
  inline 
  void TransientAddress::setAlias(const std::set<std::string>& keys)
  {
    m_transientAlias = keys;
  } 

  /// remove alias
  inline bool TransientAddress::removeAlias(const std::string& key)
  {
    return (m_transientAlias.erase(key) == 1) ? true:false;
  }

  /// get transient Alias'
  inline
  const TransientAddress::TransientAliasSet& TransientAddress::alias() const
  { 
    return m_transientAlias;
  }

  /// set the clearAddress flag: IOA will not be deleted in proxy
  inline
  void TransientAddress::clearAddress(const bool& flag) 
  {
    m_clearAddress = flag;
  }

  /// Return the clearAddress flag.
  inline
  bool TransientAddress::clearAddress() const
  {
    return m_clearAddress;
  }

  inline
  void TransientAddress::consultProvider(const bool& flag)
  {
    m_consultProvider = flag;
  }

  ///< cache the pointer to the Address provider which can update
  ///< this transient address
  inline
  IAddressProvider* TransientAddress::provider() const
  {
    return m_pAddressProvider;
  }
  inline
  StoreID::type TransientAddress::storeID() const
  {
    return m_storeID;
  }

  inline
    void TransientAddress::setProvider(IAddressProvider* provider, 
				       StoreID::type storeID)
  {
    m_pAddressProvider = provider;
    m_consultProvider = true;
    m_storeID=storeID;
  }
} //end namespace SG

#endif // STOREGATE_TRANSIENTADDRESS










