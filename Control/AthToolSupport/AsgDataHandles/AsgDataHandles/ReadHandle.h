// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AsgDataHandles/ReadHandle.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov> (for original)
 * @brief Handle class for reading from StoreGate.
 */

#ifndef ASG_DATA_HANDLES_READ_HANDLE_H
#define ASG_DATA_HANDLES_READ_HANDLE_H


#ifndef XAOD_STANDALONE
#include <StoreGate/ReadHandle.h>
#else

#include "AsgDataHandles/VarHandleBase.h"
// #include "AsgDataHandles/ReadHandleKey.h"



namespace SG {
  template <class T> class ReadHandleKey;


/**
 * @class SG::ReadHandle<T>
 * @brief a smart pointer to an object of a given type in the event store
 *
 * See StoreGate/ReadHandle for details.
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


  // /**
  //  * @brief Default constructor.
  //  *
  //  * The handle will not be usable until a non-blank key is assigned.
  //  */
  // ReadHandle();


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
   * @brief Can the handle be successfully dereferenced?
   */
  bool isValid();


  /**
   * @brief Dereference the pointer, but don't cache anything.
   */
  const_pointer_type get() const;


  /**
   * @brief Dereference the pointer, but don't cache anything.
   * @param ctx The event context to use.
   */
  const_pointer_type get (const EventContext& ctx) const;


private:
  /**
   * @brief Helper: dereference the pointer.
   * Throws ExcNullReadHandle on failure.
   */
  const_pointer_type checkedCPtr();

  /**
   * @brief Helper: dereference the pointer.
   */
  const_pointer_type getCPtr() const;
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
 *
 * Returns the object.  Returns nullptr if the key is null or if there's an error.
 */
template <class T>
const T* get (const ReadHandleKey<T>& key);



/**
 * @brief Convenience function to retrieve an object given a @c ReadHandleKey.
 * @param key The key to retrieve.
 * @param ctx The event context.
 *
 * Returns the object.  Returns nullptr if the key is null or if there's an error.
 */
template <class T>
const T* get (const ReadHandleKey<T>& key,
              const EventContext& ctx);


} /* namespace SG */



#include "AsgDataHandles/ReadHandle.icc"

#endif

#endif //> !STOREGATE_SG_READHANDLE_H
