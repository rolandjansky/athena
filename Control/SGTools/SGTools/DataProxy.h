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
#include <atomic>
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/getMessageSvc.h" /*Athena::IMessageSvcHolder*/
#include "SGTools/TransientAddress.h"
#include "SGTools/IRegisterTransient.h"
#include "SGTools/exceptions.h"
#include "CxxUtils/checker_macros.h"


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
class DataStore;

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

    DataProxy(TransientAddress&& tAddr,
              IConverter* pDataLoader,
              bool constFlag=false, bool resetOnly=true);

    ///build from DataObject
    DataProxy(DataObject* dObject, 
              TransientAddress* tAddr,
              bool constFlag=false, bool resetOnly=true);

    DataProxy(DataObject* dObject, 
              TransientAddress&& tAddr,
              bool constFlag=false, bool resetOnly=true);

    // Destructor
    virtual ~DataProxy();

    ///\name IRegistry implementation
    //@{
    /// Add reference to object
    virtual unsigned long addRef() override final;

    /// release reference to object
    virtual unsigned long release() override final;

    /// return refCount
    unsigned long refCount() const;

    /// Retrieve data object key == string
    virtual const name_type& name() const override final;

    /// Retrieve data object key == string
    /// duplicated for Gaudi folks does same as name()
    virtual const id_type& identifier() const override final;

    /// Retrieve DataObject
    virtual DataObject* object ATLAS_NOT_CONST_THREAD_SAFE () const override final;

    /// set an IOpaqueAddress
    virtual void setAddress(IOpaqueAddress* ioa) override final;

    /// Retrieve IOpaqueAddress
    virtual IOpaqueAddress* address() const override final;

    /// set DataSvc (Gaudi-specific); do nothing for us
    virtual IDataProviderSvc* dataSvc() const override final;
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
    virtual void registerTransient (void* p) override final;

    /// Set the store of which we're a part.
    void setStore (IProxyDict* store);

    /// Return the store of which we're a part.
    IProxyDict* store();

    /// Return the store of which we're a part.
    const IProxyDict* store() const;

    /// reset the bound DataHandles
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    void resetBoundHandles (bool hard);

    IConverter* loader();

  private:
    /// For access to requestRelease.
    friend class SG::DataStore;

    DataProxy(const DataProxy&) = delete;
    DataProxy& operator=(const DataProxy&) = delete;


    /**
     * @brief Reset/release a proxy at the end of an event.
     * @param force If true, force a release rather than a reset.
     * @param hard Do a hard reset if true.
     * @returns True if the caller should release the proxy.
     *
     * This is usually called at the end of an event.
     * No locking is done, so there should be no other threads accessing
     * this proxy.
     *
     * `Release' means that we want to remove the proxy from the store.
     * `Reset' means that we keep the proxy, but remove the data object
     * that it references.
     * Each proxy has a flag saying whether it wants to do a release or a reset.
     * This can be forced via the FORCE argument; this would typically be done
     * when deleting the store.
     * This function does not actually release the proxy.  If it returns
     * true, the caller is expected to release the proxy.
     *
     * See AthenaKernel/IResetable.h for the meaning of HARD.
     */
    bool requestRelease(bool force, bool hard);

    /// Drop the reference to the data object.
    void resetRef();

    unsigned int m_refCount;

    ///reset and not delete: default is true
    bool m_resetFlag;        

    /// True if there are any bound handles.
    // Strictly redundant with m_handles below, but put here to speed up the
    // test for m_handles.empty() --- both by eliminating the pointer
    // comparison and by moving the data into the part of DataProxy covered
    // by the first cache line.
    bool m_boundHandles;

    /// Is the proxy currently const?
    bool m_const;
    /// Was the proxy created as const?
    bool m_origConst;

    std::atomic<DataObject*> m_dObject;

    TransientAddress m_tAddress;

    IConverter* m_dataLoader;

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

    // For m_dObject.
    typedef std::recursive_mutex objMutex_t;
    typedef std::lock_guard<objMutex_t> objLock_t;
    mutable objMutex_t m_objMutex; // For m_dObject.

    
    bool isValidAddress (lock_t&) const;


    /**
     * @brief Lock the data object we're holding, if any.
     *
     * Should be called with the mutex held.
     */
    void lock (objLock_t&);


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
    std::unique_ptr<DataObject> readData (objLock_t& objLock, ErrNo* errNo) const;


    /// set DataObject
    void setObject (objLock_t& objLock, DataObject* obj);
  };

  ///cast the proxy into the concrete data object it proxies
  //@{
  template<typename DATA>
  DATA* DataProxy_cast(DataProxy* proxy);

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


// DP+ 0: two vptrs
// DP+10: int m_refCount
// DP+14: bool m_resetFlag
// DP+15: bool m_boundHandles
// DP+16: bool m_const
// DP+17: bool m_origConst
// DP+18: DataObject* m_dObject

// TA = DP+20
//  + 0:  CLID m_clid
//  + 4:  sgkey_t m_sgkey
//  + 8:  type m_storeID (size 4)
//  + c:  bool m_clearAddress
//  + d:  bool m_consultProvider
//  + e:  padding
//  +10:  IOpaqueAddress* m_address
//  +18:  std::string m_name         <== 2nd cache line starts at +20
//  +38:  vector m_transientID 
//  +50:  set m_transientAlias       <== 3rd cache line starts at +60
//  +80:  IAddressProvider* m_pAddressProvider 
//  +88.. 

// DP+a8: IConverter* m_dataLoader
// DP+b0: vector m_handles           <== 4th cache line starts at +c0
// DP+c8: T2PMap* m_t2p
// DP+d0: IMessageSvc* m_ims
// DP+d8: ErrNo m_errno
// DP+dc: padding
// DP+e0: m_store
// DP+e8: m_mutex                    <== 5th cache line starts at +100
// DP+110: m_objMutex
// DP+138: end


#include "SGTools/DataProxy.icc"

#endif // SGTOOLS_DATAPROXY




