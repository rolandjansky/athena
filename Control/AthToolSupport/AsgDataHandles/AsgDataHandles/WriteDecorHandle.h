// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AsgDataHandles/WriteDecorHandle.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original version)
 * @brief Handle class for adding a decoration to an object.
 */

#ifndef ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_H
#define ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_H

#ifndef XAOD_STANDALONE
#include <StoreGate/WriteDecorHandle.h>
#else

#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/DecorKeyHelpers.h"
#include "AthContainers/AuxElement.h"
// #include "GaudiKernel/EventContext.h"
// #include <type_traits>


namespace SG {


/**
 * @brief Handle class for adding a decoration to an object.
 *
 * See StoreGate/WriteDecorHandle for details.
 *
 * This currently (24 Aug 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
 */
template <class T, class D>
class WriteDecorHandle
  : public ReadHandle<T>
{
public:
  /// Base class.
  typedef ReadHandle<T> Base;

  /// Accessor type for aux data.
  typedef SG::AuxElement::Decorator<D> accessor_t;

  /// Type referencing an aux data item.
  typedef typename accessor_t::reference_type
    reference_type;

  /// Pointer into the vector holding an aux data item.
  typedef typename accessor_t::container_pointer_type
    container_pointer_type;

  /**
   * @brief Constructor from a WriteDecorHandleKey.
   * @param key The key object holding the clid/key/store/attr.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit WriteDecorHandle (const WriteDecorHandleKey<T>& key);


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
//   explicit WriteDecorHandle (const WriteDecorHandleKey<T>& key,
//                              const EventContext& ctx);

//   /**
//    * @brief Copy constructor.
//    */
//   WriteDecorHandle (const WriteDecorHandle& rhs);


//   /**
//    * @brief Move constructor.
//    */
//   WriteDecorHandle (WriteDecorHandle&& rhs);


//   /**
//    * @brief Destructor.  This will lock the decoration.
//    */
//   ~WriteDecorHandle();


//   /**
//    * @brief Assignment operator.
//    */
//   WriteDecorHandle& operator= (const WriteDecorHandle& rhs);


//   /**
//    * @brief Move operator.
//    */
//   WriteDecorHandle& operator= (WriteDecorHandle&& rhs);


//   /**
//    * @brief Is the referenced container present in SG?
//    *
//    * Note that this tests for the presence of the _container_,
//    * not for the decoration.
//    *
//    * Const method; the handle does not change as a result of this.
//    */
//   bool isPresent() const;


//   /**
//    * @brief Explicitly set the event store.
//    * @param store The new event store.
//    *
//    * This implicitly does a reset().
//    *
//    * We need to override this so that the setting gets made on the container
//    * handle as well.
//    */
//   virtual StatusCode setProxyDict (IProxyDict* store) override;

  
  /**
   * @brief Fetch the variable for one element, as a reference.
   * @param e The element for which to fetch the variable.
   */
  reference_type operator() (const AuxElement& e);


//   /**
//    * @brief Fetch the variable for one element, as a reference.
//    * @param index The index of the desired element.
//    *
//    * This looks up the variable in the object referenced by this handle.
//    * For a standalone object, pass an index of 0.
//    */
//   reference_type operator() (size_t index);


//   /**
//    * @brief Get a pointer to the start of the auxiliary data array,
//    *        for the referenced object.
//    */
//   container_pointer_type
//   getDecorationArray();


//   /**
//    * @brief Test to see if this variable exists in the store,
//    *        for the referenced object.
//    */
//   bool isAvailable();


//   /**
//    * @brief Return the aux id for this variable.
//    */
//   SG::auxid_t auxid() const;

  
// private:
//   /**
//    * @brief Retrieve an object from StoreGate.
//    * @param quiet If true, suppress failure messages.
//    *
//    * Extended for decoration handles: when we first retrieve the object,
//    * we make an alias for the decoration and also create the decoration itself.
//    */
//   virtual void* typeless_dataPointer_impl (bool quiet) override;


//   /**
//    * @brief Test to see if this variable exists in the store,
//    *        for the referenced object.
//    *        Specialization for the case of a standalone object
//    *        (@c T derives from @c SG::AuxElement).
//    */
//   bool isAvailable (std::true_type);


//   /**
//    * @brief Test to see if this variable exists in the store,
//    *        for the referenced object.
//    *        Specialization for the case of a container
//    *        (@c T does not derive from @c SG::AuxElement).
//    */
//   bool isAvailable (std::false_type);


//   /** 
//    * @brief Return the referenced object as a @c SG::AuxVectorData.
//    *        Specialization for the case of a standalone object
//    *        (@c T derives from @c SG::AuxElement).
//    */
//   const SG::AuxVectorData* vectorData (std::true_type);


//   /** 
//    * @brief Return the referenced object as a @c SG::AuxVectorData.
//    *        Specialization for the case of a container
//    *        (@c T does not derive from @c SG::AuxElement).
//    */
//   const SG::AuxVectorData* vectorData (std::false_type);


//   /** 
//    * @brief Return the referenced object as a @c SG::AuxVectorData.
//    *
//    * If @c T is a container object, then this should be the object itself.
//    * But if it is a standalone object, deriving from @c SG::AuxElement,
//    * then we need to call container() on the object.
//    */
//   const SG::AuxVectorData* vectorData();


//   /// Handle for reading the referenced object using its original name
//   /// (not the alias).
//   SG::ReadHandle<T> m_contHandle;


  /// Accessor for the aux data item.
  accessor_t m_acc;

//   bool m_madeAlias;
};


// /**
//  * @brief Return a @c WriteDecorHandle referencing @c key.
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
// WriteDecorHandle<T, D> makeHandle (const WriteDecorHandleKey<T>& key);


// /**
//  * @brief Return a @c WriteDecorHandle referencing @c key for an explicit context.
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
// WriteDecorHandle<T, D> makeHandle (const WriteDecorHandleKey<T>& key,
//                                    const EventContext& ctx);


/**
 * @brief These two signatures are to catch cases where the explicit
 *        template argument is omitted from the @c makeHandle call
 *        and give an error tailored to that.  Otherwise, the @c makeHandle
 *        call for @c ReadHandle would match, potentially giving a much
 *        more confusing error.
 */
template <class T>
void makeHandle (const WriteDecorHandleKey<T>& key);
template <class T>
void makeHandle (const WriteDecorHandleKey<T>& key,
                 const EventContext& ctx);


} // namespace SG


#include "AsgDataHandles/WriteDecorHandle.icc"


#endif

#endif
