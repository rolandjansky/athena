// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_DATAPROXY_H
#define SGTOOLS_DATAPROXY_H

// includes
#include <string>
#include <set>
#include <list>
#include <typeinfo>
#include <memory>
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/getMessageSvc.h" /*Athena::IMessageSvcHolder*/
#include "SGTools/TransientAddress.h"
#include "SGTools/IRegisterTransient.h"
#include "SGTools/exceptions.h"


// forward declarations:
class DataObject;
class IConversionSvc;
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

    // Constructors
    DataProxy();                      // default constructor

    ///build from TransientAddress
    DataProxy(TransientAddress* tAddr, 
              IConversionSvc* pDataLoader,
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
    virtual unsigned long addRef();

    /// release reference to object
    virtual unsigned long release();

    /// return refCount
    unsigned long refCount() const;

    /// Retrieve data object key == string
    virtual const name_type& name() const { return m_tAddress->name(); }

    /// Retrieve data object key == string
    /// duplicated for Gaudi folks does same as name()
    virtual const id_type& identifier() const {return m_tAddress->name();}

    /// Retrieve DataObject
    virtual DataObject* object() const      { return m_dObject; }

    /// set an IOpaqueAddress
    virtual void setAddress(IOpaqueAddress* ioa);

    /// Retrieve IOpaqueAddress
    virtual IOpaqueAddress* address() const { return m_tAddress->address(); } 

    /// set DataSvc (Gaudi-specific); do nothing for us
    virtual IDataProviderSvc* dataSvc() const { return 0; }
    //@}

    /// Retrieve TransientAddress
    virtual TransientAddress* transientAddress() const { return m_tAddress; }

    /// access set of proxy aliases
    const AliasCont_t& alias() const { return m_tAddress->alias(); }

    /// Other methods of DataProxy (not in Interface IRegistry):

    /// Reset DataObject, Handles and IOpaqueAddress:
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    virtual void reset (bool hard = false);
    virtual void finalReset(); ///called by destructor

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

    /// set DataObject
    void setObject(DataObject* obj);

    /// Access DataObject on-demand using conversion service
    ///@throws runtime_error when converter fails
    DataObject* accessData();

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
    std::unique_ptr<DataObject> readData (ErrNo* errNo) const;

    ErrNo errNo() const { return m_errno; }
 
    /// Retrieve clid
    CLID clID() const { return m_tAddress->clID(); }

    /// Retrieve storage type
    unsigned char svcType() const { return m_storageType; }

    /// Check if it is a const object
    bool isConst() const { return m_const; }


    /**
     * @brief Mark this object as const.  (Lock the object.)
     *
     * If the object held that derives from @c ILockable, then we also
     * call @c lock on the object.
     */
    void setConst();


    /// set the reset only flag: Clear Store will reset and not delete.
    void resetOnly(const bool& flag) { m_resetFlag = flag; }

    /// Check reset only:
    bool isResetOnly() const { return m_resetFlag; }

    bool bindHandle(IResetable* ir);
    void unbindHandle(IResetable* ir);

    /// The following kept temporarily for backward compatibility:
    const CLIDCont_t& transientID() const {return m_tAddress->transientID();}  

    // cache the t2p
    void setT2p(T2pMap* t2p);

    /**
     * @brief Register a transient object in a t2p map.
     * @param trans The object to register.
     *
     * (@c IRegisterTransient interface.)
     */
    virtual void registerTransient (void* p);

    /// Set the store of which we're a part.
    void setStore (IProxyDict* store) { m_store = store; }

    /// Return the store of which we're a part.
    IProxyDict* store() const { return m_store; }

    /// reset the bound DataHandles
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    void resetBoundHandles (bool hard);

    IConversionSvc* loader() const { return m_dataLoader; }

  private:
    DataProxy(const DataProxy&);
    DataProxy& operator=(const DataProxy&);

    TransientAddress* m_tAddress;

    unsigned long m_refCount;

    DataObject* m_dObject;
    IConversionSvc* m_dataLoader;

    /// Is the proxy currently const?
    bool m_const;
    /// Was the proxy created as const?
    bool m_origConst;

    unsigned char m_storageType;

    ///reset and not delete: default is true
    bool m_resetFlag;        

    /// list of bound DataHandles
    std::list<IResetable*> m_handles;

    T2pMap* m_t2p;
    Athena::IMessageSvcHolder m_ims;
    
    /// errno-style error code for accessData
    enum ErrNo m_errno;

    /// The store of which we are a part.
    IProxyDict* m_store;

    /**
     * @brief Lock the data object we're holding, if any.
     */
    void lock();
  };

  ///cast the proxy into the concrete data object it proxies
  //@{
  template<typename DATA>
  DATA* DataProxy_cast(DataProxy* proxy);

  ///const pointer version of the cast
  template<typename DATA>
  const DATA* DataProxy_cast(const DataProxy* proxy)
  {
    return DataProxy_cast<DATA>(const_cast<DataProxy*>(proxy));
  }

  ///const ref version of the cast. @throws SG::ExcBadDataProxyCast.
  template<typename DATA>
  DATA DataProxy_cast(const DataProxy& proxy)
  {
    const DATA* result = DataProxy_cast<DATA>(&proxy);
    if (!result) SG::throwExcBadDataProxyCast(proxy.clID(), typeid(DATA));
    return *result;
  }

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




