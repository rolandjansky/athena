// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandleBase.h 797637 2017-02-17 02:32:11Z ssnyder $
/**
 * @file StoreGate/VarHandleBase.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Base class for VarHandle classes.
 */

#ifndef STOREGATE_SG_VARHANDLEBASE_H
#define STOREGATE_SG_VARHANDLEBASE_H 1

// STL includes
#include <string>

#include "StoreGate/VarHandleKey.h"

// fwk includes
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/StoreID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"

// SGTools includes
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/StlMapClids.h"

namespace Athena_test {
  void varHandleTest(void);
  void resetableTest(void);
  void refCountTest(void);
}
namespace SG {
#ifdef NDEBUG
  const bool defaultQuiet = true;
#else
  const bool defaultQuiet = false;
#endif


  /**
   * @class SG::VarHandleBase
   * @brief Base class for VarHandle types.
   *
   * This is the base class for smart pointer types used to access objects
   * in an @c IProxyDict (such as a StoreGateSvc).  The handle types
   * @c ReadHandle, @c WriteHandle, and @c UpdateHandle derive from this;
   * see those classes for usage information.
   *
   * This class references a @c VarHandleKey, which holds the CLID of the class
   * we're referencing (which gets passed to the constructor of this class
   * from the templated derived classes), the StoreGate key of the object
   * we're referencing, and a handle to the event store.  The key object
   * can be passed to this object via the constructor; otherwise, we create
   * and own a new @c VarHandleKey object.
   *
   * In this class, we keep a pointer to the actual event store being used
   * (may be thread-dependent), a pointer to the @c DataProxy for the
   * referenced object, and a cached pointer to the object itself.
   *
   * A handle object may be used as a algorithm/tool property directly.
   * Because the handle caches state, however, this means that the component
   * using it cannot be reentrant.  In such a case, the handle will be reset
   * when the current algorithm completes.  In this case, the handle
   * will be bound to the proxy.
   *
   * The preferred way of using handles is to use a HandleKey object
   * (one of ReadHandleKey<T>, WriteHandleKey<T>, UpdateHandleKey<T>)
   * as the property, and to create a handle instance on the stack from
   * the key object (and the event context, if available).  In this
   * case, the handle will not be bound to the proxy.  A handle created
   * in this way should not live beyond the end of the algorithm in which
   * it was created.
   */
  class VarHandleBase : public IResetable
  {
    // For testing.
    friend void Athena_test::varHandleTest(void);
    friend void Athena_test::resetableTest(void);
    friend void Athena_test::refCountTest(void);
    friend std::ostream& operator<<( std::ostream&, const VarHandleBase&);
 
  public: 
    /**
     * @brief Constructor with default key.
     * @param clid CLID of the referenced class.
     * @param mode Mode of this handle (read/write/update).
     */
    explicit VarHandleBase(CLID clid, Gaudi::DataHandle::Mode mode);


    /**
     * @brief Constructor with full arguments.
     * @param clid CLID of the referenced class.
     * @param sgkey StoreGate key of the referenced object.
     * @param mode Mode of this handle (read/write/update).
     * @param storename Name of the referenced event store.
     */
    explicit VarHandleBase(CLID clid,
                           const std::string& sgkey,
                           Gaudi::DataHandle::Mode mode,
                           const std::string& storename = StoreID::storeName(StoreID::EVENT_STORE));


    /**
     * @brief Constructor from a VarHandleKey.
     * @param key The key object holding the clid/key/store.
     * @param ctx The event context to use, or nullptr.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     *
     * If the default event store has been requested, then the thread-specific
     * store from @c ctx will be used.  If @c ctx is null, then the
     * current event context will be read.
     */
    explicit VarHandleBase (const VarHandleKey& key, const EventContext* ctx);


    /**
     * @brief Copy constructor.
     */
    VarHandleBase( const VarHandleBase& rhs );


    /**
     * @brief Move constructor.
     */
    VarHandleBase( VarHandleBase&& rhs );


    /**
     * @brief Assignment operator.
     */
    VarHandleBase& operator=( const VarHandleBase& rhs ); 


    /**
     * @brief Move operator.
     */
    VarHandleBase& operator=( VarHandleBase&& rhs ); 


    /**
     * @brief Destructor.
     */
    virtual ~VarHandleBase() override;


    //*************************************************************************
    // Accessors
    //


    /**
     * @brief Return the StoreGate ID for the referenced object.
     */
    virtual const std::string& key() const override final;


    /**
     * @brief Return the StoreGate ID for the referenced object.
     *
     * A synonym for key().
     */
    const std::string& name() const ;


    /**
     * @brief Return the name of the store holding the object we are proxying.
     */
    std::string store() const;


    //*************************************************************************
    // Validity checking.
    //


    /**
     * @brief Can the handle be successfully dereferenced?
     *
     * Non-const method; the handle may cache information as a result of this.
     */
    virtual bool isValid() = 0;


    /**
     * @brief Is the referenced object present in SG?
     *
     * Const method; the handle does not change as a result of this.
     */
    bool isPresent() const;


    /**
     * @brief Has a proxy been retrieved from SG?
     *
     * (Weaker test than @c isValid, but does not touch the disk.)
     */
    bool isInitialized() const;


    /**
     * @brief Has a proxy been retrieved from SG?
     *
     * Same as @c isInitialized; this is an interface required by @c IResetable.
     */
    virtual bool isSet() const override final;


    /**
     * @brief True if this handle has a proxy, and the proxy is const.
     *
     * Refers to the state of the proxy, not of the handle.
     */
    bool isConst() const;


    /**
     * @brief Verify that the handle has been configured properly.
     * @param used If false, then this handle is not to be used.
     *             Instead of normal initialization, the key will be cleared.
     *
     * This will return failure if the key is blank or if the event store
     * cannot be found.
     */
    StatusCode initialize (bool used = true);


    /**
     * @brief Retrieve and cache all information managed by a handle.
     *
     * This will retrieve and cache the associated @c DataProxy.
     *
     * Note for the case of a WriteHandle that has not yet been written to,
     * the proxy may not exist.  We return Success in that case; however,
     * @c isInitialized will still return false.
     */
    StatusCode setState();


    //*************************************************************************
    // State setting.
    //

  
    /**
     * @brief Explicitly set the event store.
     * @param store The new event store.
     *
     * This implicitly does a reset().
     */
    virtual StatusCode setProxyDict (IProxyDict* store);


    // FIXME: Remove this once IResetable is cleaned up.
    using IResetable::reset;


    /**
     * @brief Reset this handle.
     * @param hard If true, anything depending on the event store is cleared.
     *
     * If the handle stays associated with a given event store, then hard=false.
     * In that case, we clear the cached pointer; the proxy is also dropped
     * if it is reset only.  If hard=true, then we always drop the proxy and
     * in addition clear the cached pointer to the event store.
     */
    virtual void reset (bool hard) override;


    /**
     * @brief Reset this handle at the end of processing.
     * @brief hard If true, anything depending on the event store is cleared.
     *
     * Same as reset(true);
     */
    virtual void finalReset() override final;


    /**
     * @brief Set the 'const' bit for the bound proxy in the store.
     */
    StatusCode setConst();


    //*************************************************************************
    // @c VarHandleKey methods.
    //


    /**
     * @brief Return the class ID for the referenced object.
     */
    CLID clid() const;


    /**
     * @brief Return handle to the referenced store.
     */
    const ServiceHandle<IProxyDict>& storeHandle() const;


    /**
     * @brief Return the mode (read/write/update) for this handle.
     */
    Gaudi::DataHandle::Mode mode() const;


    /**
     * @brief Return the key string of the underlying @c DataObjID.
     *
     * Compared to @c key(), this will be prefixed with the store name.
     */
    const std::string& objKey() const;


    /**
     * @brief Return the key as a @c DataObjID.
     */
    const DataObjID& fullKey() const;
 

    /**
     * @brief Return a non-const reference to the HandleKey.
     *
     * If this handle was initialized from a HandleKey, then this doesn't work
     * (since the HandleKey is const). ExcNonConstHandleKey will be thrown
     * in that case.
     */
    SG::VarHandleKey& vhKey();


    /**
     * @brief Update the underlying key from a string.
     *
     * If this handle was initialized from a HandleKey, then this doesn't work
     * (since the HandleKey is const). ExcNonConstHandleKey will be thrown
     * in that case.
     *
     * See VarHandleKey::assign.
     */
    StatusCode assign (const std::string& sgkey);

    
  protected: 
    //*************************************************************************
    // Protected methods.
    //


    /**
     * @brief Set the state of the handle to a given proxy.
     * @param proxy The proxy to set.
     *
     * The proxy must be valid; otherwise FAILURE will be returned.
     */
    StatusCode setState(SG::DataProxy* proxy);


    /**
     * @brief Set the state of a handle from a store and a key name.
     * @param store The event store to access.
     * @param name The StoreGate key to search for.
     *
     * Fails if no such object is recorded.
     */
    StatusCode setState(IProxyDict* store, const std::string& name);


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
    StatusCode record_impl (std::unique_ptr<DataObject> dobj,
                            void* dataPtr,
                            bool allowMods,
                            bool returnExisting);


    /**
     * @brief Helper to record an object in the event store.
     *        Unlike record, put does not change the handle and does not
     *        cache the pointer in the handle.
     * @param ctx The event context, or nullptr to use the current context.
     * @param dobj The wrapped data object (DataBucket) to record.
     * @param dataPtr Pointer to the transient object itself.
     * @param allowMods If false, record the object as const.
     * @param returnExisting Allow an existing object.
     * @param[out] store The store being used.
     *
     * Returns the object placed in the store, or nullptr if there
     * was an error.
     * If there was already an object in the store with the given key,
     * then return null, unless @c returnExisting is true, in which case
     * return success.  In either case, @c dobj is destroyed.
     */
    const void* put_impl (const EventContext* ctx,
                          std::unique_ptr<DataObject> dobj,
                          const void* dataPtr,
                          bool allowMods,
                          bool returnExisting,
                          IProxyDict* & store) const;


    /**
     * @brief Retrieve an object from StoreGate.
     * @param quiet If true, suppress failure messages.
     */
    virtual void* typeless_dataPointer_impl(bool quiet);


    /**
     * @brief Retrieve an object from StoreGate.
     * @param quiet If true, suppress failure messages.
     *
     * Inline method: first check cached pointer, then call the _impl method.
     */
    void* typeless_dataPointer(bool quiet=defaultQuiet);


    /**
     * @brief Retrieve an object from StoreGate as a const pointer.
     *
     * Same as typeless_dataPointer with the return value converted to const.
     */
    const void* typeless_cptr();


    /**
     * @brief Retrieve an object from StoreGate as non-const pointer.
     *
     * Calls typeless_dataPointer, then raises an exception if the
     * proxy is marked as const.
     */
    void* typeless_ptr(bool quiet=defaultQuiet);


    /*
     * @brief Retrieve an object from SG as a const pointer without caching.
     * @param ctx The event context, or nullptr to use the current context.
     * @param quiet If true, suppress failure messages.
     *
     * Like typeless_dataPointer_impl, except that we don't change
     * any members of the handle.
     */
    const void* get_impl (const EventContext* ctx,
                          bool quiet = defaultQuiet) const;


    /**
     * @brief Make a symlink or alias to the object currently referenced
     *        by this handle.
     * @param newClid CLID of link.
     * @param newKey SG key of link.
     *
     * If newClid matches the existing clid, then make an alias.
     * If newKey matches the existing key, then make a symlink.
     * If neither match, it's an error.
     */
    StatusCode symLink_impl (CLID newClid, const std::string& newKey) const;


    /**
     * @brief Is the referenced object present in SG?
     * @param key SG key to test.
     *
     * Const method; the handle does not change as a result of this.
     */
    bool isPresent_impl (const std::string& key) const;


    
  protected: 
    //*************************************************************************
    // Protected data.
    //

    /// The object to which we are bound.
    void* m_ptr;

    /// Proxy holding the object to which we are bound.
    SG::DataProxy* m_proxy;
  
    /// Pointer to the store that owns the object.
    IProxyDict* m_store;

    /// True if the store was set explicitly via setProxyDict.
    bool m_storeWasSet;


  private:
    /// An owned @c VarHandleKey.
    /// This is set in the case where a @c VarHandle is _not_ initialized
    /// from a key object.
    std::unique_ptr<VarHandleKey> m_ownedKey;

    /// The associated key object.
    /// If we were initialized from a key object, then this points at that.
    /// Otherwise, it points at the same object as @c m_ownedKey.
    const VarHandleKey* m_key;

    /**
     * @brief Return the store instance to use.
     * @param ctx The current event context, or nullptr.
     */
    IProxyDict* storeFromHandle (const EventContext* ctx) const;


    /**
     * @brief Initialize the store pointer from the store handle.
     *        Also checks that the key is valid.
     * @param ctx The current event context, or nullptr.
     */
    StatusCode setStoreFromHandle (const EventContext* ctx);


    /**
     * @brief Clear the m_proxy field and release the old proxy.
     */
    void resetProxy();


    /**
     * @brief Set a new proxy.  Release any old one first.
     * @param proxy The new proxy.
     */
    void setProxy (SG::DataProxy* proxy);


    /**
     * @brief Retrieve a pointer from a proxy.
     * @param proxy The proxy object.
     * @param quiet If true, suppress failure messages.
     *
     * Warning --- doesn't enforce const rules; the caller must do that.
     */
    void* 
    typeless_dataPointer_fromProxy (SG::DataProxy* proxy,
                                    bool quiet) const;

  }; 


  //*************************************************************************
  // Free functions.
  //


  /**
   * @brief Output stream.
   * @param out Stream to which to write.
   * @param o Object to write.
   */
  std::ostream& operator<<( std::ostream& out, const VarHandleBase& o );


  /**
   * @brief Equality comparison.
   */
  bool operator==(const VarHandleBase& l, const VarHandleBase& r);


  /**
   * @brief Inequality comparison.
   */
  bool operator!=(const VarHandleBase& l, const VarHandleBase& r);


} /* namespace SG */

// For the ConditionHandles
#include "SGTools/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondContBase , 34480459 , 1 )

#include "StoreGate/VarHandleBase.icc"


#endif //> !STOREGATE_SG_VARHANDLEBASE_H
