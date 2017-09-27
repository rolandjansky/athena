// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_DATAPROXY_H
#define SGTOOLS_DATAPROXY_H

// includes
#include <string>
#include <set>
#include <vector>
#include <typeinfo>
#include <memory>
#include <mutex>
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/getMessageSvc.h" /*Athena::IMessageSvcHolder*/
#include "SGTools/TransientAddress.h"
#include "SGTools/IRegisterTransient.h"
#include "SGTools/exceptions.h"


// forward declarations:
class DataObject;
class IConverter;
class IResetable;
class IProxyDict;

/** @class DataProxy 
 *  @brief DataProxy provides the registry services for StoreGate
 *  @author ATLAS Collaboration
 *  $Id: DataProxy.h,v 1.11 2008-07-14 22:16:25 calaf Exp $
 **/

namespace SG {
  class T2pMap;

  class DataProxy : public IRegistry, public IRegisterTransient
  {

  public:
    enum ErrNo {ALLOK, NOCNVSVC, NOIOA, CNVFAILED, T2PREGFAILED, NERRORS};
    //FIXME private typedef IRegistry::name_type name_type;
    //FIXME private typedef IRegistry::id_type id_type;
    typedef std::string name_type;
    typedef std::string id_type;
    typedef TransientAddress::TransientClidSet CLIDCont_t;
    typedef TransientAddress::TransientAliasSet AliasCont_t;
    typedef IStringPool::sgkey_t sgkey_t;

    // Constructors
    DataProxy();                      // default constructor

    ///build from TransientAddress
    ///Takes ownership of tAddr.
    DataProxy(TransientAddress* tAddr, 
              IConverter* pDataLoader,
              bool constFlag=false, bool resetOnly=true);

    ///build from TransientAddress
    DataProxy(std::unique_ptr<TransientAddress> tAddr, 
              IConverter* pDataLoader,
              bool constFlag=false, bool resetOnly=true);

    ///build from DataObject
    DataProxy(DataObject* dObject, 
              TransientAddress* tAddr,
              bool constFlag=false, bool resetOnly=true);

    // Destructor
    virtual ~DataProxy();

    ///\name IRegistry implementation
    //@{
    /// Add reference to object
    virtual unsigned long addRef() override;

    /// release reference to object
    virtual unsigned long release() override;

    /// return refCount
    unsigned long refCount() const;

    /// Retrieve data object key == string
    virtual const name_type& name() const override;

    /// Retrieve data object key == string
    /// duplicated for Gaudi folks does same as name()
    virtual const id_type& identifier() const override;

    /// Retrieve DataObject
    virtual DataObject* object() const override;

    /// set an IOpaqueAddress
    virtual void setAddress(IOpaqueAddress* ioa) override;

    /// Retrieve IOpaqueAddress
    virtual IOpaqueAddress* address() const override;

    /// set DataSvc (Gaudi-specific); do nothing for us
    virtual IDataProviderSvc* dataSvc() const override;
    //@}

    ///< Get the primary (hashed) SG key.
    sgkey_t sgkey() const;

    ///< Set the primary (hashed) SG key.
    void setSGKey (sgkey_t sgkey);

    ///< Return the ID of the store containing this proxy.
    StoreID::type storeID() const;

    ///< check if it is a transient ID (primary or symLinked):
    bool transientID (CLID id) const;

    ///< return the list of transient IDs (primary or symLinked):
    CLIDCont_t transientID() const;

    /// Add a new transient ID
    void setTransientID(CLID id);

    /// access set of proxy aliases
    /// Returns a COPY of the alias set.
    AliasCont_t alias() const;

    /// Test to see if a given string is in the alias set.
    bool hasAlias (const std::string& key) const;

    /// Add a new proxy alias.
    void setAlias(const std::string& key);

    /// remove alias from proxy
    bool removeAlias(const std::string& key);

    /// Return the address provider.
    IAddressProvider* provider();

    /// Set the address provider.
    void setProvider(IAddressProvider* provider, StoreID::type storeID);

    /// Set the CLID / key.
    /// This will only succeed if the clid/key are currently clear.
    void setID (CLID id, const std::string& key);

    /// Other methods of DataProxy (not in Interface IRegistry):

    /// Reset DataObject, Handles and IOpaqueAddress:
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    void reset (bool hard = false);
    void finalReset(); ///called by destructor

    /// release or reset according to resetOnly flag
    /// If FORCE is true, then always release.
    /// See IResetable.h for HARD.
    bool requestRelease(bool force, bool hard);

    /// am I valid?
    bool isValid() const;

    /// is the address valid?
    bool isValidAddress() const;

    /// is the object valid?
    bool isValidObject() const;

    bool updateAddress();

    /// set DataObject
    void setObject(DataObject* obj);

    /// Access DataObject on-demand using conversion service
    ///@throws runtime_error when converter fails
    DataObject* accessData();

    ErrNo errNo() const;
 
    /// Retrieve clid
    CLID clID() const;

    /// Retrieve storage type
    unsigned char svcType() const;

    /// Check if it is a const object
    bool isConst() const;


    /**
     * @brief Mark this object as const.  (Lock the object.)
     *
     * If the object held that derives from @c ILockable, then we also
     * call @c lock on the object.
     */
    void setConst();


    /// set the reset only flag: Clear Store will reset and not delete.
    void resetOnly(const bool& flag);

    /// Check reset only:
    bool isResetOnly() const;

    bool bindHandle(IResetable* ir);
    void unbindHandle(IResetable* ir);

    // cache the t2p
    void setT2p(T2pMap* t2p);

    /**
     * @brief Register a transient object in a t2p map.
     * @param trans The object to register.
     *
     * (@c IRegisterTransient interface.)
     */
    virtual void registerTransient (void* p) override;

    /// Set the store of which we're a part.
    void setStore (IProxyDict* store);

    /// Return the store of which we're a part.
    IProxyDict* store() const;

    /// reset the bound DataHandles
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    void resetBoundHandles (bool hard);

    IConverter* loader() const;

  private:
    DataProxy(const DataProxy&) = delete;
    DataProxy& operator=(const DataProxy&) = delete;

    std::unique_ptr<TransientAddress> m_tAddress;

    unsigned long m_refCount;

    DataObject* m_dObject;
    IConverter* m_dataLoader;

    /// Is the proxy currently const?
    bool m_const;
    /// Was the proxy created as const?
    bool m_origConst;

    unsigned char m_storageType;

    ///reset and not delete: default is true
    bool m_resetFlag;        

    /// list of bound DataHandles
    typedef std::vector<IResetable*> handleList_t;
    handleList_t m_handles;

    T2pMap* m_t2p;
    Athena::IMessageSvcHolder m_ims;
    
    /// errno-style error code for accessData
    enum ErrNo m_errno;

    /// The store of which we are a part.
    IProxyDict* m_store;


    // Needs to be recursive since updateAddress can call back
    // into the DataProxy.
    typedef std::recursive_mutex mutex_t;
    typedef std::lock_guard<mutex_t> lock_t;
    mutable mutex_t m_mutex;

    
    bool isValidAddress (lock_t&) const;


    /**
     * @brief Lock the data object we're holding, if any.
     *
     * Should be called with the mutex held.
     */
    void lock (lock_t&);


    /**
     * @brief Read in a new copy of the object referenced by this proxy.
     * @param errNo If non-null, set to the resulting error code.
     *
     * If this proxy has an associated loader and address, then load
     * a new copy of the object and return it.  Any existing copy
     * held by the proxy is unaffected.
     *
     * This will fail if the proxy does not refer to an object read from an
     * input file.
     */
    std::unique_ptr<DataObject> readData (lock_t&, ErrNo* errNo) const;


    /// set DataObject
    void setObject (lock_t&, DataObject* obj);
  };

  ///cast the proxy into the concrete data object it proxies
  //@{
  template<typename DATA>
  DATA* DataProxy_cast(DataProxy* proxy);

  ///const pointer version of the cast
  template<typename DATA>
  const DATA* DataProxy_cast(const DataProxy* proxy);

  ///const ref version of the cast. @throws SG::ExcBadDataProxyCast.
  template<typename DATA>
  DATA DataProxy_cast(const DataProxy& proxy);

  /**
   * @brief Try to get the pointer back from a @a DataProxy,
   *        converted to be of type @a clid.
   * @param proxy The @a DataProxy.
   * @param clid The ID of the class to which to convert.
   *
   * Only works if the held object is a @a DataBucket.
   * Returns 0 on failure,
   */
  void* DataProxy_cast(DataProxy* proxy, CLID clid);
  //@}

} //end namespace SG

#include "SGTools/DataProxy.icc"

#endif // SGTOOLS_DATAPROXY




