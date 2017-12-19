// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReadHandle.h 797637 2017-02-17 02:32:11Z ssnyder $
/**
 * @file StoreGate/ReadHandle.h
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov>
 * @date Updated: Feb, 2016
 * @brief Handle class for reading from StoreGate.
 */

#ifndef STOREGATE_SG_READHANDLE_H
#define STOREGATE_SG_READHANDLE_H 1


#include "StoreGate/VarHandleBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "SGTools/ClassID_traits.h"
#include "GaudiKernel/EventContext.h"
#include <string>



namespace SG {


/**
 * @class SG::ReadHandle<T>
 * @brief a smart pointer to an object of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * @c SG::ReadHandle<T> can access const and non-const proxies in StoreGate but
 * cannot modify them (i.e.: it is actually a const T*).
 * A valid proxy must already exist in StoreGate.
 *
 * Usage example:
 * @code
 *   class MyAlg : public AthAlgorithm
 *   {
 *     SG::ReadHandle<int> m_int;
 *   };
 *
 *   MyAlg::MyAlg(...) : ..., m_int("MyIntSgKey") {
 *      declareProperty("IntHandle",
 *                      m_int = SG::ReadHandle<int>("MyIntSgKey"),
 *                      "a handle to an int in StoreGate");
 *   }
 *
 *   StatusCode MyAlg::execute()
 *   {
 *     ATH_MSG_INFO("int value @[" << m_int.name() << "]="
 *                  << *m_int);
 *     return StatusCode::SUCCESS;
 *   }
 @endcode
 *
 * For more information have a look under the package
 *     Control/AthenaExamples/AthExHelloWorld
 *
 */
template <class T>
class ReadHandle
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
  ReadHandle();


  /**
   * @brief Constructor with full arguments.
   * @param sgkey StoreGate key of the referenced object.
   * @param storename Name of the referenced event store.
   */
  explicit ReadHandle(const std::string& sgkey, 
                      const std::string& storename = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief Constructor from a ReadHandleKey.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit ReadHandle (const ReadHandleKey<T>& key);


  /**
   * @brief Constructor from a ReadHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit ReadHandle (const ReadHandleKey<T>& key, const EventContext& ctx);


  /**
   * @brief Copy constructor.
   */
  ReadHandle( const ReadHandle& rhs );


  /**
   * @brief Move constructor.
   */
  ReadHandle( ReadHandle&& rhs );


  /**
   * @brief Assignment operator.
   */
  ReadHandle& operator=( const ReadHandle& rhs ); 


  /**
   * @brief Move operator.
   */
  ReadHandle& operator=( ReadHandle&& rhs ); 


  //************************************************************************
  // Dereference.
  //

  
  /**
   * @brief Dereference the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_pointer_type  operator->();


  /**
   * @brief Dereference the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_reference_type operator*();


  /**
   * @brief Dereference the pointer.
   * Returns nullptr on failure.
   */
  const_pointer_type cptr();


  /**
   * @brief Dereference the pointer.
   * Returns nullptr on failure.
   */
  const_pointer_type ptr();


  /**
   * @brief Return the cached pointer directly; no lookup.
   */
  const_pointer_type cachedPtr() const;


  /**
   * @brief Can the handle be successfully dereferenced?
   */
  virtual bool isValid() override final;


  /**
   * @brief Dereference the pointer, but don't cache anything.
   */
  const_pointer_type get() const;


  /**
   * @brief Dereference the pointer, but don't cache anything.
   * @param ctx The event context to use.
   */
  const_pointer_type get (const EventContext& ctx) const;


  //************************************************************************
  // Alias.
  //


  /**
   * @brief Make an alias.
   * @param key Alternate key by which the referenced object should be known.
   *
   * The current handle should be valid and referencing an object.
   *
   * The object will also be known by the name given in @c key.
   */
  StatusCode alias (const WriteHandleKey<T>& key);


protected:
  /**
   * @brief Protected constructor used by WriteDecorHandle.
   * @param key The key object holding the clid/key.
   * @param ctx The event context, or nullptr to use the global default.
   */
  explicit ReadHandle (const VarHandleKey& key, const EventContext* ctx);


private:
  /**
   * @brief Helper: dereference the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_pointer_type checkedCPtr();
}; 


/**
 * @brief Return a @c ReadHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 */
template <class T>
ReadHandle<T> makeHandle (const ReadHandleKey<T>& key);


/**
 * @brief Return a @c ReadHandle referencing @c key for an explicit context.
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
ReadHandle<T> makeHandle (const ReadHandleKey<T>& key,
                          const EventContext& ctx);


/**
 * @brief Convenience function to retrieve an object given a @c ReadHandleKey.
 * @param key The key to retrieve.
 * @param ctx The event context.
 *
 * Returns the object.  Returns nullptr if the key is null or if there's an error.
 */
template <class T>
const T* get (const ReadHandleKey<T>& key);


/**
 * @brief Convenience function to retrieve an object given a @c ReadHandleKey.
 * @param key The key to retrieve.
 *
 * Returns the object.  Returns nullptr if the key is null or if there's an error.
 */
template <class T>
const T* get (const ReadHandleKey<T>& key,
              const EventContext& ctx);


} /* namespace SG */



#include "StoreGate/ReadHandle.icc"


#ifndef NO_LEGACY_HANDLES
namespace SG {
  template <class T>
  using RVar = ReadHandle<T>;
} 
#endif


#endif //> !STOREGATE_SG_READHANDLE_H
