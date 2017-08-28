// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file StoreGate/ReadDecorHandle.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Handle class for reading a decoration on an object.
 */


#ifndef STOREGATE_READDECORHANDLE_H
#define STOREGATE_READDECORHANDLE_H


#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/tools/DecorKeyHelpers.h"
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/EventContext.h"
#include <type_traits>


namespace SG {


/**
 * @brief Handle class for reading a decoration on an object.
 *
 * This (and the corresponding @c WriteDecorHandle class) allow the scheduler
 * to track dependencies on decorations on objects, in addition to the objects
 * themselves.  For example, one algorithm could produce a container C,
 * a second could read C and add a decoration to it C.d, and a third algorithm
 * could read the decoration C.d.  This allows the scheduler to know that
 * the third algorithm must be executed after the second completes.
 *
 * A @c ReadDecorHandle<T, D> works just like a @c ReadHandle<T> (except that
 * using it as a property directly is not supported).  In addition, it has
 * functionality for accessing a decoration of type @c D, similarly
 * to the interface provided by @c Accessor objects.  You can either
 * read the decoration from a @c SG::AuxElement object:
 *
 *@code
 *  x = handle(o);
 @endcode
 *
 * or reference the element by its index in the container:
 *
 *@code
 *  x = handle(i);
 @endcode
 *
 * Example:
 *
 *@code
 *  class MyAlgorithm : public AthReentrantAlgorithm {
 *    ...
 *    SG::ReadDecorHandleKey<MyCont> m_key;
 *  };
 *  ...
 *  MyAlgorithm::MyAlgorithm (const std::string& name, ISvcLocator* svcloc)
 *    : AthReentrantAlgorithm (name, svcloc), ...
 *  {
 *    declareProperty ("Key", m_key = "container.decor");
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::initialize()
 *  {
 *    ATH_CHECK( m_key.initialize() );
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::execute_r (const EventContext& ctx) const
 *  {
 *    SG::ReadDecorHandle<MyCont, float> h (m_key, ctx);
 *    for (const MyObj& o : *h) {  // Access the container.
 *      doSomething (h (o));  // Access the decoration.
 *      ...
 @endcode
 *
 * Implementation note: we keep track of decorations using aliases.
 * For a container C with decoration d, the @c WriteDecorHandle will make
 * an alias C.d for C.  The @c ReadDecorHandle will then retrieve C.d from
 * StoreGate.  The alias C.d is also what enters into scheduling decisions.
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


  /**
   * @brief Constructor from a ReadDecorHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit ReadDecorHandle (const ReadDecorHandleKey<T>& key,
                            const EventContext& ctx);

  
  /**
   * @brief Copy constructor.
   */
  ReadDecorHandle (const ReadDecorHandle& rhs);


  /**
   * @brief Move constructor.
   */
  ReadDecorHandle (ReadDecorHandle&& rhs);


  /**
   * @brief Assignment operator.
   */
  ReadDecorHandle& operator= (const ReadDecorHandle& rhs);


  /**
   * @brief Move operator.
   */
  ReadDecorHandle& operator= (ReadDecorHandle&& rhs);

  
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


  /**
   * @brief Get a pointer to the start of the auxiliary data array,
   *        for the referenced object.
   */
  const_container_pointer_type
  getDataArray();


  /**
   * @brief Test to see if this variable exists in the store,
   *        for the referenced object.
   */
  bool isAvailable();


  /**
   * @brief Return the aux id for this variable.
   */
  SG::auxid_t auxid() const;

  
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


/**
 * @brief Return a @c ReadDecorHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * The type of the decoration must be included as an explicit template parameter:
 *
 *@code
 *   auto handle = SG::makeHandle<float> (key);
 @endcode
 *
 * Note that @c D comes first in the argument list.  It's given explicitly,
 * while @c T is inferred from @c key.
 */
template <class D, class T>
ReadDecorHandle<T, D> makeHandle (const ReadDecorHandleKey<T>& key);


/**
 * @brief Return a @c ReadDecorHandle referencing @c key for an explicit context.
 * @param key The key object holding the clid/key/store.
 * @param ctx The event context.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * If the default event store has been requested, then the thread-specific
 * store from the event context will be used.
 *
 * The type of the decoration must be included as an explicit template parameter:
 *
 *@code
 *   auto handle = SG::makeHandle<float> (key, ctx);
 @endcode
 *
 * Note that @c D comes first in the argument list.  It's given explicitly,
 * while @c T is inferred from @c key.
 */
template <class D, class T>
ReadDecorHandle<T, D> makeHandle (const ReadDecorHandleKey<T>& key,
                                  const EventContext& ctx);


/**
 * @brief These two signatures are to catch cases where the explicit
 *        template argument is omitted from the @c makeHandle call
 *        and give an error tailored to that.  Otherwise, the @c makeHandle
 *        call for @c ReadHandle would match, potentially giving a much
 *        more confusing error.
 */
template <class T>
void makeHandle (const ReadDecorHandleKey<T>& /*key*/);
template <class T>
void makeHandle (const ReadDecorHandleKey<T>& /*key*/,
                 const EventContext& /*ctx*/);


} // namespace SG


#include "StoreGate/ReadDecorHandle.icc"


#endif // not STOREGATE_READDECORHANDLE_READDECORHANDLE_H
