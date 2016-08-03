///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleBase.cxx 
// Implementation file for class VarHandleBase
// Author: Paolo Calafiura
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#undef DEBUG_VHB  /* define to get verbose debug printouts */

#include "StoreGate/VarHandleBase.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/exceptions.h"

#include "SGTools/DataBucketBase.h"
#include "SGTools/StorableConversions.h"

// fwk includes
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/IHiveStore.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <algorithm>
#include <sstream>


namespace errorcheck {


  /**
   * @brief Return a context name for a VarHandle.
   * @param context The VarHandle.
   *
   * For use with the CHECK and REPORT_MESSAGE macros.
   */
  std::string context_name (const SG::VarHandleBase* context)
  {
    std::ostringstream ss;
    ss << "VarHandle(" 
       << context->storeHandle().name() << "/" << context->key()
       << "[" << context->clid() << "]"
       << ")";
    return ss.str();
  }


} // namespace errorcheck


namespace SG {


  /**
   * @brief Constructor with default key.
   * @param clid CLID of the referenced class.
   * @param mode Mode of this handle (read/write/update).
   */
  VarHandleBase::VarHandleBase(CLID clid, Gaudi::DataHandle::Mode mode) :
    VarHandleKey (clid, "", mode),
    IResetable(),
    m_ptr(0),
    m_proxy(0),
    m_store(nullptr)
  {
#ifdef DEBUG_VHB
    std::cerr << "VarHandleBase() " << this << std::endl;
#endif
  }


  /**
   * @brief Constructor with full arguments.
   * @param clid CLID of the referenced class.
   * @param sgkey StoreGate key of the referenced object.
   * @param mode Mode of this handle (read/write/update).
   * @param storename Name of the referenced event store.
   */
  VarHandleBase::VarHandleBase(CLID clid,
                               const std::string& sgkey,
                               Gaudi::DataHandle::Mode mode,
                               const std::string& storename) :  
    VarHandleKey(clid, sgkey, mode, storename),
    IResetable(),
    m_ptr(NULL),
    m_proxy(NULL),
    m_store(nullptr)
  {
  }


  /**
   * @brief Constructor from a VarHandleKey.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  VarHandleBase::VarHandleBase(const VarHandleKey& key) :
    VarHandleKey(key),
    IResetable(),
    m_ptr(nullptr),
    m_proxy(nullptr),
    m_store(nullptr)
  {
    if (key.storeHandle().get() == nullptr)
      throw SG::ExcUninitKey (key.clid(), key.key(),
                              key.storeHandle().name());
    
    if (storeFromHandle().isFailure())
      throw SG::ExcHandleInitError (key.clid(), key.key(),
                                    key.storeHandle().name());
  }


  /**
   * @brief Constructor from a VarHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The current event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  VarHandleBase::VarHandleBase(const VarHandleKey& key, const EventContext& ctx) :
    VarHandleBase (key)
  {
    if (storeHandle().name() == "StoreGateSvc")
      m_store = ctx.proxy();
  }


  /**
   * @brief Copy constructor.
   */
  VarHandleBase::VarHandleBase( const VarHandleBase& rhs ) :
    VarHandleKey(rhs),
    IResetable(),
    m_ptr(rhs.m_ptr),
    m_proxy(nullptr),
    m_store(rhs.m_store)
  {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::copy constr from " << &rhs
              << " to " << this << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->key()
              << std::endl;
#endif

    setProxy (rhs.m_proxy);
  }


  /**
   * @brief Move constructor.
   */
  VarHandleBase::VarHandleBase( VarHandleBase&& rhs ) :
    VarHandleKey (std::move (rhs)),
    IResetable(),
    m_ptr(rhs.m_ptr),
    m_proxy(nullptr),
    m_store(rhs.m_store)
  {
    rhs.m_ptr=0;

    if (rhs.m_proxy) {
      rhs.m_proxy->unbindHandle (&rhs);
      rhs.m_proxy->bindHandle(this);
      m_proxy = rhs.m_proxy;
      rhs.m_proxy=0; //no release: this has the ref now
    }
#ifdef DEBUG_VHB
    std::cerr << "::VHB:: move constr from " << &rhs
              << "to " << this << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->key()
              << std::endl;
#endif
  }


  /**
   * @brief Assignment operator.
   */
  VarHandleBase& 
  VarHandleBase::operator=( const VarHandleBase& rhs )
  {
    if (this != &rhs) {
      VarHandleKey::operator= (rhs);
      m_ptr =    rhs.m_ptr;
      m_store =  rhs.m_store;
      setProxy (rhs.m_proxy);
    }
#ifdef DEBUG_VHB
    std::cerr << "::VHB::assignment from " << &rhs
              << " to " << this << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->key()
              << std::endl;
#endif
    return *this;
  }


  /**
   * @brief Move operator.
   */
  VarHandleBase& 
  VarHandleBase::operator=( VarHandleBase&& rhs )
  {
    if (this != &rhs) {
      VarHandleKey::operator= (std::move (rhs));

      m_ptr =    rhs.m_ptr;
      m_store =  rhs.m_store;

      rhs.m_ptr=0;

      resetProxy();
      if (rhs.m_proxy) {
        rhs.m_proxy->unbindHandle (&rhs);
        rhs.m_proxy->bindHandle (this);
        m_proxy =  rhs.m_proxy;
        rhs.m_proxy=0; //no release: this has the ref now
      }
    }
#ifdef DEBUG_VHB
    std::cerr << "::VHB:: move assign from " << &rhs
              << " to " << this << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->m_sgkey
              << std::endl;
#endif
    return *this;
  }


  /**
   * @brief Destructor.
   */
  VarHandleBase::~VarHandleBase()
  {
#ifdef DEBUG_VHB
    std::cerr << "~VarHandleBase(" << this
              << ",ptr=" << this->m_ptr 
              << ",proxy=" << this->m_proxy << ", ";
    if (m_proxy) {
      std::cerr << " -- isValid: " << m_proxy->isValid()
                << " -- isConst: " << m_proxy->isConst();
    }
    std::cerr << ", key=" <<this->m_sgkey << ")...\n";
#endif

    resetProxy();
    m_ptr = 0;
  }



  //*************************************************************************
  // Accessors
  //


  /**
   * @brief Return the StoreGate ID for the referenced object.
   *
   * This is defined in @c VarHandleKey.  We need to redefine it here because
   * it's also in @c IResetable.  (Otherwise there would be an ambiguity.)
   */
  const std::string& VarHandleBase::key() const
  {
    return VarHandleKey::key();
  }


  /**
   * @brief Return the StoreGate ID for the referenced object.
   *
   * A synonym for key().
   */
  const std::string& VarHandleBase::name() const 
  {
    return this->key();
  }


  /**
   * @brief Return the name of the store holding the object we are proxying.
   */
  std::string VarHandleBase::store() const
  {
    if (m_store)
      return m_store->name();
    return this->storeHandle().name();
  }


  //*************************************************************************
  // Validity checking.
  //


  /**
   * @brief Is the referenced object present in SG?
   *
   * Const method; the handle does not change as a result of this.
   */
  bool VarHandleBase::isPresent() const
  {
    const DataProxy* proxy = m_proxy;
    if (!proxy) {
      IProxyDict* store = m_store;
      if (!store)
        store = this->storeHandle().get();
      if (store)
        proxy = m_store->proxy(this->clid(), this->key());
    }
    if (proxy) {
      return proxy->isValid();
    }
    return false;
  }


  /**
   * @brief Has a proxy been retrieved from SG?
   *
   * (Weaker test than @c isValid, but does not touch the disk.)
   */
  bool 
  VarHandleBase::isInitialized() const
  {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::isInitialized(" 
              << this
              << ", proxy" << m_proxy << ") const\n";
#endif
    return (0 != m_proxy);
  }


  /**
   * @brief Has a proxy been retrieved from SG?
   *
   * Same as @c isInitialized; this is an interface required by @c IResetable.
   */
  bool VarHandleBase::isSet() const
  {
    return isInitialized();
  }


  /**
   * @brief True if this handle has a proxy, and the proxy is const.
   *
   * Refers to the state of the proxy, not of the handle.
   */
  bool 
  VarHandleBase::isConst() const
  {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::isConst(" 
              << this
              << ", proxy" << m_proxy; 
    if (m_proxy) {
      std::cerr
        << " -- isValid: " << m_proxy->isValid()
        << " -- isConst: " << m_proxy->isConst();
    }
    std::cerr << ") const\n";
#endif
    return 0 != m_proxy 
      ? m_proxy->isConst() 
      : false;
  }


  /**
   * @brief Retrieve and cache all information managed by a handle.
   *
   * This will retrieve and cache the associated @c DataProxy.
   *
   * Note for the case of a WriteHandle that has not yet been written to,
   * the proxy may not exist.  We return Success in that case; however,
   * @c isInitialized will still return false.
   */
  StatusCode 
  VarHandleBase::initialize()
  {
    if (!m_store) {
      CHECK( VarHandleKey::initialize() );
      m_store = this->storeHandle().get();
    }

    if (!m_store)
      return StatusCode::FAILURE;

    StatusCode sc = this->setState(m_store->proxy(this->clid(), this->key()));

    // Failure to find the proxy is ok in the case of a @c WriteHandle
    // that has not yet been written to.
    if (sc.isFailure() && mode() == Gaudi::DataHandle::Writer && m_ptr == nullptr)
      return StatusCode::SUCCESS;

    return sc;
  }


  /**
   * @brief Retrieve and cache all information managed by a handle.
   *
   * Synonym for initialize().
   */
  StatusCode 
  VarHandleBase::setState()
  {
    return initialize();
  }


  //*************************************************************************
  // State setting.
  //


  /**
   * @brief Explicitly set the event store.
   * @param store The new event store.
   *
   * This implicitly does a reset().
   */
  StatusCode VarHandleBase::setProxyDict (IProxyDict* store)
  {
    reset(true);
    m_store = store;
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Reset this handle.
   * @param hard If true, anything depending on the event store is cleared.
   *
   * If the handle stays associated with a given event store, then hard=false.
   * In that case, we clear the cached pointer; the proxy is also dropped
   * if it is reset only.  If hard=true, then we always drop the proxy and
   * in addition clear the cached pointer to the event store.
   */
  void 
  VarHandleBase::reset (bool hard) {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::reset("
              << "ptr=" << this->m_ptr << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->key()
              << ")..." 
              << std::endl;
#endif
    m_ptr = 0; 

    if (hard)
      m_store = 0;

    //if the proxy is not resetOnly then release it as it will become invalid
    // Also release on a hard reset.
    if (0 != m_proxy && (!m_proxy->isResetOnly() || hard)) {
      resetProxy();
    }
  }


  /**
   * @brief Reset this handle at the end of processing.
   * @brief hard If true, anything depending on the event store is cleared.
   *
   * Same as reset(true);
   */
  void VarHandleBase::finalReset() {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::finalReset("
              << "ptr=" << this->m_ptr << ", "
              << "proxy=" << this->m_proxy << ", "
              << "key=" <<this->key()
              << ")..." 
              << std::endl;
#endif
    reset (true);
  }


  /**
   * @brief Set the 'const' bit for the bound proxy in the store.
   */
  StatusCode VarHandleBase::setConst()
  {
    if (typeless_dataPointer()) {
      m_proxy->setConst();
      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  }


  //*************************************************************************
  // Protected methods.
  //


  /**
   * @brief Set the state of the handle to a given proxy.
   * @param proxy The proxy to set.
   *
   * The proxy must be valid; otherwise FAILURE will be returned.
   */
  StatusCode 
  VarHandleBase::setState(SG::DataProxy* proxy)
  {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::setState(" 
              << proxy;
    if (0 != proxy) {
      std::cerr << " -- isValid: " << proxy->isValid()
                << " -- isConst: " << proxy->isConst();
    }
    std::cerr << ") const\n";
#endif
    if (0 == proxy || !proxy->isValid()) {
      return StatusCode::FAILURE;
    }

    if (m_proxy != proxy) {
      // We're changing proxies.  Release the old and bind to the new.
      setProxy (proxy);
      // Clear cached pointer.
      m_ptr=0;
    }

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Set the state of a handle from a store and a key name.
   * @param store The event store to access.
   * @param name The StoreGate key to search for.
   */
  StatusCode 
  VarHandleBase::setState(IProxyDict* store, const std::string& key)
  {
    if (0 == store) {
      return StatusCode::FAILURE;
    }
#ifdef DEBUG_VHB
    std::cerr << "::VHB::setState(" 
              << store->name() << ", "
              << key
              << ") const\n";
#endif
    CLID cid = this->clid();
    SG::DataProxy* proxy = store->proxy(cid, key);
    // std::cerr << "::VHB:: -- clid=[" << cid << "] proxy=[" << proxy << "]\n";
    return this->setState(proxy);
  }


  /**
   * @brief Helper to record an object in the event store.
   * @param The wrapped data object (DataBucket) to record.
   * @param dataPtr Pointer to the transient object itself.
   * @param allowMods If false, record the object as const.
   * @param returnExisting Allow an existing object.
   *
   * If there is already an existing object with our key, then return
   * failure, unless @c returnExisting is true, in which case
   * return success.  In either case, @c dobj is destroyed.
   */
  StatusCode
  VarHandleBase::record_impl (std::unique_ptr<DataObject> dobj,
                              void* dataPtr,
                              bool allowMods,
                              bool returnExisting)
  {
    if (!m_store) {
      CHECK (VarHandleKey::initialize());
      m_store = this->storeHandle().get();
    }

    if (this->name() == "") {
      REPORT_ERROR (StatusCode::FAILURE) << "Attempt to record an object with a null key";
      return StatusCode::FAILURE;
    }

    SG::DataObjectSharedPtr<DataObject> sptr (dobj.release());
    unsigned int initRefCount = sptr->refCount();
    SG::DataProxy* new_proxy = 
      m_store->recordObject (sptr, this->name(), allowMods, returnExisting);
    if (!new_proxy) {
      REPORT_ERROR (StatusCode::FAILURE) << "recordObject failed";
      resetProxy();
      return StatusCode::FAILURE;
    }
    if (m_proxy != new_proxy) {
      CHECK (this->setState (new_proxy));
    }
  
    if (new_proxy && initRefCount == sptr->refCount() && new_proxy->isValid()) {
      // If the reference count hasn't changed, then we've returned an existing
      // object rather than recording a new one.  Retrieve the pointer, making
      // sure that it isn't const.
      if (m_proxy->isConst()) {
        REPORT_ERROR (StatusCode::FAILURE)
          << "Found an existing const object from recordOrRetrieve.";
        return StatusCode::FAILURE;
      }
      m_ptr = typeless_dataPointer_impl(true);
      if (!allowMods)
        CHECK( setConst() );
    }
    else
      m_ptr=(void*)dataPtr;

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Retrieve an object from StoreGate.
   * @param quiet If true, suppress failure messages.
   */
  void* 
  VarHandleBase::typeless_dataPointer_impl (bool quiet)
  {
#ifdef DEBUG_VHB
    std::cerr << "::VHB::typeless_dataPointer_impl("
              << this
              << ",ptr=" << this->m_ptr 
              << ",proxy=" << this->m_proxy << ", ";
    if (m_proxy) {
      std::cerr << " -- isValid: " << m_proxy->isValid()
                << " -- isConst: " << m_proxy->isConst();
    }
    std::cerr << ", key=" <<this->key() << ")...\n";
#endif

    // First check for cached pointer.
    if (0 != m_ptr)
      return m_ptr;

    if (0 == m_proxy) {
      // No proxy, need to look it up.
      if (this->setState().isFailure() || !m_proxy) {
        if (!quiet) {
          REPORT_MESSAGE(MSG::WARNING)
            << "could not get proxy for key " << key();
          if (this->mode() != Gaudi::DataHandle::Reader) {
            REPORT_MESSAGE(MSG::WARNING)<< " try using a ReadHandle";
          }
        } //quiet
        return 0;
      } //setstate
    } //m_proxy

    if (!m_proxy || !m_proxy->isValid()) {
      // invalid proxy
      if (!quiet) {
        REPORT_MESSAGE(MSG::WARNING)
          << "Proxy "
          << " [" << (m_proxy != 0 ? m_proxy->clID() : 0)
          << "/" << (m_proxy != 0 
                     ? m_proxy->name() 
                     : std::string("<N/A>"))
          << "] is in an invalid state";
      } //quiet
      return m_ptr;
    }

    DataObject* dobj = m_proxy->accessData();
    if (!dobj) {
      // invalid dobj
      if (!quiet) {
        REPORT_MESSAGE(MSG::WARNING)
          << "this proxy " << MSG::hex << m_proxy
          << MSG::dec << " has a NULL data object ptr";
      }
      return m_ptr;
    }

    const CLID clid = this->clid();
    m_ptr = SG::Storable_cast(dobj, clid, m_proxy);
    if (m_ptr) {
      return m_ptr;
    }

    // if m_ptr is null, probably the clid we gave wasn't the clid
    // the object was stored with, nor it inherits from it.
    // before giving up, let's check its transient CLIDs
    DataBucketBase *dbb = 0;
    if (m_proxy->transientAddress()->transientID(clid) &&
        0 != (dbb = dynamic_cast<DataBucketBase*>(dobj))) {
      // it is a symlink after all.
      // Let's hard cast (and keep our fingers Xed)
      m_ptr = static_cast<void*>(dbb->object());
    } else {
      if (!quiet) {
        REPORT_MESSAGE(MSG::WARNING)
          << "Request for an invalid object; requested CLID = " 
          << clid 
          << ", proxy primary ID is " << m_proxy->clID();
      }
    } // try symlink -- endif
    return m_ptr;
  }


  /**
   * @brief Retrieve an object from StoreGate as non-const pointer.
   *
   * Calls typeless_dataPointer, then raises an exception if the
   * proxy is marked as const.
   */
  void* VarHandleBase::typeless_ptr(bool quiet/*=defaultQuiet*/)
  {
    void* p = typeless_dataPointer(quiet);
    if (p != nullptr && isConst())
      throw SG::ExcConstObject (clid(), key(), store());
    return p;
  }


  /**
   * @brief Initialize the store pointer from the store handle.
   *        Also checks that the key is valid.
   */
  StatusCode VarHandleBase::storeFromHandle()
  {
    CHECK( VarHandleKey::initialize() );
    if (this->storeHandle()->name() == "StoreGateSvc")
      m_store = Gaudi::Hive::currentContext().proxy();
    else {
      m_store = &*this->storeHandle();
      if (IHiveStore* hs = dynamic_cast<IHiveStore*> (m_store))
        m_store = hs->hiveProxyDict();
    }
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Clear the m_proxy field and release the old proxy.
   */
  void VarHandleBase::resetProxy()
  {
    if (m_proxy) {
      m_proxy->unbindHandle(this);
      m_proxy->release();
      m_proxy = nullptr;
    }
  }


  /**
   * @brief Set a new proxy.  Release any old one first.
   * @param proxy The new proxy.
   */
  void VarHandleBase::setProxy (SG::DataProxy* proxy)
  {
    resetProxy();
    if (proxy) {
      proxy->addRef();
      proxy->bindHandle (this);
      m_proxy = proxy;
    }
  }


  //*************************************************************************
  // Free functions.
  //


  /**
   * @brief Output stream.
   * @param out Stream to which to write.
   * @parma o Object to write.
   */
  std::ostream& operator<<( std::ostream& out, const VarHandleBase& o )
  {
    out << "VarHandleBase @" << &o
        << " store=" <<o.store()
        << ", clid=" <<o.clid()
        << ", key=" <<o.key()
        << "----------- ptr@" << o.m_ptr 
        << ", proxy@" << o.m_proxy ;
    if (o.m_proxy)
      out << ", DataObject@" << o.m_proxy->object();
    return out;
  }


  /**
   * @brief Equality comparison.
   */
  bool operator==(const VarHandleBase& l, const VarHandleBase& r)
  {
    return (l.clid() == r.clid() &&
            l.mode() == r.mode() &&
            l.name() == r.name() &&
            l.store() == r.store());
  }


  /**
   * @brief Inequality comparison.
   */
  bool operator!=(const VarHandleBase& l, const VarHandleBase& r)
  {
    return !(l==r);
  }


} /* namespace SG */
