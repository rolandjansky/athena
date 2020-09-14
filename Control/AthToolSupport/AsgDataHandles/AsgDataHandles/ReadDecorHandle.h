// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file AsgDataHandles/ReadDecorHandle.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original)
 * @brief Handle class for reading a decoration on an object.
 */


#ifndef ASG_DATA_HANDLES_READ_DECOR_HANDLE_H
#define ASG_DATA_HANDLES_READ_DECOR_HANDLE_H

#ifndef XAOD_STANDALONE
#include <StoreGate/ReadDecorHandle.h>
#else

#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/DecorKeyHelpers.h"
// #include "AthContainers/AuxElement.h"
// #include "GaudiKernel/EventContext.h"
// #include <type_traits>


namespace SG {


/**
 * @brief Handle class for reading a decoration on an object.
 *
 * See StoreGate/ReadDecorHandle for details.
 *
 * This currently (31 Jul 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
 */
template <class T, class D>
class ReadDecorHandle
  : public ReadHandle<T>
{
public:
  /// Base class.
  typedef ReadHandle<T> Base;

  /// Accessor type for aux data.
  typedef SG::AuxElement::ConstAccessor<D> accessor_t;

  /// Type referencing an aux data item.
  typedef typename accessor_t::const_reference_type
    const_reference_type;

  /// Pointer into the vector holding an aux data item.
  typedef typename accessor_t::const_container_pointer_type
    const_container_pointer_type;


  /**
   * @brief Constructor from a ReadDecorHandleKey.
   * @param key The key object holding the clid/key/store/attr.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit ReadDecorHandle (const ReadDecorHandleKey<T>& key);


//   /**
//    * @brief Constructor from a ReadDecorHandleKey and an explicit event context.
//    * @param key The key object holding the clid/key.
//    * @param ctx The event context.
//    *
//    * This will raise an exception if the StoreGate key is blank,
//    * or if the event store cannot be found.
//    *
//    * If the default event store has been requested, then the thread-specific
//    * store from the event context will be used.
//    */
//   explicit ReadDecorHandle (const ReadDecorHandleKey<T>& key,
//                             const EventContext& ctx);

  
//   /**
//    * @brief Copy constructor.
//    */
//   ReadDecorHandle (const ReadDecorHandle& rhs);


//   /**
//    * @brief Move constructor.
//    */
//   ReadDecorHandle (ReadDecorHandle&& rhs);


//   /**
//    * @brief Assignment operator.
//    */
//   ReadDecorHandle& operator= (const ReadDecorHandle& rhs);


//   /**
//    * @brief Move operator.
//    */
//   ReadDecorHandle& operator= (ReadDecorHandle&& rhs);

  
  /**
   * @brief Is the referenced container present in SG?
   *
   * Note that this tests for the presence of the _container_,
   * not for the decoration.
   *
   * Const method; the handle does not change as a result of this.
   */
  bool isPresent() const;


  /**
   * @brief Fetch the variable for one element, as a const reference.
   * @param e The element for which to fetch the variable.
   */
  const_reference_type operator() (const AuxElement& e) const;


  /**
   * @brief Fetch the variable for one element, as a const reference.
   * @param index The index of the desired element.
   *
   * This looks up the variable in the object referenced by this handle.
   * For a standalone object, pass an index of 0.
   */
  const_reference_type operator() (size_t index);


//   /**
//    * @brief Get a pointer to the start of the auxiliary data array,
//    *        for the referenced object.
//    */
//   const_container_pointer_type
//   getDataArray();


//   /**
//    * @brief Test to see if this variable exists in the store,
//    *        for the referenced object.
//    */
//   bool isAvailable();


//   /**
//    * @brief Return the aux id for this variable.
//    */
//   SG::auxid_t auxid() const;

  
private:
  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *        Specialization for the case of a standalone object
   *        (@c T derives from @c SG::AuxElement).
   */
  const SG::AuxVectorData* vectorData (std::true_type);


  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *        Specialization for the case of a container
   *        (@c T does not derive from @c SG::AuxElement).
   */
  const SG::AuxVectorData* vectorData (std::false_type);


  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *
   * If @c T is a container object, then this should be the object itself.
   * But if it is a standalone object, deriving from @c SG::AuxElement,
   * then we need to call container() on the object.
   */
  const SG::AuxVectorData* vectorData();


  /// Accessor for the aux data item.
  accessor_t m_acc;
};


// /**
//  * @brief Return a @c ReadDecorHandle referencing @c key.
//  * @param key The key object holding the clid/key/store.
//  *
//  * This will raise an exception if the StoreGate key is blank,
//  * or if the event store cannot be found.
//  *
//  * The type of the decoration must be included as an explicit template parameter:
//  *
//  *@code
//  *   auto handle = SG::makeHandle<float> (key);
//  @endcode
//  *
//  * Note that @c D comes first in the argument list.  It's given explicitly,
//  * while @c T is inferred from @c key.
//  */
// template <class D, class T>
// ReadDecorHandle<T, D> makeHandle (const ReadDecorHandleKey<T>& key);


// /**
//  * @brief Return a @c ReadDecorHandle referencing @c key for an explicit context.
//  * @param key The key object holding the clid/key/store.
//  * @param ctx The event context.
//  *
//  * This will raise an exception if the StoreGate key is blank,
//  * or if the event store cannot be found.
//  *
//  * If the default event store has been requested, then the thread-specific
//  * store from the event context will be used.
//  *
//  * The type of the decoration must be included as an explicit template parameter:
//  *
//  *@code
//  *   auto handle = SG::makeHandle<float> (key, ctx);
//  @endcode
//  *
//  * Note that @c D comes first in the argument list.  It's given explicitly,
//  * while @c T is inferred from @c key.
//  */
// template <class D, class T>
// ReadDecorHandle<T, D> makeHandle (const ReadDecorHandleKey<T>& key,
//                                   const EventContext& ctx);


// /**
//  * @brief These two signatures are to catch cases where the explicit
//  *        template argument is omitted from the @c makeHandle call
//  *        and give an error tailored to that.  Otherwise, the @c makeHandle
//  *        call for @c ReadHandle would match, potentially giving a much
//  *        more confusing error.
//  */
// template <class T>
// void makeHandle (const ReadDecorHandleKey<T>& key);
// template <class T>
// void makeHandle (const ReadDecorHandleKey<T>& key,
//                  const EventContext& ctx);


} // namespace SG


#include "AsgDataHandles/ReadDecorHandle.icc"

#endif

#endif // not STOREGATE_READDECORHANDLE_READDECORHANDLE_H
