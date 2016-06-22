// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WriteHandle.h 756419 2016-06-21 02:02:43Z ssnyder $
/**
 * @file StoreGate/WriteHandle.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Handle class for recording to StoreGate.
 */

#ifndef STOREGATE_SG_WRITEHANDLE_H
#define STOREGATE_SG_WRITEHANDLE_H 1


#include "StoreGate/VarHandleBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/StoreGateSvc.h" /* needed by clients */
#include "GaudiKernel/EventContext.h"
#include <string>
#include <memory> /*unique_ptr*/


namespace SG {


/**
 * @class SG::WriteHandle<T>
 * @brief A smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc).
 *
 * @c SG::WriteHandle<T> can only create new objects in StoreGate; no proxy
 * should already exist.  This handle will only return the pointer that
 * has been recorded.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::WriteHandle<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::WriteHandle<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
 *     ATH_CHECK( m_int.record (CxxUtils::make_unique<int>(42)) );
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     *m_int += 10;
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     return StatusCode::SUCCESS;
 *   }
 * @endcode
 *
 * For more information have a look under the package
 *     Control/AthenaExamples/AthExHelloWorld
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


  /**
   * @brief Default constructor.
   *
   * The handle will not be usable until a non-blank key is assigned.
   */
  WriteHandle();


  /**
   * @brief Constructor with full arguments.
   * @param sgkey StoreGate key of the referenced object.
   * @param storename Name of the referenced event store.
   */
  explicit WriteHandle(const std::string& sgkey, 
                       const std::string& storename = "StoreGateSvc");


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


  /**
   * @brief Derefence the pointer.
   * Returns the cached pointer.  Throws ExcNullWriteHandle if null.
   */
  pointer_type operator->() const;


  /**
   * @brief Derefence the pointer.
   * Returns the cached pointer.  Throws ExcNullWriteHandle if null.
   */
  reference_type operator*() const;


  /**
   * @brief Dereference the pointer.
   * Returns the cached pointer.
   */
  const_pointer_type cptr() const;


  /**
   * @brief Dereference the pointer.
   * Returns the cached pointer.
   */
  pointer_type ptr() const;


  /**
   * @brief Return the cached pointer directly; no lookup.
   */
  pointer_type cachedPtr() const;


  /**
   * @brief Can the handle be successfully dereferenced?
   */
  virtual bool isValid() override final;


  //************************************************************************
  // Record.
  //


  /**
   * @brief Record a const object to the store.
   * @param data The object to record.
   */
  StatusCode record (std::unique_ptr<T> data);


  /**
   * @brief Record a non-const object to the store.
   * @param data The object to record.
   */
  StatusCode recordNonConst (std::unique_ptr<T> data);

  
  /**
   * @brief Record a const object and its auxiliary store to the store.
   * @param data The object to record.
   * @param auxstore Auxiliary store object.
   */
  template <class AUXSTORE>
  StatusCode record (std::unique_ptr<T> data,
                     std::unique_ptr<AUXSTORE> store);


  /**
   * @brief Record a non-const object and its auxiliary store to the store.
   * @param data The object to record.
   * @param auxstore Auxiliary store object.
   */
  template <class AUXSTORE>
  StatusCode recordNonConst (std::unique_ptr<T> data,
                             std::unique_ptr<AUXSTORE> store);


  /**
   * @brief Record a const shared DataObject to the store.
   * @param data The object to record.
   *
   * The event store takes shared ownership of the object.
   */
  StatusCode record (SG::DataObjectSharedPtr<T> data);


  /**
   * @brief Record a non-const shared DataObject to the store.
   * @param data The object to record.
   *
   * The event store takes shared ownership of the object.
   */
  StatusCode recordNonConst (SG::DataObjectSharedPtr<T> data);


  // Commit this out for now since it looks like we don't actually need it.
#if 0
  /**
   * @brief Record a new non-const object, or retrieve an old one.
   * @param data The object to record.
   * @param isConst If true, record the objects as const.
   *
   * If no object for this handle's key exists already in StoreGate,
   * then record @c data.
   * Otherwise, if one exists and is non-const, then initialize
   * the handle to refer to that object (and destroy @c data).
   * Otherwise return an error.
   *
   * No const version of this method, since it then doesn't make sense
   * to do that in multiple threads.
   */
  StatusCode recordOrRetrieve (std::unique_ptr<T> data);
#endif


  /**
   * @brief Alternate notation for record.  Records a non-const object.
   * @param data Object to record.
   *
   * Throws an execption on failure.
   */
  WriteHandle& operator=( std::unique_ptr<T> data );



private:
  /**
   * @brief Return the cached pointer directly.
   *
   * If it is null, throw ExcNullWriteHandle.
   */
  pointer_type checkedCachedPtr() const;


  /**
   * @brief Helper for record.
   * @param data The object to record.
   * @param isConst If true, record the object as const.
   * @param returnExisting Allow an existing object.
   */
  template <class U>
  StatusCode doRecord (U data,
                       bool isConst,
                       bool returnExisting);


  /**
   * @brief Record an object and its auxiliary store to the store.
   * @param data The object to record.
   * @param auxstore Auxiliary store object.
   * @param isConst If true, record the objects as const.
   */
  template <class AUXSTORE>
  StatusCode
  record (std::unique_ptr<T> data,
          std::unique_ptr<AUXSTORE> auxstore,
          bool isConst);


private:
  /// If non-null, then we need to lock the associated aux store object
  /// when we're deleted.
  ///
  /// This is set when we record an object along with the associated aux const
  /// with the const flag set (the default).  Recall that for a const record,
  /// we want to support the semantics that you can get a non-const pointer
  /// back from the handle as long as it exists, to finish initialization
  /// of the object.  For an aux store, though, just getting back a non-const
  /// pointer is not sufficient, since the store will have been locked
  /// at the time of the record, preventing changes to the store.
  ///
  /// So if we're meant to record a const aux store object, we don't actually
  /// set it const on the record, but instead set this and do the
  /// setConst in the destructor.
  SG::DataProxy* m_lockAuxPending = nullptr;
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


#include "StoreGate/WriteHandle.icc"


#ifndef NO_LEGACY_HANDLES
namespace SG {
  template <class T>
  using WVar = WriteHandle<T>;
} 
#endif
#endif //> !STOREGATE_SG_WRITEHANDLE_H
