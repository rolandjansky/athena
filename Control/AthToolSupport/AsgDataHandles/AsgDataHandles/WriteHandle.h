// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AsgDataHandles/WriteHandle.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov> (for original version)
 * @brief Handle class for recording to StoreGate.
 */


#ifndef ASG_DATA_HANDLES_WRITE_HANDLE_H
#define ASG_DATA_HANDLES_WRITE_HANDLE_H

#ifndef XAOD_STANDALONE
#include <StoreGate/WriteHandle.h>
#else

#include "AsgDataHandles/VarHandleBase.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include <string>
#include <memory> /*unique_ptr*/


namespace SG {


/**
 * @class SG::WriteHandle<T>
 *
 * See StoreGate/WriteHandle for details.
 *
 * This currently (07 Aug 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
 */
template <class T>
class WriteHandle
  : public SG::VarHandleBase
{ 
public: 
  typedef T*               pointer_type; // FIXME: better handling of
  typedef const T*   const_pointer_type; //        qualified T type ?
  typedef T&             reference_type;
  typedef const T& const_reference_type;


  //************************************************************************
  // Constructors, etc.
  //


  // /**
  //  * @brief Default constructor.
  //  *
  //  * The handle will not be usable until a non-blank key is assigned.
  //  */
  // WriteHandle();


  // /**
  //  * @brief Constructor with full arguments.
  //  * @param sgkey StoreGate key of the referenced object.
  //  * @param storename Name of the referenced event store.
  //  */
  // explicit WriteHandle(const std::string& sgkey, 
  //                      const std::string& storename = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief Constructor from a WriteHandleKey.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit WriteHandle (const WriteHandleKey<T>& key);


  /**
   * @brief Constructor from a WriteHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit WriteHandle (const WriteHandleKey<T>& key, const EventContext& ctx);

  
  /**
   * @brief Copy constructor.
   */
  WriteHandle( const WriteHandle& rhs );


  /**
   * @brief Move constructor.
   */
  WriteHandle( WriteHandle&& rhs );


  /**
   * @brief Assignment operator.
   */
  WriteHandle& operator=( const WriteHandle& rhs ); 


  /**
   * @brief Move operator.
   */
  WriteHandle& operator=( WriteHandle&& rhs );


  /**
   * @brief Destructor.
   *
   * Lock an aux object if m_lockAuxPending is set.
   */
  ~WriteHandle();


  //************************************************************************
  // Deference.  These all return only the cached pointer.
  //


  // /**
  //  * @brief Dereference the pointer.
  //  * Returns the cached pointer.  Throws ExcNullWriteHandle if null.
  //  */
  // pointer_type operator->();


  // /**
  //  * @brief Dereference the pointer.
  //  * Returns the cached pointer.  Throws ExcNullWriteHandle if null.
  //  */
  // reference_type operator*();


  // /**
  //  * @brief Dereference the pointer.
  //  * Returns the cached pointer.
  //  */
  // const_pointer_type cptr() const;


  // /**
  //  * @brief Dereference the pointer.
  //  * Returns the cached pointer.
  //  */
  // pointer_type ptr();


  // /**
  //  * @brief Return the cached pointer directly; no lookup.
  //  */
  // pointer_type cachedPtr();


  // /**
  //  * @brief Can the handle be successfully dereferenced?
  //  */
  // virtual bool isValid() override final;


  //************************************************************************
  // Record.
  //


  // /**
  //  * @brief Record a const object to the store.
  //  * @param data The object to record.
  //  */
  // StatusCode record (std::unique_ptr<T> data);


  // /**
  //  * @brief Record a non-const object to the store.
  //  * @param data The object to record.
  //  */
  // StatusCode recordNonConst (std::unique_ptr<T> data);

  
  /**
   * @brief Record a const object and its auxiliary store to the store.
   * @param data The object to record.
   * @param auxstore Auxiliary store object.
   */
  template <class AUXSTORE>
  StatusCode record (std::unique_ptr<T> data,
                     std::unique_ptr<AUXSTORE> store);


  // /**
  //  * @brief Record a non-const object and its auxiliary store to the store.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  */
  // template <class AUXSTORE>
  // StatusCode recordNonConst (std::unique_ptr<T> data,
  //                            std::unique_ptr<AUXSTORE> store);


  // /**
  //  * @brief Record a const shared DataObject to the store.
  //  * @param data The object to record.
  //  *
  //  * The event store takes shared ownership of the object.
  //  */
  // StatusCode record (SG::DataObjectSharedPtr<T> data);


  // /**
  //  * @brief Record a non-const shared DataObject to the store.
  //  * @param data The object to record.
  //  *
  //  * The event store takes shared ownership of the object.
  //  */
  // StatusCode recordNonConst (SG::DataObjectSharedPtr<T> data);


  // /**
  //  * @brief Record an object to the store.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (std::unique_ptr<T> data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (std::unique_ptr<const T> data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (std::unique_ptr<const ConstDataVector<T> > data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (const EventContext& ctx,
  //                         std::unique_ptr<const ConstDataVector<T> > data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (const EventContext& ctx,
  //                         std::unique_ptr<T> data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object?
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // const_pointer_type put (const EventContext& ctx,
  //                         std::unique_ptr<const T> data,
  //                         bool returnExisting = false) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param data The object to record.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  *
  //  * The event store takes shared ownership of the object.
  //  */
  // const_pointer_type put (SG::DataObjectSharedPtr<T> data) const;


  // /**
  //  * @brief Record an object to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  *
  //  * The event store takes shared ownership of the object.
  //  */
  // const_pointer_type put (const EventContext& ctx,
  //                         SG::DataObjectSharedPtr<T> data) const;


  // /**
  //  * @brief Record an object and its auxiliary store to the store.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  */
  // template <class AUXSTORE>
  // const_pointer_type
  // put (std::unique_ptr<T> data,
  //      std::unique_ptr<AUXSTORE> auxstore) const;


  // /**
  //  * @brief Record an object and its auxiliary store to the store.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  *
  //  * Unlike the version taking unique_ptr<T>, this does not alter the
  //  * store pointer of @c data.
  //  */
  // template <class AUXSTORE>
  // const_pointer_type
  // put (std::unique_ptr<const T> data,
  //      std::unique_ptr<const AUXSTORE> auxstore) const;


  // /**
  //  * @brief Record an object and its auxiliary store to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  */
  // template <class AUXSTORE>
  // const_pointer_type
  // put (const EventContext& ctx,
  //      std::unique_ptr<T> data,
  //      std::unique_ptr<AUXSTORE> auxstore) const;


  // /**
  //  * @brief Record an object and its auxiliary store to the store.
  //  * @param ctx The event context to use.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  *
  //  * Unlike the version taking unique_ptr<T>, this does not alter the
  //  * store pointer of @c data.
  //  */
  // template <class AUXSTORE>
  // const_pointer_type
  // put (const EventContext& ctx,
  //      std::unique_ptr<const T> data,
  //      std::unique_ptr<const AUXSTORE> auxstore) const;


  /**
   * @brief Alternate notation for record.  Records a non-const object.
   * @param data Object to record.
   *
   * Throws an exception on failure.
   */
  WriteHandle& operator=( std::unique_ptr<T> data );


  // /**
  //  * @brief Make an alias.
  //  * @param key Alternate key by which the referenced object should be known.
  //  *
  //  * The current handle should be valid and referencing an object
  //  * (i.e., @c record should have been called on it).
  //  *
  //  * The object will also be known by the name given in @c key.
  //  */
  // StatusCode alias (const WriteHandleKey<T>& key);


  // /**
  //  * @brief Make an explicit link.
  //  * @param key Alternate clid by which the referenced object
  //  *            should be known.  The SG key must match the key of the
  //  *            current handle.
  //  *
  //  * You should generally not be using this!
  //  *
  //  * The current handle should be valid and referencing an object
  //  * (i.e., @c record should have been called on it).
  //  *
  //  * This makes a symlink: the object will be retrievable
  //  * as a different type.
  //  * 
  //  * Note that if @c T and @c @U are related via @c SG_BASE and/or
  //  * @c DATAVECTOR_BASE, then you shouldn't need to explicitly make a symlink;
  //  * that should happen automatically.
  //  *
  //  * If a @c U* is not convertable to a @c T* via C++ rules, then you likely
  //  * will be, at best, relying on undefined behavior.  You will probably
  //  * get warnings from the undefined behavior sanitizer when if you try
  //  * to dereference the @c U*.
  //  *
  //  * This usage is here mainly to assist in migrating some existing
  //  * patterns to MT.  You should think several times before using
  //  * in new code.
  //  */
  // template <class U>
  // StatusCode symLink (const WriteHandleKey<U>& key);

  
private:
  // /**
  //  * @brief Return the cached pointer directly.
  //  *
  //  * If it is null, throw ExcNullWriteHandle.
  //  */
  // pointer_type checkedCachedPtr();


  // /**
  //  * @brief Helper for record.
  //  * @param data The object to record.
  //  * @param isConst If true, record the object as const.
  //  * @param returnExisting Allow an existing object.
  //  */
  // template <class U>
  // StatusCode doRecord (U data,
  //                      bool isConst,
  //                      bool returnExisting);


  // /**
  //  * @brief Helper for put.
  //  * @param ctx The event context, or nullptr to use the current context.
  //  * @param data The object to record.
  //  * @param returnExisting Allow an existing object.
  //  * @param[out] store The store being used.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, unless @c returnExisting is true, in which case
  //  * return success.  In either case, @c data is destroyed.
  //  */
  // template <class U>
  // const_pointer_type doPut (const EventContext* ctx,
  //                           U data,
  //                           bool returnExisting,
  //                           IProxyDict* & store) const;
  

  // /**
  //  * @brief Helper for recording an object and its auxiliary store to the store.
  //  * @param ctx The event context, or nullptr to use the current context.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  */
  // template <class AUXSTORE>
  // typename WriteHandle<T>::const_pointer_type
  // doPut (const EventContext* ctx,
  //        std::unique_ptr<T> data,
  //        std::unique_ptr<AUXSTORE> auxstore) const;


  // /**
  //  * @brief Helper for recording an object and its auxiliary store to the store.
  //  * @param ctx The event context, or nullptr to use the current context.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  *
  //  * Unlike record(), this does not change the handle object.
  //  * That means that one will not be able to get the object back
  //  * by dereferencing the handle.
  //  * Returns the object placed in the store, or nullptr if there
  //  * was an error.
  //  * If there was already an object in the store with the given key,
  //  * then return null, and the objects passed in are destroyed.
  //  */
  // template <class AUXSTORE>
  // typename WriteHandle<T>::const_pointer_type
  // doPut (const EventContext* ctx,
  //        std::unique_ptr<const T> data,
  //        std::unique_ptr<const AUXSTORE> auxstore) const;


  // /**
  //  * @brief Record an object and its auxiliary store to the store.
  //  * @param data The object to record.
  //  * @param auxstore Auxiliary store object.
  //  * @param isConst If true, record the objects as const.
  //  */
  // template <class AUXSTORE>
  // StatusCode
  // record (std::unique_ptr<T> data,
  //         std::unique_ptr<AUXSTORE> auxstore,
  //         bool isConst);


private:
  // /// If non-null, then we need to lock the associated aux store object
  // /// when we're deleted.
  // ///
  // /// This is set when we record an object along with the associated aux const
  // /// with the const flag set (the default).  Recall that for a const record,
  // /// we want to support the semantics that you can get a non-const pointer
  // /// back from the handle as long as it exists, to finish initialization
  // /// of the object.  For an aux store, though, just getting back a non-const
  // /// pointer is not sufficient, since the store will have been locked
  // /// at the time of the record, preventing changes to the store.
  // ///
  // /// So if we're meant to record a const aux store object, we don't actually
  // /// set it const on the record, but instead set this and do the
  // /// setConst in the destructor.
  // SG::DataProxy* m_lockAuxPending = nullptr;
}; 


/**
 * @brief Return a @c WriteHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 */
template <class T>
WriteHandle<T> makeHandle (const WriteHandleKey<T>& key);


/**
 * @brief Return a @c WriteHandle referencing @c key for an explicit context.
 * @param key The key object holding the clid/key/store.
 * @param ctx The event context.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * If the default event store has been requested, then the thread-specific
 * store from the event context will be used.
 */
template <class T>
WriteHandle<T> makeHandle (const WriteHandleKey<T>& key,
                           const EventContext& ctx);


} /* namespace SG */


#include "AsgDataHandles/WriteHandle.icc"


// #ifndef NO_LEGACY_HANDLES
// namespace SG {
//   template <class T>
//   using WVar = WriteHandle<T>;
// } 
// #endif

#endif

#endif
